#include "MyApp.h"
#include "GLUtils.hpp"

#include <vector>
#include <math.h>
#include <ctime>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_floor_textureID = 0;

	m_suzanne = 0;
	m_bush = 0;
	m_coin = 0;
	m_diamond = 0;
	for (int i = 0; i < 4; ++i) moving[i] = false;
	tm = SDL_GetTicks();
	t = SDL_GetTicks();
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	Vertex vert[] =
	{ 
		{glm::vec3(-10, 0, -10), glm::vec3( 0, 1, 0), glm::vec2(0, 0)},
		{glm::vec3(-10, 0,  10), glm::vec3( 0, 1, 0), glm::vec2(0, 1)},
		{glm::vec3( 10, 0, -10), glm::vec3( 0, 1, 0), glm::vec2(1, 0)},
		{glm::vec3( 10, 0,  10), glm::vec3( 0, 1, 0), glm::vec2(1, 1)},
	};

    GLushort indices[]=
    {
        0,1,2,
        2,1,3,
    };

	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData( GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	// position attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE,	sizeof(Vertex),	0); 

	//surface normal attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	sizeof(Vertex),	(void*)(sizeof(glm::vec3)) );

	//texture coordinates
	glEnableVertexAttribArray(2); 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(glm::vec3)) );

	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	m_program = createShaderProgram("shaders/shader.vert", "shaders/shader.frag");
	
	m_inst_program = createShaderProgram("shaders/instanced.vert", "shaders/shader.frag");

	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 10000.0f );

	/*m_loc_mvp = glGetUniformLocation( m_programID, "MVP");
	m_loc_world = glGetUniformLocation(m_programID, "world");
	m_loc_wit = glGetUniformLocation(m_programID, "WorldIT");

	m_loc_texture = glGetUniformLocation( m_programID, "texture" );

	m_loc_sunpos = glGetUniformLocation(m_programID, "Sp");
	m_loc_moonpos = glGetUniformLocation(m_programID, "Mp");

	m_loc_eye = glGetUniformLocation(m_programID, "eye");

	m_loc_ka = glGetUniformLocation(m_programID, "ka");
	m_loc_kd = glGetUniformLocation(m_programID, "kd");
	m_loc_ks = glGetUniformLocation(m_programID, "ks");

	i_loc_vp = glGetUniformLocation( m_inst_programID, "VP");
	i_loc_world = glGetUniformLocation(m_inst_programID, "world");
	i_loc_wit = glGetUniformLocation(m_inst_programID, "WorldIT");

	i_loc_texture = glGetUniformLocation( m_inst_programID, "texture" );

	i_loc_sunpos = glGetUniformLocation(m_inst_programID, "Sp");
	i_loc_moonpos = glGetUniformLocation(m_inst_programID, "Mp");

	i_loc_eye = glGetUniformLocation(m_inst_programID, "eye");

	i_loc_ka = glGetUniformLocation(m_inst_programID, "ka");
	i_loc_kd = glGetUniformLocation(m_inst_programID, "kd");
	i_loc_ks = glGetUniformLocation(m_inst_programID, "ks");*/


	m_floor_textureID = TextureFromFile("textures/floor.bmp");
	m_bush_texture_ID = TextureFromFile("textures/bush.bmp");
	m_coin_texture_ID = TextureFromFile("textures/coin.bmp");
	m_diamond_texture_ID = TextureFromFile("textures/diamond.bmp");
	m_fire_texture_ID = TextureFromFile("textures/fire.bmp");
	m_brown_texture_ID = TextureFromFile("textures/brown.bmp");

	m_suzanne = ObjParser::parse("models/Suzanne.obj");
	m_suzanne->initBuffers();

	m_bush = ObjParser::parse("models/hedge.obj");
	m_bush->initBuffers();
	m_bush_backup = ObjParser::parse("models/bush.obj");
	m_bush_backup->initBuffers();

	m_coin = ObjParser::parse("models/coin.obj");
	m_coin->initBuffers();

	m_diamond = ObjParser::parse("models/diamond.obj");
	m_diamond->initBuffers();

	m_shot = ObjParser::parse("models/shot.obj");
	m_shot->initBuffers();

	genBushes();
	genCoins();
	genDiamonds();
	suzpos = { rand() % 50, rand() % 50, rand() % 4 };

	return true;
}

void CMyApp::Clean()
{
	delete m_suzanne;
	delete m_bush;
	delete m_coin;
	delete m_diamond;
	delete m_shot;
	glDeleteTextures(1, &m_floor_textureID);

	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	m_program.Clean();
	m_inst_program.Clean();
}

void CMyApp::Update()
{
	float v = 0.002f;
	if (mega) v *= 1.5f;
	if (moving[0]){
		suzpos += Suzanne(v * (SDL_GetTicks() - t)*m_up.x, v * (SDL_GetTicks() - t)* m_up.z, 0.0f);
		t = SDL_GetTicks();
		if (t - t0 > 1/v){
			moving[0] = false;
			suzpos.x = roundf(suzpos.x);
			suzpos.y = roundf(suzpos.y);
			CheckMoney();
		}
	}
	if (moving[1]){
		suzpos += Suzanne(-v * (SDL_GetTicks() - t)*m_up.x, -v * (SDL_GetTicks() - t)* m_up.z, 0.0f);
		t = SDL_GetTicks();
		if (t - t0 > 1/v){
			moving[1] = false;
			suzpos.x = roundf(suzpos.x);
			suzpos.y = roundf(suzpos.y);
			CheckMoney();
		}
	}
	if (moving[2]){
		suzpos.ir += v * (SDL_GetTicks()-t);
		t = SDL_GetTicks();
		if (t - t0 > 1/v){
			moving[2] = false;
			suzpos.ir = (float)roundf(suzpos.ir);
		}
	}
	if (moving[3]){
		suzpos.ir -= v * (SDL_GetTicks() - t);
		t = SDL_GetTicks();
		if (t - t0 > 1/v){
			moving[3] = false;
			suzpos.ir = (float)roundf(suzpos.ir);
		}
	}
	for (auto it = m_list_shots.begin(); it != m_list_shots.end();){
		if (it->x<-1 || it->y<-1 || it->x>50 || it->y>50 || HitWall(*it)){
			it = m_list_shots.erase(it);
			continue;
		}
		if ((int)it->ir % 4 == 0){
			(*it) += Suzanne(0.0f, 0.01*(SDL_GetTicks() - ts), 0.0f);
		}
		if ((int)it->ir % 4 == 1){
			*it += Suzanne(0.01*(SDL_GetTicks() - ts), 0.0f, 0.0f);
		}
		if ((int)it->ir % 4 == 2){
			*it += Suzanne(0.0f, -0.01*(SDL_GetTicks() - ts), 0.0f);
		}
		if ((int)it->ir % 4 == 3){
			*it += Suzanne(-0.01*(SDL_GetTicks() - ts), 0.0f, 0.0f);
		}
		++it;
	}
	ts = SDL_GetTicks();
	if (win){
		float c = cosf(3.14159 / 2 * SDL_GetTicks() / 2000.0f);
		float s = sinf(3.14159 / 2 * SDL_GetTicks() / 2000.0f);
		m_eye = glm::vec3(suzpos.x * 20 - s * 10, 6 + 4 * c * s, suzpos.y * 20 - c * 10);
		m_up = glm::vec3(s, 0, c);
		glm::vec3 m_at = glm::vec3(suzpos.x * 20, 4, suzpos.y * 20);
		m_matView = glm::lookAt(m_eye, m_at, m_up);
	}
	else if (lose){
		glm::vec3 m_at = glm::vec3(suzpos.x * 20, 0, suzpos.y * 20);
		Uint32 dt = glm::clamp(1000.0f-float(SDL_GetTicks() - tl), 0.0f, 1000.0f);
		float c = cosf(3.14159 / 2 * suzpos.ir);
		float s = sinf(3.14159 / 2 * suzpos.ir);
		m_eye = glm::vec3(suzpos.x * 20 + s * 10 + c*dt / 100.0f, 5 + dt / 100.0f, suzpos.y * 20 + c * 10 + s*dt / 100.0f);
		m_up = glm::vec3(-s, 0, -c);
		m_matView = glm::lookAt(m_eye, m_at, m_up);
	}
	else if (camera){
		float c = cosf(3.14159 / 2 * suzpos.ir);
		float s = sinf(3.14159 / 2 * suzpos.ir);
		m_eye = glm::vec3(suzpos.x * 20 - s * 30, 30, suzpos.y * 20 - c * 30);
		m_up = glm::vec3(s, 0, c);
		glm::vec3 m_at = glm::vec3(suzpos.x * 20, 0, suzpos.y * 20);
		m_matView = glm::lookAt(m_eye, m_at, m_up);
	}
	else{
		float c = cosf(3.14159 / 2 * suzpos.ir);
		float s = sinf(3.14159 / 2 * suzpos.ir);
		m_up = glm::vec3(s, 0, c);
		m_matView = glm::lookAt(glm::vec3(suzpos.x * 20, 200, suzpos.y * 20-1), glm::vec3(suzpos.x * 20, 0, suzpos.y * 20), glm::vec3(0, 1, 0));
	}

	sunpos = glm::vec3(1000.0f*sinf(SDL_GetTicks() / 20000.0f), 1000.0f*cosf(SDL_GetTicks() / 20000.0f), 0.0f);
	moonpos = glm::vec3(-1000.0f*sinf(SDL_GetTicks() / 20000.0f), -1000.0f*cosf(SDL_GetTicks() / 20000.0f), 0.0f);

	if (SDL_GetTicks() - tm > 10000) mega = false;

	m_program.On();
	m_program.SetUniform("Sp", sunpos);
	m_program.SetUniform("Mp", moonpos);
	m_program.SetUniform("eye", m_eye);
	m_program.Off();

}


void CMyApp::DrawGround()
{
	m_program.On();
	m_program.SetUniform("kd", glm::vec4(1.0, 1.0, 1.0, 1.0));
	
	m_program.SetTexture("texture", 0, m_floor_textureID);

	glBindVertexArray(m_vaoID);

	for (int i = 0; i < 50; ++i){
		for (int j = 0; j < 50; ++j){
			m_matWorld = glm::translate<float>(glm::vec3(20.f * i, 0.f, 20.f * j));
			glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
			glm::mat4 WIT = glm::transpose(glm::inverse(m_matWorld));
			
			m_program.SetUniform("MVP", mvp);
			m_program.SetUniform("world", m_matWorld);
			m_program.SetUniform("WorldIT", WIT);

			glDrawElements(GL_TRIANGLES, 6,	GL_UNSIGNED_SHORT, 0);
		}
	}
	glBindVertexArray(0);

	m_program.SetTexture("texture", 0, 0);

	m_program.Off();
}

void CMyApp::DrawBushes(bool detailed){
	std::vector<glm::mat4> world_range;
	std::vector<glm::mat4> worldIT_range;
	for (auto it : m_list_bushes){
		if(detailed)
			m_matWorld = glm::translate<float>(glm::vec3(it.ir?-10:0,0,it.ir?10:0))*glm::translate<float>(glm::vec3(it.x*20, 0.0, it.y*20-10.0))*glm::rotate<float>(it.ir?M_PI_2:0,glm::vec3(0,1,0))
				*glm::translate<float>(glm::vec3(0,4,0))*glm::scale<float>(glm::vec3(30, 50, 20))*glm::translate<float>(glm::vec3(0,-0.1,0));
		else
			m_matWorld = glm::translate<float>(glm::vec3(it.ir?-10:0,0,it.ir?10:0))*glm::translate<float>(glm::vec3(it.x*20, 0.0, it.y*20-10.0))*glm::rotate<float>(it.ir?M_PI_2:0,glm::vec3(0,1,0))
				*glm::translate<float>(glm::vec3(0,4,0))*glm::rotate<float>(M_PI_2, glm::vec3(1, 0, 0))*glm::scale<float>(glm::vec3(5.0, 3.0, 5.0));
		world_range.push_back(m_matWorld);
		worldIT_range.push_back(glm::transpose(glm::inverse(m_matWorld)));
		
		if(detailed)
		{
			glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
			glm::vec4 v = mvp*glm::vec4(m_bush->getSampleVertPos(),1); v /= v.w;
			// Early clipping (totally heuristic)
			if(v.x > 1.6 || v.y > 1.3 || v.z > 1.6 || v.x < -1.6 || v.y < -2.5 || v.z < -4) { world_range.pop_back(); worldIT_range.pop_back(); }
		}
	}
	m_inst_program.On();
	glm::mat4 vp = m_matProj * m_matView;
	m_inst_program.SetUniform("ka", glm::vec4(0.0, 0.8, 0.0, 1.0));
	m_inst_program.SetUniform("kd", glm::vec4(0.0, 0.8, 0.0, 1.0));
	m_inst_program.SetUniform("VP", vp);
	m_inst_program.SetTexture("texture", 0, m_bush_texture_ID);
	if(detailed)
	{
		m_bush->addTransformAttribute(3, world_range);
		m_bush->addTransformAttribute(7, worldIT_range);
		m_bush->drawInstanced(world_range.size());
	}
	else
	{
		m_bush_backup->addTransformAttribute(3, world_range);
		m_bush_backup->addTransformAttribute(7, worldIT_range);
		m_bush_backup->drawInstanced(world_range.size());
	}
	m_inst_program.SetUniform("ka", glm::vec4(0.1, 0.1, 0.1, 1.0));
	m_program.SetTexture("texture", 0, 0);
	m_inst_program.Off();
}

void CMyApp::DrawCoins(){
	m_program.On();
	m_program.SetUniform("kd", glm::vec4(1.0, 1.0, 0.0, 1.0));
	for (auto it : m_list_coins){
		m_matWorld = glm::translate<float>(glm::vec3(it.x*20, 5, it.y*20))*glm::rotate<float>(2*3.14159*SDL_GetTicks()/2000.0f,glm::vec3(0,1,0))*glm::rotate<float>(M_PI_2,glm::vec3(1,0,0))*glm::scale<float>(glm::vec3(8,8,8));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::transpose(glm::inverse(m_matWorld));
		
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("world", m_matWorld);
		m_program.SetUniform("WorldIT", WIT);
		m_program.SetTexture("texture", 0, m_coin_texture_ID);
		m_coin->draw();
	}
	m_program.SetTexture("texture", 0, 0);
	m_program.Off();
}

void CMyApp::DrawDiamonds(){
	m_program.On();
	m_program.SetUniform("kd", glm::vec4(2.0, 2.0, 3.0, 1.0));
	for (auto it : m_list_diamonds){
		m_matWorld = glm::translate<float>(glm::vec3(it.x * 20, 2, it.y * 20))*glm::rotate<float>(2 * 3.14159*SDL_GetTicks() / 2000.0f, glm::vec3(0, 1, 0))*glm::scale<float>(glm::vec3(5, 5, 5))*glm::rotate<float>(M_PI_2, glm::vec3(1, 0, 0))*glm::scale<float>(glm::vec3(2, 2, 2));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::transpose(glm::inverse(m_matWorld));
		
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("world", m_matWorld);
		m_program.SetUniform("WorldIT", WIT);
		m_program.SetTexture("texture", 0, m_diamond_texture_ID);
		m_diamond->draw();
	}
	m_program.SetTexture("texture", 0, 0);
	m_program.Off();
}

void CMyApp::DrawSuzanne()
{
	m_program.On();
	if (mega)
		m_program.SetUniform("ks", glm::vec4(2.0, 0.0, 0.0, 1.0));

	m_program.SetUniform("kd", glm::vec4(1.0, 1.0, 1.0, 1.0));

	glm::mat4 jump;
	if (moving[0] || moving[1] || moving[2] || moving[3]){
		jump = glm::translate<float>(glm::vec3(0, 3 * fabs(sinf((SDL_GetTicks() - t0) * 4 * 3.14159f / 1000.0f)), 0));
		if (mega)
			jump = glm::translate<float>(glm::vec3(0, 3 * fabs(sinf((SDL_GetTicks() - t0) * 4 * 3.14159f / 750.0f)), 0));
	}
	else if (win)
		jump = glm::translate<float>(glm::vec3(0, log1pf(money) * fabs(sinf((SDL_GetTicks() - t0) * 4 * 3.14159f / 1000.0f)), 0));
	else if (lose)
		jump = glm::rotate<float>(glm::clamp(-M_PI/6 * (SDL_GetTicks() - tl) / 1000.0f, -M_PI/6, 0.0), glm::vec3(m_up.z, 0, -m_up.x));
	else jump = glm::mat4(1.0f);

	glm::mat4 size;
	if (mega)
		size = glm::scale<float>(glm::vec3(4.5f, 4.5f, 4.5f));
	else size = glm::scale<float>(glm::vec3(3, 3, 3));

	m_matWorld = glm::translate<float>(glm::vec3(suzpos.x*20, 3, suzpos.y*20))*jump*size*glm::rotate<float>(suzpos.ir*M_PI_2,glm::vec3(0,1,0));
	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
	glm::mat4 WIT = glm::transpose(glm::inverse(m_matWorld));
	
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("world", m_matWorld);
	m_program.SetUniform("WorldIT", WIT);
	m_program.SetTexture("texture", 0, m_brown_texture_ID);

	m_suzanne->draw();

	m_program.SetUniform("ks", glm::vec4(1.0, 1.0, 1.0, 1.0));

	m_program.SetTexture("texture", 0, 0);
	m_program.Off();
}

void CMyApp::DrawShots(){
	m_program.On();
	for (auto it : m_list_shots){
		m_program.SetUniform("kd", glm::vec4(2.0, 2.0, 3.0, 1.0));
		m_matWorld = glm::translate<float>(glm::vec3(it.x * 20, 2, it.y * 20))*glm::rotate<float>(2 * 3.14159*SDL_GetTicks() / 100.0f, glm::vec3(0, 1, 0))*glm::scale<float>(glm::vec3(5, 5, 5));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::transpose(glm::inverse(m_matWorld));
		
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("world", m_matWorld);
		m_program.SetUniform("WorldIT", WIT);
		m_program.SetTexture("texture", 0, m_fire_texture_ID);
		
		
		m_shot->draw();
	}
	m_program.SetTexture("texture", 0, 0);
	m_program.Off();
}

void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawGround();
	DrawBushes(camera && !win && !lose);
	DrawCoins();
	DrawDiamonds();
	DrawSuzanne();
	DrawShots();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	if (!moving[0] && !moving[1] && !moving[2] && !moving[3] && !win && !lose)
		switch (key.keysym.sym){
		case SDLK_w:
		case SDLK_UP:
			if (CheckWall((Coin)suzpos, (Coin)suzpos + Coin(m_up.x, m_up.z))){
				moving[0] = true;
				t = SDL_GetTicks();
				t0 = t;
			}
			break;
		case SDLK_s:
		case SDLK_DOWN:
			if (CheckWall((Coin)suzpos, (Coin)suzpos + Coin(-1 * m_up.x, -1 * m_up.z))){
				moving[1] = true;
				t = SDL_GetTicks();
				t0 = t;
			}
			break;
		case SDLK_a:
		case SDLK_LEFT:
			moving[2] = true;
			t = SDL_GetTicks();
			t0 = t;
			break;
		case SDLK_d:
		case SDLK_RIGHT:
			moving[3] = true;
			t = SDL_GetTicks();
			t0 = t;
			break;
		case SDLK_c:
			camera = !camera;
			break;
		case SDLK_m:
			if (money >= 10){
				mega = true;
				tm = SDL_GetTicks();
				money -= 10;
				std::cout << "Money: " << money << std::endl;
			}
			break;
		case SDLK_k:
			lose = true;
			tl = SDL_GetTicks();
			break;
		case SDLK_SPACE:
			while (suzpos.ir < 0) suzpos.ir += 4;
			if (mega){
				m_list_shots.push_back(suzpos);
			}
			break;
		case SDLK_F11:
			++money;
			std::cout << "Money: " << money << std::endl;
			break;
		case SDLK_F12:
			win = true;
			break;
		}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(45.0f, _w / (float)_h, 0.01f, 10000.0f);
}

void CMyApp::genBushes(){
	srand(time(NULL));
	for(int i = 0; i < 50; ++i){
		m_list_bushes.insert({ i, 0, 0 });
		m_list_bushes.insert({ 0, i, 1 });
		m_list_bushes.insert({ i, 50, 0 });
		m_list_bushes.insert({ 50, i, 1 });
	}
	int i=0;
	do{
		Bush b = { rand() % 50, rand() % 50, rand() % 2 };
		m_list_bushes.insert(b);
		++i;
	} while (m_list_bushes.size() != 1000);
}

void CMyApp::genCoins(){
	srand(time(NULL));
	for (int i = 0; i < 100; ++i){
		do{
			Coin c = { rand() % 50, rand() % 50 };
			if (m_list_coins.find(c) == m_list_coins.end())
				m_list_coins.insert(c);
		} while (m_list_coins.size() != i + 1);
	}
}

void CMyApp::genDiamonds(){
	srand(time(NULL));
	for (int i = 0; i < 10; ++i){
		do{
			Coin d = { rand() % 50, rand() % 50 };
			if (m_list_coins.find(d) == m_list_coins.end() && m_list_diamonds.find(d) == m_list_diamonds.end()){
				m_list_diamonds.insert(d);
			}
		} while (m_list_diamonds.size() != i + 1);
	}
}

bool CMyApp::HitWall(Suzanne s){
	Suzanne z = s;
	if ((int)s.ir % 4 == 0){
		z += Suzanne(0.0f, 1.0f, 0.0f);
	}
	if ((int)s.ir % 4 == 1){
		z += Suzanne(1.0f, 0.0f, 0.0f);
	}
	if ((int)s.ir % 4 == 2){
		z += Suzanne(0.0f, -1.0f, 0.0f);
	}
	if ((int)s.ir % 4 == 3){
		z += Suzanne(-1.0f, 0.0f, 0.0f);
	}
	Bush w = { s.x >= z.x ? static_cast<int>(s.x) : static_cast<int>(z.x), s.y >= z.y ? static_cast<int>(s.y) : static_cast<int>(z.y), s.x == z.x ? 0 : 1 };
	if (m_list_bushes.find(w) != m_list_bushes.end()){
		m_list_bushes.erase(w);
		return true;
	}
	return false;
}


bool CMyApp::CheckWall(Coin a, Coin b){
	Bush w = { a.x >= b.x ? a.x : b.x, a.y >= b.y ? a.y : b.y, a.x == b.x ? 0 : 1 };
	return b.x >= 0 && b.y >= 0 && b.x < 50 && b.y < 50 && m_list_bushes.find(w) == m_list_bushes.end();
}

void CMyApp::CheckMoney(){
	auto it = m_list_coins.find(Coin(suzpos));
	if (it != m_list_coins.end()){
		++money;
		std::cout << "Money: " << money << std::endl;
		m_list_coins.erase(it);
	}
	auto id = m_list_diamonds.find(Coin(suzpos));
	if (id != m_list_diamonds.end()){
		++diamonds;
		std::cout << "Diamonds: " << diamonds << std::endl;
		m_list_diamonds.erase(id);
	}
	if (diamonds == 10){
		win = true;
	}
}

bool operator<(Bush a, Bush b){
	std::vector<int> v1 = { a.x, a.y, a.ir }, v2 = { b.x, b.y, b.ir };
	return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());
}
bool operator<(Coin a, Coin b){
	if (a.x < b.x){
		return true;
	}
	else if (a.x > b.x){
		return false;
	}
	else{
		return a.y < b.y;
	}
}
