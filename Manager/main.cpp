#include "main.h"

/*
class FilesView : public View
{
 public:
   FilesView(const Rect &r);
   void MouseDown(const Point &Position, uint32 nButtons);
};

FilesView::FilesView(const Rect &r) : View(r, "TestView",CF_FOLLOW_LEFT|CF_FOLLOW_TOP,WID_WILL_DRAW|WID_CLEAR_BACKGROUND)
{

}

void FilesView::MouseDown(const Point &Position, uint32 nButtons)
{
	exit(0);
}

*/

class ManagerApplication : public Application
{
 public:
	ManagerApplication(void); /*constructor*/
	appConfig *mainConfig;
	private:
	folderWindow *win;
};


ManagerApplication::ManagerApplication(void) : Application("application/x-vnd.Arbogad-Manager")
{
 char typesFile[PATH_MAX];
 char *userHome = getenv("HOME");
 
 mainConfig = (appConfig *)malloc(sizeof(appConfig));
 
 strcpy(typesFile, userHome);
 strcat(typesFile, "/config/Manager/types.conf");
 mainConfig->mainMimeTable = new mimeTable(typesFile);

win = new folderWindow(Rect(20,100,500,400),"/home/root",mainConfig);
 win->Show();
}


void main(void)
{
	ManagerApplication *theManager;

	theManager = new ManagerApplication();
	theManager->Run();
	return 0;
}








