#version 450 core

const vec4 QuadPoints[4] = vec4[4]( vec4(-1.0, -1.0, 1.0, 1.0),
									vec4(1.0, -1.0, 1.0, 1.0),
									vec4(1.0, 1.0, 1.0, 1.0),
									vec4(-1.0, 1.0, 1.0, 1.0));

out vec3 RayDirection;
layout(location = 0)uniform float AspectRatio;
layout(location = 3)uniform vec2 CameraAngle;

void main(void)
{
	gl_Position = QuadPoints[gl_VertexID];
	RayDirection = normalize(QuadPoints[gl_VertexID].xyz*vec3(AspectRatio, 1.0, 1.0));

	float cosX = cos(CameraAngle.x);
	float sinX = sin(CameraAngle.x);
	mat3 xRotation = mat3(1.0, 0.0, 0.0, 0.0, cosX, sinX, 0.0, -sinX, cosX);

	float cosY = cos(CameraAngle.y);
	float sinY = sin(CameraAngle.y);
	mat3 yRotation = mat3(cosY, 0.0, -sinY, 0.0, 1.0, 0.0, sinY, 0.0, cosY);

	mat3 rotation = xRotation*yRotation;

	RayDirection = RayDirection*rotation;
}
