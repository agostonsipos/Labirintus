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
#include <glm/gtx/transform.hpp>

// mesh
#include "ObjParser_OGL3.hpp"
#include "gShaderProgram.hpp"

#include "GameObjects.hpp"

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
	// Drawing of objects
	void DrawGround();
	void DrawBushes(bool detailed);
	void DrawCoins();
	void DrawDiamonds();
	void DrawSuzanne();
	void DrawShots();

	// Internal checks
	void CheckCoinDiamond();
	bool CheckWallBetween(FixedObject a, FixedObject b);
	bool HitWall(Shot s);

	// Shader programs
	gShaderProgram m_program;
	gShaderProgram m_inst_program;

	// Vertex array of ground
	GLuint m_vaoID;
	GLuint m_vboID;
	GLuint m_ibID;

	// 3D objects
	Mesh *m_suzanne;
	Mesh *m_bush;
	Mesh *m_bush_backup;
	Mesh *m_coin;
	Mesh *m_diamond;
	Mesh *m_shot;

	// Textures
	GLuint m_floor_textureID;
	GLuint m_bush_texture_ID;
	GLuint m_coin_texture_ID;
	GLuint m_diamond_texture_ID;
	GLuint m_brown_texture_ID;
	GLuint m_fire_texture_ID;

	// Transformation matrices
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;
	
	// Camera details
	glm::vec3 m_eye;
	glm::vec3 m_at;
	glm::vec3 m_up;

	// Game objects
	std::set<Bush> m_list_bushes;
	void genBushes();

	std::set<Coin> m_list_coins;
	void genCoins();

	std::set<Diamond> m_list_diamonds;
	void genDiamonds();

	std::list<Shot> m_list_shots;

	PlayerObject position;
	
	// Moving state of player
	bool moving[4];

	// Timing variables
	Uint32 t;
	Uint32 t0, tm, ts, tl;

	// State variables
	bool camera = true;
	bool mega = true;
	bool win = false;
	bool lose = false;
	int money = 0;
	int diamonds = 0;

	// Light positions
	glm::vec3 sunpos;
	glm::vec3 moonpos;

};

