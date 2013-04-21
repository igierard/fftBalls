#version 120
#extension GL_EXT_gpu_shader4 : require
varying vec4 vertex_pos;
varying vec3 vertex_norm;
uniform sampler2DRect fftData;
uniform int fftDataW;
uniform int fftDataH;
uniform int ballSegmentsH;
uniform int ballSegmentsR;
uniform int currentSampleIndex;
uniform float s;
uniform float r;
void main()
{
    mat4 m = gl_ModelViewProjectionMatrix;
    vec4 t =  gl_Vertex;
    vertex_norm = gl_Normal;
    vertex_pos = t;
    mat3 norm_mat = gl_NormalMatrix;
    
    vec3 normal, lightDir;
	vec4 diffuse;
	float NdotL;
    
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    lightDir = normalize(vec3(gl_LightSource[0].position));
    normal = normalize(norm_mat * vertex_norm);
    NdotL = max(dot(normal, lightDir), 0.0);
    
    int id = gl_VertexID;
    int row = (id-1)/(ballSegmentsR+1);
    
    int col = id-row*(ballSegmentsR+1);
    row = (row + currentSampleIndex+1) % ballSegmentsH;
    vec4 val = texture2DRect(fftData,vec2(float(col)/float(1),float(row)/float(1)));
    float tex_val = val.z;
    vec3 a = vertex_norm*tex_val*s*r;
    vec4 b = vec4(a.x,a.y,a.z,0.0);
    vertex_pos += b;
	
	gl_FrontColor =  NdotL * diffuse;

    gl_Position =  m*vertex_pos;
}