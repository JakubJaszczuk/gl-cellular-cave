#version 450

layout(binding=0) uniform usampler2D automata_grid;
in vec2 texture_coord;
out vec4 out_color;

void main(void)
{
	out_color = texture(automata_grid, texture_coord).rrra * 0.6 + 0.2;
}
