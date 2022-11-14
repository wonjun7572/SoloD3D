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

void CMathUtils::Cross(const XMFLOAT4 & vThis, const XMFLOAT4 & v1, const XMFLOAT4 & v2, XMFLOAT4 & result)
{
	XMVECTOR x1 = XMLoadFloat4(&vThis);
	XMVECTOR x2 = XMLoadFloat4(&v1);
	XMVECTOR x3 = XMLoadFloat4(&v2);
	XMVECTOR R = XMVector4Cross(x1, x2, x3);
	XMStoreFloat4(&result, R);
}

XMFLOAT4 CMathUtils::Cross(const XMFLOAT4 & vThis, const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&vThis);
	XMVECTOR x2 = XMLoadFloat4(&v1);
	XMVECTOR x3 = XMLoadFloat4(&v2);
	XMVECTOR R = XMVector4Cross(x1, x2, x3);

	XMFLOAT4 result;
	XMStoreFloat4(&result, R);
	return result;
}

void CMathUtils::Normalize(const XMFLOAT4 & vThis, XMFLOAT4 & result)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR X = XMVector4Normalize(v1);
	XMStoreFloat4(&result, X);
}

void CMathUtils::Clamp(const XMFLOAT4 & vmin, const XMFLOAT4 & vmax, const XMFLOAT4 & vThis, XMFLOAT4 & result)
{
	XMVECTOR v1 = XMLoadFloat4(&vThis);
	XMVECTOR v2 = XMLoadFloat4(&vmin);
	XMVECTOR v3 = XMLoadFloat4(&vmax);
	XMVECTOR X = XMVectorClamp(v1, v2, v3);
	XMStoreFloat4(&result, X);
}

float CMathUtils::Distance(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector3Length(V);
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

void CMathUtils::Min(const XMFLOAT4 & v1, const XMFLOAT4 & v2, XMFLOAT4& result)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorMin(x1, x2);
	XMStoreFloat4(&result, X); 
}

XMFLOAT4 CMathUtils::Min(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorMin(x1, x2);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Max(const XMFLOAT4 & v1, const XMFLOAT4 & v2, XMFLOAT4 & result)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorMax(x1, x2);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::Max(const XMFLOAT4 & v1, const XMFLOAT4 & v2)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorMax(x1, x2);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Lerp(const XMFLOAT4 & v1, const XMFLOAT4 & v2, float t, XMFLOAT4 & result)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::Lerp(const XMFLOAT4 & v1, const XMFLOAT4 & v2, float t)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::SmoothStep(const XMFLOAT4 & v1, const XMFLOAT4 & v2, float t, XMFLOAT4 & result)
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t*(3.f - 2.f*t);
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::SmoothStep(const XMFLOAT4 & v1, const XMFLOAT4 & v2, float t)
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t*(3.f - 2.f*t);
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Barycentric(const XMFLOAT4 & v1, const XMFLOAT4 & v2, const XMFLOAT4 & v3, float f, float g, XMFLOAT4 & result)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR x3 = XMLoadFloat4(&v3);
	XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::Barycentric(const XMFLOAT4 & v1, const XMFLOAT4 & v2, const XMFLOAT4 & v3, float f, float g)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR x3 = XMLoadFloat4(&v3);
	XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::CatmullRom(const XMFLOAT4 & v1, const XMFLOAT4 & v2, const XMFLOAT4 & v3, const XMFLOAT4 & v4, float t, XMFLOAT4 & result)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR x3 = XMLoadFloat4(&v3);
	XMVECTOR x4 = XMLoadFloat4(&v4);
	XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::CatmullRom(const XMFLOAT4 & v1, const XMFLOAT4 & v2, const XMFLOAT4 & v3, const XMFLOAT4 & v4, float t)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&v2);
	XMVECTOR x3 = XMLoadFloat4(&v3);
	XMVECTOR x4 = XMLoadFloat4(&v4);
	XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Hermite(const XMFLOAT4 & v1, const XMFLOAT4 & t1, const XMFLOAT4 & v2, const XMFLOAT4 & t2, float t, XMFLOAT4 & result)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&t1);
	XMVECTOR x3 = XMLoadFloat4(&v2);
	XMVECTOR x4 = XMLoadFloat4(&t2);
	XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::Hermite(const XMFLOAT4 & v1, const XMFLOAT4 & t1, const XMFLOAT4 & v2, const XMFLOAT4 & t2, float t)
{
	XMVECTOR x1 = XMLoadFloat4(&v1);
	XMVECTOR x2 = XMLoadFloat4(&t1);
	XMVECTOR x3 = XMLoadFloat4(&v2);
	XMVECTOR x4 = XMLoadFloat4(&t2);
	XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Reflect(const XMFLOAT4 & ivec, const XMFLOAT4 & nvec, XMFLOAT4 & result)
{
	XMVECTOR i = XMLoadFloat4(&ivec);
	XMVECTOR n = XMLoadFloat4(&nvec);
	XMVECTOR X = XMVector4Reflect(i, n);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::Reflect(const XMFLOAT4 & ivec, const XMFLOAT4 & nvec)
{
	XMVECTOR i = XMLoadFloat4(&ivec);
	XMVECTOR n = XMLoadFloat4(&nvec);
	XMVECTOR X = XMVector4Reflect(i, n);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}

void CMathUtils::Reflect(const XMFLOAT4 & ivec, const XMFLOAT4 & nvec, float refractionIndex, XMFLOAT4 & result)
{
	XMVECTOR i = XMLoadFloat4(&ivec);
	XMVECTOR n = XMLoadFloat4(&nvec);
	XMVECTOR X = XMVector4Refract(i, n, refractionIndex);
	XMStoreFloat4(&result, X);
}

XMFLOAT4 CMathUtils::Reflect(const XMFLOAT4 & ivec, const XMFLOAT4 & nvec, float refractionIndex)
{
	XMVECTOR i = XMLoadFloat4(&ivec);
	XMVECTOR n = XMLoadFloat4(&nvec);
	XMVECTOR X = XMVector4Refract(i, n, refractionIndex);

	XMFLOAT4 result;
	XMStoreFloat4(&result, X);
	return result;
}
