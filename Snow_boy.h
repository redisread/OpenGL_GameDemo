#pragma once
#include "Angel.h"
#include "Mesh_Painter.h"
#include "mesh.h"



namespace snow_boy{
    extern Mesh_Painter * snow_painter;     //  
    extern std::vector<My_Mesh*> snow_meshes;
    extern int snow_index;
    extern int Num;
    extern std::vector<std::string>  sv;
    extern std::vector<std::string>  sf;


    void init();
    void update();
    void draw();
    void dele();



}