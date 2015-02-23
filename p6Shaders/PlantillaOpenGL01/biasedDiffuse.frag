uniform float bias;
uniform float sharpness = 0.840;
uniform float roughness = 0.140;
varying vec4 cLightDiff, cLightAmb, cLightSpec;
varying vec4 cMatDiff, cMatAmb, cMatSpec;
varying vec3 camDirection;
varying vec3 N;
varying vec4 L;
varying vec4 V;


float biasFunc(float t,float a){

	return pow(t, -(log(a)/log(2)));
}

void main (void)  
{     
   
   vec4 cFinal = vec4(0.0,0.0,0.0,1.0);
   float iDiff, iSpec;
   vec3 vRef;



    //Componente Specular Glossy
   float g = .18 * (1-sharpness);
   vec4 H = normalize(normalize(L)+V);
   float derp = dot(H.xyz,N);
   //iSpec = smoothstep( .72-g, .72+g, pow(max(0,derp),1/roughness));

   vRef = -normalize(reflect(L.xyz,N));
   iSpec = smoothstep( .72-g, .72+g, pow(max(dot(vRef, normalize(camDirection)), 0.0),1/roughness));
   //iSpec = pow(max(dot(vRef, normalize(camDirection)), 0.0),10.0);

   //Componente difuso.
   iDiff = max(biasFunc(dot(normalize(N),normalize(L.xyz)),bias), 0.0) ;

   cFinal = vec4(10.0,0.0,0.0,1.0)*cLightAmb*cMatAmb + iDiff*(cLightDiff*cMatDiff) + iSpec*(cLightSpec*cMatSpec);

   cFinal.w = 1.0;
   gl_FragColor = cFinal;

}    
