#include "application.h"
#include <cstdio>
using namespace os;
AddApp::AddApp(int argc, char** argv) : Application("application./x-vnd.RGC-address")
{
    settings = new Settings();
    if (argc == 1)
        pcWindow = new AddressWindow();

    else
    {
        FILE* fin = fopen(argv[1],"r");
        if (fin)
            pcWindow = new AddressWindow(argv[1]);
        else
            pcWindow = new AddressWindow();
    }
    pcWindow->Show();
    pcWindow->MakeFocus();
}

AddApp::~AddApp()
{}
