#version 450 core

layout(location = 0)out vec4 Color;
layout(location = 1)uniform float Time;
layout(location = 2)uniform vec3 Camera;

const int SpheresCount = 7;
const int PlanesCount = 6;
const int CubesCount = 1;
const int MaxRayBounces = 16;
const float Infinity = 1.0e13;
const float LightIntensity = 0.96;
const float AmbientIntensity = 0.087;
const float ShadowDarkness = 0.197;
const float SunBrightness = 0.967;

struct Sphere
{
	vec3 Coords;
	float r;
	vec3 Color;
	float ReflectionRatio;
	float SpecularRatio;
	float SpecularScale;
};

struct Plane
{
	vec3 Normal;
	float Distance;
	vec3 Color;
	float ReflectionRatio;
	float SpecularRatio;
	float SpecularScale;
};

struct Cube
{
	vec3 Coords;
	float EdgeHalfLength;
	vec3 Color;
	float ReflectionRatio;
	float SpecularRatio;
	float SpecularScale;
};

layout(binding = 0, std430)buffer Objects
{
	Sphere Spheres[SpheresCount];
	Plane Planes[PlanesCount];
	Cube Cubes[CubesCount];
};

const int SunIndex = SpheresCount - 1;

in vec3 RayDirection;

float CheckSphereIntersection(Sphere CheckedSphere, vec3 Origin, vec3 Direction, out float tDelta)
{
	vec3 OC = Origin - CheckedSphere.Coords;
	float A = dot(Direction, Direction);
	float B = -2.0*dot(OC, Direction);
	float C = dot(OC, OC) - CheckedSphere.r*CheckedSphere.r;
	float D = B*B - 4.0*A*C;

	if(D < 0.0)
	{
		tDelta = 0.0;
		return Infinity;
	}

	D = sqrt(D);
	float t0 = (B + D)/(2.0*A);
	float t1 = (B - D)/(2.0*A);
	tDelta = abs(t0 - t1)/(2.0*CheckedSphere.r);
	float t = min(max(t0, 0.0), max(t1, 0.0));

	if(t == 0.0)
		return Infinity;

	return t;
}

float CheckPlaneIntersection(Plane CheckedPlane, vec3 Origin, vec3 Direction)
{
	float Denominator = -dot(Direction, CheckedPlane.Normal);
	if(Denominator <= 0.0)
		return Infinity;

	float t = (dot(Origin, CheckedPlane.Normal) + CheckedPlane.Distance)/Denominator;

	return t;
}

float CheckCubeIntersection(Cube CheckedCube, vec3 Origin, vec3 Direction, out vec3 outNormal)
{
	vec3 m = 1.0/(Direction - CheckedCube.Coords); // can precompute if traversing a set of aligned boxes
	vec3 n = m*(Origin - CheckedCube.Coords);   // can precompute if traversing a set of aligned boxes
	vec3 k = abs(m)*CheckedCube.EdgeHalfLength*2.0;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	if( tN>tF || tF<0.0) return Infinity; // no intersection
	outNormal = -sign(Direction - CheckedCube.Coords)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
	return tF;
}


void main(void)
{
	float t, Nearest_t, tDelta;
	int NearestObjectIndex;
	vec3 NearestObjectColor, NearestCubeNormal, CubeNormal;

	vec3 IntersectionPoint, Normal;
	vec3 Origin = Camera, DirectionVector = RayDirection;

	float LightAmount, ReflectionRatio, SpecularRatio, SpecularScale, AccumedReflectionRatio = 1.0, PrevLightAmount = 1.0, SpecularAccumedScale = 1.0;
	vec3 AccumulatedColor = vec3(0.0);
	vec3 LightVector;

	for(int i = 0; i < MaxRayBounces + 1; i++)
	{
		Nearest_t = Infinity;
		LightAmount = 0.0;
		for(int j = 0; j < SpheresCount; j++)
		{
			t = CheckSphereIntersection(Spheres[j], Origin, DirectionVector, tDelta);
			if(Nearest_t > t)
			{
				Nearest_t = t;
				NearestObjectIndex = j;
				if(j == SunIndex)
					LightAmount = SunBrightness;
			}
		}
		if(Nearest_t < Infinity)
		{
			ReflectionRatio = Spheres[NearestObjectIndex].ReflectionRatio;
			NearestObjectColor = Spheres[NearestObjectIndex].Color;
			IntersectionPoint = Origin + Nearest_t*DirectionVector;
			Normal = normalize(IntersectionPoint - Spheres[NearestObjectIndex].Coords);
			SpecularRatio = Spheres[NearestObjectIndex].SpecularRatio;
			SpecularScale = Spheres[NearestObjectIndex].SpecularScale;
			if(ReflectionRatio > 1.0)
			{
				vec3 RefractedVector = refract(DirectionVector, Normal, ReflectionRatio - 1.0);
				CheckSphereIntersection(Spheres[NearestObjectIndex], IntersectionPoint, RefractedVector, tDelta);
				Origin = IntersectionPoint + 2.0*Spheres[NearestObjectIndex].r*tDelta*RefractedVector;
				continue;
			}
		}
/*
		NearestObjectIndex = -1;
		for(int j = 0; j < CubesCount; j++)
		{
			t = CheckCubeIntersection(Cubes[j], Origin, DirectionVector, CubeNormal);
			if(Nearest_t > t)
			{
				NearestCubeNormal = CubeNormal;
				Nearest_t = t;
				NearestObjectIndex = j;
			}
		}
		if(Nearest_t < Infinity && NearestObjectIndex != -1)
		{
			ReflectionRatio = Cubes[NearestObjectIndex].ReflectionRatio;
			NearestObjectColor = Cubes[NearestObjectIndex].Color;
			IntersectionPoint = Origin + Nearest_t*DirectionVector;
			Normal = NearestCubeNormal;
			SpecularRatio = Cubes[NearestObjectIndex].SpecularRatio;
			SpecularScale = Cubes[NearestObjectIndex].SpecularScale;
		}*/

		NearestObjectIndex = -1;
		for(int j = 0; j < PlanesCount; j++)
		{
			t = CheckPlaneIntersection(Planes[j], Origin, DirectionVector);
			if(Nearest_t > t)
			{
				Nearest_t = t;
				NearestObjectIndex = j;
			}
		}
		if(NearestObjectIndex != -1)
		{
			ReflectionRatio = Planes[NearestObjectIndex].ReflectionRatio;
			NearestObjectColor = Planes[NearestObjectIndex].Color;
			IntersectionPoint = Origin + Nearest_t*DirectionVector;
			Normal = Planes[NearestObjectIndex].Normal;
			SpecularRatio = Planes[NearestObjectIndex].SpecularRatio;
			SpecularScale = Planes[NearestObjectIndex].SpecularScale;
		}

		if(Nearest_t == Infinity)
			break;

		LightVector = Spheres[SunIndex].Coords - IntersectionPoint;
		for(int j = 0; j < SunIndex && t > 1.0; j++)
		{
			if(Spheres[j].ReflectionRatio > 1.0)
				continue;
			t = CheckSphereIntersection(Spheres[j], IntersectionPoint + Normal*0.01, LightVector, tDelta);
		}


		LightVector = normalize(LightVector);
		LightAmount += max(dot(LightVector, Normal), AmbientIntensity)*LightIntensity;
		LightAmount *= mix(1.0, ShadowDarkness, (1.0 - ReflectionRatio)*step(t, 1.0)*smoothstep(0.067, 0.14, tDelta));

		DirectionVector = reflect(DirectionVector, Normal);
		AccumulatedColor += (NearestObjectColor*LightAmount + vec3(pow(max(dot(DirectionVector, LightVector), 0.0), SpecularRatio)*SpecularScale*SpecularAccumedScale))*PrevLightAmount*(1.0 - ReflectionRatio)*AccumedReflectionRatio;

		if(ReflectionRatio == 0.0)
			break;

		Origin = IntersectionPoint;
		PrevLightAmount *= LightAmount;
		AccumedReflectionRatio *= ReflectionRatio;
		SpecularAccumedScale *= SpecularScale;
	}
	Color = vec4(AccumulatedColor, 1.0);
}
