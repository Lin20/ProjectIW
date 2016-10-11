#pragma once

#include <d3d11.h>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace MDC
{
	struct VertexPositionColorNormal
	{
		Vector3 position;
		Vector3 color;
		Vector3 normal;

		VertexPositionColorNormal()
		{
			position = Vector3(0, 0, 0);
			color = Vector3(0.3f, 0.5f, 0.7f);
			normal = Vector3(0, 0, 0);
		}

		VertexPositionColorNormal(Vector3 _position, Vector3 _normal)
		{
			position = Vector3(_position.x, _position.y, _position.z);
			color = Vector3(0.3f, 0.5f, 0.7f);
			normal = _normal;
		}

		static D3D11_INPUT_ELEMENT_DESC layout[];
		static const unsigned int num_elements = 3;
	};
}
