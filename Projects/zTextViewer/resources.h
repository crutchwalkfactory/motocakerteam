#ifndef __Z6_TEXTVIEWER_STRINGTABLE_H__

//////////////////////////////////////////////////////////////////////////
// Motivation
// Currently I don`t know how to use resource files here and this is sad,
// but if you want to localize this application, you should make two steps:
// 1) translate string table
// 2) recompile project
//
// If you know how to manage stringtables in better way contact me please:
// mail/gtalk: craz87@gmail.com
// icq: 978140

#include "BuildConfig.h"
#include "ZConfig.h"

//non localized strings
#define APP_NAME "zTextViewer"
#define APP_ABOUT_TEXT  "zTextViewer ver. 0.3\nby BeZ\n   e-mail: bezols@gmail.com\n\n   cr_az\n   e-mail:craz87@gmail.com"
#define APP_CFG "settings.cfg"

#define Z6KEY_LEFT              0x1012
#define Z6KEY_UP                0x1013
#define Z6KEY_RIGHT             0x1014
#define Z6KEY_DOWN              0x1015
#define Z6KEY_CENTER            0x1004

#define Z6KEY_SIDE_SELECT       0x1005

#define Z6KEY_SIDE_UP           0x1016
#define Z6KEY_SIDE_DOWN         0x1017

#define Z6KEY_GREEN             0x1030
#define Z6KEY_RED               0x1031
#define Z6KEY_C                 0x1032
#define Z6KEY_LEFT_SOFTKEY      0x1038
#define Z6KEY_RIGHT_SOFTKEY     0x103a
#define Z6KEY_MUSIC             0x1048


//binary section
#define IDB_IMAGE_ABOUTAPP     "zTextViewer.png"
#define IDB_IMAGE_TEXTSIZE     "img/textsize.png"
#define IDB_IMAGE_LANGUAGE     "img/language.png"

#define IDB_IMAGE_FILE         "img/file.png"
#define IDB_IMAGE_FOLDER       "img/folder.png"
#define IDB_IMAGE_BACK         "img/back.png"
#define IDB_IMAGE_EXIT         "img/exit.png"
#define IDB_IMAGE_ABOUT        "img/about.png"
#define IDB_IMAGE_OPTIONS      "img/options.png"
#define IDB_IMAGE_SAVE         "img/save.png"
#define IDB_IMAGE_SAVEAS       "img/saveas.png"
#define IDB_IMAGE_OPEN         "img/open.png"
#define IDB_IMAGE_NEW          "img/new.png"
#define IDB_IMAGE_ARROW_UP     "img/up.png"
#define IDB_IMAGE_ARROW_DOWN   "img/down.png"
#define IDB_IMAGE_HOME         "img/home.png"
#define IDB_IMAGE_END          "img/end.png"
#define IDB_IMAGE_SEARCH       "img/search.png"

#define IDB_IMAGE_EDIT          "img/edit.png"
#define IDB_IMAGE_NAVIGATE      "img/navigate.png"

#define IDB_IMAGE_COPY          "img/copy.png"
#define IDB_IMAGE_CUT           "img/cut.png"
#define IDB_IMAGE_PASTE         "img/paste.png"
#define IDB_IMAGE_SELALL        "img/select_all.png"
#define IDB_IMAGE_BLOCKBEGIN    "img/block_begin.png"
#define IDB_IMAGE_BLOCKEND      "img/block_end.png"

#endif


