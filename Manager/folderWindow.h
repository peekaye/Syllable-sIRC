class folderWindow : public Window
{
 public:
 	folderWindow(char *path, appConfig *inputConfig);
 	void FrameSized(Point &cDelta);
 	bool OkToQuit();
 	virtual void HandleMessage(Message *pcMessage);
 	void DirectoryUp();
 	char windowPath[MAX_PATH_SIZE];
 	_appConfig *mainConfig;
 private:
 	Menu *mainMenu, *tempSubMenu;
	Button *parentButton;
	StringView *locationLabel;
	TextView *locationBar;
	FilesView *mainView;
	DirectoryView *mainView2;
	StringView *infoLabel;
};





