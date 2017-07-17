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
#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <vector>
#include <queue>
#include "gui/Gui.h"
#include "HomebrewWindow.h"
#include "gui/GuiParticleImage.h"

class CVideo;

class MainWindow : public sigslot::has_slots<>
{
public:
    MainWindow(int w, int h);
    virtual ~MainWindow();

    void appendTv(GuiElement *e)
    {
        if(!e)
            return;

        removeTv(e);
        tvElements.push_back(e);
    }
    void appendDrc(GuiElement *e)
    {
        if(!e)
            return;

        removeDrc(e);
        drcElements.push_back(e);
    }

    void append(GuiElement *e)
    {
        appendTv(e);
        appendDrc(e);
    }

    void insertTv(u32 pos, GuiElement *e)
    {
        if(!e)
            return;

        removeTv(e);
        tvElements.insert(tvElements.begin() + pos, e);
    }
    void insertDrc(u32 pos, GuiElement *e)
    {
        if(!e)
            return;

        removeDrc(e);
        drcElements.insert(drcElements.begin() + pos, e);
    }

    void insert(u32 pos, GuiElement *e)
    {
        insertTv(pos, e);
        insertDrc(pos, e);
    }

    void removeTv(GuiElement *e)
    {
        for(u32 i = 0; i < tvElements.size(); ++i)
        {
            if(e == tvElements[i])
            {
                tvElements.erase(tvElements.begin() + i);
                break;
            }
        }
    }
    void removeDrc(GuiElement *e)
    {
        for(u32 i = 0; i < drcElements.size(); ++i)
        {
            if(e == drcElements[i])
            {
                drcElements.erase(drcElements.begin() + i);
                break;
            }
        }
    }

    void remove(GuiElement *e)
    {
        removeTv(e);
        removeDrc(e);
    }
    void removeAll()
    {
        tvElements.clear();
        drcElements.clear();
    }

    void drawDrc(CVideo *video);
    void drawTv(CVideo *video);
    void update(GuiController *controller);
    void updateEffects();
private:
    int width, height;
    std::vector<GuiElement *> drcElements;
    std::vector<GuiElement *> tvElements;

    GuiImage bgImageColor;
    GuiParticleImage bgParticleImg;
    HomebrewWindow homebrewWindow;

    GuiImageData *pointerImgData[5];
    GuiImage *pointerImg[5];
    bool pointerValid[5];
};

#endif //_MAIN_WINDOW_H_
