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

uniform sampler2D uHeightMap;
uniform float uMaxBendingFactor;
uniform float uTime;
uniform float uFieldSize;
uniform int uWindEnabled;

layout(std430, binding=0) buffer patchTranslationBuffer
{
    mat4 patchTranslations[];
};

void main()
{
   /* Calculate world space position */
   float patchX = patchTranslations[gl_InstanceID][3][0];
   float patchY = patchTranslations[gl_InstanceID][3][1];
   float patchZ = patchTranslations[gl_InstanceID][3][2];
   vec3 worldPos = vec3(patchX + position.x, patchY + position.y, patchZ + position.z);
   vec3 centerWorldPos = vec3(patchX + centerPosition.x, patchY + centerPosition.y, patchZ + centerPosition.z);

   /* Calculate height map coordinates */
   vec2 mapCoords = vec2(centerWorldPos.x / uFieldSize, centerWorldPos.z / uFieldSize);
   vec4 heightSample = texture(uHeightMap, mapCoords);

   /* Apply random variation influenced by world space position */
   // float r0 = fract((position.w       * worldPos.x) / worldPos.z);
   // float r1 = fract((centerPosition.w * worldPos.x) / worldPos.z);
   // float r2 = fract((texCoord.z       * worldPos.x) / worldPos.z);
   float c = 0.5;
   float r0 = position.w       * sin(c * worldPos.x);
   float r1 = centerPosition.w * sin(c * worldPos.z);
   float r2 = texCoord.z       * sin(c * worldPos.x);

   float angle = 2 * M_PI * r0;
   float xDelta = position.x - centerPosition.x;
   float zDelta = position.z - centerPosition.z;
	float newX = centerPosition.x + cos(angle) * (xDelta) - sin(angle) * (zDelta);   // x rotated around center
	float newZ = centerPosition.z + sin(angle) * (xDelta) + cos(angle) * (zDelta);   // z rotated around center

   if (centerPosition.y > 0.99f) // upper vertices
   {   
      newX = newX + (uMaxBendingFactor * (2 * r1 - 1));
      newZ = newZ + (uMaxBendingFactor * (2 * r2 - 1));
   }

   /* Wind function */
   if ((centerPosition.y > 0.99f) && (uWindEnabled == 1)) // upper vertices
   {
      /* Inspired by Horizon Zero Dawn GDC presentation */
      newX = newX + (2 * sin (1 * (worldPos.x + worldPos.y + worldPos.z + uTime/150))) + 1;
      newZ = newZ + (1 * sin (2 * (worldPos.x + worldPos.y + worldPos.z + uTime/150))) + 0.5;
   }

   /* New height sampled from height map */
   float newY = position.y + mix(0.0, 30.0, 1 - heightSample.b) ;

   gl_Position     = vec4(newX, newY, newZ, 1.0f);

   vPosition       = patchTranslations[gl_InstanceID] * gl_Position;
   vCenterPosition = centerPosition;
   vTexCoord       = texCoord;
   vRandoms        = randoms;
}