#pragma once
#include "Engine_Define.h"

class ENGINE_DLL CGameMath
{
private:
	CGameMath() = default;
	
public:
	static bool  InBounds(const XMFLOAT4& vThis, const XMFLOAT4& Bounds);

	static float Length(const XMFLOAT4& vThis);

	static float LengthSquared(const XMFLOAT4& vThis);

	static float Dot(const XMFLOAT4& vThis, const XMFLOAT4& V);

	static void		Cross(const XMFLOAT4& vThis, const XMFLOAT4& v1, const XMFLOAT4& v2, XMFLOAT4& result);
	static XMFLOAT4 Cross(const XMFLOAT4& vThis, const XMFLOAT4& v1, const XMFLOAT4& v2);

	static void	Normalize(const XMFLOAT4& vThis, XMFLOAT4& result);

	static void Clamp(const XMFLOAT4& vmin, const XMFLOAT4& vmax, const XMFLOAT4& vThis, XMFLOAT4& result);

	// �� Float4�� �Ÿ� ����
	static float Distance(const XMFLOAT4& v1, const XMFLOAT4& v2);
	// �� Float4�� �Ÿ� ���� ����
	static float DistanceSquared(const XMFLOAT4& v1, const XMFLOAT4& v2);
	
	static void		Min(const XMFLOAT4& v1, const XMFLOAT4& v2, XMFLOAT4& result);
	static XMFLOAT4 Min(const XMFLOAT4& v1, const XMFLOAT4& v2);

	static void		Max(const XMFLOAT4& v1, const XMFLOAT4& v2, XMFLOAT4& result);
	static XMFLOAT4 Max(const XMFLOAT4& v1, const XMFLOAT4& v2);

	// Linear ������
	static void		Lerp(const XMFLOAT4& v1, const XMFLOAT4& v2, float t , XMFLOAT4& result);
	static XMFLOAT4	Lerp(const XMFLOAT4& v1, const XMFLOAT4& v2, float t);

	//  Hermite ������
	static void		SmoothStep(const XMFLOAT4& v1, const XMFLOAT4& v2, float t, XMFLOAT4& result);
	static XMFLOAT4	SmoothStep(const XMFLOAT4& v1, const XMFLOAT4& v2, float t);

	// ���� �߽� ��ǥ ���� ��
	static void		Barycentric(const XMFLOAT4& v1, const XMFLOAT4& v2, const XMFLOAT4& v3, float f, float g, XMFLOAT4& result);
	static XMFLOAT4 Barycentric(const XMFLOAT4& v1, const XMFLOAT4& v2, const XMFLOAT4& v3, float f, float g);

	// � Catmull-Rom ������
	static void		CatmullRom(const XMFLOAT4& v1, const XMFLOAT4& v2, const XMFLOAT4& v3, const XMFLOAT4& v4, float t, XMFLOAT4& result);
	static XMFLOAT4 CatmullRom(const XMFLOAT4& v1, const XMFLOAT4& v2, const XMFLOAT4& v3, const XMFLOAT4& v4, float t);

	// � ����� �� Hermite ������ 
	static void		Hermite(const XMFLOAT4& v1, const XMFLOAT4& t1, const XMFLOAT4& v2, const XMFLOAT4& t2, float t, XMFLOAT4& result);
	static XMFLOAT4	Hermite(const XMFLOAT4& v1, const XMFLOAT4& t1, const XMFLOAT4& v2, const XMFLOAT4& t2, float t);

	// Refracts an incident 4D vector across a 4D normal vector.
	static void		Reflect(const XMFLOAT4& ivec, const XMFLOAT4& nvec, XMFLOAT4& result);
	static XMFLOAT4 Reflect(const XMFLOAT4& ivec, const XMFLOAT4& nvec);
	static void		Reflect(const XMFLOAT4& ivec, const XMFLOAT4& nvec, float refractionIndex, XMFLOAT4& result);
	static XMFLOAT4 Reflect(const XMFLOAT4& ivec, const XMFLOAT4& nvec, float refractionIndex);
};

