#version 430

in vec3 Normal;
in vec3 FragPos;
in vec4 myPos;

uniform vec3 lightPos;
uniform mat4 projection;

out vec4 color;

void main(void)
{
	// AMBIENT
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
  	
    // DIFFUSE
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);


    if(myPos.x<-0.5 && myPos.y<-0.5)   
    {
    vec3 colorAttribute = vec3(1.0, 0.0, 0.0);        
    vec3 result = (ambient + diffuse) * colorAttribute;
    color = vec4(result, 1.0);
    }
    else if(myPos.x>1 && myPos.y>1)   
    {
    vec3 colorAttribute = vec3(0.0, 1.0, 0.0);        
    vec3 result = (ambient + diffuse) * colorAttribute;
    color = vec4(result, 1.0);
    }
    else
    {
    vec3 colorAttribute = vec3(0.0, 0.0, 1.0);        
    vec3 result = (ambient + diffuse) * colorAttribute;
    color = vec4(result, 1.0);
    }
}