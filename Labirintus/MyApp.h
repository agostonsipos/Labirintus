#pragma once

#include <vector>
#include <list>
#include <set>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

// mesh
#include "ObjParser_OGL3.h"

struct Suzanne{
	float x;
	float y;
	float ir;
	Suzanne(float a, float b, float c) :x(a), y(b), ir(c){}
	Suzanne(int a = 0, int b = 0, int c = 0) :x(a), y(b), ir(c){}
	Suzanne operator+=(Suzanne s){
		x += s.x; y += s.y; ir += s.ir;
		return *this;
	}
};
struct Bush{
	int x;
	int y;
	bool ir;
};
struct Coin{
	int x;
	int y;
	Coin(int a, int b) :x(a), y(b){}
	Coin(Suzanne s) :x(s.x), y(s.y){}
	Coin operator+(Coin b){ return Coin(x + b.x, y + b.y); }
};
class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	// belsõ eljárások
	void DrawGround();
	void DrawBushes();
	void DrawCoins();
	void DrawDiamonds();
	void DrawSuzanne();
	void DrawShots();

	bool CheckWall(Coin a, Coin b);
	bool HitWall(Suzanne s);

	void CheckMoney();

	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	// transzformációs mátrixok
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// mátrixok helye a shaderekben
	GLuint	m_loc_mvp;
	GLuint  m_loc_wit;
	GLuint	m_loc_texture;
	GLuint  m_loc_eye;
	GLuint  m_loc_sunpos;
	GLuint  m_loc_moonpos;
	GLuint  m_loc_kd;
	GLuint  m_loc_ks;


	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító
	GLuint m_ibID;  // index buffer object erõforrás azonosító
	GLuint m_floor_textureID; // fájlból betöltött textúra azonosítója
	GLuint m_bush_texture_ID;
	GLuint m_coin_texture_ID;
	GLuint m_diamond_texture_ID;
	GLuint m_brown_texture_ID;
	GLuint m_fire_texture_ID;

	glm::vec3 m_eye = glm::vec3(-19, 10, -12);
	glm::vec3 m_at = glm::vec3( 0, 0, 0 );
	glm::vec3 m_up = glm::vec3(0, 1, 0);


	struct Vertex
	{
		glm::vec3 p; // pozíció
		glm::vec3 c; // szín
		glm::vec2 t; // textúra koordináták
	};


	std::set<Bush> m_list_bushes;
	void genBushes();


	std::set<Coin> m_list_coins;
	void genCoins();

	std::set<Coin> m_list_diamonds;
	void genDiamonds();


	Suzanne suzpos;
	bool moving[4];
	void Move(Suzanne& s);

	Uint32 t;
	Uint32 t0, tm, ts, tl;

	bool camera = true;

	bool mega = true;

	std::list<Suzanne> m_list_shots;

	bool win = false;
	bool lose = false;

	glm::vec3 sunpos;
	glm::vec3 moonpos;

	int money = 0;
	int diamonds = 0;

	// mesh adatok
	Mesh *m_suzanne;
	Mesh *m_bush;
	Mesh *m_coin;
	Mesh *m_diamond;
	Mesh *m_shot;
};

bool operator<(Bush a, Bush b);
bool operator<(Coin a, Coin b);

