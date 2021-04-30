#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include <geGL/geGL.h>

class Terrain
{
public:
    Terrain(float terrainWidth, float terrainLength, int rows, int cols);
    ~Terrain();

    int getIndexCount();
    int getRestartIndex();
    float getTerrainWidth();
    float getTerrainLength();

    std::shared_ptr<ge::gl::Buffer> getTerrainVertexBuffer();
    std::shared_ptr<ge::gl::Buffer> getTerrainIndexBuffer();

protected:
    void generateTerrain();

    int rowColToIndex(int row, int col);

private:
    float terrainWidth;
    float terrainLength;
    int rows;
    int cols;
    int indexCount;
    int restartIndex;

    std::vector<glm::vec2> *terrainVertices;
    std::vector<unsigned int> *terrainIndices;

};