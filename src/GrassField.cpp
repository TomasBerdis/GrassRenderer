#include "GrassField.hpp"

GrassField::GrassField(float fieldSize, float patchSize)
    : fieldSize{ fieldSize }, patchSize{ patchSize }
{
	worldCenterPos = { 0.0f, 0.0f, 0.0f };
	generatePatchPositions();
}

GrassField::~GrassField()
{
	delete patchPositions;
}

std::vector<glm::vec3> *GrassField::getPatchPositions()
{
	return patchPositions;
}

void GrassField::generatePatchPositions()
{
	patchPositions = new std::vector<glm::vec3>();

	/* Calculate first patch's position in lower left corner of a field */
	glm::vec3 startPos = worldCenterPos - glm::vec3(fieldSize / 2, 0.0f, fieldSize / 2);
	startPos = startPos + glm::vec3(patchSize / 2, 0.0f, patchSize / 2);	// we want the center of the patch

	int patchesInRowOrCol = fieldSize / patchSize;
	for (size_t row = 0; row < patchesInRowOrCol; row++)
	{
		for (size_t col = 0; col < patchesInRowOrCol; col++)
		{
			patchPositions->push_back(startPos + glm::vec3((row * patchSize), 0.0f, (col * patchSize)));
		}
	}
}