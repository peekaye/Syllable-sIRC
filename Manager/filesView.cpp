#include "main.h"

FilesView::FilesView(Rect r,char *defaultPath) : View(r,"fView",CF_FOLLOW_LEFT|CF_FOLLOW_TOP,0)
{
	SetBgColor(255,255,255,255);
	SetFgColor(0,0,0,255);
	DrawString(defaultPath);
}

void FilesView::MouseDown(const Point &cPosition, uint32 nButtons)
{
	printf("Coucou\n");
	MovePenTo(10,10);
	SetBgColor(200,200,200,255);
	SetFgColor(0,0,0,255);
	DrawString("Ah que coucou!");
	Flush();

}
