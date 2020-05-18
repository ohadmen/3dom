#pragma once
class ZviewInfImpl;
class ZviewIntrace
{
    ZviewInfImpl* zv;
public:
 ZviewIntrace();
 ~ZviewIntrace();
 void loadFile(const char* filename);
};