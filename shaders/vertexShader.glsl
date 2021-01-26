#version 410 core

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 centerPosition;

out vec3 vPosition;
out vec3 vCenterPosition;

void main()
{
   vec3 newPosition = position;

   if (centerPosition.y == 1.0f)
   {   
      float R1 = -0.1f;
      float R2 = 0.5f;
      float b  = 0.5f;
      float newX = position.x + (b * (2 * R1 - 1));
      float newZ = position.z + (b * (2 * R2 - 1));
      newPosition = vec3(newX, position.y, newZ);
   }

   gl_Position = vec4(newPosition, 1.0f);
   vPosition = newPosition;

   // vPosition = gl_Position.xyz;
   vCenterPosition = centerPosition;
}