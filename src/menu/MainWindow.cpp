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
#include "MainWindow.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "Application.h"
#include "utils/StringTools.h"
#include "utils/logger.h"


MainWindow::MainWindow(int w, int h)
    : width(w)
    , height(h)
    , bgImageColor(w, h, (GX2Color){ 0, 0, 0, 0 })
    , bgParticleImg(w, h, 500)
    , homebrewWindow(w, h)
{
    bgImageColor.setImageColor((GX2Color){  79, 153, 239, 255 }, 0);
    bgImageColor.setImageColor((GX2Color){  79, 153, 239, 255 }, 1);
    bgImageColor.setImageColor((GX2Color){  59, 159, 223, 255 }, 2);
    bgImageColor.setImageColor((GX2Color){  59, 159, 223, 255 }, 3);
    append(&bgImageColor);
    append(&bgParticleImg);

    for(int i = 0; i < 5; i++)
    {
        std::string filename = strfmt("player%i_point.png", i);
        pointerImgData[i] = Resources::GetImageData(filename.c_str());
        pointerImg[i] = new GuiImage(pointerImgData[i]);
        pointerImg[i]->setScale(1.5f);
        pointerValid[i] = false;
    }

    append(&homebrewWindow);
}

MainWindow::~MainWindow()
{
    remove(&homebrewWindow);
    remove(&bgImageColor);
    remove(&bgParticleImg);

    while(!tvElements.empty())
    {
        delete tvElements[0];
        remove(tvElements[0]);
    }
    while(!drcElements.empty())
    {
        delete drcElements[0];
        remove(drcElements[0]);
    }
    for(int i = 0; i < 5; i++)
    {
        delete pointerImg[i];
        Resources::RemoveImageData(pointerImgData[i]);
    }
}

void MainWindow::updateEffects()
{
    //! dont read behind the initial elements in case one was added
    u32 tvSize = tvElements.size();
    u32 drcSize = drcElements.size();

    for(u32 i = 0; (i < drcSize) && (i < drcElements.size()); ++i)
    {
        drcElements[i]->updateEffects();
    }

    //! only update TV elements that are not updated yet because they are on DRC
    for(u32 i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
    {
        u32 n;
        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++)
        {
            if(tvElements[i] == drcElements[n])
                break;
        }
        if(n == drcElements.size())
        {
            tvElements[i]->updateEffects();
        }
    }
}

void MainWindow::update(GuiController *controller)
{
    //! dont read behind the initial elements in case one was added
    //u32 tvSize = tvElements.size();

    if(controller->chan & GuiTrigger::CHANNEL_1)
    {
        u32 drcSize = drcElements.size();

        for(u32 i = 0; (i < drcSize) && (i < drcElements.size()); ++i)
        {
            drcElements[i]->update(controller);
        }
    }
    else
    {
        u32 tvSize = tvElements.size();

        for(u32 i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
        {
            tvElements[i]->update(controller);
        }
    }

//    //! only update TV elements that are not updated yet because they are on DRC
//    for(u32 i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
//    {
//        u32 n;
//        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++)
//        {
//            if(tvElements[i] == drcElements[n])
//                break;
//        }
//        if(n == drcElements.size())
//        {
//            tvElements[i]->update(controller);
//        }
//    }

    if(controller->showPointer && controller->data.validPointer)
    {
        int wpadIdx = controller->chanIdx;
        f32 posX = controller->data.x;
        f32 posY = controller->data.y;
        pointerImg[wpadIdx]->setPosition(posX, posY);
        pointerImg[wpadIdx]->setAngle(controller->data.pointerAngle);
        pointerValid[wpadIdx] = true;
    }
    
    if(!controller->showPointer)
    {
        pointerValid[controller->chanIdx] = false;
    }
}

void MainWindow::drawDrc(CVideo *video)
{
    for(u32 i = 0; i < drcElements.size(); ++i)
    {
        drcElements[i]->draw(video);
    }

    if(pointerValid[0])
    {
        pointerImg[0]->setAlpha(1.0f);
        pointerImg[0]->draw(video);
        pointerImg[0]->setAlpha(1.0f);
    }
    
    for(int i = 1; i < 5; i++)
    {
        if(pointerValid[i])
        {
            pointerImg[i]->setAlpha(0.5f);
            pointerImg[i]->draw(video);
            pointerImg[i]->setAlpha(1.0f);
        }
    }
}

void MainWindow::drawTv(CVideo *video)
{
    for(u32 i = 0; i < tvElements.size(); ++i)
    {
        tvElements[i]->draw(video);
    }

    if(pointerValid[0])
    {
        pointerImg[0]->setAlpha(0.5f);
        pointerImg[0]->draw(video);
        pointerImg[0]->setAlpha(1.0f);
    }
    
    for(int i = 1; i < 5; i++)
    {
        if(pointerValid[i])
        {
            pointerImg[i]->draw(video);
            pointerValid[i] = false;
        }
    }
}
