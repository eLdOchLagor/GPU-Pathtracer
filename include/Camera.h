#pragma once

#include "VectorUtils4.h"



class Camera
{
public:
	Camera(vec3 pos, vec3 fwd, vec3 up, float fov, int width, int height) {
		_position = pos;
		_forward = normalize(fwd);
		
		_aspectRatio = (float)width / height;
		_right = normalize(cross(_forward, up));
		_trueUp = cross(_right, _forward);
		_imagePlaneHeight = 2 * tan(M_PI*fov/180.0f / 2.0f);
		_imagePlaneWidth = _imagePlaneHeight * _aspectRatio;
	};

	vec3 GetPosition() { return _position; }
	vec3 GetForward() { return _forward; }
	vec3 GetUp() { return _trueUp; }
	vec3 GetRight() { return _right; }

    float GetImagePlaneHeight() { return _imagePlaneHeight; }
	float GetImagePlaneWidth() { return _imagePlaneWidth; }

private:
	vec3 _position;
	vec3 _forward;
	vec3 _trueUp;
	vec3 _right;
	
	float _aspectRatio; // Only used in Camera constructor

	float _imagePlaneHeight;
	float _imagePlaneWidth;
};
