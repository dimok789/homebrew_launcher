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
#include "ProgressWindow.h"
#include "video/CVideo.h"

ProgressWindow::ProgressWindow(const std::string & title)
    : GuiFrame(0, 0)
    , bgImageData(Resources::GetImageData("progressWindow.png"))
    , bgImage(bgImageData)
    , progressImageBlack(bgImage.getWidth(), bgImage.getHeight()/2, (GX2Color){0, 0, 0, 255})
    , progressImageColored(bgImage.getWidth(), bgImage.getHeight()/2, (GX2Color){0, 0, 0, 255})
{
    titleChanged = false;
    currentTitle = title;
    width = bgImage.getWidth();
    height = bgImage.getHeight();

    append(&progressImageBlack);
    append(&progressImageColored);
    append(&bgImage);

	ColourData = new ColorXML("fs:/vol/content/color.xml");
	Color3 = ColourData->GetColor3();
    progressImageColored.setAlignment(ALIGN_TOP_LEFT);
    progressImageColored.setImageColor((GX2Color){Color3.R,Color3.G,Color3.B,Color3.A}, 0);
    progressImageColored.setImageColor((GX2Color){Color3.R,Color3.G,Color3.B,Color3.A}, 1);
	Color4 = ColourData->GetColor4();
    progressImageColored.setImageColor((GX2Color){Color4.R,Color4.G,Color4.B,Color4.A}, 2);
    progressImageColored.setImageColor((GX2Color){Color4.R,Color4.G,Color4.B,Color4.A}, 3);

    titleText.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    titleText.setFontSize(36);
    titleText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    titleText.setPosition(50, 0);
    titleText.setBlurGlowColor(5.0f, glm::vec4(0.0, 0.0, 0.0f, 1.0f));
    titleText.setText(currentTitle.c_str());
    append(&titleText);

    progressImageColored.setParent(&progressImageBlack);
    titleText.setParent(&progressImageBlack);

    setProgress(0.0f);
}

ProgressWindow::~ProgressWindow()
{
    Resources::RemoveImageData(bgImageData);
}

void ProgressWindow::setTitle(const std::string & title)
{
    titleMutex.lock();
    currentTitle = title;
    titleChanged = true;
    titleMutex.unlock();
}

void ProgressWindow::setProgress(f32 percent)
{
    progressImageColored.setSize(percent * 0.01f * progressImageBlack.getWidth(), progressImageColored.getHeight());
}

void ProgressWindow::draw(CVideo * v)
{
    if(titleChanged)
    {
        titleMutex.lock();
        titleChanged = false;
        titleText.setText(currentTitle.c_str());
        titleMutex.unlock();
    }

    GuiFrame::draw(v);
}
