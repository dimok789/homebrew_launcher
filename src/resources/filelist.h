/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes 13 files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

typedef struct _RecourceFile
{
	const char          *filename;
	const unsigned char *DefaultFile;
	const unsigned int  &DefaultFileSize;
	unsigned char	    *CustomFile;
	unsigned int        CustomFileSize;
} RecourceFile;

static RecourceFile RecourceList[] =
{
	{"bgMusic.ogg", NULL, 0, NULL, 0},
	{"button.png", NULL, 0, NULL, 0},
	{"button_click.mp3", NULL, 0, NULL, 0},
	{"font.ttf", NULL, 0, NULL, 0},
	{"homebrewButton.png", NULL, 0, NULL, 0},
	{"launchMenuBox.png", NULL, 0, NULL, 0},
	{"leftArrow.png", NULL, 0, NULL, 0},
	{"player1_point.png", NULL, 0, NULL, 0},
	{"player2_point.png", NULL, 0, NULL, 0},
	{"player3_point.png", NULL, 0, NULL, 0},
	{"player4_point.png", NULL, 0, NULL, 0},
	{"progressWindow.png", NULL, 0, NULL, 0},
	{"rightArrow.png", NULL, 0, NULL, 0},
	{NULL, NULL, 0, NULL, 0}
};

#endif
