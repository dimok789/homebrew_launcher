/**
 * This class implements the DPAD mode for the GamePad, allowing you to use the DPAD to move a virtual
 * on screen pointer, rather than using the touch screen. The program will not be able to detect
 * any DPAD/A button presses in this mode, as it may interfere with the user who is navigating the pointer.
 *
 * Created by CreeperMario in July 2017.
 */

#ifndef DPAD_CONTROLLER_H_
#define DPAD_CONTROLLER_H_

#include <vpad/input.h>
#include "GuiController.h"

class DVPadController : public GuiController
{
public:
    
    //!Constructor
    DVPadController(int channel) : GuiController(channel)
    {
        memset(&vpad, 0, sizeof(VPADStatus));
        memset(&data, 0, sizeof(PadData));
        memset(&lastData, 0, sizeof(PadData));
        
        data.validPointer = true;
        isDPadMode = true;
        showPointer = true;
    }
    
    //!Destructor
    virtual ~DVPadController() {}
    
    //Remove the DPAD buttons (by clearing their bits) so that they aren't used by the Gui processes.
    u32 fixButtons(u32 buttons)
    {
        buttons &= ~VPAD_BUTTON_LEFT;
        buttons &= ~VPAD_BUTTON_RIGHT;
        buttons &= ~VPAD_BUTTON_UP;
        buttons &= ~VPAD_BUTTON_DOWN;
        buttons &= ~VPAD_BUTTON_A;
        return buttons;
    }
    
    bool update(int width, int height)
    {
        lastData = data;
        
        VPADReadError vpadError = VPAD_READ_SUCCESS;
        VPADRead(0, &vpad, 1, &vpadError);
        
        if(vpadError == VPAD_READ_SUCCESS)
        {
            if(vpad.hold & VPAD_BUTTON_LEFT)
            {
                if(data.x > -(width / 2)) data.x -= 10;
            }
            if(vpad.hold & VPAD_BUTTON_RIGHT)
            {
                if(data.x < (width / 2)) data.x += 10;
            }
            if(vpad.hold & VPAD_BUTTON_UP)
            {
                if(data.y < (height / 2)) data.y += 10;
            }
            if(vpad.hold & VPAD_BUTTON_DOWN)
            {
                if(data.y > -(height / 2)) data.y -= 10;
            }
            
            if(vpad.hold & VPAD_BUTTON_A)
                data.touched = true;
            else
                data.touched = false;
            
            data.buttons_r = fixButtons(vpad.release);
            data.buttons_h = fixButtons(vpad.hold);
            data.buttons_d = fixButtons(vpad.trigger);
            
            return true;
        }
        
        return false;
    }
    
private:
    VPADStatus vpad;
};

#endif
