#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(glm::vec3 cameraPosition, float fov, float aspectRatio, float nearClip, float farClip);

	enum class Direction { UP, DOWN, LEFT, RIGHT, FORWARDS, BACKWARDS };

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::vec3 getPosition();
	void increaseFov(float fovDelta);
	void decreaseFov(float fovDelta);
	void rotateCamera(float horizontalDelta, float verticalDelta);
	void moveCamera(Direction direction, float speed);


protected:
	void calculateViewMatrix();
	void calculateProjectionMatrix();

private:
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	glm::vec3 cameraPosition;
	glm::vec3 frontVector;
	glm::vec3 upVector;

	float aspectRatio;
	float nearClip;
	float farClip;
	float yaw;
	float pitch;
	float minFov;
	float maxFov;
	float fov;
};