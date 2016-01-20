#version 130

in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;
out vec4 fs_out_col;

uniform sampler2D texture;
uniform sampler2D texture2;

uniform vec3 eye;

uniform vec4 Sd = vec4(1.0,1.0,0.8,1.0); //nap - diff�z f�ny
uniform vec4 Ss = vec4(1.0,1.0,0.6,1.0); //nap - spekul�ris f�ny

uniform vec3 Sp; //nap - poz�ci�

uniform vec4 Md = vec4(0.4,0.4,0.5,1.0); //hold - diff�z f�ny
uniform vec4 Ms = vec4(1.0,1.0,1.0,1.0); //hold - spekul�ris f�ny

uniform vec3 Mp; //hold - poz�ci�

uniform vec4 ka = vec4(0.1,0.1,0.1,1); //ambiens f�ny
uniform vec4 kd = vec4(1,1,1,1); // diff�z f�ny visszaver�si ar�nya
uniform vec4 ks = vec4(1,1,1,1); // spekul�ris f�ny visszaver�si ar�nya
uniform float sp = 16; //csillan�si m�rt�k


void main()
{
	vec4 ambient;
	//diff�z
	vec4 diffuse;
	vec4 specular;
	if(Sp.y > 0){ //napf�ny
		ambient = Sd * ka;
		vec3 Slightdir = normalize(Sp - vs_out_pos);
		float Scosa = dot(normalize(vs_out_normal),Slightdir);
		float Sdi = clamp(Scosa,0,1);
		diffuse = Sdi * Sd * kd;
		//spekul�ris
		if(Scosa > 0){
			vec3 Slightdir = normalize(Sp - vs_out_pos);
			vec3 r = normalize(reflect(-Slightdir,normalize(vs_out_normal)));
			vec3 e = normalize(eye-vs_out_pos);
			float si = dot(normalize(vs_out_normal),Slightdir)>=0 ? pow(clamp(dot(e,r),0,1),sp) : 0;
			specular = si*Ss*ks;
		}
	}
	else{ //holdf�ny
		ambient = Md * ka;
		vec3 Mlightdir = normalize(Mp - vs_out_pos);
		float Mcosa = dot(normalize(vs_out_normal),Mlightdir);
		float Mdi = clamp(Mcosa,0,1);
		diffuse = Mdi * Md * kd;
		//spekul�ris
		if(Mcosa > 0){
			vec3 Mlightdir = normalize(Mp - vs_out_pos);
			vec3 r = normalize(reflect(-Mlightdir,normalize(vs_out_normal)));
			vec3 e = normalize(eye-vs_out_pos);
			float mi = dot(normalize(vs_out_normal),Mlightdir)>=0 ? pow(clamp(dot(e,r),0,1),sp) : 0;
			specular = mi*Ms*ks;
		}
	}



	fs_out_col = (ambient+diffuse+specular)*texture2D(texture, vs_out_tex0.st);
}