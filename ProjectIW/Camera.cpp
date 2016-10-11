#include "Camera.h"

const XMVECTOR UnitXVector = XMVectorSet(1, 0, 0, 0);
const XMVECTOR UnitYVector = XMVectorSet(0, 1, 0, 0);
const XMVECTOR UnitZVector = XMVectorSet(0, 0, 1, 0);

Camera::Camera(float aspectRatio)
{
	rotationX = PI * 0.25f;
	rotationY = PI * 0.25f;
	ZeroMemory(&terrain_constants, sizeof(terrain_constants));
	Rotation = XMMatrixRotationX(rotationY) * XMMatrixRotationY(rotationX);
	SetWorld(XMMatrixIdentity());
	SetProjection(aspectRatio);

	this->SetPosition(XMFLOAT3(0, 1, 0), XMFLOAT3(0, 0, 0));
	VectorPosition = XMVectorSet(0, 1, 0, 0);

	//SetView(XMMatrixLookAtLH(this->VectorPosition, this->VectorTarget, this->VectorUp));

	terrain_buffer = new ConstantsBuffer();
	terrain_buffer->Create(sizeof(terrain_constants), &terrain_constants);

	speed_multiplier = 0.0f;

	UpdateFirstPerson(true);
}

Camera::~Camera()
{
	if (terrain_buffer)
	{
		delete terrain_buffer;
		terrain_buffer = 0;
	}
}

void Camera::SetPosition(XMFLOAT3& pos, XMFLOAT3& target)
{
	if (&Position != &pos)
		memcpy(&Position, &pos, sizeof(pos));
	if (&Target != &target)
		memcpy(&Target, &target, sizeof(target));
	this->VectorPosition = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	this->VectorTarget = XMVectorSet(target.x, target.y, target.z, 1.0f);
	this->VectorUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	SetView(XMMatrixLookAtLH(this->VectorPosition, this->VectorTarget, this->VectorUp));
}

XMMATRIX Camera::GetWorld()
{
	return World;
}
XMMATRIX Camera::GetView()
{
	return View;
}
XMMATRIX Camera::GetProjection()
{
	return Projection;
}
void Camera::SetWorld(XMMATRIX& m)
{
	World = XMMatrixTranspose(m);
	terrain_constants.world = World;
}
void Camera::SetView(XMMATRIX& m)
{
	View = XMMatrixTranspose(m);
	terrain_constants.view = View;
}
void Camera::SetProjection(XMMATRIX& m)
{
	Projection = XMMatrixTranspose(m);
	terrain_constants.projection = Projection;
}
void Camera::SetProjection(float aspectRatio)
{
	Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(PI / 4.0f, aspectRatio, 0.01f, 10000.0f * 1000.0f));
	terrain_constants.projection = Projection;
}

bool Camera::UpdateFirstPerson(bool update)
{
	if (!update)
	{
		int deltaX = engine->input->CurrentDeltaX();
		int deltaY = engine->input->CurrentDeltaY();
		if (!deltaX && !deltaY)
			goto CheckKeys;
		rotationX += deltaX * 0.01f;
		rotationY += deltaY * 0.01f;
		Rotation = XMMatrixRotationX(rotationY) * XMMatrixRotationY(rotationX);
	}
	VectorUp = XMVector3Transform(UnitYVector, Rotation);
	update = true;

CheckKeys:
	float speed = 0.25f;

	if (engine->input->CurrentKey(DIK_LSHIFT))
		speed *= 5.0f;
	if (engine->input->CurrentKey(DIK_SPACE))
		speed *= 20.0f;
	if (engine->input->CurrentKey(DIK_LCONTROL))
		speed *= 20.0f;
	
	if (engine->input->CurrentKeyOnly(DIK_1))
		speed_multiplier += 1.0f;
	if (engine->input->CurrentKeyOnly(DIK_2) && speed_multiplier >= 0.99f)
		speed_multiplier -= 1.0f;
	speed *= powf(2, speed_multiplier);

	if (engine->input->CurrentKey(DIK_W))
	{
		VectorPosition = VectorPosition + XMVector3Transform(UnitZVector * speed, Rotation);
		update = true;
	}
	else if (engine->input->CurrentKey(DIK_S))
	{
		VectorPosition = VectorPosition + XMVector3Transform(-UnitZVector * speed, Rotation);
		update = true;
	}
	if (engine->input->CurrentKey(DIK_A))
	{
		VectorPosition = VectorPosition + XMVector3Transform(-UnitXVector * speed, Rotation);
		update = true;
	}
	else if (engine->input->CurrentKey(DIK_D))
	{
		VectorPosition = VectorPosition + XMVector3Transform(UnitXVector * speed, Rotation);
		update = true;
	}
	if (engine->input->CurrentKey(DIK_Q))
	{
		VectorPosition = VectorPosition + UnitYVector * speed;
		update = true;
	}
	else if (engine->input->CurrentKey(DIK_E))
	{
		VectorPosition = VectorPosition + UnitYVector * -speed;
		update = true;
	}
	/*if (engine->input->CurrentKey(DIK_SPACE))
	{
		VectorPosition = VectorPosition + XMVector3Transform(UnitYVector * speed, Rotation);
		update = true;
	}*/

	if (!update)
		return false;
	Position = XMFLOAT3(XMVectorGetX(VectorPosition), XMVectorGetY(VectorPosition), XMVectorGetZ(VectorPosition));
	VectorTarget = VectorPosition + XMVector3Transform(UnitZVector, Rotation);
	Target = XMFLOAT3(XMVectorGetX(VectorTarget), XMVectorGetY(VectorTarget), XMVectorGetZ(VectorTarget));
	SetView(XMMatrixLookAtLH(this->VectorPosition, this->VectorTarget, this->VectorUp));
	return true;
}

void Camera::SetShaderConstants()
{
	terrain_buffer->SetData(&terrain_constants, 0, sizeof(terrain_constants));
}
