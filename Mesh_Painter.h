#pragma once

#include "mesh.h"
// #include "myPhong.h"

class Mesh_Painter
{
public:
	Mesh_Painter();
	~Mesh_Painter();

	void add_mesh(My_Mesh*);
	void dele_mesh();

	void init_shaders(std::vector<std::string>  vs, std::vector<std::string> fs);
	
	void update_vertex_buffer();
	void update_texture();
	void update_snow_beterx_buffer(My_Mesh *mm);

	void draw_meshes();
	void clear_meshes();

	void setCameraMatrix(mat4 model,mat4 view,mat4 pro);
	void setShadowMateix(mat4 shaowMat);
	void setLightPos(vec3 pos);
	// void setPhong(unsigned char key,int mouse_x,int mouse_y);
	// void setmouseWheel(int button, int dir, int x, int y);
	// void setModelMatrix(mat4 matrix,int i);
private:

	void load_texture_STBImage(std::string file_name, GLuint& m_texName);

	std::vector<GLuint> textures_all;
	std::vector<GLuint> program_all;
	std::vector<GLuint> vao_all;
	std::vector<GLuint> buffer_all;
	std::vector<GLuint> vPosition_all;
	std::vector<GLuint> vColor_all;
	std::vector<GLuint> vTexCoord_all;
	std::vector<GLuint> vNormal_all;
	std::vector<GLuint> theta_all;
	std::vector<GLuint> trans_all;
	
	std::vector<My_Mesh*> m_my_meshes_;
	// std::vector<Phong*> my_phong;
};