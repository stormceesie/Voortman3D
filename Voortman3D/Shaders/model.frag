#version 450

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);
	vec3 V = normalize(inViewVec);
	vec3 R = reflect(-L, N);

	// Verhoog de ambient en diffuse intensiteit
	vec3 ambient = vec3(0.2);  // Lichte verhoging van de omgevingsverlichting
	vec3 diffuse = max(dot(N, L), 0.0) * vec3(1.0);
	
	// Verlaag de specular intensiteit en exponent
	vec3 specular = pow(max(dot(R, V), 0.0), 8.0) * vec3(0.3); // Minder intens en bredere reflectie

	outFragColor = vec4((ambient + diffuse) * inColor.rgb + specular, 1.0);		
}