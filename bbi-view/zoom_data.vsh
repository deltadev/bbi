#version 150
//
// zoom_data vertex shader
//
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

in vec4 aColor;
in vec3 aVertex;
in vec3 aNormal;

smooth out vec4 vColor;

void main (void) 
{
  aNormal;
  uNormalMatrix;
  uModelViewMatrix;

  vColor.rgb = aColor.rgb;
  vColor.a = aColor.a;

  vec3 vOut = aVertex;

  vec4 v = uModelViewProjectionMatrix * vec4(vOut, 1.0);



  gl_Position = v;
}
