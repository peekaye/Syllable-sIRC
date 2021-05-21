#define MAX_PATH_SIZE 4096



#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/menu.h>
#include <gui/button.h>
#include <gui/textview.h>
#include <gui/stringview.h>
#include <gui/directoryview.h>
#include <gui/requesters.h>
#include <util/application.h>
#include <util/message.h>


#include <stdio.h>


using namespace os;


#include "mimeTable.h"

#define appConfig struct _appConfig

struct _appConfig
	{
	mimeTable *mainMimeTable;
	};

#include "filesView.h"

#include "folderWindow.h"



