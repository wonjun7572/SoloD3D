#include "..\Public\MathUtils.h"
#include "Engine_Define.h"

bool CMathUtils::InBounds(const XMFLOAT4 & vThis, const XMFLOAT4 & Bounds)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR v2 = XMLoadFloat4(&Bounds);
	return XMVector4InBounds(v1, v2);
}

float CMathUtils::Length(const XMFLOAT4 & vThis)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR X = XMVector4Length(v1);
	return XMVectorGetX(X);
}

float CMathUtils::LengthSquared(const XMFLOAT4 & vThis)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR X = XMVector4LengthSq(v1);
	return XMVectorGetX(X);
}

float CMathUtils::Dot(const XMFLOAT4 & vThis, const XMFLOAT4 & V)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR v2 = XMLoadFloat4(&V);
	XMVECTOR X = XMVector4Dot(v1, v2);
	return XMVectorGetX(X);
}

XMFLOAT4 CMathUtils::Cross(const XMFLOAT4& vThis, const XMFLOAT4& v1, const XMFLOAT4& v2)
{
	XMVECTOR x1 = XMLoadFloat4(&vThis);
	XMVECTOR x2 = XMLoadFloat4(&v1);
	XMVECTOR x3 = XMLoadFloat4(&v2);
	XMVECTOR R = XMVector4Cross(x1, x2, x3);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, R);
	return result;
}

XMFLOAT4 CMathUtils::Normalize(XMFLOAT4 & vThis)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR X = XMVector4Normalize(v1);
	XMStoreFloat4(&vThis, X);
	return vThis;
}

XMFLOAT4 CMathUtils::Clamp(const XMFLOAT4 & vmin, const XMFLOAT4 & vmax, XMFLOAT4 & vThis)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR v2 = XMLoadFloat4(&vmin);
	XMVECTOR v3 = XMLoadFloat4(&vmax);
	XMVECTOR X = XMVectorClamp(v1, v2, v3);
	XMStoreFloat4(&vThis, X);
	return vThis;
}

float CMathUtils::Distance(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector4Length(V);
	return XMVectorGetX(X);
}

float CMathUtils::DistanceSquared(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector4LengthSq(V);
	return XMVectorGetX(X);
}

XMFLOAT4 CMathUtils::Min(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorMin(x1, x2);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Max(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorMax(x1, x2);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Lerp(const XMFLOAT4 & v1, const XMFLOAT4 & v2, float t)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::SmoothStep(const XMFLOAT4 & v1, const XMFLOAT4 & v2, float t)
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t*(3.f - 2.f*t);
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Barycentric(const XMFLOAT4 & v1, const XMFLOAT4 & v2, const XMFLOAT4 & v3, float f, float g)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR x3 = XMLoadFloat4(&v3);
	XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::CatmullRom(const XMFLOAT4 & v1, const XMFLOAT4 & v2, const XMFLOAT4 & v3, const XMFLOAT4 & v4, float t)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR x3 = XMLoadFloat4(&v3);
	XMVECTOR x4 = XMLoadFloat4(&v4);
	XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Hermite(const XMFLOAT4 & v1, const XMFLOAT4 & t1, const XMFLOAT4 & v2, const XMFLOAT4 & t2, float t)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&t1);
	XMVECTOR x3 = XMLoadFloat4(&v2);
	XMVECTOR x4 = XMLoadFloat4(&t2);
	XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Reflect(const XMFLOAT4 & ivec, const XMFLOAT4 & nvec)
{
	XMVECTOR i = XMLoadFloat4(&ivec);
	XMVECTOR n = XMLoadFloat4(&nvec);
	XMVECTOR X = XMVector4Reflect(i, n);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Reflect(const XMFLOAT4 & ivec, const XMFLOAT4 & nvec, float refractionIndex)
{
	XMVECTOR i = XMLoadFloat4(&ivec);
	XMVECTOR n = XMLoadFloat4(&nvec);
	XMVECTOR X = XMVector4Refract(i, n, refractionIndex);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::Transform(const XMFLOAT4 & v, const XMFLOAT4X4 & m)
{
	XMVECTOR v1 = XMLoadFloat4(&v);
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVECTOR X = XMVector4Transform(v1, M);

	XMFLOAT4 result = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Transform(const XMFLOAT4 * varray, size_t count, const XMFLOAT4X4 & m, OUT XMFLOAT4 * resultArray)
{
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVector4TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT4), count, M);
}

bool CMathUtils::InBounds(const XMFLOAT3 & vThis, const XMFLOAT3 & Bounds)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR v2 = XMLoadFloat3(&Bounds);
	return XMVector3InBounds(v1, v2);
}

float CMathUtils::Length(const XMFLOAT3 & vThis)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR X = XMVector3Length(v1);
	return XMVectorGetX(X);
}

float CMathUtils::LengthSquared(const XMFLOAT3 & vThis)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR X = XMVector3LengthSq(v1);
	return XMVectorGetX(X);
}

float CMathUtils::Dot(const XMFLOAT3 & vThis, const XMFLOAT3 & V)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR v2 = XMLoadFloat3(&V);
	XMVECTOR X = XMVector3Dot(v1, v2);
	return XMVectorGetX(X);
}

XMFLOAT3 CMathUtils::Cross(const XMFLOAT3& vThis, const XMFLOAT3& V)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR v2 = XMLoadFloat3(&V);
	XMVECTOR R = XMVector3Cross(v1, v2);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, R);
	return result;
}

XMFLOAT3 CMathUtils::Normalize(XMFLOAT3 & vThis)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR X = XMVector3Normalize(v1);
	XMStoreFloat3(&vThis, X);
	return vThis;
}

XMFLOAT3 CMathUtils::Clamp(const XMFLOAT3 & vmin, const XMFLOAT3 & vmax, XMFLOAT3 & vThis)
{
	XMVECTOR v1 = XMLoadFloat3(&vThis);
	XMVECTOR v2 = XMLoadFloat3(&vmin);
	XMVECTOR v3 = XMLoadFloat3(&vmax);
	XMVECTOR X = XMVectorClamp(v1, v2, v3);
	XMStoreFloat3(&vThis, X);
	return vThis;
}

float CMathUtils::Distance(const XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector3Length(V);
	return XMVectorGetX(X);
}

float CMathUtils::DistanceSquared(const XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector3LengthSq(V);
	return XMVectorGetX(X);
}

XMFLOAT3 CMathUtils::Min(const XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR X = XMVectorMin(x1, x2);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Max(const XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR X = XMVectorMax(x1, x2);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Lerp(const XMFLOAT3 & v1, const XMFLOAT3 & v2, float t)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::SmoothStep(const XMFLOAT3 & v1, const XMFLOAT3 & v2, float t)
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t*(3.f - 2.f*t);
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Barycentric(const XMFLOAT3 & v1, const XMFLOAT3 & v2, const XMFLOAT3 & v3, float f, float g)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR x3 = XMLoadFloat3(&v3);
	XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::CatmullRom(const XMFLOAT3 & v1, const XMFLOAT3 & v2, const XMFLOAT3 & v3, const XMFLOAT3 & v4, float t)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR x3 = XMLoadFloat3(&v3);
	XMVECTOR x4 = XMLoadFloat3(&v4);
	XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Hermite(const XMFLOAT3 & v1, const XMFLOAT3 & t1, const XMFLOAT3 & v2, const XMFLOAT3 & t2, float t)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&t1);
	XMVECTOR x3 = XMLoadFloat3(&v2);
	XMVECTOR x4 = XMLoadFloat3(&t2);
	XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Reflect(const XMFLOAT3 & ivec, const XMFLOAT3 & nvec)
{
	XMVECTOR i = XMLoadFloat3(&ivec);
	XMVECTOR n = XMLoadFloat3(&nvec);
	XMVECTOR X = XMVector3Reflect(i, n);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Reflect(const XMFLOAT3 & ivec, const XMFLOAT3 & nvec, float refractionIndex)
{
	XMVECTOR i = XMLoadFloat3(&ivec);
	XMVECTOR n = XMLoadFloat3(&nvec);
	XMVECTOR X = XMVector3Refract(i, n, refractionIndex);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Transform(const XMFLOAT3 & v, const XMFLOAT4X4 & m)
{
	XMVECTOR v1 = XMLoadFloat3(&v);
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVECTOR X = XMVector3TransformCoord(v1, M);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

void CMathUtils::Transform(const XMFLOAT3 * varray, size_t count, const XMFLOAT4X4 & m, OUT XMFLOAT3 * resultArray)
{
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVector3TransformCoordStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, M);
}

void CMathUtils::Transform(const XMFLOAT3 & v, const XMFLOAT4X4 & m, OUT XMFLOAT4 & result)
{
	XMVECTOR v1 = XMLoadFloat3(&v);
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVECTOR X = XMVector3Transform(v1, M);
	XMStoreFloat4(&result, X);
}

void CMathUtils::Transform(const XMFLOAT3 * varray, size_t count, const XMFLOAT4X4 & m, OUT XMFLOAT4 * resultArray)
{
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVector3TransformStream(resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT3), count, M);
}

XMFLOAT3 CMathUtils::TransformNormal(const XMFLOAT3 & v, const XMFLOAT4X4 & m)
{
	XMVECTOR v1 = XMLoadFloat3(&v);
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVECTOR X = XMVector3TransformNormal(v1, M);

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

void CMathUtils::TransformNormal(const XMFLOAT3 * varray, size_t count, const XMFLOAT4X4 & m, OUT XMFLOAT3 * resultArray)
{
	XMMATRIX M = XMLoadFloat4x4(&m);
	XMVector3TransformNormalStream(resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, M);
}

XMFLOAT3 CMathUtils::Add_Float3(const XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR X = x1 + x2;

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT3 CMathUtils::Sub_Float3(const XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	XMVECTOR x1 = XMLoadFloat3(&v1);
	XMVECTOR x2 = XMLoadFloat3(&v2);
	XMVECTOR X = x1 - x2;

	XMFLOAT3 result = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&result, X);
	return result;
}

XMFLOAT4 CMathUtils::MulNum_Float4(const float & f1, const XMFLOAT4 & v1)
{
	return XMFLOAT4(f1 * v1.x, f1* v1.y, f1*v1.z, v1.w);
}

XMFLOAT3 CMathUtils::MulNum_Float3(const float & f1, const XMFLOAT3 & v1)
{
	return XMFLOAT3(f1 * v1.x, f1* v1.y, f1*v1.z);
}

XMFLOAT4X4 CMathUtils::Add_Matrix(const XMFLOAT4X4 & M1, const XMFLOAT4X4 & M2)
{
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

	XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
	XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
	XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
	XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

	x1 = XMVectorAdd(x1, y1);
	x2 = XMVectorAdd(x2, y2);
	x3 = XMVectorAdd(x3, y3);
	x4 = XMVectorAdd(x4, y4);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());

	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
	return R;
}

XMFLOAT4X4 CMathUtils::Sub_Matrix(const XMFLOAT4X4 & M1, const XMFLOAT4X4 & M2)
{
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

	XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
	XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
	XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
	XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

	x1 = XMVectorSubtract(x1, y1);
	x2 = XMVectorSubtract(x2, y2);
	x3 = XMVectorSubtract(x3, y3);
	x4 = XMVectorSubtract(x4, y4);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());

	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
	return R;
}

XMFLOAT4X4 CMathUtils::Mul_Matrix(const XMFLOAT4X4 & M1, const XMFLOAT4X4 & M2)
{
	XMMATRIX m1 = XMLoadFloat4x4(&M1);
	XMMATRIX m2 = XMLoadFloat4x4(&M2);
	XMMATRIX X = XMMatrixMultiply(m1, m2);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, X);
	return R;
}

XMFLOAT4X4 CMathUtils::Div_Matrix(const XMFLOAT4X4 & M1, const XMFLOAT4X4 & M2)
{
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

	XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
	XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
	XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
	XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

	x1 = XMVectorDivide(x1, y1);
	x2 = XMVectorDivide(x2, y2);
	x3 = XMVectorDivide(x3, y3);
	x4 = XMVectorDivide(x4, y4);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
	return R;
}

XMFLOAT4X4 CMathUtils::Mul_MatrixFloat(const XMFLOAT4X4 & M, float S)
{
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

	x1 = XMVectorScale(x1, S);
	x2 = XMVectorScale(x2, S);
	x3 = XMVectorScale(x3, S);
	x4 = XMVectorScale(x4, S);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
	return R;
}

XMFLOAT4X4 CMathUtils::Div_MatrixFloat(const XMFLOAT4X4 & M, float S)
{
	using namespace DirectX;
	assert(S != 0.f);

	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

	float rs = 1.f / S;

	x1 = XMVectorScale(x1, rs);
	x2 = XMVectorScale(x2, rs);
	x3 = XMVectorScale(x3, rs);
	x4 = XMVectorScale(x4, rs);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
	return R;
}

XMFLOAT4X4 CMathUtils::Mul_FloatMatrix(float S, const XMFLOAT4X4 & M)
{
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

	x1 = XMVectorScale(x1, S);
	x2 = XMVectorScale(x2, S);
	x3 = XMVectorScale(x3, S);
	x4 = XMVectorScale(x4, S);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
	return R;
}

XMFLOAT4X4 CMathUtils::Transpose(const XMFLOAT4X4 & mThis)
{
	XMMATRIX M = XMLoadFloat4x4(&mThis);
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixTranspose(M));
	return R;
}

XMFLOAT4X4 CMathUtils::Invert(const XMFLOAT4X4 & mThis)
{
	XMMATRIX M = XMLoadFloat4x4(&mThis);
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMVECTOR det;
	XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
	return R;
}

float CMathUtils::Determinant(const XMFLOAT4X4 & mThis)
{
	XMMATRIX M = XMLoadFloat4x4(&mThis);
	return XMVectorGetX(XMMatrixDeterminant(M));
}

bool CMathUtils::Equal_Matrix(const XMFLOAT4X4 & M1, const XMFLOAT4X4 & M2)
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

	XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
	XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
	XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
	XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

	if ((XMVector4Equal(x1, y1)
		&& XMVector4Equal(x2, y2)
		&& XMVector4Equal(x3, y3)
		&& XMVector4Equal(x4, y4)) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

XMFLOAT4X4 CMathUtils::CreateTranslation(const XMFLOAT3 & vPosition)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateTranslation(float x, float y, float z)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixTranslation(x, y, z));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateScale(const XMFLOAT3 & vScale)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixScaling(vScale.x, vScale.y, vScale.z));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateScale(float xs, float ys, float zs)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixScaling(xs, ys, zs));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateScale(float scale)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixScaling(scale, scale, scale));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateRotationX(float radians)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixRotationX(radians));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateRotationY(float radians)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixRotationY(radians));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateRotationZ(float radians)
{
	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat4x4(&R, XMMatrixRotationZ(radians));
	return R;
}

XMFLOAT4X4 CMathUtils::CreateWorld(const XMFLOAT3 & position, const XMFLOAT3 & forward, const XMFLOAT3 & up)
{
	XMVECTOR zaxis = XMVector3Normalize(XMVectorNegate(XMLoadFloat3(&forward)));
	XMVECTOR yaxis = XMLoadFloat3(&up);
	XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
	yaxis = XMVector3Cross(zaxis, xaxis);

	XMFLOAT4X4 R;
	XMStoreFloat4x4(&R, XMMatrixIdentity());
	XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), xaxis);
	XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), yaxis);
	XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), zaxis);
	R._14 = R._24 = R._34 = 0.f;
	R._41 = position.x; R._42 = position.y; R._43 = position.z;
	R._44 = 1.f;
	return R;
}

XMFLOAT4X4 CMathUtils::Lerp(const XMFLOAT4X4 & M1, const XMFLOAT4X4 & M2, float t)
{
	XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
	XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
	XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
	XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

	XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
	XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
	XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
	XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

	x1 = XMVectorLerp(x1, y1, t);
	x2 = XMVectorLerp(x2, y2, t);
	x3 = XMVectorLerp(x3, y3, t);
	x4 = XMVectorLerp(x4, y4, t);

	XMFLOAT4X4 result;
	XMStoreFloat4x4(&result, XMMatrixIdentity());
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
	return result;
}
