#ifndef APPLICATION_H
#define APPLICATION_H

#include <util/application.h>
#include <util/settings.h>

#include "address.h"
using namespace os;

class AddApp : public Application
{
public:
    AddApp(int argc, char** argv);
    ~AddApp();

private:
    AddressWindow* pcWindow;
    Settings* settings;
};

#endif





