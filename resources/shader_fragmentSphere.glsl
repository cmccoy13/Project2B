#version 330 core
out vec3 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec4 gl_FragCoord;
uniform vec4 Center;
//uniform vec3 campos;
void main()
{
	vec3 n = normalize(vertex_normal);
	vec3 lp=vec3(100,10,100);
	vec3 ld = normalize(vertex_pos - lp);
	float diffuse = dot(n,ld);

	float dist = distance(gl_FragCoord, Center);

	color = vec3(0, 0, 0);
	color += dist/180;
	if(vertex_pos.z > 0.0)
	{
		discard;
	}
}
