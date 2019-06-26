#version 450 core

struct PointLight {
	vec3 position;
	vec3 color;
	float attConstant;
	float attLinear;
	float attQuadratic;
};
struct DirectionalLight {
	vec3 direction;
	vec3 color;
};
struct SpotLight {
	vec3 position;
	vec3 direction;
	float cosineInnerCutoff;
	float cosineOuterCutoff;
	vec3 color;
};


out vec4 fragmentColor;

in vec3 v2fNormal;
in vec3 v2fTextureCoord;
in vec3 v2fWorldFragmentPosition;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform PointLight pointLights[16];
uniform int numPointLights;
uniform DirectionalLight dirLight;
uniform SpotLight spotLights[16];
uniform int numSpotLights;
uniform vec3 cameraPosition;

const float PI = 3.14159265359;


vec3 getNormalFromMap() {
	vec3 tangentNormal = texture(normalMap, v2fTextureCoord.xy).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v2fWorldFragmentPosition);
    vec3 Q2  = dFdy(v2fWorldFragmentPosition);
    vec2 st1 = dFdx(v2fTextureCoord.xy);
    vec2 st2 = dFdy(v2fTextureCoord.xy);

    vec3 N   = normalize(v2fNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float TrowbridgeReitzNDF(vec3 normal, vec3 halfway, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, halfway), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0)) + 1.0;
	denom = PI * denom * denom;

	return a2 / max(denom, 0.001);
}

float SchlickGGX(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}
float SmithGeometry(vec3 normal, vec3 viewingDirection, vec3 lightDirection, float roughness) {
	float NdotV = max(dot(normal, viewingDirection), 0.0);
	float NdotL = max(dot(normal, lightDirection), 0.0);
	float ggx1 = SchlickGGX(NdotV, roughness);
	float ggx2 = SchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosine, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosine, 5.0);
}

vec3 pointLightsContribution() {
	vec3 albedo = pow(texture(albedoMap, v2fTextureCoord.xy).rgb, vec3(2.2));
	float metallic = texture(metallicMap, v2fTextureCoord.xy).r;
	float roughness = texture(roughnessMap, v2fTextureCoord.xy).r;
	float ao = texture(aoMap, v2fTextureCoord.xy).r;

	vec3 normal = getNormalFromMap();
	vec3 viewingDirection = normalize(cameraPosition - v2fWorldFragmentPosition);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < numPointLights; ++i) {
		vec3 lightDirection = normalize(pointLights[i].position - v2fWorldFragmentPosition);
		vec3 halfway = normalize(viewingDirection + lightDirection);
		float lightDistance = length(pointLights[i].position - v2fWorldFragmentPosition);
		float attenuation = 1.0 / (
			pointLights[i].attConstant + 
			(pointLights[i].attLinear * lightDistance) +
			(pointLights[i].attQuadratic * lightDistance * lightDistance)
		);
		vec3 radiance = pointLights[i].color * attenuation;

		float D = TrowbridgeReitzNDF(normal, viewingDirection, roughness);
		float G = SmithGeometry(normal, viewingDirection, lightDirection, roughness);
		vec3 F = FresnelSchlick(clamp(dot(halfway, viewingDirection), 0.0, 1.0), F0);

		vec3 nom = D * G * F;
		float denom = 4 * max(dot(normal, viewingDirection), 0.0) * max(dot(normal, lightDirection), 0.0);
		vec3 specular = nom / max(denom, 0.001);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= (1.0 - metallic);

		float NdotL = max(dot(normal, lightDirection), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	 
	vec3 ambient = vec3(0.03) *	ao * albedo;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	return color;
}

vec3 directionalLightContribution() {
	vec3 albedo = pow(texture(albedoMap, v2fTextureCoord.xy).rgb, vec3(2.2));
	float metallic = texture(metallicMap, v2fTextureCoord.xy).r;
	float roughness = texture(roughnessMap, v2fTextureCoord.xy).r;
	float ao = texture(aoMap, v2fTextureCoord.xy).r;

	vec3 normal = getNormalFromMap();
	vec3 viewingDirection = normalize(cameraPosition - v2fWorldFragmentPosition);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);


	vec3 lightDirection = normalize(-dirLight.direction);
	vec3 halfway = normalize(viewingDirection + lightDirection);
	vec3 radiance = dirLight.color;

	float D = TrowbridgeReitzNDF(normal, viewingDirection, roughness);
	float G = SmithGeometry(normal, viewingDirection, lightDirection, roughness);
	vec3 F = FresnelSchlick(clamp(dot(halfway, viewingDirection), 0.0, 1.0), F0);

	vec3 nom = D * G * F;
	float denom = 4 * max(dot(normal, viewingDirection), 0.0) * max(dot(normal, lightDirection), 0.0);
	vec3 specular = nom / max(denom, 0.001);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= (1.0 - metallic);
	float NdotL = max(dot(normal, lightDirection), 0.0);
	vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;


	vec3 ambient = vec3(0.03) *	ao * albedo;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	return color;
}

vec3 spotLightsContribution() {
	vec3 albedo = pow(texture(albedoMap, v2fTextureCoord.xy).rgb, vec3(2.2));
	float metallic = texture(metallicMap, v2fTextureCoord.xy).r;
	float roughness = texture(roughnessMap, v2fTextureCoord.xy).r;
	float ao = texture(aoMap, v2fTextureCoord.xy).r;

	vec3 normal = getNormalFromMap();
	vec3 viewingDirection = normalize(cameraPosition - v2fWorldFragmentPosition);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < numSpotLights; ++i) {
		vec3 lightDirection = normalize(spotLights[i].position - v2fWorldFragmentPosition);
		float theta = dot(lightDirection, normalize(-spotLights[i].direction));
		float epsilon = spotLights[i].cosineInnerCutoff - spotLights[i].cosineOuterCutoff;
		float intensity = clamp((theta - spotLights[i].cosineOuterCutoff) / epsilon, 0.0, 1.0);

		vec3 halfway = normalize(viewingDirection + lightDirection);
		vec3 radiance = spotLights[i].color * intensity;

		float D = TrowbridgeReitzNDF(normal, viewingDirection, roughness);
		float G = SmithGeometry(normal, viewingDirection, lightDirection, roughness);
		vec3 F = FresnelSchlick(clamp(dot(halfway, viewingDirection), 0.0, 1.0), F0);

		vec3 nom = D * G * F;
		float denom = 4 * max(dot(normal, viewingDirection), 0.0) * max(dot(normal, lightDirection), 0.0);
		vec3 specular = nom / max(denom, 0.001);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= (1.0 - metallic);

		float NdotL = max(dot(normal, lightDirection), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	 
	vec3 ambient = vec3(0.03) *	ao * albedo;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	return color;
}

void main() {
	vec3 color = spotLightsContribution();
	fragmentColor = vec4(color, 1.0);
}