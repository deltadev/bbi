#version 150
//
// Default vertex shader
//
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat3 uNormalMatrix;

in vec4 aColor;
in vec3 aVertex;
in vec3 aNormal;

smooth out vec4 vColor;

const vec3 lightDirection = vec3(1,1,0.1);

void main (void) 
{
    //gl_PointSize = 10.0;
    vec3 normal = uNormalMatrix * aNormal;
    normal = normalize(normal);
    
    uModelViewMatrix;
    vec3 camLightDir = lightDirection;
    camLightDir = normalize(camLightDir);
    float intensity = clamp(dot(normal, camLightDir), 0, 1);
    
    vColor.rgb = aColor.rgb * intensity;
    vColor.a = aColor.a;
    
    gl_Position	= uModelViewProjectionMatrix * vec4(aVertex, 1.0);

}
