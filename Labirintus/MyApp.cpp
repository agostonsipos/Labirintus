#include "MyApp.h"
#include "GLUtils.hpp"

//#include <GL/GLU.h>
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
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{ 
		//          x,  y, z               nx,ny,nz			 s, t
		{glm::vec3(-10, 0, -10), glm::vec3( 0, 1, 0), glm::vec2(0, 0)},
		{glm::vec3(-10, 0,  10), glm::vec3( 0, 1, 0), glm::vec2(0, 1)},
		{glm::vec3( 10, 0, -10), glm::vec3( 0, 1, 0), glm::vec2(1, 0)},
		{glm::vec3( 10, 0,  10), glm::vec3( 0, 1, 0), glm::vec2(1, 1)},
	};

	// indexpuffer adatai
    GLushort indices[]=
    {
		// 1. h�romsz�g
        0,1,2,
		// 2. h�romsz�g
        2,1,3,
    };

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
	// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				  sizeof(vert),		// ennyi b�jt nagys�gban
				  vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
				  GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat
	

	// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	); 

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	// text�rakoordin�t�k bekapcsol�sa a 2-es azonos�t�j� attrib�tom csatorn�n
	glEnableVertexAttribArray(2); 
	glVertexAttribPointer(
		2,
		2, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2*sizeof(glm::vec3)) );

	// index puffer l�trehoz�sa
	glGenBuffers(1, &m_ibID);
	// a VAO �szreveszi, hogy bind-olunk egy index puffert �s feljegyzi, hogy melyik volt ez!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	//
	// shaderek bet�lt�se
	//
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attrib�tumok hozz�rendel�se a shader v�ltoz�khoz
	// FONTOS: linkel�s el�tt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonos�t�ja, amib�l egy v�ltoz�hoz szeretn�nk hozz�rendel�st csin�lni
							0,				// a VAO-beli azonos�t� index
							"vs_in_pos");	// a shader-beli v�ltoz�n�v
	glBindAttribLocation( m_programID, 1, "vs_in_normal");
	glBindAttribLocation( m_programID, 2, "vs_in_tex0");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] S�der Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	//
	// egy�b inicializ�l�s
	//

	// vet�t�si m�trix l�trehoz�sa
	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 10000.0f );

	// shader-beli transzform�ci�s m�trixok c�m�nek lek�rdez�se
	m_loc_mvp = glGetUniformLocation( m_programID, "MVP");
	m_loc_world = glGetUniformLocation(m_programID, "world");
	m_loc_wit = glGetUniformLocation(m_programID, "WorldIT");

	m_loc_texture = glGetUniformLocation( m_programID, "texture" );

	m_loc_sunpos = glGetUniformLocation(m_programID, "Sp");
	m_loc_moonpos = glGetUniformLocation(m_programID, "Mp");

	m_loc_eye = glGetUniformLocation(m_programID, "eye");

	m_loc_kd = glGetUniformLocation(m_programID, "kd");
	m_loc_ks = glGetUniformLocation(m_programID, "ks");
	//
	// egy�b er�forr�sok bet�lt�se
	//

	// text�ra bet�lt�se
	m_floor_textureID = TextureFromFile("floor.bmp");
	m_bush_texture_ID = TextureFromFile("bush.bmp");
	m_coin_texture_ID = TextureFromFile("coin.bmp");
	m_diamond_texture_ID = TextureFromFile("diamond.bmp");
	m_fire_texture_ID = TextureFromFile("fire.bmp");
	m_brown_texture_ID = TextureFromFile("brown.bmp");

	// mesh betoltese
	m_suzanne = ObjParser::parse("Suzanne.obj");
	m_suzanne->initBuffers();

	m_bush = ObjParser::parse("bush.obj");
	m_bush->initBuffers();

	m_coin = ObjParser::parse("coin.obj");
	m_coin->initBuffers();

	m_diamond = ObjParser::parse("diamond.obj");
	m_diamond->initBuffers();

	m_shot = ObjParser::parse("shot.obj");
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

	glDeleteProgram( m_programID );
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
		// n�zeti transzform�ci� be�ll�t�sa
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
		m_matView = glm::lookAt(glm::vec3(suzpos.x * 20, 500, suzpos.y * 20-1), glm::vec3(suzpos.x * 20, 0, suzpos.y * 20), glm::vec3(0, 1, 0));
	}

	sunpos = glm::vec3(1000.0f*sinf(SDL_GetTicks() / 20000.0f), 1000.0f*cosf(SDL_GetTicks() / 20000.0f), 0.0f);
	moonpos = glm::vec3(-1000.0f*sinf(SDL_GetTicks() / 20000.0f), -1000.0f*cosf(SDL_GetTicks() / 20000.0f), 0.0f);

	if (SDL_GetTicks() - tm > 10000) mega = false;

	glUseProgram(m_programID);
	glUniform3fv(m_loc_sunpos, 1, &sunpos[0]);
	glUniform3fv(m_loc_moonpos, 1, &moonpos[0]);
	glUniform3fv(m_loc_eye, 1, &m_eye[0]);
	glUseProgram(0);

}


void CMyApp::DrawGround()
{
	// a talaj kirajzolasahoz szukseges shader beallitasa
	glUseProgram(m_programID);
	glUniform4fv(m_loc_kd, 1, &glm::vec4(1.0, 1.0, 1.0, 1.0)[0]);

	// shader parameterek be�ll�t�sa
	
	// aktiv�ljuk a 0-�s text�ra mintav�telez� egys�get
	glActiveTexture(GL_TEXTURE0);
	// aktiv�ljuk a gener�lt text�r�nkat 
	glBindTexture(GL_TEXTURE_2D, m_floor_textureID);
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(	m_loc_texture,	// ezen azonos�t�j� sampler 2D
					0);				// olvassa az ezen index� mintav�telez�t

	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	for (int i = 0; i < 50; ++i){
		for (int j = 0; j < 50; ++j){
			m_matWorld = glm::translate<float>(glm::vec3(20.f * i, 0.f, 20.f * j));
			glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
			glm::mat4 WIT = glm::inverse(m_matWorld);
			// majd k�ldj�k �t a megfelel� m�trixokat!
			glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
				1,			// egy darab m�trixot
				GL_FALSE,	// NEM transzpon�lva
				&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot

			glUniformMatrix4fv(m_loc_world,
				1,
				GL_FALSE,	// transzpon�lva
				&(m_matWorld[0][0]));
			glUniformMatrix4fv(m_loc_wit,
				1,
				GL_TRUE,	// transzpon�lva
				&(WIT[0][0]));

			// kirajzol�s
			glDrawElements(GL_TRIANGLES,		// primit�v t�pus
				6,					// hany csucspontot hasznalunk a kirajzolashoz
				GL_UNSIGNED_SHORT,	// indexek tipusa
				0);		// indexek cime
		}
	}
	// VAO kikapcsolasa
	glBindVertexArray(0);

	// text�ra kikapcsol�sa
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram( 0 );
}

void CMyApp::DrawBushes(){
	for (auto it : m_list_bushes){
		glUseProgram(m_programID);
		glUniform4fv(m_loc_kd, 1, &glm::vec4(0.0, 0.8, 0.0, 1.0)[0]);
		m_matWorld = glm::translate<float>(glm::vec3(it.ir?-10:0,0,it.ir?10:0))*glm::translate<float>(glm::vec3(it.x*20, 0.0, it.y*20-10.0))*glm::rotate<float>(it.ir?90:0,glm::vec3(0,1,0))
			*glm::translate<float>(glm::vec3(0,4,0))*glm::rotate<float>(90, glm::vec3(1, 0, 0))*glm::scale<float>(glm::vec3(5.0, 3.0, 5.0));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::inverse(m_matWorld);
		glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
			1,			// egy darab m�trixot
			GL_FALSE,	// NEM transzpon�lva
			&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
		glUniformMatrix4fv(m_loc_world,
			1,
			GL_FALSE,	// transzpon�lva
			&(m_matWorld[0][0]));
		glUniformMatrix4fv(m_loc_wit,
			1,
			GL_TRUE,	// transzpon�lva
			&(WIT[0][0]));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_bush_texture_ID);
		glUniform1i(m_loc_texture,	// ezen azonos�t�j� sampler 2D
			0);				// olvassa az ezen index� mintav�telez�t
		m_bush->draw();
	}
}

void CMyApp::DrawCoins(){
	for (auto it : m_list_coins){
		glUseProgram(m_programID);
		glUniform4fv(m_loc_kd, 1, &glm::vec4(1.0, 1.0, 0.0, 1.0)[0]);
		m_matWorld = glm::translate<float>(glm::vec3(it.x*20, 5, it.y*20))*glm::rotate<float>(2*3.14159*SDL_GetTicks()/100.0f,glm::vec3(0,1,0))*glm::rotate<float>(90,glm::vec3(1,0,0))*glm::scale<float>(glm::vec3(5,5,5));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::inverse(m_matWorld);
		glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
			1,			// egy darab m�trixot
			GL_FALSE,	// NEM transzpon�lva
			&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
		glUniformMatrix4fv(m_loc_world,
			1,
			GL_FALSE,	// transzpon�lva
			&(m_matWorld[0][0]));
		glUniformMatrix4fv(m_loc_wit,
			1,
			GL_TRUE,	// transzpon�lva
			&(WIT[0][0]));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_coin_texture_ID);
		glUniform1i(m_loc_texture,	// ezen azonos�t�j� sampler 2D
			0);				// olvassa az ezen index� mintav�telez�t
		m_coin->draw();
	}
}

void CMyApp::DrawDiamonds(){
	for (auto it : m_list_diamonds){
		glUseProgram(m_programID);
		glUniform4fv(m_loc_kd, 1, &glm::vec4(2.0, 2.0, 3.0, 1.0)[0]);
		m_matWorld = glm::translate<float>(glm::vec3(it.x * 20, 2, it.y * 20))*glm::rotate<float>(2 * 3.14159*SDL_GetTicks() / 100.0f, glm::vec3(0, 1, 0))*glm::scale<float>(glm::vec3(5, 5, 5))*glm::rotate<float>(90, glm::vec3(1, 0, 0))*glm::scale<float>(glm::vec3(2, 2, 2));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::inverse(m_matWorld);
		glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
			1,			// egy darab m�trixot
			GL_FALSE,	// NEM transzpon�lva
			&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
		glUniformMatrix4fv(m_loc_world,
			1,
			GL_FALSE,	// transzpon�lva
			&(m_matWorld[0][0]));
		glUniformMatrix4fv(m_loc_wit,
			1,
			GL_TRUE,	// transzpon�lva
			&(WIT[0][0]));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_diamond_texture_ID);
		glUniform1i(m_loc_texture,	// ezen azonos�t�j� sampler 2D
			0);				// olvassa az ezen index� mintav�telez�t
		m_diamond->draw();
	}
}
#include <iostream>
void CMyApp::DrawSuzanne()
{
	// a mesh kirajzolasahoz hasznalt shader bekapcsolasa
	glUseProgram(m_programID);
	if (mega)
		glUniform4fv(m_loc_ks, 1, &glm::vec4(2.0, 0.0, 0.0, 1.0)[0]);

	glUniform4fv(m_loc_kd, 1, &glm::vec4(1.0, 1.0, 1.0, 1.0)[0]);

	glm::mat4 jump;
	if (moving[0] || moving[1] || moving[2] || moving[3]){
		jump = glm::translate<float>(glm::vec3(0, 3 * fabs(sinf((SDL_GetTicks() - t0) * 4 * 3.14159f / 1000.0f)), 0));
		if (mega)
			jump = glm::translate<float>(glm::vec3(0, 3 * fabs(sinf((SDL_GetTicks() - t0) * 4 * 3.14159f / 750.0f)), 0));
	}
	else if (win)
		jump = glm::translate<float>(glm::vec3(0, log1pf(money) * fabs(sinf((SDL_GetTicks() - t0) * 4 * 3.14159f / 1000.0f)), 0));
	else if (lose)
		jump = glm::rotate<float>(glm::clamp(-30.0f * (SDL_GetTicks() - tl) / 1000.0f, -30.0f, 0.0f), glm::vec3(m_up.z, 0, -m_up.x));
	else jump = glm::mat4(1.0f);

	glm::mat4 size;
	if (mega)
		size = glm::scale<float>(glm::vec3(4.5f, 4.5f, 4.5f));
	else size = glm::scale<float>(glm::vec3(3, 3, 3));

	m_matWorld = glm::translate<float>(glm::vec3(suzpos.x*20, 3, suzpos.y*20))*jump*size*glm::rotate<float>(suzpos.ir*90,glm::vec3(0,1,0));
	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
	glm::mat4 WIT = glm::inverse(m_matWorld);
	// majd k�ldj�k �t a megfelel� m�trixokat!
	glUniformMatrix4fv( m_loc_mvp,// erre a helyre t�lts�nk �t adatot
						1,			// egy darab m�trixot
						GL_FALSE,	// NEM transzpon�lva
						&(mvp[0][0]) ); // innen olvasva a 16 x sizeof(float)-nyi adatot
	glUniformMatrix4fv(m_loc_world,
		1,
		GL_FALSE,	// transzpon�lva
		&(m_matWorld[0][0]));
	glUniformMatrix4fv(m_loc_wit,
		1,
		GL_TRUE,	// transzpon�lva
		&(WIT[0][0]));

	// aktiv�ljuk a 0-�s text�ra mintav�telez� egys�get
	glActiveTexture(GL_TEXTURE0);
	// aktiv�ljuk a gener�lt text�r�nkat 
	glBindTexture(GL_TEXTURE_2D, m_brown_texture_ID);
	// text�ra mintav�telez� �s shader-beli sampler2D �sszerendel�se
	glUniform1i(	m_loc_texture,	// ezen azonos�t�j� sampler 2D
					0);				// olvassa az ezen index� mintav�telez�t


	m_suzanne->draw();

	glUniform4fv(m_loc_ks, 1, &glm::vec4(1.0, 1.0, 1.0, 1.0)[0]);

	glUseProgram( 0 );
}

void CMyApp::DrawShots(){
	for (auto it : m_list_shots){
		glUseProgram(m_programID);
		glUniform4fv(m_loc_kd, 1, &glm::vec4(2.0, 2.0, 3.0, 1.0)[0]);
		m_matWorld = glm::translate<float>(glm::vec3(it.x * 20, 2, it.y * 20))*glm::rotate<float>(2 * 3.14159*SDL_GetTicks() / 100.0f, glm::vec3(0, 1, 0))*glm::scale<float>(glm::vec3(5, 5, 5));
		glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
		glm::mat4 WIT = glm::inverse(m_matWorld);
		glUniformMatrix4fv(m_loc_mvp,// erre a helyre t�lts�nk �t adatot
			1,			// egy darab m�trixot
			GL_FALSE,	// NEM transzpon�lva
			&(mvp[0][0])); // innen olvasva a 16 x sizeof(float)-nyi adatot
		glUniformMatrix4fv(m_loc_world,
			1,
			GL_FALSE,	// transzpon�lva
			&(m_matWorld[0][0]));
		glUniformMatrix4fv(m_loc_wit,
			1,
			GL_TRUE,	// transzpon�lva
			&(WIT[0][0]));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_fire_texture_ID);
		glUniform1i(m_loc_texture,	// ezen azonos�t�j� sampler 2D
			0);				// olvassa az ezen index� mintav�telez�t
		m_shot->draw();
	}
}

void CMyApp::Render()
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawGround();
	DrawBushes();
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
	Bush w = { s.x >= z.x ? s.x : z.x, s.y >= z.y ? s.y : z.y, s.x == z.x ? 0 : 1 };
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
