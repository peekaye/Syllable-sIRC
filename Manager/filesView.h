class FilesView : public View
{
	public :
	FilesView(Rect r,char *defaultPath);
	void MouseDown(const Point &cPosition, uint32 nButtons);
	char *path;
};

