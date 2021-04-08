#pragma once

#include <memory>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <geGL/geGL.h>

#include "Terrain.hpp"


class GrassField
{
public:
    struct BladeDimensions
    {
        float wMin;
        float wMax;
        float hMin;
        float hMax;
    };

    GrassField(float fieldSize, float patchSize, int grassBladeCount, BladeDimensions bladeDimensions);
    ~GrassField();

    int getFieldSize();
    int getGrassBladeCount();
    int getPatchCount();


    std::vector<glm::vec3> *getPatchPositions();
    std::vector<glm::vec4> *getGrassVertexPositions();
    std::vector<glm::vec4> *getGrassCenterPositions();
    std::vector<glm::vec4> *getGrassTextureCoords();
    std::vector<glm::vec4> *getGrassRandoms();

    std::shared_ptr<ge::gl::Buffer> getPatchTransSSBO();
    std::shared_ptr<ge::gl::Buffer> getGrassVertexBuffer();
    std::shared_ptr<ge::gl::Buffer> getGrassCenterBuffer();
    std::shared_ptr<ge::gl::Buffer> getGrassTexCoordBuffer();
    std::shared_ptr<ge::gl::Buffer> getGrassRandomsBuffer();
    std::shared_ptr<ge::gl::VertexArray> getGrassVAO();

protected:
    void generatePatchPositions();
    void generateRandoms();
    void generateGrassGeometry(BladeDimensions bladeDimensions);

private:
    float fieldSize;
    float patchSize;
    float randoms[11];
    int grassBladeCount;
    int patchCount;
    glm::vec3 worldCenterPos;

    std::vector<glm::vec3> *patchPositions;
    std::vector<glm::vec4> *grassVertexPositions;
    std::vector<glm::vec4> *grassCenterPositions;
    std::vector<glm::vec4> *grassTextureCoords;
    std::vector<glm::vec4> *grassRandoms;
};