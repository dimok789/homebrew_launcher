/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef GUI_CONTROLLER_H_
#define GUI_CONTROLLER_H_

#include <string.h>
#include "GuiTrigger.h"

class GuiController
{
public:
    //!Constructor
    GuiController(int channel)
        : chan(channel)
    {
        memset(&lastData, 0, sizeof(lastData));
        memset(&data, 0, sizeof(data));

        switch(chan)
        {
        default:
        case GuiTrigger::CHANNEL_1:
            chanIdx = 0;
            break;
        case GuiTrigger::CHANNEL_2:
            chanIdx = 1;
            break;
        case GuiTrigger::CHANNEL_3:
            chanIdx = 2;
            break;
        case GuiTrigger::CHANNEL_4:
            chanIdx = 3;
            break;
        case GuiTrigger::CHANNEL_5:
            chanIdx = 4;
            break;
        }
        isDPadMode = false;
        showPointer = false;
    }

    //!Destructor
    virtual ~GuiController()  {}

    virtual bool update(int width, int height) = 0;

    typedef struct
    {
        unsigned int buttons_h;
        unsigned int buttons_d;
        unsigned int buttons_r;
        bool validPointer;
        bool touched;
        float pointerAngle;
        int x;
        int y;
    } PadData;

    int chan;
    int chanIdx;
    PadData data;
    PadData lastData;
    bool isDPadMode;
    bool showPointer;

};

#endif
