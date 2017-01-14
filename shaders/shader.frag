#version 330

layout(early_fragment_tests) in;

in vec3 vs_out_pos;
in vec3 vs_out_normal;
in vec2 vs_out_tex0;
out vec4 fs_out_col;

uniform sampler2D texture;
uniform sampler2D texture2;

uniform vec3 eye;

uniform vec4 Sd = vec4(1.0,1.0,0.8,1.0); //sun - diffuse
uniform vec4 Ss = vec4(1.0,1.0,0.6,1.0); //sun - specular

uniform vec3 Sp; //sun - position

uniform vec4 Md = vec4(0.4,0.4,0.5,1.0); //moon - diffuse
uniform vec4 Ms = vec4(1.0,1.0,1.0,1.0); //moon - specular

uniform vec3 Mp; //moon - position

uniform vec4 ka = vec4(0.1,0.1,0.1,1);
uniform vec4 kd = vec4(1,1,1,1);
uniform vec4 ks = vec4(1,1,1,1);
uniform float sp = 16;


void main()
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	if(Sp.y > 0){ //sun is up
		ambient = Sd * ka;
		vec3 Slightdir = normalize(Sp - vs_out_pos);
		float Scosa = dot(normalize(vs_out_normal),Slightdir);
		float Sdi = clamp(Scosa,0,1);
		diffuse = Sdi * Sd * kd;
		if(Scosa > 0){
			vec3 Slightdir = normalize(Sp - vs_out_pos);
			vec3 r = normalize(reflect(-Slightdir,normalize(vs_out_normal)));
			vec3 e = normalize(eye-vs_out_pos);
			float si = pow(clamp(dot(e,r),0,1),sp);
			specular = si*Ss*ks;
		}
	}
	else{ //moon is up
		ambient = Md * ka;
		vec3 Mlightdir = normalize(Mp - vs_out_pos);
		float Mcosa = dot(normalize(vs_out_normal),Mlightdir);
		float Mdi = clamp(Mcosa,0,1);
		diffuse = Mdi * Md * kd;
		if(Mcosa > 0){
			vec3 Mlightdir = normalize(Mp - vs_out_pos);
			vec3 r = normalize(reflect(-Mlightdir,normalize(vs_out_normal)));
			vec3 e = normalize(eye-vs_out_pos);
			float mi = pow(clamp(dot(e,r),0,1),sp);
			specular = mi*Ms*ks;
		}
	}



	fs_out_col = (ambient+diffuse+specular)*texture2D(texture, vs_out_tex0.st);
}
