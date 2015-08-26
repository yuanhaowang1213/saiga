
##GL_VERTEX_SHADER

#version 400
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec3 in_color;
layout(location=3) in vec3 in_data;

//instancing
layout(location=4) in mat4 in_model;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 MV;
uniform mat4 MVP;

out vec3 normal;
out vec3 normalW;
out vec3 vertexMV;
out vec3 vertex;
out vec3 color;
out vec3 data;

void main() {
    color = in_color;
    normal = normalize(vec3(view*in_model * vec4( in_normal, 0 )));
    normalW = normalize(vec3(in_model * vec4( in_normal, 0 )));
    vertexMV = vec3(view * in_model * vec4( in_position, 1 ));
    vertex = vec3(in_model * vec4( in_position, 1 ));
    data = in_data;
    gl_Position = proj*view *in_model* vec4(in_position,1);
}





##GL_FRAGMENT_SHADER

#version 400
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 normal;
in vec3 normalW;
in vec3 vertexMV;
in vec3 vertex;
in vec3 color;
in vec3 data;

#include "deferred_fs.glsl"


void main() {
    setGbufferData(color,vertexMV,normal,data);
}


