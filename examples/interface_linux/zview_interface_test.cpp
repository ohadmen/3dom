#include "zview_inf.h"
#include <iostream>

int main()
{
    ZviewIntrace zvi;
    std::string fn = "../examples/models/horse.stl";
    if (!zvi.loadFile(fn.c_str()))
    {
        std::cout << "failed to load file" << std::endl;
    }
    else
    {
        std::cout << "success" << std::endl;
    }

    return 0;
}