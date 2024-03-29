#include "GrassField.hpp"

GrassField::GrassField(float fieldSize, float patchSize, int grassBladeCount, BladeDimensions bladeDimensions)
	: fieldSize{ fieldSize }, patchSize{ patchSize }, grassBladeCount{ grassBladeCount }
{
	worldCenterPos = { 0.0f, 0.0f, 0.0f };
	patchCount = pow(fieldSize / patchSize, 2);
	generatePatchPositions();
	generateGrassGeometry(bladeDimensions);
}

GrassField::~GrassField()
{
	delete patchPositions;
	delete grassVertexPositions;
	delete grassCenterPositions;
	delete grassTextureCoords;
	delete grassRandoms;
}

int GrassField::getFieldSize()
{
	return fieldSize;
}

int GrassField::getGrassBladeCount()
{
	return grassBladeCount;
}

int GrassField::getPatchCount()
{
	return patchCount;
}

std::vector<glm::vec3> *GrassField::getPatchPositions()
{
	return patchPositions;
}

std::vector<glm::vec4> *GrassField::getGrassVertexPositions()
{
	return grassVertexPositions;
}

std::vector<glm::vec4> *GrassField::getGrassCenterPositions()
{
	return grassCenterPositions;
}

std::vector<glm::vec4> *GrassField::getGrassTextureCoords()
{
	return grassTextureCoords;
}

std::vector<glm::vec4> *GrassField::getGrassRandoms()
{
	return grassRandoms;
}

std::shared_ptr<ge::gl::Buffer> GrassField::getPatchTransSSBO()
{
	std::shared_ptr<ge::gl::Buffer> patchTransSSBO;

	std::vector<glm::vec3> *patchPositions = getPatchPositions();
	std::vector<glm::mat4> patchTranslations;

	for (size_t i = 0; i < patchPositions->size(); i++)
	{
		glm::mat4 mat = glm::translate(glm::mat4(1.0f), patchPositions->at(i));
		patchTranslations.push_back(mat);
	}

	patchTransSSBO = std::make_shared<ge::gl::Buffer>(patchTranslations.size() * sizeof(glm::mat4), patchTranslations.data());
	return patchTransSSBO;
}

std::shared_ptr<ge::gl::Buffer> GrassField::getPatchRandomsSSBO()
{
	std::shared_ptr<ge::gl::Buffer> patchRandomsSSBO;
	std::vector<int> patchRandoms;

	for (size_t i = 0; i < patchCount; i++)
	{
		int random = rand();
		patchRandoms.push_back(random);
	}

	patchRandomsSSBO = std::make_shared<ge::gl::Buffer>(patchRandoms.size() * sizeof(int), patchRandoms.data());
	return patchRandomsSSBO;
}

std::shared_ptr<ge::gl::Buffer> GrassField::getGrassVertexBuffer()
{
	std::shared_ptr<ge::gl::Buffer> grassVertexBuffer;
	grassVertexBuffer = std::make_shared<ge::gl::Buffer>(grassVertexPositions->size() * sizeof(float) * 4, grassVertexPositions->data());

	return grassVertexBuffer;
}

std::shared_ptr<ge::gl::Buffer> GrassField::getGrassCenterBuffer()
{
	std::shared_ptr<ge::gl::Buffer> grassCenterBuffer;
	grassCenterBuffer = std::make_shared<ge::gl::Buffer>(grassCenterPositions->size() * sizeof(float) * 4, grassCenterPositions->data());

	return grassCenterBuffer;
}

std::shared_ptr<ge::gl::Buffer> GrassField::getGrassTexCoordBuffer()
{
	std::shared_ptr<ge::gl::Buffer> grassTexCoordBuffer;
	grassTexCoordBuffer = std::make_shared<ge::gl::Buffer>(grassTextureCoords->size() * sizeof(float) * 4, grassTextureCoords->data());

	return grassTexCoordBuffer;
}

std::shared_ptr<ge::gl::Buffer> GrassField::getGrassRandomsBuffer()
{
	std::shared_ptr<ge::gl::Buffer> grassRandomsBuffer;
	grassRandomsBuffer = std::make_shared<ge::gl::Buffer>(grassRandoms->size() * sizeof(float) * 4, grassRandoms->data());

	return grassRandomsBuffer;
}

std::shared_ptr<ge::gl::VertexArray> GrassField::getGrassVAO()
{
	std::shared_ptr<ge::gl::VertexArray> grassVAO;
	grassVAO = std::make_shared<ge::gl::VertexArray>();

	std::shared_ptr<ge::gl::Buffer> grassPositionBuffer = getGrassVertexBuffer();
	std::shared_ptr<ge::gl::Buffer> grassCenterPositionBuffer = getGrassCenterBuffer();
	std::shared_ptr<ge::gl::Buffer> grassTexCoordBuffer = getGrassTexCoordBuffer();
	std::shared_ptr<ge::gl::Buffer> grassRandomsBuffer = getGrassRandomsBuffer();

	grassVAO->addAttrib(grassPositionBuffer, 0, 4, GL_FLOAT);
	grassVAO->addAttrib(grassCenterPositionBuffer, 1, 4, GL_FLOAT);
	grassVAO->addAttrib(grassTexCoordBuffer, 2, 4, GL_FLOAT);
	grassVAO->addAttrib(grassRandomsBuffer, 3, 4, GL_FLOAT);

	return grassVAO;
}

void GrassField::generatePatchPositions()
{
	patchPositions = new std::vector<glm::vec3>();

	/* Calculate first patch's position in lower left corner of a field */
	glm::vec3 startPos = worldCenterPos + glm::vec3(-fieldSize / 2, 0.0f, fieldSize / 2);
	startPos = startPos + glm::vec3(patchSize / 2, 0.0f, -patchSize / 2);	// we want the center of the patch

	int patchesInRowOrCol = fieldSize / patchSize;
	for (size_t row = 0; row < patchesInRowOrCol; row++)
	{
		for (size_t col = 0; col < patchesInRowOrCol; col++)
		{
			patchPositions->push_back(startPos + glm::vec3((col * patchSize), 0.0f, -(row * patchSize)));
		}
	}
}

void GrassField::generateRandoms()
{
	/* Rendering pipeline random values */
	randoms[0] = glm::linearRand(0.00f, 360.0f);	// angle
	randoms[1] = glm::linearRand(-1.00f, 1.00f);	// x offset
	randoms[2] = glm::linearRand(-1.00f, 1.00f);	// z offset
	randoms[3] = glm::linearRand(-0.25f, 0.25f);	// TCS
	randoms[4] = glm::linearRand(0.75f, 1.25f);		// TCS
	randoms[5] = glm::linearRand(0.00f, 0.05f);		// R
	randoms[6] = glm::linearRand(0.00f, 0.05f);		// G
	randoms[7] = glm::linearRand(0.00f, 0.05f);		// B

	/* Values for blade generation */
	randoms[8]  = glm::linearRand(0.0f, 1.0f);						// w, h
	randoms[9]  = glm::linearRand(-patchSize / 2, patchSize / 2);	// x offset
	randoms[10] = glm::linearRand(-patchSize / 2, patchSize / 2);	// z offset
}

void GrassField::generateGrassGeometry(BladeDimensions bladeDimensions)
{
	grassVertexPositions = new std::vector<glm::vec4>();
	grassCenterPositions = new std::vector<glm::vec4>();
	grassTextureCoords   = new std::vector<glm::vec4>();
	grassRandoms		 = new std::vector<glm::vec4>();
	srand(time(0));	// reset generator seed

	for (size_t i = 0; i < grassBladeCount; i++)
	{
		generateRandoms();

		float w =  bladeDimensions.wMin + randoms[8] * (bladeDimensions.wMax - bladeDimensions.wMin);
		float h = (bladeDimensions.hMin + randoms[8] * (bladeDimensions.hMax - bladeDimensions.hMin));

		glm::vec4 pc{ 0.0f, 0.0f, 0.0f, 1.0f };	// bottom center

		/* Grass blade vertices */
		glm::vec4 p1 = pc + glm::vec4(-0.5f * w, 0.0f, 0.0f, 0.0f);
		glm::vec4 p2 = pc + glm::vec4( 0.5f * w, 0.0f, 0.0f, 0.0f);
		glm::vec4 p3 = pc + glm::vec4( 0.5f * w,	h, 0.0f, 0.0f);
		glm::vec4 p4 = pc + glm::vec4(-0.5f * w,	h, 0.0f, 0.0f);

		/* Move within a patch */
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(randoms[9], 0.0f, randoms[10]));
		p1 = model * p1;
		p2 = model * p2;
		p3 = model * p3;
		p4 = model * p4;
		pc = model * pc;

		/* Add random value (x,y,z,r0) */
		p1.w = randoms[0];
		p2.w = randoms[0];
		p3.w = randoms[0];
		p4.w = randoms[0];

		/* Add to list */
		grassVertexPositions->push_back(p1);
		grassVertexPositions->push_back(p2);
		grassVertexPositions->push_back(p3);
		grassVertexPositions->push_back(p4);

		/* Blade's center positions (x,y,z,r1) */
		grassCenterPositions->push_back(glm::vec4(pc.x, 0.0f, pc.z, randoms[1]));
		grassCenterPositions->push_back(glm::vec4(pc.x, 0.0f, pc.z, randoms[1]));
		grassCenterPositions->push_back(glm::vec4(pc.x, 1.0f, pc.z, randoms[1]));
		grassCenterPositions->push_back(glm::vec4(pc.x, 1.0f, pc.z, randoms[1]));

		/* Blade's texture coordinates (s,t,r2,r3) */
		grassTextureCoords->push_back(glm::vec4(0.0f, 0.0f, randoms[2], randoms[3]));
		grassTextureCoords->push_back(glm::vec4(1.0f, 0.0f, randoms[2], randoms[3]));
		grassTextureCoords->push_back(glm::vec4(1.0f, 1.0f, randoms[2], randoms[3]));
		grassTextureCoords->push_back(glm::vec4(0.0f, 1.0f, randoms[2], randoms[3]));

		/* Additional random values */
		grassRandoms->push_back(glm::vec4(randoms[4], randoms[5], randoms[6], randoms[7]));
		grassRandoms->push_back(glm::vec4(randoms[4], randoms[5], randoms[6], randoms[7]));
		grassRandoms->push_back(glm::vec4(randoms[4], randoms[5], randoms[6], randoms[7]));
		grassRandoms->push_back(glm::vec4(randoms[4], randoms[5], randoms[6], randoms[7]));
	}
}
