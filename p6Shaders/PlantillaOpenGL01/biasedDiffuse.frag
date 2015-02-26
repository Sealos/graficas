#version 120

//Bias Diffuse variables
uniform float bias = 0.5;
uniform float sharpness = 0.840;
uniform float roughness = 0.140;

//Cook-Torrance variables
uniform float m;
uniform float R0;
uniform float refraction;

//Fresnel
uniform float eta = 1.0;
uniform float Kfr = 1.0;

uniform float specularIntensity = 1.0;
uniform float diffuseIntensity = 1.0;

uniform float fresnel;
uniform float cookSpec;
uniform float glossySharp;
uniform float biasedDiff;

varying vec4 cLightDiff, cLightAmb, cLightSpec;
varying vec4 cMatDiff, cMatAmb, cMatSpec;
varying vec3 camDirection;
varying vec3 N;
varying vec4 L;
varying vec4 V;

float distro(vec3 H){
	float ndoth = dot(N,H);
	float beta = acos(ndoth);
	float tanbeta = tan(beta);
	float tanbeta_over_m = tanbeta/m;
	float D = exp(-(tanbeta_over_m * tanbeta_over_m));
	D = D/(4.0*m*m*pow(ndoth,4.0));
	return D;
}

float geom(vec3 H){
	float ndoth = dot(N,H);
	float ndotv = dot(N,normalize(camDirection));
	float ndotl = dot(N,L.xyz);
	float vdoth = dot(normalize(camDirection),H);

	float masking = 2.0*ndoth*ndotv/vdoth;
	float shadowing = 2.0*ndoth*ndotl/vdoth;
	return min (1.0, min(masking,shadowing));
}

float fresnelCT() { 
	return refraction + (1.0- refraction) * pow(1.0- dot(L.xyz,N.xyz),5.0);
}

float cookTorrance(vec3 H){
	float cook = 0.0;
	float D = distro(H);
	float G = geom(H);
	float F = fresnelCT();
	cook = D*G*F;

	float vdotn =  dot(normalize(camDirection),N);
	cook = cook / vdotn;

	return cook;
}

float glossyComp(vec3 H){
	float c = 0.0;
	float w = 0.18 * (1.0 - sharpness);
	
	c = smoothstep(0.72 - w, 0.72 + w, pow( max(0.0, dot(N,H)), 1.0/roughness));
	return c;
}

float biasFunc(float t,float a) {
	return pow(t, -(log(a)/log(2.0)));
}

vec4 fresnelSchlick(vec3 H) {

	vec3 Nn = normalize(N);
	vec3 Vn = -normalize(H);

	float dotnv = abs(dot(Nn, Vn));
	float Kr = eta + (1 - eta) * pow(1 - dotnv, 5);
	Kr = Kfr * biasFunc(Kr, bias);
	
	vec4 color = vec4(Kr, Kr, Kr, 1.0);

	return color;
	
}

void main (void)  
{
	vec4 iSpec = vec4(1.0,1.0,1.0,1.0);
	vec4 iDiff = vec4(1.0,1.0,1.0,1.0);

	vec4 cFinal = vec4(0.0,0.0,0.0,1.0);
	vec3 H = normalize(L.xyz + normalize(camDirection));
	if (cookSpec > 0.0)
		iSpec = cookTorrance(H) * cLightSpec * cMatSpec;

	if (glossySharp > 0.0)
		iSpec = glossyComp(H) * cLightSpec * cMatSpec;
	
	iSpec = clamp(iSpec,0.0,1.0) * specularIntensity;

	//Componente difuso.
	if (biasedDiff > 0.0)
		iDiff = biasFunc(dot(N,L.xyz),bias) * (cLightDiff * cMatDiff);

	iDiff = clamp(iDiff,0.0,1.0) * diffuseIntensity;

	if (fresnel > 0.0)
		cFinal = fresnelSchlick(camDirection)*cLightAmb*cMatAmb;
	else
		cFinal = vec4(10.0,0.0,0.0,1.0)*cLightAmb*cMatAmb + iDiff + iSpec;

	cFinal.w = 1.0;
	gl_FragColor = cFinal;

}  
