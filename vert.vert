#version 450

out vec2 texture_coord;

void main(void)
{
	const vec4[] vertices = vec4[](
		vec4(-1, -1, 0, 1.0),
		vec4(1, -1, 0, 1.0),
		vec4(-1, 1, 0, 1.0),
		vec4(1, 1, 0, 1.0)
	);

	const vec2[] texture_coordinates = vec2[](
		vec2(0, 0), 
		vec2(1, 0), 
		vec2(0, 1), 
		vec2(1, 1)
	);

	gl_Position = vertices[gl_VertexID];
	texture_coord = texture_coordinates[gl_VertexID];
}
