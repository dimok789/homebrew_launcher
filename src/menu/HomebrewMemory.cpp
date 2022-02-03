#include <string.h>
#include "common/common.h"
#include <dynamic_libs/os_functions.h>
#include "custom/system/memory_area_table.h"
#include <utils/utils.h>
#include <utils/logger.h>
#include <kernel/syscalls.h>

static s_mem_area *mem_map = 0;
static u32 mapPosition = 0;

void HomebrewInitMemory(void)
{
    memoryInitAreaTable();

    mem_map = MEM_AREA_TABLE;
    mapPosition = 0;

    ELF_DATA_ADDR = 0;
    ELF_DATA_SIZE = 0;
    RPX_MAX_SIZE = 0x40000000;
    RPX_MAX_CODE_SIZE = 0x03000000;
}

int HomebrewCopyMemory(u8 *address, u32 bytes)
{
    if(ELF_DATA_SIZE == 0)
    {
        // check if we load an RPX or an ELF
        if(*(u16*)&address[7] != 0xCAFE)
        {
            // assume ELF
            ELF_DATA_ADDR = getApplicationEndAddr();
        }
        else
        {
            // RPX
            ELF_DATA_ADDR = MEM_AREA_TABLE->address;
        }
    }

    //! if we load an ELF file
    if(ELF_DATA_ADDR < 0x01000000)
    {
        u32 targetAddress = ELF_DATA_ADDR + ELF_DATA_SIZE;
        if((targetAddress + bytes) > 0x01000000)
            return -1;

        memcpy((void*)(targetAddress + ELF_DATA_SIZE), address, bytes);
        ELF_DATA_SIZE += bytes;
        MAIN_ENTRY_ADDR = 0xDEADC0DE;
    }
    else
    {
        DCFlushRange(address, bytes);

        u32 done = 0;
        u32 addressPhysical = (u32)OSEffectiveToPhysical(address);

        while((done < bytes) && mem_map)
        {
            if(mapPosition >= mem_map->size)
            {
                 mem_map = mem_map->next;
                 if(!mem_map)
                    return -1;

                 mapPosition = 0;
            }

            u32 blockSize = bytes - done;
            if((mapPosition + blockSize) > mem_map->size)
            {
                blockSize = mem_map->size - mapPosition;
            }

            SC0x25_KernelCopyData(mem_map->address + mapPosition, (addressPhysical + done), blockSize);

            mapPosition += blockSize;
            done += blockSize;
        }

        ELF_DATA_SIZE += done;
    }
    return bytes;
}
