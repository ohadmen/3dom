#include "zview_inf.h"
#include <iostream>

int main()
{
    ZviewInf* zvi = ZviewInf::create();
    std::string fn = "examples/models/horse.stl";
    if (!zvi->loadFile(fn.c_str()))
    {
        std::cout << "failed to load file" << std::endl;
    }
    else
    {
        std::cout << "success" << std::endl;
    }
    zvi->setCameraLookAt(0,10,0,0,0,0,1,0,0);
    zvi->destroy();

    return 0;
}