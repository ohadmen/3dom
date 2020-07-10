
#include "C:\\Program Files\\zview\\zview_inf.h"
#include <iostream>
#include <windows.h>
typedef ZviewInf* (__cdecl* zviewinf_factory)();

int main()
{
    
    // Load the DLL
    HINSTANCE dll_handle = ::LoadLibrary(TEXT(R"(C:\Program Files\zview\zview_inf.dll)"));
    if (!dll_handle) {
        std::cerr << "Unable to load DLL!\n";
        return 1;
    }
         // Get the function from the DLL
    zviewinf_factory factory_func = reinterpret_cast<zviewinf_factory>(
         ::GetProcAddress(dll_handle, "create_zviewinf"));
     if (!factory_func) {
         std::cerr << "Unable to load create_zviewinf from DLL!\n";
         ::FreeLibrary(dll_handle);
         return 1;
     }
 
     // Ask the factory for a new object implementing the IKlass
     // interface
     ZviewInf* instance = factory_func();
 
     std::string fn = "../../examples/models/horse.stl";
     if (!instance->loadFile(fn.c_str()))
     {
         std::cout << "failed to load file" << std::endl;
     }
     else
     {
         std::cout << "success" << std::endl;
     }

     // Destroy it explicitly
     instance->destroy();
     ::FreeLibrary(dll_handle);
}

