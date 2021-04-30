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
uniform float uMaxTerrainHeight;
uniform int uTime;
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
   float timeScale = (1 - uWindParams.z) * 500 + 1500;  // z = 0 -> slow, z = 1 -> fast
   float a = M_PI * p.x + 10.0 * (uWindParams.z + 1.0) + (M_PI / 4) / (abs(cos(c2 * M_PI * p.z)) + 0.00001);
   return sin(c1 * a) * cos(c3 * a);
}

vec2 rotate(vec2 point, float degree, vec2 pivot)
{
    float radAngle = radians(degree);
    float x = point.x;
    float y = point.y;

    float rX = pivot.x + (x - pivot.x) * cos(radAngle) - (y - pivot.y) * sin(radAngle);
    float rY = pivot.y + (x - pivot.x) * sin(radAngle) + (y - pivot.y) * cos(radAngle);

    return vec2(rX, rY);
}

void main()
{
   vDiscardBlade = 0;
   vec2 rotation;
   float newX = position.x;
   float newY = position.y;
   float newZ = position.z;
   float centerNewX = centerPosition.x;
   float centerNewY = centerPosition.y;
   float centerNewZ = centerPosition.z;

   /* Rotate patch */
   rotation = rotate(vec2(newX, newZ), (gl_InstanceID % 4) * 90, vec2(0.0, 0.0));
   newX = rotation.x;
   newZ = rotation.y;
   rotation = rotate(vec2(centerNewX, centerNewZ), (gl_InstanceID % 4) * 90, vec2(0.0, 0.0));
   centerNewX = rotation.x;
   centerNewZ = rotation.y;
   
   /* Calculate world space position */
   float patchX = patchTranslations[gl_InstanceID][3][0];
   float patchY = patchTranslations[gl_InstanceID][3][1];
   float patchZ = patchTranslations[gl_InstanceID][3][2];
   vec3 worldPos       = vec3(patchX + newX, patchY + newY, patchZ + newZ);
   vec3 centerWorldPos = vec3(patchX + centerNewX, patchY + centerNewY, patchZ + centerNewZ);

   /* Calculate height map coordinates */
   float x =      (centerWorldPos.x + uFieldSize/2) / uFieldSize;    // normalize x (possitive x is pointing away from us)
   float z = 1 - ((centerWorldPos.z + uFieldSize/2) / uFieldSize);   // normalize z (possitive z is pointing towards us)
   vec2 mapCoords = vec2(x, z);
   vec4 heightSample = texture(uHeightMap, mapCoords);

   /* Apply random variation influenced by world space position */
   float c = (worldPos.x + worldPos.z + uFieldSize) / uFieldSize * 2;
   float r0 = mix(0.0, 360.0, sin(c + position.w)/2 + 0.5);
   float r1 = centerPosition.w;
   float r2 = mix(-1.0, 1.0, sin(c + texCoord.z)/2 + 0.5);
   // vPosition.w       = r0;
   // vCenterPosition.w = r1;
   vTexCoord         = texCoord;
   vTexCoord.z       = r2;
   vTexCoord.w       = mix(-0.25, 0.25, sin(c + texCoord.w)/2 + 0.5);
   vRandoms.x        = mix(0.75, 1.25, sin(c + randoms.x)/2 + 0.5);
   vRandoms.y        = mix(0.00, 0.05, sin(c + randoms.y)/2 + 0.5);
   vRandoms.z        = mix(0.00, 0.05, sin(c + randoms.z)/2 + 0.5);
   vRandoms.w        = mix(0.00, 0.05, sin(c + randoms.w)/2 + 0.5);

   /* Discard blades based on density */
   float d = abs(centerPosition.w) + (1 - heightSample.r);
   if (d > 1)
      vDiscardBlade = 1;

   /* Rotate blades around center */
   rotation = rotate(vec2(newX, newZ), r0, vec2(centerNewX, centerNewZ));
   newX = rotation.x;
   newZ = rotation.y;

   /* New height sampled from height map */
   newY = newY + mix(0.0, uMaxTerrainHeight, 1 - heightSample.b) ;

   /* Scale blade dimensions based on sampled height */
   if (heightSample.g > 0.1)
   {
      newX = newX + (centerNewX - newX) * (1 - heightSample.g);
      newZ = newZ + (centerNewZ - newZ) * (1 - heightSample.g);
      if (centerNewY > 0.99f) // upper vertices
         newY = newY - (1 - heightSample.g) * 5;
   }
   else
      vDiscardBlade = 1;

   /* Upper vertex starting offset */
   if (centerNewY > 0.99f)
   {
      // scale offset based on height (heightSample.g)
      newX = newX + heightSample.g * ((uMaxBendingFactor * (/*2 * */r1) /*- 1.0*/));
      newZ = newZ + heightSample.g * ((uMaxBendingFactor * (/*2 * */r2) /*- 1.0*/));
   }

   /* Wind calculation */
   if ((centerPosition.y > 0.99f) && (uWindEnabled == 1)) // upper vertices
   {
      /* Inspired by Horizon Zero Dawn GDC presentation */
      newX = newX + heightSample.g * ((1.0 * sin (0.03 * (worldPos.x + worldPos.y + worldPos.z + uTime/30 ))) + 1.0);
      newZ = newZ + heightSample.g * ((0.5 * sin (0.03 * (worldPos.x + worldPos.y + worldPos.z + uTime/100))) + 0.5);
      newX = newX + heightSample.g * w(vec3(centerWorldPos.x, newY, centerWorldPos.z));
      newZ = newZ + heightSample.g * w(vec3(centerWorldPos.x, newY, centerWorldPos.z));
   }

   gl_Position     = vec4(newX, newY, newZ, 1.0f);

   vPosition          = patchTranslations[gl_InstanceID] * gl_Position; // move the patch
   vCenterPosition    = patchTranslations[gl_InstanceID] * vec4(centerNewX, 1.0f, centerNewZ, 1.0f);
   vCenterPosition.y  = newY;  // update center's y coordinate with actual height
   vCenterPosition.w  = centerPosition.w;
   // vTexCoord       = texCoord;
   // vRandoms = randoms;
}