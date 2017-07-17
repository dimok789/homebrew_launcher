/**
 * This class implements the DPAD mode for Wii Remote, Classic and Pro controllers, allowing you to use
 * the DPAD on your controller to move a virtual on screen pointer, rather than having to point at the
 * screen. This means that users who do not have a sensor bar or primarily use Classic/Pro controllers
 * can now use this program without trouble. The program will not be able to detect any DPAD button
 * presses in this mode, as it may interfere with the user who is navigating the pointer.
 *
 * Created by CreeperMario in July 2017.
 */

#ifndef DWPAD_CONTROLLER_H_
#define DWPAD_CONTROLLER_H_

#include "GuiController.h"
#include "dynamic_libs/padscore_functions.h"

class DWPadController : public GuiController
{
public:
    
    //!Constructor
    DWPadController(int channel) : GuiController(channel)
    {
        memset(&kpadData, 0, sizeof(KPADData));
        
        data.validPointer = true;
        isDPadMode = true;
        showPointer = true;
    }
    
    //!Destructor
    virtual ~DWPadController() {}
    
    //Remove the DPAD buttons (by ignoring their bits) so that they aren't used by the Gui processes.
    u32 remapWiiMoteButtons(u32 buttons)
    {
        u32 temp = 0;
        
        if(buttons & WPAD_BUTTON_MINUS)
            temp |= GuiTrigger::BUTTON_MINUS;
        if(buttons & WPAD_BUTTON_PLUS)
            temp |= GuiTrigger::BUTTON_PLUS;
        if(buttons & WPAD_BUTTON_2)
            temp |= GuiTrigger::BUTTON_2;
        if(buttons & WPAD_BUTTON_1)
            temp |= GuiTrigger::BUTTON_1;
        if(buttons & WPAD_BUTTON_B)
            temp |= GuiTrigger::BUTTON_B;
        if(buttons & WPAD_BUTTON_A)
            temp |= GuiTrigger::BUTTON_A;
        if(buttons & WPAD_BUTTON_Z) //Nunchuk only
            temp |= GuiTrigger::BUTTON_Z;
        if(buttons & WPAD_BUTTON_C) //Nunchuk only
            temp |= GuiTrigger::BUTTON_C;
        if(buttons & WPAD_BUTTON_HOME)
            temp |= GuiTrigger::BUTTON_HOME;
        
        return temp;
    }
    
    //Remove the DPAD buttons (by ignoring their bits) so that they aren't used by the Gui processes.
    u32 remapClassicButtons(u32 buttons)
    {
        u32 temp = 0;
        
        if(buttons & WPAD_CLASSIC_BUTTON_MINUS)
            temp |= GuiTrigger::BUTTON_MINUS;
        if(buttons & WPAD_CLASSIC_BUTTON_PLUS)
            temp |= GuiTrigger::BUTTON_PLUS;
        if(buttons & WPAD_CLASSIC_BUTTON_X)
            temp |= GuiTrigger::BUTTON_X;
        if(buttons & WPAD_CLASSIC_BUTTON_Y)
            temp |= GuiTrigger::BUTTON_Y;
        if(buttons & WPAD_CLASSIC_BUTTON_B)
            temp |= GuiTrigger::BUTTON_B;
        if(buttons & WPAD_CLASSIC_BUTTON_A)
            temp |= GuiTrigger::BUTTON_A;
        if(buttons & WPAD_CLASSIC_BUTTON_HOME)
            temp |= GuiTrigger::BUTTON_HOME;
        if(buttons & WPAD_CLASSIC_BUTTON_ZR)
            temp |= GuiTrigger::BUTTON_ZR;
        if(buttons & WPAD_CLASSIC_BUTTON_ZL)
            temp |= GuiTrigger::BUTTON_ZL;
        if(buttons & WPAD_CLASSIC_BUTTON_R)
            temp |= GuiTrigger::BUTTON_R;
        if(buttons & WPAD_CLASSIC_BUTTON_L)
            temp |= GuiTrigger::BUTTON_L;
        
        return temp;
    }
    
    bool update(int width, int height)
    {
        lastData = data;
        u32 controller_type;
        
        //! check if the controller is connected
        if(WPADProbe(chanIdx-1, &controller_type) != 0)
            return false;
        
        KPADRead(chanIdx-1, &kpadData, 1);
        
        if(kpadData.device_type <= 1)
        {
            if(kpadData.btns_h & WPAD_BUTTON_LEFT)
            {
                if(data.x > -(width / 2)) data.x -= 10;
            }
            if(kpadData.btns_h & WPAD_BUTTON_RIGHT)
            {
                if(data.x < (width / 2)) data.x += 10;
            }
            if(kpadData.btns_h & WPAD_BUTTON_UP)
            {
                if(data.y < (height / 2)) data.y += 10;
            }
            if(kpadData.btns_h & WPAD_BUTTON_DOWN)
            {
                if(data.y > -(height / 2)) data.y -= 10;
            }
            
            data.buttons_r = remapWiiMoteButtons(kpadData.btns_r);
            data.buttons_h = remapWiiMoteButtons(kpadData.btns_h);
            data.buttons_d = remapWiiMoteButtons(kpadData.btns_d);
        }
        else
        {
            if(kpadData.classic.btns_h & WPAD_CLASSIC_BUTTON_LEFT)
            {
                if(data.x > -(width / 2)) data.x -= 10;
            }
            if(kpadData.classic.btns_h & WPAD_CLASSIC_BUTTON_RIGHT)
            {
                if(data.x < (width / 2)) data.x += 10;
            }
            if(kpadData.classic.btns_h & WPAD_CLASSIC_BUTTON_UP)
            {
                if(data.y < (height / 2)) data.y += 10;
            }
            if(kpadData.classic.btns_h & WPAD_CLASSIC_BUTTON_DOWN)
            {
                if(data.y > -(height / 2)) data.y -= 10;
            }
            
            data.buttons_r = remapClassicButtons(kpadData.classic.btns_r);
            data.buttons_h = remapClassicButtons(kpadData.classic.btns_h);
            data.buttons_d = remapClassicButtons(kpadData.classic.btns_d);
        }
        
        return true;
    }
    
private:
    KPADData kpadData;
};

#endif
