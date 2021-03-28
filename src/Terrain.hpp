#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include <geGL/geGL.h>

class Terrain
{
public:
    Terrain(float width, float height, int rows, int cols);
    ~Terrain();

    int getIndexCount();
    int getRestartIndex();

    std::shared_ptr<ge::gl::Buffer> getTerrainVertexBuffer();
    std::shared_ptr<ge::gl::Buffer> getTerrainIndexBuffer();
    std::shared_ptr<ge::gl::Buffer> getTerrainTexCoordBuffer();

protected:
    void generateTerrain();

private:
    float width;
    float height;
    int rows;
    int cols;
    int indexCount;
    int restartIndex;

    std::vector<glm::vec2> *terrainVertices;
    std::vector<unsigned int> *terrainIndices;
    std::vector<glm::vec2> *terrainTexCoords;

};