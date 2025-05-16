#pragma once

#include "VectorUtils4.h"

class Camera
{
public:
	Camera() {};

	Camera(vec3 pos, vec3 fwd, vec3 up, float fov, int width, int height) {
		position = pos;
		_forward = normalize(fwd);
		
		_fov = fov;

		_aspectRatio = (float)width / height;
		_right = normalize(cross(_forward, up));
		_trueUp = cross(_right, _forward);
		_imagePlaneHeight = 2 * tan(M_PI* _fov/180.0f / 2.0f);
		_imagePlaneWidth = _imagePlaneHeight * _aspectRatio;

		lastX = width / 2.0f;
	    lastY = height / 2.0f;

		viewMatrix = lookAt(position, position + _forward, _trueUp);
		projectionMatrix = perspective(_fov * M_PI / 180.0f, _aspectRatio, 0.1f, 100.0f);
	};

	vec3 GetPosition() { return position; }
	vec3 GetForward() { return _forward; }
	vec3 GetUp() { return _trueUp; }
	vec3 GetRight() { return _right; }

    float GetImagePlaneHeight() { return _imagePlaneHeight; }
	float GetImagePlaneWidth() { return _imagePlaneWidth; }
	float GetFOV() { return _fov; }

	void SetForward(vec3 fwd) { 
		_forward = normalize(fwd); 
		UpdateCameraVectors();
	}

	void SetFOV(float fov) {
		_fov = fov;
		UpdateCameraPlane();
	}

	void UpdateCameraVectors() {
		_right = normalize(cross(_forward, vec3(0.0f, 1.0f, 0.0f)));
		_trueUp = cross(_right, _forward);

		viewMatrix = lookAt(position, position + _forward, _trueUp);
	}

	void UpdateCameraPlane() {
		_imagePlaneHeight = 2 * tan(M_PI * _fov / 180.0f / 2.0f);
		_imagePlaneWidth = _imagePlaneHeight * _aspectRatio;

		// A change in fov occurs
		projectionMatrix = perspective(_fov * M_PI / 180.0f, _aspectRatio, 0.1f, 100.0f);
	}

	vec3 position;

	// Mouse movement
	float lastX, lastY;
	float pitch = 0.0f;
	float yaw = 0.0f;

	bool cameraEnabled = false;

	mat4 viewMatrix;
	mat4 projectionMatrix;

private:
	vec3 _forward;
	vec3 _trueUp;
	vec3 _right;

	float _fov;
	float _aspectRatio; // Only used in Camera constructor

	float _imagePlaneHeight;
	float _imagePlaneWidth;
};
