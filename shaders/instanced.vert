#version 330

in layout(location = 0) vec3 vs_in_pos;
in layout(location = 1) vec3 vs_in_normal;
in layout(location = 2) vec2 vs_in_tex0;
in layout(location = 3) mat4 world;
in layout(location = 7) mat4 worldIT;

out vec3 vs_out_normal;
out vec2 vs_out_tex0;
out vec3 vs_out_pos;

uniform mat4 VP;

void main()
{
	vec4 worldPos = world * vec4( vs_in_pos, 1 );
	gl_Position = VP * worldPos;

	vs_out_pos = worldPos.xyz;
	vs_out_normal  = (worldIT * vec4(vs_in_normal, 0)).xyz;
	vs_out_tex0 = vs_in_tex0;
}
