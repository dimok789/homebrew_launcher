#include "types.h"
#include "elf_abi.h"
#include "kexploit.h"
#include "structs.h"
#include "sd_loader.h"

#define MEM_BASE                                        0xC0800000
#include "../src/common/common.h"
#include "../src/common/os_defs.h"
#include "coreinit.h"

//! this shouldnt depend on OS
#define LIB_CODE_RW_BASE_OFFSET                         0xC1000000
#define CODE_RW_BASE_OFFSET                             0xC0000000
#define DATA_RW_BASE_OFFSET                             0xC0000000

#if ( (VER == 532) || (VER == 540) )
    #define ADDRESS_OSTitle_main_entry_ptr              0x1005d180
    #define ADDRESS_main_entry_hook                     0x0101c55c

    #define KERN_SYSCALL_TBL_1                          0xFFE84C70 // unknown
    #define KERN_SYSCALL_TBL_2                          0xFFE85070 // works with games
    #define KERN_SYSCALL_TBL_3                          0xFFE85470 // works with loader
    #define KERN_SYSCALL_TBL_4                          0xFFEA9CE0 // works with home menu
    #define KERN_SYSCALL_TBL_5                          0xFFEAA0E0 // works with browser (previously KERN_SYSCALL_TBL)
#elif ( (VER == 500) || (VER == 510) )
    #define ADDRESS_OSTitle_main_entry_ptr              0x1005CB00
    #define ADDRESS_main_entry_hook                     0x0101C15C

    #define KERN_SYSCALL_TBL_1                          0xFFE84C70 // unknown
    #define KERN_SYSCALL_TBL_2                          0xFFE85070 // works with games
    #define KERN_SYSCALL_TBL_3                          0xFFE85470 // works with loader
    #define KERN_SYSCALL_TBL_4                          0xFFEA9120 // works with home menu
    #define KERN_SYSCALL_TBL_5                          0xFFEA9520 // works with browser (previously KERN_SYSCALL_TBL)
#elif (VER == 550)
    #define ADDRESS_OSTitle_main_entry_ptr              0x1005E040
    #define ADDRESS_main_entry_hook                     0x0101c56c

    #define KERN_SYSCALL_TBL_1                          0xFFE84C70 // unknown
    #define KERN_SYSCALL_TBL_2                          0xFFE85070 // works with games
    #define KERN_SYSCALL_TBL_3                          0xFFE85470 // works with loader
    #define KERN_SYSCALL_TBL_4                          0xFFEAAA60 // works with home menu
    #define KERN_SYSCALL_TBL_5                          0xFFEAAE60 // works with browser (previously KERN_SYSCALL_TBL)
#elif (VER == 410)
    #define ADDRESS_OSTitle_main_entry_ptr              0x1005A8C0
    #define ADDRESS_main_entry_hook                     0x0101BD4C

    #define KERN_SYSCALL_TBL_1                          0xFFE84C90
    #define KERN_SYSCALL_TBL_2                          0xFFE85090
    #define KERN_SYSCALL_TBL_3                          0xFFE85C90
    #define KERN_SYSCALL_TBL_4                          0xFFE85490
    #define KERN_SYSCALL_TBL_5                          0xFFE85890 // works with browser
#elif (VER == 400)
    #define ADDRESS_OSTitle_main_entry_ptr              0x1005A600
    #define ADDRESS_main_entry_hook                     0x0101BD4C

    #define KERN_SYSCALL_TBL_1                          0xFFE84C90
    #define KERN_SYSCALL_TBL_2                          0xFFE85090
    #define KERN_SYSCALL_TBL_3                          0xFFE85C90
    #define KERN_SYSCALL_TBL_4                          0xFFE85490
    #define KERN_SYSCALL_TBL_5                          0xFFE85890 // works with browser
#elif ( (VER == 300) || (VER == 310) )
    #define ADDRESS_OSTitle_main_entry_ptr              0x1005BBC0
    #define ADDRESS_main_entry_hook                     0x0101894C // used OSDynLoad_Acquire 0x01022CBC from libwiiu to calculate

    #define KERN_SYSCALL_TBL_1                          0xFFE84D50
    #define KERN_SYSCALL_TBL_2                          0xFFE85150
    #define KERN_SYSCALL_TBL_3                          0xFFE85D50 // comes after KERN_SYSCALL_TBL_5
    #define KERN_SYSCALL_TBL_4                          0xFFE85550
    #define KERN_SYSCALL_TBL_5                          0xFFE85950
#else
    #error Please define valid values for firmware.
#endif // VER

#define ROOTRPX_DBAT0U_VAL                              0xC00003FF
#define COREINIT_DBAT0U_VAL                             0xC20001FF
#if (VER >= 410)
    #define ROOTRPX_DBAT0L_VAL                          0x30000012
    #define COREINIT_DBAT0L_VAL                         0x32000012
#elif (VER <= 400)
    #define ROOTRPX_DBAT0L_VAL                          0x4E000012
    #define COREINIT_DBAT0L_VAL                         0x4D000012
#else
    #error Please define valid values for firmware.
#endif

/* Install functions */
static void InstallMain(private_data_t *private_data);
static void InstallPatches(private_data_t *private_data);
static void ExitFailure(private_data_t *private_data, const char *failure);

static int show_install_menu(unsigned int coreinit_handle, unsigned int *ip_address);
static void thread_callback(int argc, void *argv);

static void SetupKernelSyscall(unsigned int addr);
static void KernelCopyData(unsigned int addr, unsigned int src, unsigned int len);

/* assembly functions */
extern void SC_0x25_KernelCopyData(void* addr, void* src, unsigned int len);
extern void Syscall_0x36(void);
extern void KernelPatches(void);

/* ****************************************************************** */
/*                               ENTRY POINT                          */
/* ****************************************************************** */
void __main(void)
{
    /* Get coreinit handle and keep it in memory */
    unsigned int coreinit_handle;
    OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);

    /* Get our memory functions */
    unsigned int* functionPointer;
    void* (*memset)(void * dest, unsigned int value, unsigned int bytes);
    OSDynLoad_FindExport(coreinit_handle, 0, "memset", &memset);

    private_data_t private_data;
    memset(&private_data, 0, sizeof(private_data_t));

    private_data.coreinit_handle = coreinit_handle;
    private_data.memset = memset;
    private_data.data_elf = (unsigned char *) ___sd_loader_sd_loader_elf; // use this address as temporary to load the elf

    OSDynLoad_FindExport(coreinit_handle, 1, "MEMAllocFromDefaultHeapEx", &functionPointer);
    private_data.MEMAllocFromDefaultHeapEx = (void*(*)(unsigned int, unsigned int))*functionPointer;
    OSDynLoad_FindExport(coreinit_handle, 1, "MEMFreeToDefaultHeap", &functionPointer);
    private_data.MEMFreeToDefaultHeap = (void (*)(void *))*functionPointer;

    OSDynLoad_FindExport(coreinit_handle, 0, "memcpy", &private_data.memcpy);
    OSDynLoad_FindExport(coreinit_handle, 0, "OSEffectiveToPhysical", &private_data.OSEffectiveToPhysical);
    OSDynLoad_FindExport(coreinit_handle, 0, "DCFlushRange", &private_data.DCFlushRange);
    OSDynLoad_FindExport(coreinit_handle, 0, "ICInvalidateRange", &private_data.ICInvalidateRange);
    OSDynLoad_FindExport(coreinit_handle, 0, "_Exit", &private_data._Exit);

    if (private_data.OSEffectiveToPhysical((void *)0xa0000000) == (void *)0)
    {
        run_kexploit(&private_data);
    }
    else
    {
        /* Get functions to send restart signal */
        int(*IM_Open)();
        int(*IM_Close)(int fd);
        int(*IM_SetDeviceState)(int fd, void *mem, int state, int a, int b);
        void*(*OSAllocFromSystem)(unsigned int size, int align);
        void(*OSFreeToSystem)(void *ptr);
        OSDynLoad_FindExport(coreinit_handle, 0, "IM_Open", &IM_Open);
        OSDynLoad_FindExport(coreinit_handle, 0, "IM_Close", &IM_Close);
        OSDynLoad_FindExport(coreinit_handle, 0, "IM_SetDeviceState", &IM_SetDeviceState);
        OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);
        OSDynLoad_FindExport(coreinit_handle, 0, "OSFreeToSystem", &OSFreeToSystem);

        /* Send restart signal to get rid of uneeded threads */
        /* Cause the other browser threads to exit */
        int fd = IM_Open();
        void *mem = OSAllocFromSystem(0x100, 64);
        if(!mem)
            ExitFailure(&private_data, "Not enough memory. Exit and re-enter browser.");

        private_data.memset(mem, 0, 0x100);

        /* Sets wanted flag */
        IM_SetDeviceState(fd, mem, 3, 0, 0);
        IM_Close(fd);
        OSFreeToSystem(mem);

        /* Waits for thread exits */
        unsigned int t1 = 0x1FFFFFFF;
        while(t1--) ;

        /* restore kernel memory table to original state */
        kern_write((void*)(KERN_ADDRESS_TBL + (0x12 * 4)), 0);
        kern_write((void*)(KERN_ADDRESS_TBL + (0x13 * 4)), 0x14000000);
    }

    /* Prepare for thread startups */
    int (*OSCreateThread)(void *thread, void *entry, int argc, void *args, unsigned int stack, unsigned int stack_size, int priority, unsigned short attr);
    int (*OSResumeThread)(void *thread);
    int (*OSIsThreadTerminated)(void *thread);

    OSDynLoad_FindExport(coreinit_handle, 0, "OSCreateThread", &OSCreateThread);
    OSDynLoad_FindExport(coreinit_handle, 0, "OSResumeThread", &OSResumeThread);
    OSDynLoad_FindExport(coreinit_handle, 0, "OSIsThreadTerminated", &OSIsThreadTerminated);

    /* Allocate a stack for the thread */
    /* IMPORTANT: libcurl uses around 0x1000 internally so make
        sure to allocate more for the stack to avoid crashes */
    void *stack = private_data.MEMAllocFromDefaultHeapEx(0x4000, 0x20);
    /* Create the thread variable */
    void *thread = private_data.MEMAllocFromDefaultHeapEx(0x1000, 8);
    if(!thread || !stack)
        ExitFailure(&private_data, "Thread memory allocation failed. Exit and re-enter browser.");

    // the thread stack is too small on current thread, switch to an own created thread
    // create a detached thread with priority 0 and use core 1
    int ret = OSCreateThread(thread, thread_callback, 1, (void*)&private_data, (unsigned int)stack+0x4000, 0x4000, 0, 0x1A);
    if (ret == 0)
        ExitFailure(&private_data, "Failed to create thread. Exit and re-enter browser.");

    /* Schedule it for execution */
    OSResumeThread(thread);

    // Keep this main thread around for ELF loading
    // Can not use OSJoinThread, which hangs for some reason, so we use a detached one and wait for it to terminate
    while(OSIsThreadTerminated(thread) == 0)
    {
        asm volatile (
        "    nop\n"
        "    nop\n"
        "    nop\n"
        "    nop\n"
        "    nop\n"
        "    nop\n"
        "    nop\n"
        "    nop\n"
        );
    }

    /* setup kernel copy data syscall */
    kern_write((void*)(KERN_SYSCALL_TBL_5 + (0x25 * 4)), (unsigned int)KernelCopyData);

    /* Install our code now */
    InstallMain(&private_data);

    /* setup our own syscall and call it */
    SetupKernelSyscall((unsigned int)KernelPatches);
    Syscall_0x36();

    /* Patch functions and our code for usage */
    InstallPatches(&private_data);

    /* Free thread memory and stack */
    private_data.MEMFreeToDefaultHeap(thread);
    private_data.MEMFreeToDefaultHeap(stack);

    //! we are done -> exit browser now
    private_data._Exit();
}

void ExitFailure(private_data_t *private_data, const char *failure)
{
    /************************************************************************/
    // Prepare screen
    void (*OSScreenInit)();
    unsigned int (*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
    unsigned int (*OSScreenSetBufferEx)(unsigned int bufferNum, void * addr);
    unsigned int (*OSScreenClearBufferEx)(unsigned int bufferNum, unsigned int temp);
    unsigned int (*OSScreenFlipBuffersEx)(unsigned int bufferNum);
    unsigned int (*OSScreenPutFontEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, const char * buffer);

    OSDynLoad_FindExport(private_data->coreinit_handle, 0, "OSScreenInit", &OSScreenInit);
    OSDynLoad_FindExport(private_data->coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);
    OSDynLoad_FindExport(private_data->coreinit_handle, 0, "OSScreenSetBufferEx", &OSScreenSetBufferEx);
    OSDynLoad_FindExport(private_data->coreinit_handle, 0, "OSScreenClearBufferEx", &OSScreenClearBufferEx);
    OSDynLoad_FindExport(private_data->coreinit_handle, 0, "OSScreenFlipBuffersEx", &OSScreenFlipBuffersEx);
    OSDynLoad_FindExport(private_data->coreinit_handle, 0, "OSScreenPutFontEx", &OSScreenPutFontEx);

    // Prepare screen
    int screen_buf0_size = 0;
    int screen_buf1_size = 0;
    unsigned int screen_color = 0; // (r << 24) | (g << 16) | (b << 8) | a;

    // Init screen and screen buffers
    OSScreenInit();
    screen_buf0_size = OSScreenGetBufferSizeEx(0);
    screen_buf1_size = OSScreenGetBufferSizeEx(1);
    OSScreenSetBufferEx(0, (void *)0xF4000000);
    OSScreenSetBufferEx(1, (void *)0xF4000000 + screen_buf0_size);

    // Clear screens
    OSScreenClearBufferEx(0, screen_color);
    OSScreenClearBufferEx(1, screen_color);

    // Flush the cache
    private_data->DCFlushRange((void *)0xF4000000, screen_buf0_size);
    private_data->DCFlushRange((void *)0xF4000000 + screen_buf0_size, screen_buf1_size);

    // Flip buffers
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);

    OSScreenPutFontEx(1, 0, 0, failure);

    OSScreenFlipBuffersEx(1);
    OSScreenClearBufferEx(1, 0);

    unsigned int t1 = 0x3FFFFFFF;
    while(t1--) asm volatile("nop");

    private_data->_Exit();
}

/* *****************************************************************************
 * Base functions
 * ****************************************************************************/
static void SetupKernelSyscall(unsigned int address)
{
    // Add syscall #0x36
    kern_write((void*)(KERN_SYSCALL_TBL_5 + (0x36 * 4)), address);

    // make kern_read/kern_write available in all places
    kern_write((void*)(KERN_SYSCALL_TBL_1 + (0x34 * 4)), KERN_CODE_READ);
    kern_write((void*)(KERN_SYSCALL_TBL_2 + (0x34 * 4)), KERN_CODE_READ);
    kern_write((void*)(KERN_SYSCALL_TBL_3 + (0x34 * 4)), KERN_CODE_READ);
    kern_write((void*)(KERN_SYSCALL_TBL_4 + (0x34 * 4)), KERN_CODE_READ);

    kern_write((void*)(KERN_SYSCALL_TBL_1 + (0x35 * 4)), KERN_CODE_WRITE);
    kern_write((void*)(KERN_SYSCALL_TBL_2 + (0x35 * 4)), KERN_CODE_WRITE);
    kern_write((void*)(KERN_SYSCALL_TBL_3 + (0x35 * 4)), KERN_CODE_WRITE);
    kern_write((void*)(KERN_SYSCALL_TBL_4 + (0x35 * 4)), KERN_CODE_WRITE);
}

static void KernelCopyData(unsigned int addr, unsigned int src, unsigned int len)
{
    /*
     * Setup a DBAT access for our 0xC0800000 area and our 0xBC000000 area which hold our variables like GAME_LAUNCHED and our BSS/rodata section
     */
    register unsigned int dbatu0, dbatl0, target_dbat0u, target_dbat0l;
    // setup mapping based on target address
    if ((addr >= 0xC0000000) && (addr < 0xC2000000)) // root.rpx address
    {
        target_dbat0u = ROOTRPX_DBAT0U_VAL;
        target_dbat0l = ROOTRPX_DBAT0L_VAL;
    }
    else if ((addr >= 0xC2000000) && (addr < 0xC3000000))
    {
        target_dbat0u = COREINIT_DBAT0U_VAL;
        target_dbat0l = COREINIT_DBAT0L_VAL;
    }
    // save the original DBAT value
    asm volatile("mfdbatu %0, 0" : "=r" (dbatu0));
    asm volatile("mfdbatl %0, 0" : "=r" (dbatl0));
    asm volatile("mtdbatu 0, %0" : : "r" (target_dbat0u));
    asm volatile("mtdbatl 0, %0" : : "r" (target_dbat0l));
    asm volatile("eieio; isync");

    unsigned char *src_p = (unsigned char*)src;
    unsigned char *dst_p = (unsigned char*)addr;

    unsigned int i;
    for(i = 0; i < len; i++)
    {
        dst_p[i] = src_p[i];
    }

    unsigned int flushAddr = addr & ~31;

    while(flushAddr < (addr + len))
    {
        asm volatile("dcbf 0, %0; sync" : : "r"(flushAddr));
        flushAddr += 0x20;
    }

    /*
     * Restore original DBAT value
     */
    asm volatile("mtdbatu 0, %0" : : "r" (dbatu0));
    asm volatile("mtdbatl 0, %0" : : "r" (dbatl0));
    asm volatile("eieio; isync");
}

static void thread_callback(int argc, void *argv)
{
    /* Pre-load the Mii Studio to be executed on _Exit - thanks to wj444 for sharing it */
    unsigned int sysapp_handle;
    void (*_SYSLaunchMiiStudio)(void) = 0;
    OSDynLoad_Acquire("sysapp.rpl", &sysapp_handle);
    OSDynLoad_FindExport(sysapp_handle, 0, "_SYSLaunchMiiStudio", &_SYSLaunchMiiStudio);

    _SYSLaunchMiiStudio();
}

static int strcmp(const char *s1, const char *s2)
{
    while(*s1 && *s2)
    {
        if(*s1 != *s2) {
            return -1;
        }
        s1++;
        s2++;
    }

    if(*s1 != *s2) {
        return -1;
    }
    return 0;
}

static unsigned int get_section(private_data_t *private_data, unsigned char *data, const char *name, unsigned int * size, unsigned int * addr, int fail_on_not_found)
{
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) data;

    if (   !data
        || !IS_ELF (*ehdr)
        || (ehdr->e_type != ET_EXEC)
        || (ehdr->e_machine != EM_PPC))
    {
        ExitFailure(private_data, "Invalid elf file");
    }

    Elf32_Shdr *shdr = (Elf32_Shdr *) (data + ehdr->e_shoff);
    int i;
    for(i = 0; i < ehdr->e_shnum; i++)
    {
        const char *section_name = ((const char*)data) + shdr[ehdr->e_shstrndx].sh_offset + shdr[i].sh_name;
        if(strcmp(section_name, name) == 0)
        {
            if(addr)
                *addr = shdr[i].sh_addr;
            if(size)
                *size = shdr[i].sh_size;
            return shdr[i].sh_offset;
        }
    }

    if(fail_on_not_found)
        ExitFailure(private_data, (char*)name);

    return 0;
}

/* ****************************************************************** */
/*                         INSTALL MAIN CODE                          */
/* ****************************************************************** */
static void InstallMain(private_data_t *private_data)
{
    // get .text section
    unsigned int main_text_addr = 0;
    unsigned int main_text_len = 0;
    unsigned int section_offset = get_section(private_data, private_data->data_elf, ".text", &main_text_len, &main_text_addr, 1);
    unsigned char *main_text = private_data->data_elf + section_offset;
    /* Copy main .text to memory */
    if(section_offset > 0)
        SC_0x25_KernelCopyData((void*)(CODE_RW_BASE_OFFSET + main_text_addr), main_text, main_text_len);

    // get the .rodata section
    unsigned int main_rodata_addr = 0;
    unsigned int main_rodata_len = 0;
    section_offset = get_section(private_data, private_data->data_elf, ".rodata", &main_rodata_len, &main_rodata_addr, 0);
    if(section_offset > 0)
    {
        unsigned char *main_rodata = private_data->data_elf + section_offset;
        /* Copy main rodata to memory */
        SC_0x25_KernelCopyData((void*)(DATA_RW_BASE_OFFSET + main_rodata_addr), main_rodata, main_rodata_len);
    }

    // get the .data section
    unsigned int main_data_addr = 0;
    unsigned int main_data_len = 0;
    section_offset = get_section(private_data, private_data->data_elf, ".data", &main_data_len, &main_data_addr, 0);
    if(section_offset > 0)
    {
        unsigned char *main_data = private_data->data_elf + section_offset;
        /* Copy main data to memory */
        SC_0x25_KernelCopyData((void*)(DATA_RW_BASE_OFFSET + main_data_addr), main_data, main_data_len);
    }

    // get the .bss section
    unsigned int main_bss_addr = 0;
    unsigned int main_bss_len = 0;
    section_offset = get_section(private_data, private_data->data_elf, ".bss", &main_bss_len, &main_bss_addr, 0);
    if(section_offset > 0)
    {
        unsigned char *main_bss = private_data->data_elf + section_offset;
        /* Copy main data to memory */
        SC_0x25_KernelCopyData((void*)(DATA_RW_BASE_OFFSET + main_bss_addr), main_bss, main_bss_len);
    }
}

/* ****************************************************************** */
/*                         INSTALL PATCHES                            */
/* All OS specific stuff is done here                                 */
/* ****************************************************************** */
static void InstallPatches(private_data_t *private_data)
{
    OsSpecifics osSpecificFunctions;
    private_data->memset(&osSpecificFunctions, 0, sizeof(OsSpecifics));

    unsigned int bufferU32;
    /* Pre-setup a few options to defined values */
    bufferU32 = VER;
    SC_0x25_KernelCopyData((void*)&OS_FIRMWARE, &bufferU32, sizeof(bufferU32));
    bufferU32 = 0xDEADC0DE;
    SC_0x25_KernelCopyData((void*)&MAIN_ENTRY_ADDR, &bufferU32, sizeof(bufferU32));
    SC_0x25_KernelCopyData((void*)&ELF_DATA_ADDR, &bufferU32, sizeof(bufferU32));
    bufferU32 = 0;
    SC_0x25_KernelCopyData((void*)&ELF_DATA_SIZE, &bufferU32, sizeof(bufferU32));

    unsigned int jump_main_hook = 0;
    osSpecificFunctions.addr_OSDynLoad_Acquire = (unsigned int)OSDynLoad_Acquire;
    osSpecificFunctions.addr_OSDynLoad_FindExport = (unsigned int)OSDynLoad_FindExport;

    osSpecificFunctions.addr_KernSyscallTbl1 = KERN_SYSCALL_TBL_1;
    osSpecificFunctions.addr_KernSyscallTbl2 = KERN_SYSCALL_TBL_2;
    osSpecificFunctions.addr_KernSyscallTbl3 = KERN_SYSCALL_TBL_3;
    osSpecificFunctions.addr_KernSyscallTbl4 = KERN_SYSCALL_TBL_4;
    osSpecificFunctions.addr_KernSyscallTbl5 = KERN_SYSCALL_TBL_5;
    //! pointer to main entry point of a title
    osSpecificFunctions.addr_OSTitle_main_entry = ADDRESS_OSTitle_main_entry_ptr;

    SC_0x25_KernelCopyData((void*)OS_SPECIFICS, &osSpecificFunctions, sizeof(OsSpecifics));

    //! at this point we dont need to check header and stuff as it is sure to be OK
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) private_data->data_elf;
    unsigned int mainEntryPoint = ehdr->e_entry;

    //! Install our entry point hook
    unsigned int repl_addr = ADDRESS_main_entry_hook;
    unsigned int jump_addr = mainEntryPoint & 0x03fffffc;
    bufferU32 = 0x48000003 | jump_addr;
    SC_0x25_KernelCopyData((void*)(LIB_CODE_RW_BASE_OFFSET + repl_addr), &bufferU32, sizeof(bufferU32));
    // flush caches and invalidate instruction cache
    private_data->ICInvalidateRange((void*)(repl_addr), 4);
}
