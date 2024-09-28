#include"UBHS_Core.h"

const char* ubhs::spriteVertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec4 vertex;"
"out vec2 texCoords;"
"uniform mat4 transform;"
"uniform mat4 projection;"
"void main()"
"{"
"gl_Position = projection * transform * vec4(vertex.xy, 0.0, 1.0);"
"texCoords = vertex.zw;"
"}";

const char* ubhs::spriteFragmentShaderSource =
"#version 330 core\n"
"out vec4 fragColor;"
"in vec2 texCoords;"
"uniform sampler2D texture0;"
"uniform vec3 spriteColor;"
"float alphaCutout = 0.01;"
"void main()"
"{"
"vec4 texColor = texture(texture0, texCoords);"
"if (texColor.a < alphaCutout) discard;"
"fragColor = vec4(spriteColor, 1.0) * texture(texture0, texCoords);"
"}";