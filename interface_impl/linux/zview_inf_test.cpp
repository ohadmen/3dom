#include "zview_inf.h"
#include <iostream>
#include <random>
#include <unistd.h>
int main()
{
    ZviewInf *zvi = ZviewInf::create();
    std::string fn = "../../../models/horse.stl";
    zvi->removeShape(-1);
    // if (!zvi->loadFile(fn.c_str()))
    // {
    //     std::cout << "failed to load file" << std::endl;
    // }
    // else
    // {
    //     std::cout << "success" << std::endl;
    // }

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, .5);

    int npoints = 10000;
    std::vector<float> verts(npoints * 3);
    int k = zvi->addPoints("uniform_dist", npoints, &verts[0]);
    
    for (size_t i = 0; i != 10000; ++i)
    {
        for (int i = 0; i != verts.size(); ++i)
        {
            verts[i] = dis(gen);
        }
        zvi->updatePoints(k,npoints,&verts[0]);
        usleep(1000);

    }
    // zvi->setCameraLookAt(0, 10, 0, 0, 0, 0, 1, 0, 0);
    zvi->destroy();

    return 0;
}