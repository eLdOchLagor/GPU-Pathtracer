#pragma once

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class Camera
{
public:
	Camera(glm::fvec3 pos, glm::fvec3 fwd, glm::fvec3 up, float fov, int width, int height) {
		_position = pos;
		_forward = glm::normalize(fwd);
		
		_aspectRatio = (float)width / height;
		_right = glm::normalize(glm::cross(_forward, up));
		_trueUp = glm::cross(_right, _forward);
		_imagePlaneHeight = 2 * glm::tan(glm::radians(fov) / 2.0f);
		_imagePlaneWidth = _imagePlaneHeight * _aspectRatio;
	};

	glm::fvec3 GetPosition() { return _position; }
	glm::fvec3 GetForward() { return _forward; }
	glm::fvec3 GetUp() { return _trueUp; }
	glm::fvec3 GetRight() { return _right; }

    float GetImagePlaneHeight() { return _imagePlaneHeight; }
	float GetImagePlaneWidth() { return _imagePlaneWidth; }

private:
	glm::fvec3 _position;
	glm::fvec3 _forward;
	glm::fvec3 _trueUp;
	glm::fvec3 _right;
	
	float _aspectRatio; // Only used in Camera constructor

	float _imagePlaneHeight;
	float _imagePlaneWidth;
};
