#ifndef ADDRESS_H
#define ADDRESS_H

#include <gui/window.h>
#include <gui/menu.h>
#include <gui/imagebutton.h>
#include <gui/tabview.h>
#include <gui/textview.h>
#include <gui/stringview.h>
#include <gui/dropdownmenu.h>
#include <gui/requesters.h>
#include <gui/filerequester.h>
#include <gui/view.h>
#include <storage/fsnode.h>
#include <storage/file.h>
#include "imageitem.h"
#include "loadbitmap.h"

#include <util/application.h>
#include <util/message.h>

#include "crect.h"

using namespace os;

class AddressView : public View
{
public:
    AddressView(const Rect&, Window*);
    const char* InitAddressView(const char*);
    uint GetCountry();
    void SetState(bool);
    const char* SaveAddressInformation();
    TextView*    pcFirstNameTView;

private:
    void Init();
    ImageButton* pcHeadButton;
    void SetTabOrder();
    StringView* pcFirstNameSView;


    StringView* pcMidNameSView;
    TextView* pcMidNameTView;

    StringView* pcLastNameSView;
    TextView* pcLastNameTView;

    StringView* pcAddressStreetSView;
    TextView*    pcAddressStreetTView;

    StringView* pcAddressCitySView;
    TextView* pcAddressCityTView;

    StringView* pcAddressStateSView;
    DropdownMenu* pcAddressStateDView;

    DropdownMenu* pcAddressCountryDView;

    StringView* pcAddressZipSView;
    TextView* pcAddressZipTView;

    StringView* pcAddressEmailOneSView;
    TextView* pcAddressEmailOneTView;

    StringView* pcAddressHomeNumSView;
    TextView* pcAddressHomeNumTView;

    StringView* pcAddressWorkNumSView;
    TextView* pcAddressWorkNumTView;

    StringView* pcAddressEmailTwoSView;
    TextView* pcAddressEmailTwoTView;

    void SetStates(DropdownMenu* pcDrop);
    void SetCountries(DropdownMenu* pcDrop);



    Window* pcParentWindow;
};

class AddressWindow : public Window
{
public:
    AddressWindow();
    AddressWindow(const char*);
    ~AddressWindow();

private:
    Menu* pcMain;
    Menu* pcHelp;
    Menu* pcFile;
    Menu* pcEdit;
    Menu* pcTool;
    Menu* pcApp;
    Menu* pcInsert;
    Menu* pcFormat;
    Menu* pcActions;
    Menu* pcVieu;
    void _Init();
    void  SetupMenus();
    void   SetupTabView(bool);
    void HandleMessage(Message* pcMessage);
    void ReadFromSettings(const char*);
    bool OkToQuit();
    TabView* pcTabView;
    AddressView* pcView;
    FileRequester* pcRequester;
    const char* pzFPath;
    void deleteTab();
    void createTab();
};

#endif














