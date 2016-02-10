
#include "post_processing_vertex_shader.glsl"


##GL_FRAGMENT_SHADER
#version 400

#include "post_processing_helper_fs.glsl"


vec3 unpackNormal3 (vec2 enc)
{
    vec2 fenc = enc*4-vec2(2);
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}

void getDepthMinMax(sampler2D tex, vec2 tc, out float dMin, out float dMax){
    float dNW = texture(tex, tc + (vec2(-1.0, -1.0) * screenSize.zw)).r;
    float dNE = texture(tex, tc + (vec2(+1.0, -1.0) * screenSize.zw)).r;
    float dSW = texture(tex, tc + (vec2(-1.0, +1.0) * screenSize.zw)).r;
    float dSE = texture(tex, tc + (vec2(+1.0, +1.0) * screenSize.zw)).r;
    
    dMin = min(min(dNW, dNE), min(dSW, dSE));
    dMax = max(max(dNW, dNE), max(dSW, dSE));
}

void getNormalMinMax(sampler2D tex, vec2 tc, out vec3 dMin, out vec3 dMax){
    vec3 dNW = texture(tex, tc + (vec2(-1.0, -1.0) * screenSize.zw)).xyz;
    vec3 dNE = texture(tex, tc + (vec2(+1.0, -1.0) * screenSize.zw)).xyz;
    vec3 dSW = texture(tex, tc + (vec2(-1.0, +1.0) * screenSize.zw)).xyz;
    vec3 dSE = texture(tex, tc + (vec2(+1.0, +1.0) * screenSize.zw)).xyz;
    
    dNW = unpackNormal3(dNW.xy);
    dNE = unpackNormal3(dNE.xy);
    dSW = unpackNormal3(dSW.xy);
    dSE = unpackNormal3(dSE.xy);
    dMin = min(min(dNW, dNE), min(dSW, dSE));
    dMax = max(max(dNW, dNE), max(dSW, dSE));
}

void getColorMinMax(sampler2D tex, vec2 tc, out vec3 dMin, out vec3 dMax){
    vec3 dNW = texture(tex, tc + (vec2(-1.0, -1.0) * screenSize.zw)).xyz;
    vec3 dNE = texture(tex, tc + (vec2(+1.0, -1.0) * screenSize.zw)).xyz;
    vec3 dSW = texture(tex, tc + (vec2(-1.0, +1.0) * screenSize.zw)).xyz;
    vec3 dSE = texture(tex, tc + (vec2(+1.0, +1.0) * screenSize.zw)).xyz;
    

    dMin = min(min(dNW, dNE), min(dSW, dSE));
    dMax = max(max(dNW, dNE), max(dSW, dSE));
}

void getLumaMinMax(sampler2D tex, vec2 tc, out float lumaMin, out float lumaMax){
    vec3 dNW = texture(tex, tc + (vec2(-1.0, -1.0) * screenSize.zw)).xyz;
    vec3 dNE = texture(tex, tc + (vec2(+1.0, -1.0) * screenSize.zw)).xyz;
    vec3 dSW = texture(tex, tc + (vec2(-1.0, +1.0) * screenSize.zw)).xyz;
    vec3 dSE = texture(tex, tc + (vec2(+1.0, +1.0) * screenSize.zw)).xyz;
    
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(dNW, luma);
    float lumaNE = dot(dNE, luma);
    float lumaSW = dot(dSW, luma);
    float lumaSE = dot(dSE, luma);

    lumaMin = min(min(lumaNW, lumaNE), min(lumaSW, lumaSE));
    lumaMax = max(max(lumaNW, lumaNE), max(lumaSW, lumaSE));
}



void main() {
    float n = 1;
    float f = 30;
    float distanceThreshold = 1990.015f;
    float normalThreshold = 10.45f;
    float colorThreshold = 0.01f;
    float lumaThreshold = 1990.1f;
    //TODO:
    //check normals or colors


    float dMin,dMax, dDiff;
    getDepthMinMax(gbufferDepth,tc,dMin,dMax);
    dMin = linearDepth(dMin,n,f);
    dMax = linearDepth(dMax,n,f);
    dDiff = dMax-dMin;


    vec3 nMin,nMax, nDiff;
    getNormalMinMax(gbufferNormals,tc,nMin,nMax);
    nDiff = nMax-nMin;
    
    vec3 cMin,cMax, cDiff;
    getColorMinMax(gbufferColor,tc,cMin,cMax);
    cDiff = cMax-cMin;
    
    float lMin,lMax, lDiff;
    getLumaMinMax(gbufferColor,tc,lMin,lMax);
    lDiff = lMax-lMin;
    
    out_color = texture(image,tc).rgb;

    float asdf = dot(cDiff,vec3(1));

    if( length(cDiff) > colorThreshold || length(nDiff) > normalThreshold || dDiff > distanceThreshold|| lDiff > lumaThreshold)
        out_color = vec3(0.0);

//    if(dDiff > distanceThreshold)
//        out_color = vec3(0);
        
//    out_color = vec3(dMax-dMin);

}


