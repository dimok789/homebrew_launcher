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
#include <coreinit/core.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include "Application.h"
#include "common/common.h"
#include "gui/FreeTypeGX.h"
#include "gui/VPadController.h"
#include "gui/WPadController.h"
#include "gui/DVPadController.h"
#include "gui/DWPadController.h"
#include "resources/Resources.h"
#include "sounds/SoundHandler.hpp"
#include "system/memory.h"
#include "utils/logger.h"

Application *Application::applicationInstance = NULL;
bool Application::exitApplication = false;
bool Application::quitRequest = false;

Application::Application()
	: CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x20000)
	, bgMusic(NULL)
	, video(NULL)
    , mainWindow(NULL)
    , fontSystem(NULL)
    , exitCode(EXIT_RELAUNCH_ON_LOAD)
{
    controller[0] = new VPadController(GuiTrigger::CHANNEL_1);
    controller[1] = new WPadController(GuiTrigger::CHANNEL_2);
    controller[2] = new WPadController(GuiTrigger::CHANNEL_3);
    controller[3] = new WPadController(GuiTrigger::CHANNEL_4);
    controller[4] = new WPadController(GuiTrigger::CHANNEL_5);

    //! load resources
    Resources::LoadFiles("fs:/wiiu/apps/homebrew_launcher/resources");

    //! create bgMusic
    bgMusic = new GuiSound(Resources::GetFile("bgMusic.ogg"), Resources::GetFileSize("bgMusic.ogg"));
    bgMusic->SetLoop(true);
    bgMusic->Play();
    bgMusic->SetVolume(50);

	exitApplication = false;

    ProcUIInit(OSSavesDone_ReadyToRelease);
}

Application::~Application()
{
    log_printf("Destroy music\n");

    delete bgMusic;

    log_printf("Destroy controller\n");

    for(int i = 0; i < 5; i++)
        delete controller[i];

    log_printf("Destroy async deleter\n");
	AsyncDeleter::destroyInstance();

    log_printf("Clear resources\n");
    Resources::Clear();

    log_printf("Stop sound handler\n");
	SoundHandler::DestroyInstance();

    ProcUIShutdown();

    if(quitRequest)
    {
        SYSRelaunchTitle(0, 0);
    }
}

int Application::exec()
{
    //! start main GX2 thread
    resumeThread();
    //! now wait for thread to finish
	shutdownThread();

	return exitCode;
}

void Application::quit(int code)
{
    exitCode = code;
    exitApplication = true;
    quitRequest = true;
}

void Application::fadeOut()
{
    GuiImage fadeOut(video->getTvWidth(), video->getTvHeight(), (GX2Color){ 0, 0, 0, 255 });

	for(int i = 0; i < 255; i += 10)
    {
        if(i > 255)
            i = 255;

        fadeOut.setAlpha(i / 255.0f);

        //! start rendering DRC
	    video->prepareDrcRendering();
	    mainWindow->drawDrc(video);

        GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_ALWAYS);
        fadeOut.draw(video);
        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);

	    video->drcDrawDone();

        //! start rendering TV
	    video->prepareTvRendering();

	    mainWindow->drawTv(video);

        GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_ALWAYS);
        fadeOut.draw(video);
        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);

	    video->tvDrawDone();

	    //! as last point update the effects as it can drop elements
	    mainWindow->updateEffects();

	    video->waitForVSync();
    }

    //! one last cleared black screen
    video->prepareDrcRendering();
    video->drcDrawDone();
    video->prepareTvRendering();
    video->tvDrawDone();
    video->waitForVSync();
    video->tvEnable(false);
    video->drcEnable(false);
}

bool Application::procUI(void)
{
    bool executeProcess = false;

    switch(ProcUIProcessMessages(true))
    {
    case PROCUI_STATUS_EXITING:
    {
        log_printf("PROCUI_STATUS_EXITING\n");
        exitCode = EXIT_SUCCESS;
        exitApplication = true;
        break;
    }
    case PROCUI_STATUS_RELEASE_FOREGROUND:
    {
        log_printf("PROCUI_STATUS_RELEASE_FOREGROUND\n");
        if(video != NULL)
        {
            // we can turn of the screen but we don't need to and it will display the last image
            video->tvEnable(true);
            video->drcEnable(true);

            log_printf("delete fontSystem\n");
            delete fontSystem;
            fontSystem = NULL;

            log_printf("delete video\n");
            delete video;
            video = NULL;

            log_printf("deinitialze memory\n");
            memoryRelease();
            ProcUIDrawDoneRelease();
        }
        else
        {
            ProcUIDrawDoneRelease();
        }
        break;
    }
    case PROCUI_STATUS_IN_FOREGROUND:
    {
        if(!quitRequest)
        {
            if(video == NULL)
            {
                log_printf("PROCUI_STATUS_IN_FOREGROUND\n");
                log_printf("initialze memory\n");
                memoryInitialize();

                log_printf("Initialize video\n");
                video = new CVideo(GX2_TV_SCAN_MODE_720P, GX2_DRC_RENDER_MODE_SINGLE);
                log_printf("Video size %i x %i\n", video->getTvWidth(), video->getTvHeight());

                //! setup default Font
                log_printf("Initialize main font system\n");
                FreeTypeGX *fontSystem = new FreeTypeGX(Resources::GetFile("font.ttf"), Resources::GetFileSize("font.ttf"), true);
                GuiText::setPresetFont(fontSystem);

                if(mainWindow == NULL)
                {
                    log_printf("Initialize main window\n");
                    mainWindow = new MainWindow(video->getTvWidth(), video->getTvHeight());
                }

            }
            executeProcess = true;
        }
        break;
    }
    case PROCUI_STATUS_IN_BACKGROUND:
    default:
        break;
    }

    return executeProcess;
}


void Application::executeThread(void)
{
    log_printf("Entering main loop\n");

    //! main GX2 loop (60 Hz cycle with max priority on core 1)
	while(!exitApplication)
	{
	    if(procUI() == false)
	        continue;

	    //! Read out inputs
	    for(int i = 0; i < 5; i++)
        {
            if(controller[i] == NULL) continue;
            
            if(controller[i]->update(video->getTvWidth(), video->getTvHeight()) == false)
                continue;

            //! update controller states
            mainWindow->update(controller[i]);
            
            //If the + button on the GamePad is pressed, switch to DPAD mode and vice versa.
            if((i == 0) && (controller[i]->data.buttons_d & GuiTrigger::BUTTON_PLUS))
            {
                if(controller[i]->isDPadMode)
                {
                    delete controller[i];
                    controller[i] = new VPadController(GuiTrigger::CHANNEL_1);
                }
                else
                {
                    delete controller[i];
                    controller[i] = new DVPadController(GuiTrigger::CHANNEL_1);
                }
            }
            
            //If the + button on any other controller is pressed, switch to DPAD mode and vice versa.
            else if(controller[i]->data.buttons_d & GuiTrigger::BUTTON_PLUS)
            {
                if(controller[i]->isDPadMode)
                {
                    delete controller[i];
                    switch(i)
                    {
                        case 1:
                            controller[i] = new WPadController(GuiTrigger::CHANNEL_2);
                            break;
                        case 2:
                            controller[i] = new WPadController(GuiTrigger::CHANNEL_3);
                            break;
                        case 3:
                            controller[i] = new WPadController(GuiTrigger::CHANNEL_4);
                            break;
                        case 4:
                            controller[i] = new WPadController(GuiTrigger::CHANNEL_5);
                            break;
                    }
                }
                else
                {
                    delete controller[i];
                    switch(i)
                    {
                        case 1:
                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_2);
                            break;
                        case 2:
                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_3);
                            break;
                        case 3:
                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_4);
                            break;
                        case 4:
                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_5);
                            break;
                    }
                }
            }
        }

        //! start rendering DRC
	    video->prepareDrcRendering();
	    mainWindow->drawDrc(video);
	    video->drcDrawDone();

        //! start rendering TV
	    video->prepareTvRendering();
	    mainWindow->drawTv(video);
	    video->tvDrawDone();

        //! enable screen after first frame render
	    if(video->getFrameCount() == 0) {
            video->tvEnable(true);
            video->drcEnable(true);
	    }

	    //! as last point update the effects as it can drop elements
	    mainWindow->updateEffects();

	    video->waitForVSync();

        //! transfer elements to real delete list here after all processes are finished
        //! the elements are transfered to another list to delete the elements in a separate thread
        //! and avoid blocking the GUI thread
        AsyncDeleter::triggerDeleteProcess();
	}

    //! in case we exit to a homebrew let's smoothly fade out
    if(video)
    {
        fadeOut();
    }

    log_printf("delete mainWindow\n");
    delete mainWindow;
    mainWindow = NULL;

    log_printf("delete fontSystem\n");
    delete fontSystem;
    fontSystem = NULL;

    log_printf("delete video\n");
    delete video;
    video = NULL;

    log_printf("deinitialze memory\n");
    memoryRelease();
}
