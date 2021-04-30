#include "Terrain.hpp"

Terrain::Terrain(float terrainWidth, float terrainLength, int rows, int cols)
    : terrainWidth{ terrainWidth }, terrainLength{ terrainLength }, rows{ rows }, cols{ cols }
{
    indexCount = (rows - 1) * cols * 2 + rows - 1;
    generateTerrain();
}

Terrain::~Terrain()
{
}

int Terrain::getIndexCount()
{
    return indexCount;
}

int Terrain::getRestartIndex()
{
    return restartIndex;
}

float Terrain::getTerrainWidth()
{
    return terrainWidth;
}

float Terrain::getTerrainLength()
{
    return terrainLength;
}

std::shared_ptr<ge::gl::Buffer> Terrain::getTerrainVertexBuffer()
{
    std::shared_ptr<ge::gl::Buffer> terrainVertexBuffer;
    terrainVertexBuffer = std::make_shared<ge::gl::Buffer>(terrainVertices->size() * sizeof(float) * 2, terrainVertices->data());

    return terrainVertexBuffer;
}

std::shared_ptr<ge::gl::Buffer> Terrain::getTerrainIndexBuffer()
{
    std::shared_ptr<ge::gl::Buffer> terrainIndexBuffer;
    terrainIndexBuffer = std::make_shared<ge::gl::Buffer>(terrainIndices->size() * sizeof(unsigned int), terrainIndices->data());

    return terrainIndexBuffer;
}

void Terrain::generateTerrain()
{
    terrainVertices  = new std::vector<glm::vec2>;
    terrainIndices   = new std::vector<unsigned int>;

    /* Generate vertices */
    for (size_t row = 0; row < rows; row++)
    {
        for (size_t col = 0; col < cols; col++)
        {
            float normalizedLength = (float)row / (rows - 1);
            float normalizedWidth = (float)col / (cols - 1);
            float xOffset = glm::mix(-terrainWidth  / 2,  terrainWidth  / 2, normalizedWidth);
            float zOffset = glm::mix( terrainLength / 2, -terrainLength / 2, normalizedLength);
            terrainVertices->push_back(glm::vec2(xOffset, zOffset));
        }
    }

    restartIndex = terrainVertices->size();

    /* Generate indices */
    for (auto i = 0; i < rows - 1; i++)
    {
        for (auto j = 0; j < cols; j++)
        {
            for (auto k = 0; k < 2; k++)
            {
                const auto row = i + k;
                const auto index = row * cols + j;
                terrainIndices->push_back(index);
            }
        }
        // Restart triangle strips
        terrainIndices->push_back(restartIndex);
    }
}

int Terrain::rowColToIndex(int row, int col)
{
    return row * cols + col;
}