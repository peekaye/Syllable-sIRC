#include "address.h"
#include "messages.h"
#include "countries.h"
#include "states.h"
using namespace std;

static uint nTabNum  = 0;
static int nTabOrder = 0;

AddressWindow::AddressWindow() : Window(CRect(700,150), "Address", "Address Book 0.2",WND_NOT_RESIZABLE)
{
    _Init();
}

AddressWindow::AddressWindow(const char* path) : Window(CRect(700,150),"Address", "Address Book 0.2",WND_NOT_RESIZABLE)
{
    pcRequester = NULL;
    SetupMenus();
    SetupTabView(false);
    ReadFromSettings(path);
}

AddressWindow::~AddressWindow()
{}

void AddressWindow::_Init()
{
    pcRequester = NULL;
    SetupMenus();
    SetupTabView(true);
    nTabNum++;
}

void AddressWindow::SetupMenus()
{
    Rect cMenuFrame = GetBounds();
    Rect cMainFrame = cMenuFrame;
    cMenuFrame.bottom = 18;
    pcMain = new Menu( cMenuFrame, "main_menu", ITEMS_IN_ROW, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

    pcApp = new Menu( Rect( 0, 0, 0, 0 ), "Application", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_TOP );
    pcApp->AddItem( new ImageItem("Settings...", new Message( ID_APP_SET ), "Ctrl + S", LoadBitmapFromResource("settings-16.png")) ) ;
    pcApp->AddItem( new MenuSeparator() );
    pcApp->AddItem( new ImageItem("Exit", new Message( ID_QUIT ), "Ctrl + Q", LoadBitmapFromResource("close-16.png")) );

    pcFile = new Menu(Rect(0,0,0,0), "File", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    pcFile->AddItem(new ImageItem("Open...", new Message(ID_FILE_OPEN), "Ctrl + O", LoadBitmapFromResource("open-16.png")));
    pcFile->AddItem(new MenuSeparator());
    pcFile->AddItem(new ImageItem("New Contact",new Message(ID_NEW_TAB), "Ctrl + N", LoadBitmapFromResource("new.png")));
    pcFile->AddItem(new ImageItem("Delete Contact",new Message(ID_CLOSE_TAB),"Ctrl + D", LoadBitmapFromResource("remove.png")) );
    pcFile->AddItem(new ImageItem("Save Contact",new Message(ID_SAVE_TAB),"Ctrl + D", LoadBitmapFromResource("save.png")) );

    /*pcVieu = new Menu(Rect(0,0,0,0), "Insert", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    pcInsert = new Menu(Rect(0,0,0,0), "View", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    pcFormat = new Menu(Rect(0,0,0,0), "Format", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    pcTool = new Menu(Rect(0,0,0,0), "Tools", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    pcActions = new Menu(Rect(0,0,0,0), "Actions", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    */
    pcHelp = new Menu(Rect(0,0,0,0), "Help", ITEMS_IN_COLUMN, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP);
    pcHelp->AddItem(new ImageItem("About...", new Message(ID_APP_ABOUT), "Ctrl + A", LoadBitmapFromResource("about-16.png")));


    pcMain->AddItem(pcApp);
    pcMain->AddItem(pcFile);
    /*pcMain->AddItem(pcVieu);
    pcMain->AddItem(pcInsert);
    pcMain->AddItem(pcFormat);
    pcMain->AddItem(pcTool);
    pcMain->AddItem(pcActions);*/
    pcMain->AddItem(pcHelp);

    cMenuFrame.bottom = pcMain->GetPreferredSize( true ).y + -4.0f;  //Sizes the menu.  The higher the negative, the smaller the menu.
    cMainFrame.top = cMenuFrame.bottom + 1;
    pcMain->SetFrame( cMenuFrame );
    pcMain->SetTargetForItems( this );
    AddChild( pcMain );

}

void AddressWindow::SetupTabView(bool bAddDefaultTab)
{
    pcTabView = new TabView(Rect(0,20,GetBounds().Width(),600), "Tab", CF_FOLLOW_ALL);
    AddChild(pcTabView);

    if( bAddDefaultTab == true)
    {
        pcView = new AddressView(GetBounds(),this);

        pcTabView->AppendTab("Untitled",pcView);
        SetFocusChild(pcView->pcFirstNameTView);
    }
}

bool AddressWindow::OkToQuit()
{
    Application::GetInstance()->PostMessage( M_QUIT );
    return ( false );
}

void AddressWindow::HandleMessage(Message* pcMessage)
{
    switch (pcMessage->GetCode())
    {
    case ID_QUIT:
        for (uint i = 0; i<pcTabView->GetTabCount(); i++)
        {
            AddressView* temp = (AddressView*)pcTabView->GetTabView(i);
            temp->SaveAddressInformation();
        }
        OkToQuit();
        break;

    case ID_APP_SET:
        break;

    case ID_SAVE_TAB:
        {
            AddressView* temp = (AddressView*)pcTabView->GetTabView(pcTabView->GetSelection());
            const char* zTemp = temp->SaveAddressInformation();
            pcTabView->SetTabTitle(pcTabView->GetSelection(),zTemp);
        }
        break;
    case ID_APP_ABOUT:
        {
            Alert* pcAbout = new Alert("About Address Book","Address Book 0.2\n\nAddress Book for Syllable    \nCopyright 2002 - 2003 Syllable Desktop Team\n\nAddress Book is released under the GNU General\nPublic License. Please go to www.gnu.org more\ninformation.\n",  Alert::ALERT_INFO, 0x00, "OK", NULL);
            pcAbout->CenterInWindow(this);
            pcAbout->Go(new Invoker());
        }
        break;

    case ID_FILE_OPEN:
        {
            std::string home = (std::string)getenv("HOME") + (std::string)"/People/";
            if (pcRequester==NULL)
                pcRequester = new FileRequester(FileRequester::LOAD_REQ, new Messenger(this),home.c_str());
            pcRequester->CenterInWindow(this);
            pcRequester->Show();
            pcRequester->MakeFocus();
        }
        break;

    case M_LOAD_REQUESTED:
        {
            if(pcMessage->FindString( "file/path", &pzFPath) == 0)
            {
                ReadFromSettings(pzFPath);
            }
        }
        break;

    case ID_COUNTRY:
        {
            AddressView* temp = (AddressView*)pcTabView->GetTabView(pcTabView->GetSelection());
            if (temp->GetCountry() == 141)
                temp->SetState(true);
            else
                temp->SetState(false);
        }
        break;

    case ID_CLOSE_TAB:
        deleteTab();
        break;

    case ID_NEW_TAB:
        createTab();
        break;

    default:
        Window::HandleMessage(pcMessage);
    }
}

void AddressWindow::ReadFromSettings(const char* path)
{
    AddressView* pcAddressNewView = new AddressView(GetBounds(),this);
    const char* zTemp = pcAddressNewView->InitAddressView(path);
    pcTabView->AppendTab(zTemp,pcAddressNewView);
    pcTabView->SetSelection(nTabNum);
    nTabNum++;
    MakeFocus(pcAddressNewView);
    SetFocusChild(pcAddressNewView->pcFirstNameTView);

}

void AddressWindow::deleteTab()
{
    uint i = pcTabView->GetSelection();
    int nt = pcTabView->GetTabCount();
    if(nt>1)
    {
        pcTabView->DeleteTab(i);
        nTabNum--;
    }
}

void AddressWindow::createTab()
{
    Rect r = GetBounds();
    AddressView* av = new AddressView(GetBounds(),this);
    pcTabView->AppendTab("Untitled",av );
    pcTabView->SetSelection(nTabNum);
    nTabNum++;
    SetFocusChild(av->pcFirstNameTView);
}

AddressView::AddressView(const Rect& r, Window* pcParent) : View(r, "me")
{
    pcParentWindow = pcParent;
    Init();
}

void AddressView::Init()
{
    pcFirstNameSView = new StringView(Rect(5,5,100,27), "","First Name:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcFirstNameSView);

    pcFirstNameTView = new TextView(Rect(80,5,250,27),"","",CF_FOLLOW_LEFT);
    AddChild(pcFirstNameTView);

    pcMidNameSView = new StringView(Rect(258,5,340,27),"","Middle Name:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcMidNameSView);

    pcMidNameTView = new TextView(Rect(345,5,470,27),"","",CF_FOLLOW_LEFT);
    AddChild(pcMidNameTView);

    pcLastNameSView = new StringView(Rect(475,5,550,27),"","Last Name:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcLastNameSView);

    pcLastNameTView  = new TextView(Rect(560,5,690,27),"","",CF_FOLLOW_LEFT);
    AddChild(pcLastNameTView);

    pcAddressStreetSView = new StringView(Rect(5,30,100,52), "","Street:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcAddressStreetSView);

    pcAddressStreetTView = new TextView(Rect(50,30,200,52), "","",CF_FOLLOW_LEFT);
    AddChild(pcAddressStreetTView);


    pcAddressCitySView = new StringView(Rect(210,30,240,52), "","City:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcAddressCitySView);

    pcAddressCityTView = new TextView(Rect(245,30,350,52), "","",CF_FOLLOW_LEFT);
    AddChild(pcAddressCityTView);

    pcAddressStateSView = new StringView(Rect(360,30,400,52), "","State:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcAddressStateSView);


    pcAddressStateDView = new DropdownMenu(Rect(410,30,500,49), "");
    AddChild(pcAddressStateDView);

    pcAddressCountryDView = new DropdownMenu(Rect(510,30,580,49),"");
    pcAddressCountryDView->SetTarget(pcParentWindow);
    pcAddressCountryDView->SetSelectionMessage(new Message(ID_COUNTRY));
    AddChild(pcAddressCountryDView);

    pcAddressZipSView = new StringView(Rect(590,30,610,52), "","Zip:",ALIGN_LEFT, CF_FOLLOW_LEFT);
    AddChild(pcAddressZipSView);

    pcAddressZipTView = new TextView(Rect(620,30,689,52), "","");
    AddChild(pcAddressZipTView);

    pcAddressHomeNumSView = new StringView(Rect(5,55,75,77),"","Home #:");
    AddChild(pcAddressHomeNumSView);

    pcAddressHomeNumTView = new TextView(Rect(50,55,350,77), "","");
    AddChild(pcAddressHomeNumTView);

    pcAddressWorkNumSView = new StringView(Rect(360,55,400,77),"","Work #:");
    AddChild(pcAddressWorkNumSView);

    pcAddressWorkNumTView =  new TextView(Rect(410,55,690,77), "","");
    AddChild(pcAddressWorkNumTView);


    pcAddressEmailOneSView = new StringView(Rect(5,80,75,102),"","Email:");
    AddChild(pcAddressEmailOneSView);

    pcAddressEmailOneTView = new TextView(Rect(50,80,350,102), "","");
    AddChild(pcAddressEmailOneTView);

    pcAddressEmailTwoSView = new StringView(Rect(360,80,400,102),"","Other:");
    AddChild(pcAddressEmailTwoSView);

    pcAddressEmailTwoTView =  new TextView(Rect(410,80,690,102), "","");
    AddChild(pcAddressEmailTwoTView);

    SetStates(pcAddressStateDView);
    SetCountries(pcAddressCountryDView);
    pcAddressStateDView->SetReadOnly();
    pcAddressCountryDView->SetReadOnly();
    SetTabOrder();
}


void AddressView::SetTabOrder()
{

    pcFirstNameTView->SetTabOrder(nTabOrder);
    pcMidNameTView->SetTabOrder(nTabOrder+1);
    pcLastNameTView->SetTabOrder(nTabOrder+2);
    pcAddressStreetTView->SetTabOrder(nTabOrder+3);
    pcAddressCityTView->SetTabOrder(nTabOrder+4);
    pcAddressStateDView->SetTabOrder(nTabOrder+5);
    pcAddressCountryDView->SetTabOrder(nTabOrder+6);
    pcAddressZipTView->SetTabOrder(nTabOrder+7);
    pcAddressHomeNumTView->SetTabOrder(nTabOrder+8);
    pcAddressWorkNumTView->SetTabOrder(nTabOrder+9);
    pcAddressEmailOneTView->SetTabOrder(nTabOrder+10);
    pcAddressEmailTwoTView->SetTabOrder(nTabOrder+11);
    nTabOrder+=12;
    MakeFocus(pcFirstNameTView);
}

void AddressView::SetStates(DropdownMenu* pcDrop)
{
    for (int i=0; i<59; i++)
        pcDrop->AppendItem(states[i]);

    pcDrop->SetSelection(0);
}


void AddressView::SetCountries(DropdownMenu* pcDrop)
{
    for (int i=0; i<149; i++)
        pcDrop->AppendItem(countries[i]);

    pcDrop->SetSelection(141);
}

const char* AddressView::InitAddressView(const char* path)
{
    string returntype;
    char zTempTwo[1024];
    int nSize;
    FSNode *pcNode = new FSNode();

    if( pcNode->SetTo(path) == 0 )
    {
        nSize = pcNode->ReadAttr( "firstname", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        returntype = zTempTwo;
        pcFirstNameTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "midname", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcMidNameTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "lastname", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        returntype += zTempTwo;
        pcLastNameTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "street", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressStreetTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "city", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressCityTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "zip", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressZipTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "home#", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressHomeNumTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "work#", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressWorkNumTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "email1", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressEmailOneTView->Set(zTempTwo);

        nSize = pcNode->ReadAttr( "email2", ATTR_TYPE_STRING, &zTempTwo, 0, 1023 );
        zTempTwo[nSize] = 0;
        pcAddressEmailTwoTView->Set(zTempTwo);

        pcNode->ReadAttr("state",ATTR_TYPE_INT32,&nSize,0,100);
        pcAddressStateDView->SetSelection(nSize);

        pcNode->ReadAttr("country",ATTR_TYPE_INT32,&nSize,0,100);
        pcAddressCountryDView->SetSelection(nSize);

    }

    return returntype.c_str();
}

const char* AddressView::SaveAddressInformation()
{
    String zTemp = pcFirstNameTView->GetBuffer()[0];
    std::string zReturn;
    std::string zTempTwo;
    int nTemp;

    if (zTemp.Length() > 0)
    {
        string zName = (std::string)pcFirstNameTView->GetBuffer()[0]+ (std::string)"_" + (std::string)pcLastNameTView->GetBuffer()[0];
        zReturn = (std::string)pcFirstNameTView->GetBuffer()[0]+ (std::string)" " + (std::string)pcLastNameTView->GetBuffer()[0];
        string zFileName = getenv("HOME") + (string)"/People/"+zName;
        FSNode *pcNode = new FSNode(zFileName,O_CREAT);

        zTempTwo = pcFirstNameTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "firstname", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0,zTempTwo.length() );

        zTempTwo = pcMidNameTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "midname", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo = pcLastNameTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "lastname", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo  = pcAddressStreetTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "street", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo  = pcAddressCityTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "city", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo  = pcAddressZipTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "zip", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        nTemp = pcAddressStateDView->GetSelection();
        pcNode->WriteAttr( "state", O_TRUNC, ATTR_TYPE_INT32, &nTemp,0,100 );

        nTemp = pcAddressCountryDView->GetSelection();
        pcNode->WriteAttr( "country", O_TRUNC, ATTR_TYPE_INT32, &nTemp,0,100 );

        zTempTwo = pcAddressHomeNumTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "home#", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo = pcAddressWorkNumTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "work#", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo = pcAddressEmailOneTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "email1", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

        zTempTwo = pcAddressEmailTwoTView->GetBuffer()[0].c_str();
        pcNode->WriteAttr( "email2", O_TRUNC, ATTR_TYPE_STRING, zTempTwo.c_str(), 0, zTempTwo.length() );

    }
    return zReturn.c_str();
}

uint AddressView::GetCountry()
{
    return pcAddressCountryDView->GetSelection();
}

void AddressView::SetState(bool bShowState)
{
    if (bShowState)
        pcAddressStateDView->SetEnable(true);

    else
        pcAddressStateDView->SetEnable(false);
}

















