#pragma once

#include <memory>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


class GrassField
{
public:
    GrassField(float fieldSize, float patchSize);
    ~GrassField();

    std::vector<glm::vec3> *getPatchPositions();

protected:
    void generatePatchPositions();

private:
    float fieldSize;
    float patchSize;
    glm::vec3 worldCenterPos;

    std::vector<glm::vec3> *patchPositions;
};