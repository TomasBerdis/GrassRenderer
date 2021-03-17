#version 450 core

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 centerPosition;

out vec3 vPosition;
out vec3 vCenterPosition;

void main()
{
   vec4 newPosition = position;

   if (centerPosition.y > 0.1f)
   {   
      float R1 = -2.0f;
      float R2 = 5.5f;
      float b  = 0.5f;
      float newX = position.x + (b * (2 * R1 - 1));
      float newZ = position.z + (b * (2 * R2 - 1));
      newPosition = vec4(newX, position.y, newZ, 1.0f);
   }
   newPosition = newPosition;
   vec4 newCenterPosition = centerPosition;

   gl_Position = newPosition;

   vPosition = vec3(newPosition.xyz);
   vCenterPosition = vec3(newCenterPosition.xyz);
}