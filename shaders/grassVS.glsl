#version 450 core

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 centerPosition;
layout(location = 2) in vec4 texCoord;
layout(location = 3) in vec4 randoms;

out vec4 vPosition;
out vec4 vCenterPosition;
out vec4 vTexCoord;
out vec4 vRandoms;

uniform float uMaxBendingFactor;
uniform float uTime;

layout(std430, binding=0) buffer patchTranslationBuffer
{
    mat4 patchTranslations[];
};

void main()
{
   float r0 = 2 * M_PI * position.w;   // angle
   float xDelta = position.x - centerPosition.x;
   float zDelta = position.z - centerPosition.z;
	float newX = centerPosition.x + cos(r0) * (xDelta) - sin(r0) * (zDelta);   // x rotated around center
	float newZ = centerPosition.z + sin(r0) * (xDelta) + cos(r0) * (zDelta);   // z rotated around center

   if (centerPosition.y > 0.99f) // upper vertices
   {   
      float r1 = centerPosition.w;
      float r2 = texCoord.z;
      newX = newX + (uMaxBendingFactor * (2 * r1 - 1));
      newZ = newZ + (uMaxBendingFactor * (2 * r2 - 1));
   }

   /* Calculate world space position */
   float patchX = patchTranslations[gl_InstanceID][3][0];
   float patchY = patchTranslations[gl_InstanceID][3][1];
   float patchZ = patchTranslations[gl_InstanceID][3][2];
   vec3 worldPos = vec3(patchX + newX, patchY + position.y, patchZ + newZ);

   if (centerPosition.y > 0.99f) // upper vertices
   {
      // newX = newX + sin(uTime/3 + worldPos.x + worldPos.z) + cos(uTime/4 + worldPos.x + worldPos.z) + cos(uTime/9 + worldPos.x + worldPos.z);
      // newZ = newZ + sin(uTime/6 + worldPos.x + worldPos.z);
      newX = newX + (2 * sin (1 * (worldPos.x + worldPos.y + worldPos.z + uTime/150))) + 1;
      newZ = newZ + (1 * sin (2 * (worldPos.x + worldPos.y + worldPos.z + uTime/150))) + 0.5;
   }

   gl_Position     = vec4(newX, position.y, newZ, 1.0f);

   vPosition       = patchTranslations[gl_InstanceID] * gl_Position;
   vCenterPosition = centerPosition;
   vTexCoord       = texCoord;
   vRandoms        = randoms;
}