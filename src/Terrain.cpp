#include "Terrain.hpp"

Terrain::Terrain(float width, float height, int rows, int cols)
    : width{width}, height{height}, rows{rows}, cols{cols}
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

std::shared_ptr<ge::gl::Buffer> Terrain::getTerrainTexCoordBuffer()
{
    std::shared_ptr<ge::gl::Buffer> terrainTexCoordBuffer;
    terrainTexCoordBuffer = std::make_shared<ge::gl::Buffer>(terrainTexCoords->size() * sizeof(float), terrainTexCoords->data());

    return terrainTexCoordBuffer;
}

void Terrain::generateTerrain()
{
    terrainVertices  = new std::vector<glm::vec2>;
    terrainIndices   = new std::vector<unsigned int>;
    terrainTexCoords = new std::vector<glm::vec2>;

    /* Generate vertices and texture coordinates */
    for (size_t row = 0; row < rows; row++)
    {
        for (size_t col = 0; col < cols; col++)
        {
            float normalizedHeight = (float)row / (rows - 1);
            float normalizedWidth  = (float)col / (cols - 1);
            float xOffset = glm::mix( -width / 2, width  / 2, normalizedWidth);
            float zOffset = glm::mix( height / 2, -height / 2, normalizedHeight);
            terrainVertices->push_back(glm::vec2(xOffset, zOffset));

            terrainTexCoords->push_back(glm::vec2(normalizedWidth, normalizedHeight));
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
