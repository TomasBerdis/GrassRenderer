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
out int vDiscardBlade;

uniform sampler2D uHeightMap;
uniform float uMaxBendingFactor;
uniform float uTime;
uniform float uFieldSize;
uniform int uWindEnabled;
uniform vec3 uWindParams;

layout(std430, binding=0) buffer patchTranslationBuffer
{
    mat4 patchTranslations[];
};

/* Wind function */
float w(vec3 p)
{
   float c1 = uWindParams.x;
   float c2 = 2.0;
   float c3 = uWindParams.y;
   float timeScale = (1 - uWindParams.z) * 50 + 150;  // z = 0 -> slow, z = 1 -> fast
   float a = M_PI * p.x + uTime/timeScale + (M_PI / 4) / (abs(cos(c2 * M_PI * p.z)) + 0.00001);
   return sin(c1 * a) * cos(c3 * a);
}

void main()
{
   vDiscardBlade = 0;
   
   /* Calculate world space position */
   float patchX = patchTranslations[gl_InstanceID][3][0];
   float patchY = patchTranslations[gl_InstanceID][3][1];
   float patchZ = patchTranslations[gl_InstanceID][3][2];
   vec3 worldPos       = vec3(patchX + position.x, patchY + position.y, patchZ + position.z);
   vec3 centerWorldPos = vec3(patchX + centerPosition.x, patchY + centerPosition.y, patchZ + centerPosition.z);

   /* Calculate height map coordinates */
   float x =      (centerWorldPos.x + uFieldSize/2) / uFieldSize;    // normalize x (possitive x is pointing away from us)
   float z = 1 - ((centerWorldPos.z + uFieldSize/2) / uFieldSize);   // normalize z (possitive z is pointing towards us)
   vec2 mapCoords = vec2(x, z);
   vec4 heightSample = texture(uHeightMap, mapCoords);

   /* Apply random variation influenced by world space position */
   float c = 42;
   float r0 = position.w       * (worldPos.x + uFieldSize * 2)/uFieldSize;
   float r1 = centerPosition.w * (worldPos.x + uFieldSize * 2)/uFieldSize;
   float r2 = texCoord.z       * (worldPos.x + uFieldSize * 2)/uFieldSize;
   vPosition.w       = position.w       * sin(5 * worldPos.x + worldPos.z);
   vCenterPosition.w = centerPosition.w * sin(4 * worldPos.x + worldPos.z);
   vTexCoord.zw      = texCoord.zw      * sin(9 * worldPos.x + worldPos.z);
   vRandoms          = randoms          * sin(8 * worldPos.x + worldPos.z);

    /* Discard blades based on density */
    float d = abs(centerPosition.w) + (1 - heightSample.r);
    if (d > 1)
        vDiscardBlade = 1;

   /* Rotate blades */
   float angle = 2 * M_PI * position.w;
   float xDelta = position.x - centerPosition.x;
   float zDelta = position.z - centerPosition.z;
	float newX = centerPosition.x + cos(angle) * (xDelta) - sin(angle) * (zDelta);   // x rotated around center
	float newZ = centerPosition.z + sin(angle) * (xDelta) + cos(angle) * (zDelta);   // z rotated around center
   
   /* New height sampled from height map */
   float newY = position.y + mix(0.0, 30.0, 1 - heightSample.b) ;

   if (centerPosition.y > 0.99f) // upper vertices
   {   
      newX = newX + (uMaxBendingFactor * (2 * centerPosition.w));
      newZ = newZ + (uMaxBendingFactor * (2 * texCoord.z));
   }

   /* Scale blade dimensions based on sampled height */
   if (heightSample.g > 0.3)
   {
      newX = newX + (centerPosition.x - newX) * (1 - heightSample.g);
      newZ = newZ + (centerPosition.z - newZ) * (1 - heightSample.g);
      if (centerPosition.y > 0.99f) // upper vertices
         newY = newY - (1 - heightSample.g) * 5;
   }
   else
      vDiscardBlade = 1;

   /* Wind calculation */
   if ((centerPosition.y > 0.99f) && (uWindEnabled == 1)) // upper vertices
   {
      /* Inspired by Horizon Zero Dawn GDC presentation */
      newX = newX + (2 * sin (0.05 * (worldPos.x + worldPos.y + worldPos.z + uTime/10))) + 1;
      newZ = newZ + (1 * sin (0.05 * (worldPos.x + worldPos.y + worldPos.z + uTime/50))) + 0.5;
      newX = newX + w(vec3(centerWorldPos.x, newY, centerWorldPos.z));
      newZ = newZ + w(vec3(centerWorldPos.x, newY, centerWorldPos.z));
   }

   gl_Position     = vec4(newX, newY, newZ, 1.0f);
   
   vCenterPosition.y = newY;  // update center's y coordinate with actual height

   vPosition       = patchTranslations[gl_InstanceID] * gl_Position;
   vCenterPosition = centerPosition;
   vTexCoord       = texCoord;
   vRandoms        = randoms;
}