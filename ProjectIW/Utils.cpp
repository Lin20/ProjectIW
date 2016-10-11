#pragma once

#include <d3d11.h>
#include <D3DX11.h>
#include "General.h"

using namespace DirectX;

bool RayIntersectsTriangle(XMFLOAT3 rayDirection, XMFLOAT3 rayStart, XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3)
{
	// Compute vectors along two edges of the triangle.
	XMFLOAT3 edge1, edge2;

	edge1 = ADD(vertex2, -vertex1);
	edge2 = ADD(vertex3, -vertex1);

	// Compute the determinant.
	XMFLOAT3 directionCrossEdge2;
	directionCrossEdge2 = CROSS(rayDirection, edge2);

	float determinant;
	determinant = DOT(edge1, directionCrossEdge2);

	// If the ray is parallel to the triangle plane, there is no collision.
	if (determinant > -FLT_EPSILON && determinant < FLT_EPSILON)
	{
		return false;
	}

	float inverseDeterminant = 1.0f / determinant;

	// Calculate the U parameter of the intersection point.
	XMFLOAT3 distanceVector;
	distanceVector = ADD(rayStart, -vertex1);

	float triangleU;
	triangleU = DOT(distanceVector, directionCrossEdge2);
	triangleU *= inverseDeterminant;

	// Make sure it is inside the triangle.
	if (triangleU < 0 || triangleU > 1)
	{
		return false;
	}

	// Calculate the V parameter of the intersection point.
	XMFLOAT3 distanceCrossEdge1;
	distanceCrossEdge1 = CROSS(distanceVector, edge1);

	float triangleV;
	triangleV = DOT(rayDirection, distanceCrossEdge1);
	triangleV *= inverseDeterminant;

	// Make sure it is inside the triangle.
	if (triangleV < 0 || triangleU + triangleV > 1)
	{
		return false;
	}

	// Compute the distance along the ray to the triangle.
	float rayDistance;
	rayDistance = DOT(edge2, distanceCrossEdge1);
	rayDistance *= inverseDeterminant;

	// Is the triangle behind the ray origin?
	if (rayDistance < 0)
	{
		return false;
	}

	return true;
}