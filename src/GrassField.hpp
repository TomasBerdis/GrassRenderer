#pragma once

#include <memory>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <geGL/geGL.h>


class GrassField
{
public:
    GrassField(float fieldSize, float patchSize, int grassBladeCount);
    ~GrassField();

    int getFieldSize();
    int getGrassBladeCount();
    int getPatchCount();

    std::vector<glm::vec3> *getPatchPositions();
    std::vector<glm::vec4> *getGrassVertexPositions();
    std::vector<glm::vec4> *getGrassCenterPositions();
    std::vector<glm::vec4> *getGrassTextureCoords();
    std::vector<glm::vec4> *getGrassRandoms();

    std::shared_ptr<ge::gl::Buffer> getGrassVertexBuffer();
    std::shared_ptr<ge::gl::Buffer> getGrassCenterBuffer();
    std::shared_ptr<ge::gl::Buffer> getGrassTexCoordBuffer();
    std::shared_ptr<ge::gl::Buffer> getGrassRandomsBuffer();

protected:
    void generatePatchPositions();
    void generateRandoms();
    void generateGrassGeometry();

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