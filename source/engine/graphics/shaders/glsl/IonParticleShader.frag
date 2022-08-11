/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/glsl
File:	IonParticleShader.frag
-------------------------------------------
*/

#version 330 core

struct Scene
{
	vec4 ambient;
	float gamma;
	bool has_fog;
	int light_count;
	int emissive_light_count;
};

struct Camera
{
	vec3 position;
	float rotation;
};

struct Primitive
{
	bool has_material;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
	float shininess;

	sampler2D diffuse_map;
	sampler2D specular_map;
	sampler2D normal_map;

	bool has_diffuse_map;
	bool has_specular_map;
	bool has_normal_map;
	bool lighting_enabled;
};

struct Fog
{
	int mode;
	float density;
	float near;
	float far;
	vec4 color;
};

struct Light
{
	int type;
	vec3 position;
	vec3 direction;
	float radius;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
	float outer_cutoff;
};

struct EmissiveLight
{
	vec3 position;
	float radius;
	vec4 color;
};


in vec3 vert_position;
in vec4 vert_color;
in mat3 normal_matrix;
in mat2 rotation_matrix;

out vec4 frag_color;

uniform Scene scene;
uniform Camera camera;
uniform Primitive primitive;
uniform Material material;
uniform Fog fog;
uniform Light light[8];
uniform EmissiveLight emissive_light[8];


vec2 tex_coord = (rotation_matrix * (gl_PointCoord - 0.5)).xy + 0.5;

const float log2e = 1.442695;
float fog_frag_coord = abs(vert_position.z);
float fog_scale = 1.0 / (fog.far - fog.near);


float linear_fog()
{
	return (fog.far - fog_frag_coord) * fog_scale;
}

float exp_fog()
{
	return exp2(-fog.density * fog_frag_coord * log2e);
}

float exp2_fog()
{
	return exp2(-fog.density * fog.density * fog_frag_coord * fog_frag_coord * log2e);
}


vec3 calc_point_light(int i, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(light[i].position - vert_position);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    //Attenuation
    float distance = length(light[i].position - vert_position);
    float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * (distance * distance));    

	//Combine ambient, diffuse and specular color
	ambient_color *= light[i].ambient;
	diffuse_color *= light[i].diffuse;
	specular_color *= light[i].specular;

	ambient_color.rgb *= ambient_color.a * attenuation;
	diffuse_color.rgb *= diffuse_color.a * diff * attenuation;
	specular_color.rgb *= specular_color.a * spec * attenuation;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}

vec3 calc_directional_light(int i, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(-light[i].direction);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
	
	//Combine ambient, diffuse and specular color
	ambient_color *= light[i].ambient;
	diffuse_color *= light[i].diffuse;
	specular_color *= light[i].specular;

	ambient_color.rgb *= ambient_color.a;
	diffuse_color.rgb *= diffuse_color.a * diff;
	specular_color.rgb *= specular_color.a * spec;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}

vec3 calc_spot_light(int i, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(light[i].position - vert_position);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    //Attenuation
    float distance = length(light[i].position - vert_position);
    float attenuation = 1.0 / (light[i].constant + light[i].linear * distance + light[i].quadratic * (distance * distance));    
    //Intensity
    float theta = dot(light_dir, normalize(-light[i].direction)); 
    float epsilon = light[i].cutoff - light[i].outer_cutoff;
    float intensity = clamp((theta - light[i].outer_cutoff) / epsilon, 0.0, 1.0);

    //Combine ambient, diffuse and specular color
	ambient_color *= light[i].ambient;
	diffuse_color *= light[i].diffuse;
	specular_color *= light[i].specular;

	ambient_color.rgb *= ambient_color.a * attenuation * intensity;
	diffuse_color.rgb *= diffuse_color.a * diff * attenuation * intensity;
	specular_color.rgb *= specular_color.a * spec * attenuation * intensity;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}

vec3 calc_emissive_light(int i, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	return vec3(0.0);
}


void main()
{
	//Ambient, diffuse, specular and emissive color
	vec4 ambient_color = scene.ambient * vert_color;
	vec4 diffuse_color = vert_color;
	vec4 specular_color = vec4(0.66, 0.66, 0.66, 1.0); //DarkGray
	vec4 emissive_color = vec4(0.0, 0.0, 0.0, 1.0); //Black
	float shininess = 32.0;

	//Normal/bump mapping
	vec3 normal = vec3(0.0, 0.0, 1.0);


	if (primitive.has_material)
	{
		ambient_color *= material.ambient;
		diffuse_color *= material.diffuse;
		specular_color = material.specular;
		emissive_color = material.emissive;
		shininess = material.shininess;

		if (material.has_diffuse_map)
		{
			vec4 diffuse_map_color = texture(material.diffuse_map, tex_coord);
			ambient_color *= diffuse_map_color;
			diffuse_color *= diffuse_map_color;
			emissive_color *= diffuse_map_color;
		}

		if (material.has_specular_map)
		{
			vec4 specular_map_color = texture(material.specular_map, tex_coord);
			specular_color *= specular_map_color;
		}

		if (material.has_normal_map)
		{
			normal = texture(material.normal_map, tex_coord).rgb;
			normal = normal_matrix * normalize(normal * 2.0 - 1.0);
		}
	}


	vec4 color = ambient_color +
		vec4(emissive_color.rgb * emissive_color.a, 0.0);

	//Fog effect
	if (scene.has_fog)
	{
		vec4 fog_color = scene.ambient * fog.color;
		fog_color.rgb *= fog.color.a;
		fog_color.a = color.a; //Set equal before mixing

		//Exponential fog
		if (fog.mode == 0)
			color = mix(fog_color, color, clamp(exp_fog(), 0.0, 1.0));
		//Exponential2 (squared) fog
		else if (fog.mode == 1)
			color = mix(fog_color, color, clamp(exp2_fog(), 0.0, 1.0));
		//Linear fog
		else if (fog.mode == 2)
			color = mix(fog_color, color, clamp(linear_fog(), 0.0, 1.0));
	}

	
	//Lighting
	if (scene.light_count + scene.emissive_light_count > 0 &&
		(!primitive.has_material || material.lighting_enabled))
	{
		vec3 light_color = vec3(0.0); //Black
		vec3 view_dir = normalize(camera.position - vert_position);

		//Accumulate each light
		for (int i = 0; i < scene.light_count; ++i)
		{
			//Point light
			if (light[i].type == 0)
				light_color += calc_point_light(i, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
			
			//Directional light
			else if (light[i].type == 1)
				light_color += calc_directional_light(i, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
			
			//Spot light
			else if (light[i].type == 2)
				light_color += calc_spot_light(i, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
		}

		//Accumulate each emissive light
		for (int i = 0; i < scene.emissive_light_count; ++i)
			light_color += calc_emissive_light(i, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);

		color.rgb += light_color;
	}


	//Gamma correction
    color = pow(color, vec4(1.0 / scene.gamma));

	//Fragment color
	frag_color = color;
}