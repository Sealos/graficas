//Bias Diffuse variables
uniform float bias = 0.5;
uniform float sharpness = 0.840;
uniform float roughness = 0.140;

//Cook-Torrance variables
uniform float m;
uniform float indexR = 5.500f;
uniform float R0;
uniform float refraction = 1.0f;

//Fresnel
uniform float eta = 1.0f;
uniform float Kfr =1.0f;

uniform float specularIntensity = 1.0f;
uniform float diffuseIntensity = 1.0f;

uniform float fresnel = 0.0f;
uniform float cookSpec = 1.0f;
uniform float glossySharp = 0.0f;
uniform float biasedDiff = 1.0f;

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

float fresnelCT(){ 
	return R0 + (1.0- R0) * pow(1.0- dot(L.xyz,N.xyz),5.0);
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

float biasFunc(float t,float a){

	return pow(t, -(log(a)/log(2.0)));
}

void main (void)  
{     
   
   vec4 cFinal = vec4(0.0,0.0,0.0,1.0);
   vec4 iSpec;
   vec4 iDiff;
   vec3 H = normalize(L.xyz + normalize(camDirection));
   iSpec = cookTorrance(H)* cLightSpec*cMatSpec;
   iSpec = clamp(iSpec,0.0,1.0);

   //Componente difuso.
   iDiff = biasFunc(dot(N,L.xyz),bias) * (cLightDiff * cMatDiff);
   iDiff = clamp(iDiff,0.0,1.0);


   cFinal =vec4(10.0,0.0,0.0,1.0)*cLightAmb*cMatAmb + iDiff + iSpec;

   cFinal.w = 1.0;
   gl_FragColor = cFinal;

}    
