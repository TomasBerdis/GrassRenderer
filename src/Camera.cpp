#include "Camera.hpp"

Camera::Camera(glm::vec3 cameraPosition, float fov, float aspectRatio, float nearClip, float farClip)
	: cameraPosition{ cameraPosition }, fov{ fov }, aspectRatio{ aspectRatio }, nearClip{ nearClip }, farClip{ farClip }
{
	frontVector = glm::vec3(0.0f, 0.0f, -1.0f);
	upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	yaw =   0.0;
	pitch = 0.0;

	minFov = 1.0f;
	maxFov = 120.0f;

	calculateViewMatrix();
	calculateProjectionMatrix();
}

glm::mat4 Camera::getViewMatrix()
{
	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::increaseFov(float fovDelta)
{
	if ((fov + fovDelta) > maxFov)
		fov = maxFov;
	else
		fov += fovDelta;
	calculateProjectionMatrix();
}

void Camera::decreaseFov(float fovDelta)
{
	if ((fov - fovDelta) < minFov)
		fov = minFov;
	else
		fov -= fovDelta;
	calculateProjectionMatrix();
}

void Camera::rotateCamera(float horizontalDelta, float verticalDelta)
{
	float sensitivity = 0.1f;
	yaw += horizontalDelta * sensitivity;
	pitch += verticalDelta * sensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(-yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(-yaw)) * cos(glm::radians(pitch));

	frontVector = glm::normalize(direction);

	calculateViewMatrix();
}

void Camera::moveCamera(Direction direction, float speed)
{
	switch (direction)
	{
		case Direction::FORWARDS:
			cameraPosition += speed * frontVector;
			break;
		case Direction::BACKWARDS:
			cameraPosition -= speed * frontVector;
			break;
		case Direction::LEFT:
			cameraPosition -= glm::normalize(glm::cross(frontVector, upVector)) * speed;
			break;
		case Direction::RIGHT:
			cameraPosition += glm::normalize(glm::cross(frontVector, upVector)) * speed;
			break;
		case Direction::UP:
			cameraPosition.y += speed;
			break;
		case Direction::DOWN:
			cameraPosition.y -= speed;
			break;
		default:
			break;
	}

	calculateViewMatrix();
}

void Camera::calculateViewMatrix()
{
	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + frontVector, upVector);
}

void Camera::calculateProjectionMatrix()
{
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}
