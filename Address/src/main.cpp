#include "main.h"
using namespace os;

int main(int argc, char** argv)
{
    std::string cString = (std::string) getenv("HOME");
    mkdir ((cString +  (std::string)"/People").c_str(),0700);

    AddApp* thisApp = new AddApp(argc, argv);
    thisApp->Run();
    return 0;
}





