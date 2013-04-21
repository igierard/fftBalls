#version 120
#extension GL_EXT_gpu_shader4 : require
varying vec4 vertex_pos;
varying vec3 vertex_norm;
uniform float r;
uniform float s;
uniform sampler2DRect fftData;
uniform vec3 color;
void main()
{
    // Setting Each Pixel To Red
  float t;
  vec3 target = vertex_norm*r;
  vec3 pos = vec3(vertex_pos.x,vertex_pos.y,vertex_pos.z);
  vec3 delta = pos-target;
  t = length(delta)/(r*s);
  t = max(0.0,min(1.0,t));
    float blend = 0.75;
//    t *= blend;
    
    vec3 normal, lightDir;
	vec4 diffuse;
	float NdotL;
    mat3 norm_mat = gl_NormalMatrix;
    
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    lightDir = normalize(vec3(gl_LightSource[0].position));
    normal = normalize(norm_mat * vertex_norm);
    NdotL = max(dot(normal, lightDir), 0.0);
    
    vec4 color2 = vec4(1.0,1.0,1.0,1.0);
    color2 = NdotL * diffuse;
    blend = 1-t;
    gl_FragColor = vec4(color.x*t,color.y*t,color.z*t, 1.0)+ vec4(color2.x*blend,color2.y*blend,color2.z*blend,1.0);
}