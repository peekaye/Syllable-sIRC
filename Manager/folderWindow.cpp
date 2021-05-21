#include "main.h"
#include <unistd.h>

enum
{
	ID_MENU_FILE_CLOSE,
	ID_MENU_FILE_EXIT,
	ID_MENU_EDIT_CPFILE,
	ID_MENU_EDIT_MVFILE,
	ID_MENU_EDIT_DELETE,
	ID_MENU_EDIT_CUT,
	ID_MENU_EDIT_COPY,
	ID_MENU_EDIT_PASTE,
	ID_MENU_EDIT_OPTIONS,
	ID_MENU_HELP_HELP,
	ID_MENU_HELP_ABOUT,
	
	
	ID_BUTTON_UP,
	ID_SELECTION_CHANGE,
	ID_DIRECTORY_CHANGE,
	ID_DIRECTORY_INVOKED
};


/****************************************************/
/* ltostr is a temporary function to convert a long */
/* to the corresponding string.                     */
/****************************************************/

void ltostr(long long source, char *dest)
{
	char temp[32];
	char *cursor = temp;
	int i = 3;
	if(source == 0)
	{
		strcpy(dest, "0");
		return;
	}
	while(source)
	{
		*cursor = '0' + (source % 10);
		cursor++;
		source /= 10;
		i++;
		if(!(i % 3))
			{
				*cursor = ' ';
				cursor++;
			}


	}
	cursor--;	
	while(cursor >= temp)
	{
		*dest = *cursor;
		cursor--;
		dest++;
	}
	*dest = 0;
}



folderWindow::folderWindow(const Rect &r, char *path, appConfig *inputConfig) : Window(r,"folderWindow",path,0,CURRENT_DESKTOP)
{
	Rect menuRect = GetBounds();
	Rect r;
	
	strcpy(windowPath, path);
	mainConfig = inputConfig;
	
	/* Creating the main menu for a folder window
	*/
	
	menuRect.bottom = 18;
	mainMenu = new Menu(menuRect, "mainMenu", ITEMS_IN_ROW);

	// File menu
	tempSubMenu = new Menu(Rect(0,0,0,0),"File", ITEMS_IN_COLUMN);

	/*tempSubMenu->AddItem("Close", new Message(ID_MENU_FILE_CLOSE));	*/
	tempSubMenu->AddItem("Exit", new Message(ID_MENU_FILE_EXIT));	
	
	mainMenu->AddItem(tempSubMenu);

	// Edit menu
	/*tempSubMenu = new Menu(Rect(0,0,0,0),"Edit", ITEMS_IN_COLUMN);

	tempSubMenu->AddItem("Copy file ...",new Message(ID_MENU_EDIT_CPFILE));
	tempSubMenu->AddItem("Move file ...",new Message(ID_MENU_EDIT_MVFILE));
	tempSubMenu->AddItem("Delete", new Message(ID_MENU_EDIT_DELETE));
	tempSubMenu->AddItem(new MenuSeparator());	
	tempSubMenu->AddItem("Cut", new Message(ID_MENU_EDIT_CUT));
	tempSubMenu->AddItem("Copy", new Message(ID_MENU_EDIT_COPY));
	tempSubMenu->AddItem("Paste", new Message(ID_MENU_EDIT_PASTE));
	tempSubMenu->AddItem(new MenuSeparator());	
	tempSubMenu->AddItem("Options", new Message(ID_MENU_EDIT_OPTIONS));
	
	mainMenu->AddItem(tempSubMenu);*/

	// Help menu
	tempSubMenu = new Menu(Rect(0,0,0,0),"Help", ITEMS_IN_COLUMN);

	/*tempSubMenu->AddItem("Help", new Message(ID_MENU_HELP_HELP));	*/
	tempSubMenu->AddItem("About ...", new Message(ID_MENU_HELP_ABOUT));	
	
	mainMenu->AddItem(tempSubMenu);
		
	mainMenu->SetTargetForItems(this);
	AddChild(mainMenu);
	
	/* Creating the buttons
	*/
	parentButton = new Button(Rect(5,21,25,41),"pButton","Up",new Message(ID_BUTTON_UP),CF_FOLLOW_LEFT|CF_FOLLOW_TOP, 0);
	AddChild(parentButton);
	
	locationLabel = new StringView(Rect(30,25,80,41),"lLabel","Location :", ALIGN_LEFT, CF_FOLLOW_LEFT|CF_FOLLOW_TOP, WID_WILL_DRAW|WID_FULL_UPDATE_ON_RESIZE);
	AddChild(locationLabel);
	
	r = GetBounds();
	r.left = 85;
	r.top = 21;
	r.right -= 5;
	r.bottom = 41;
	locationBar = new TextView(r,"lLabel",windowPath, CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|CF_FOLLOW_TOP, 0);
	AddChild(locationBar);
	
	/* The information label
	*/
	
	r = GetBounds();
	r.left = 5;
	r.right -= 1;
	r.bottom -= 1;
	r.top = r.bottom - 12;

	infoLabel = new StringView(r,"iLabel","", ALIGN_LEFT, CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM, WID_WILL_DRAW|WID_FULL_UPDATE_ON_RESIZE);
	AddChild(infoLabel);
	/* The main view
	*/

	r = GetBounds();
	r.top = 41;
	r.bottom -=18;
	
	//mainView = new FilesView(r,"/home/root");
	mainView2 = new DirectoryView(r,windowPath,1,CF_FOLLOW_ALL,0);
	mainView2->SetDirChangeMsg(new Message(ID_DIRECTORY_CHANGE));
    mainView2->SetInvokeMsg( new Message(ID_DIRECTORY_INVOKED) );
    mainView2->SetSelChangeMsg( new Message(ID_SELECTION_CHANGE) );
	AddChild(mainView2);

	

   /*view = new FilesView(GetBounds());
   AddChild(view);*/
}

bool folderWindow::OkToQuit(void)
{
   Application::GetInstance()->PostMessage(M_QUIT);
   return(true);
}

void folderWindow::HandleMessage(Message *pcMessage)
{
	int i,j;
	char tempFileName[4096];
	char tempAction[4096];
	char finalAction[4096];
	char *cursorFinalAction;
	switch( pcMessage->GetCode() )
	{
	case ID_MENU_FILE_EXIT:
	{
		exit(0);
	}
	
	break;
	case ID_MENU_HELP_ABOUT:
	{
		Alert *aboutBox = new Alert("Manager v0.0.1", "Manager v0.0.1 :\n\nFile management utility for AtheOS\nWritten by Stephane Aubry (arbogad@free.fr)\n\nhttp://arbogad.free.fr/atheos/\n\nThis program is completely free (GPL or someting like that),\nshare it at will !\n", 0x00, "Close", NULL);
		aboutBox->Go(new Invoker());
	}
	break;
	case ID_BUTTON_UP:
	{
		DirectoryUp();
		mainView2->SetPath(windowPath);
		mainView2->Clear();
		mainView2->ReRead();
		locationBar->Set(windowPath,true);
		SetTitle(windowPath);
	}
	break;
	case ID_DIRECTORY_CHANGE:
	{
		infoLabel->SetString("");
		strcpy(windowPath,(mainView2->GetPath()).c_str());
		locationBar->Set(windowPath,true);
		SetTitle(windowPath);
	}		
	break;
	case ID_DIRECTORY_INVOKED:
	{
		struct ::stat tempFileStat;
		for(i = mainView2->GetFirstSelected(); i <= mainView2->GetLastSelected(); i++)
		{
			if(mainView2->IsSelected(i))
			{
				
				strcpy(tempFileName,mainView2->GetFile(i)->GetName().c_str());
				strcpy(tempAction, (mainConfig->mainMimeTable->getType(tempFileName))->action);

				if(strcmp(tempAction, " "))
				{
					// The file has a predifined action
					
					j = 0;
					cursorFinalAction = finalAction;
					while(tempAction[j])
					{
						if(tempAction[j] == '%' && tempAction[j+1] == 'f')
						{
							strcpy(cursorFinalAction,tempFileName);
							cursorFinalAction += strlen(tempFileName);
							j++;
						}
						else
						{
							*cursorFinalAction = tempAction[j];
							cursorFinalAction++;
						}
					j++;
					}	
					strcpy(cursorFinalAction," &");
					chdir(windowPath);
					system(finalAction);
				}
				else
				{
					// The file has no predefined action
					tempFileStat = mainView2->GetFile(i)->GetFileStat();
					if(tempFileStat.st_mode & 0100)
					{
						chdir(windowPath);
						strcat(tempFileName, " &");
						system(tempFileName);
					}
				}
			}
		}		
		
	}		
	break;
	case ID_SELECTION_CHANGE:
	{
		int firstFileNumber = mainView2->GetFirstSelected();
		char tempInfo[1024];
		char numBuffer[33];
		struct ::stat tempFileStat;
		long long numberOfSelection, sizeOfSelection;
		if(firstFileNumber == mainView2->GetLastSelected())
			{	
			if(firstFileNumber != -1)			
			{
				// One item selected
				tempFileStat = mainView2->GetFile(firstFileNumber)->GetFileStat();
				if(tempFileStat.st_mode & 0100000)
				{
					
					// Regular file selected
					strcpy(tempFileName, mainView2->GetFile(firstFileNumber)->GetName().c_str());
					strcpy(tempInfo, "Type : ");
						
					if((tempFileStat.st_mode & 0100) && !strcmp(" ",(mainConfig->mainMimeTable->getType(tempFileName))->action))
					{
						// Executable file selected
						strcpy(tempInfo, "Type : Executable file");
					}
					else
					{
						// Non executable selected
						strcat(tempInfo,(mainConfig->mainMimeTable->getType(tempFileName))->name);
					}
				}
				else
				{
					// Directory selected
					strcpy(tempInfo, "Type : Directory");
				}
			}
			else
			{
				// No item selected
				strcpy(tempInfo, "");
			}
				

			// printf("%o\n", tempFileStat.st_mode);
			
			}
			else
			{
				numberOfSelection = 0;
				sizeOfSelection = 0;
				// Multiple items selected
				for(i = firstFileNumber; i <= mainView2->GetLastSelected(); i++)
				{
					if(mainView2->IsSelected(i))
					{
					numberOfSelection++;
					tempFileStat = mainView2->GetFile(i)->GetFileStat();
					if(tempFileStat.st_mode & 0100000)
						sizeOfSelection += tempFileStat.st_size;
					}
				}

				ltostr(numberOfSelection, numBuffer);
				strcpy(tempInfo, numBuffer);

				strcat(tempInfo, " items using ");
				
				ltostr(sizeOfSelection, numBuffer);
				strcat(tempInfo, numBuffer);
		
				strcat(tempInfo, " bytes");			
			}
	infoLabel->SetString(tempInfo);
	}
	break;
	default :
		Window::HandleMessage(pcMessage);
		break;
	}
}



void folderWindow::DirectoryUp()
{
	int i = 0;
	while(windowPath[i]!=0)
		i++;
		
	if(i > 1)
	{
		i--;
		while(windowPath[i] !='/')
			i--;
		if(i!=0)
		{
			windowPath[i] = 0;
		}
		else
		{
			windowPath[1] = 0;
		}
			
	}	
}












