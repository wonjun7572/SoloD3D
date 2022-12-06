#pragma once
#include "Engine_Define.h"

class ENGINE_DLL CMathUtils
{
private:
	CMathUtils() = default;

	// 모든 함수는 "SimpleMath.h", "SimpleMath.inl"를 참고하였습니다.

public:
#pragma region VECTOR4
	// this 벡터 요소들이 bound 벡터 안에 있으면 true를 반환 , 그렇지 않다면 false를 반환
	// 의사 코드
	// return (vThis.x <= Bounds.x && vThis.x >= -Bounds.x) &&
	//		  (vThis.y <= Bounds.y && vThis.y >= -Bounds.y) &&
	//		  (vThis.z <= Bounds.z && vThis.z >= -Bounds.z) &&
	//		  (vThis.w <= Bounds.w && vThis.w >= -Bounds.w);
	static bool  InBounds(const XMFLOAT4& vThis, const XMFLOAT4& Bounds);

	// 벡터의 길이를 계산
	static float Length(const XMFLOAT4& vThis);

	// 벡터의 길이 제곱을 계산
	static float LengthSquared(const XMFLOAT4& vThis);

	// vThis와 V를 내적하여 스칼라값을 리턴합니다.
	// w = 0 이어야함 (방향 벡터만)
	static float Dot(const XMFLOAT4& vThis, const XMFLOAT4& V);

	// v1와 v2를 외적하여 축을 만듭니다.
	// w = 0 이어야함 (방향 벡터만) 사용 자제(이것은 vector3의 외적과 다릅니다.) 자세한 내용은 MSDN 참고해주세요
	static XMFLOAT4 Cross(const XMFLOAT4& vThis, const XMFLOAT4& v1, const XMFLOAT4& v2);

	// 벡터를 정규화 시켜줍니다.
	static XMFLOAT4 Normalize(XMFLOAT4& vThis);

	// this 벡터들의 요소를 최소 최대범위 안에 고정합니다.
	//  의사 코드
	//	XMVECTOR Result;
	//	Result.x = min(max(V.x, Min.x), Max.x);
	//	Result.y = min(max(V.y, Min.y), Max.y);
	//	Result.z = min(max(V.z, Min.z), Max.z);
	//	Result.w = min(max(V.w, Min.w), Max.w);
	//	return Result;
	static XMFLOAT4 Clamp(const XMFLOAT4& vmin, const XMFLOAT4& vmax, XMFLOAT4& vThis);

	// 두 Float4의 거리 길이
	static float Distance(const XMFLOAT4& v1, const XMFLOAT4& v2);

	// 두 Float4의 거리 길이 제곱
	static float DistanceSquared(const XMFLOAT4& v1, const XMFLOAT4& v2);

	// 두 벡터의 최솟값을 리턴합니다.
	static XMFLOAT4 Min(const XMFLOAT4& v1, const XMFLOAT4& v2);

	// 두 벡터의 최대값을 리턴합니다.
	static XMFLOAT4 Max(const XMFLOAT4& v1, const XMFLOAT4& v2);

	// 선형 보간 (v1 ~ v2) (t를 통해 위치가 결정됨) (w = 1)
	// 끝점의 값이 주어졌을 때 그 사이에 위치한 값을 추정하기 위하여 직선 거리에 따라 선형적으로 계산하는 방법
	// 위치 벡터가 v1보다 작다면 0을 리턴하고, v2보다 크다면 1을 리턴한다.
	static XMFLOAT4	Lerp(const XMFLOAT4& v1, const XMFLOAT4& v2, float t);
	static XMFLOAT4 QuaternionSlerp(const XMFLOAT4& v1, const XMFLOAT4& v2, float t);

	// Hermite 보간법 (t를 통해 위치가 결정됨) (w = 1)
	// 위치 벡터가 [min, max] 사이의 값인 경우에 대해서 [0, 1] 사이에서 부드럽게 변하는 
	// Hermite 보간법을 리턴한다. 위치 벡터가 v1보다 작다면 0을 리턴하고, v2보다 크다면 1을 리턴한다.
	static XMFLOAT4	SmoothStep(const XMFLOAT4& v1, const XMFLOAT4& v2, float t);

	// 무게 중심 좌표 구할 때
	// Domain Shader, Geometry Shader에서 활용 할 것 같다.
	// ex) 현재 기본 도형의 정점들 중 기본 도형을 더 작은 조각으로 분할하기 위한 표본으로 사용할 정점들을 결정합니다.
	// 그 정점들로 부터 산출한 Barycentric coordinates(무게중심 좌표)들을 다음 단계인 DomainShader에게 넘깁니다.
	// v1, v2, v3 의 위치 벡터를 넣어서 삼각형이 만들어지고 XMVectorBaryCentric이 함수를 통해 가중 계수들을 통해 삼각형의 중심 좌표가 리턴됩니다.
	static XMFLOAT4 Barycentric(const XMFLOAT4& v1, const XMFLOAT4& v2, const XMFLOAT4& v3, float f, float g);

	// Catmull-Rom 보간법
	// 위치 벡터를 4개 받아오고 이 4개의 좌표에 대해 보간을 진행함
	// v2->v3 보간을 진행함
	// https://blasin.tistory.com/9 도약공격같은 것 할 때
	static XMFLOAT4 CatmullRom(const XMFLOAT4& v1, const XMFLOAT4& v2, const XMFLOAT4& v3, const XMFLOAT4& v4, float t);

	// Hermite 보간법 
	// v1 와 v3는 위치 벡터
	// v2 는 v1의 접선 벡터
	// v4 는 v3의 접선 벡터
	// v1 -> v3 보간을 진행함
	static XMFLOAT4	Hermite(const XMFLOAT4& v1, const XMFLOAT4& t1, const XMFLOAT4& v2, const XMFLOAT4& t2, float t);

	// 밑에 부분 코드들도 쉐이더 수업을 더욱 진행한 다음에 사용해야함.
	// Refracts an incident 4D vector across a 4D normal vector.
	// ivec은 굴절할 입사 벡터이고 nvec은 ivec을 굴절시키는 법선 벡터입니다.
	static XMFLOAT4 Reflect(const XMFLOAT4& ivec, const XMFLOAT4& nvec);

	// refractionIndex를 통해 굴절률을 조절할 수 있습니다.
	//	https://copynull.tistory.com/277
	static XMFLOAT4 Reflect(const XMFLOAT4& ivec, const XMFLOAT4& nvec, float refractionIndex);

	// 벡터와 행렬의 곱셈
	// float4 * float4x4 (TransformCoord) -> result (XMFLOAT4)
	static XMFLOAT4	Transform(const XMFLOAT4& v, const XMFLOAT4X4& m);
	// 주어진 행렬로 4D 벡터의 스트림을 변환합니다.
	static void		Transform(const XMFLOAT4* varray, size_t count, const XMFLOAT4X4& m, OUT XMFLOAT4* resultArray);
#pragma endregion VECTOR4

	// 이하 VECTOR4 와 VECTOR3의 개념 자체는 같지만 사용 편의성을 위하여 오버로드 작업

#pragma region VECTOR3
	static bool		InBounds(const XMFLOAT3& vThis, const XMFLOAT3& Bounds);
	static float	Length(const XMFLOAT3& vThis);
	static float	LengthSquared(const XMFLOAT3& vThis);
	static float	Dot(const XMFLOAT3& vThis, const XMFLOAT3& V);
	static XMFLOAT3 Cross(const XMFLOAT3& vThis, const XMFLOAT3& V);
	static XMFLOAT3 Normalize(XMFLOAT3& vThis);
	static XMFLOAT3 Clamp(const XMFLOAT3& vmin, const XMFLOAT3& vmax, XMFLOAT3& vThis);
	static float	Distance(const XMFLOAT3& v1, const XMFLOAT3& v2);
	static float	DistanceSquared(const XMFLOAT3& v1, const XMFLOAT3& v2);
	static XMFLOAT3 Min(const XMFLOAT3& v1, const XMFLOAT3& v2);
	static XMFLOAT3 Max(const XMFLOAT3& v1, const XMFLOAT3& v2);
	static XMFLOAT3	Lerp(const XMFLOAT3& v1, const XMFLOAT3& v2, float t);
	static XMFLOAT3	SmoothStep(const XMFLOAT3& v1, const XMFLOAT3& v2, float t);
	static XMFLOAT3 Barycentric(const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3, float f, float g);
	static XMFLOAT3 CatmullRom(const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3, const XMFLOAT3& v4, float t);
	static XMFLOAT3	Hermite(const XMFLOAT3& v1, const XMFLOAT3& t1, const XMFLOAT3& v2, const XMFLOAT3& t2, float t);
	static XMFLOAT3 Reflect(const XMFLOAT3& ivec, const XMFLOAT3& nvec);
	static XMFLOAT3 Reflect(const XMFLOAT3& ivec, const XMFLOAT3& nvec, float refractionIndex);

	// 벡터와 행렬의 곱셈

	// float3 * float4x4 (TransformCoord) -> return
	static XMFLOAT3 Transform(const XMFLOAT3& v, const XMFLOAT4X4& m);

	// 주어진 행렬로 3D 벡터의 스트림을 변환하고 w 좌표가 1.0이 되도록 결과 XMFLOAT3를 뱉어줍니다. (coord)(사용할 일이?)
	static void		Transform(const XMFLOAT3* varray, size_t count, const XMFLOAT4X4& m, OUT XMFLOAT3* resultArray);

	// float3 * float4x4 (TransformCoord) -> result (XMFLOAT4)
	static void		Transform(const XMFLOAT3& v, const XMFLOAT4X4& m, OUT XMFLOAT4& result);

	// 주어진 행렬로 3D 벡터의 스트림을 변환하고 w 좌표가 1.0이 되도록 결과 XMFLOAT4를 뱉어줍니다. (coord)(사용할 일이?)
	static void		Transform(const XMFLOAT3* varray, size_t count, const XMFLOAT4X4& m, OUT XMFLOAT4* resultArray);

	//XMVector3TransformNormal 는 회전 및 크기 조정을 위해 입력 행렬 행 0, 1 및 2를 사용하여 변환을 수행하고 행 3을 무시합니다.
	// float3 * float4x4(XMVector3TransformNormal) -> return
	static XMFLOAT3 TransformNormal(const XMFLOAT3& v, const XMFLOAT4X4& m);

	//  주어진 행렬로 3D 벡터의 스트림을 변환하고 w 좌표가 1.0이 되도록 결과 XMFLOAT3를 뱉어줍니다. (normal)(사용할 일이?)
	static void		TransformNormal(const XMFLOAT3* varray, size_t count, const XMFLOAT4X4& m, OUT XMFLOAT3* resultArray);

#pragma endregion VECTOR3


#pragma region MATRIX
	// 두 행렬를 더해줍니다. 벡터 대 벡터
	static XMFLOAT4X4 Add_Matrix(const XMFLOAT4X4& M1, const XMFLOAT4X4& M2);
	// 두 행렬를 빼줍니다.  벡터 대 벡터
	static XMFLOAT4X4 Sub_Matrix(const XMFLOAT4X4& M1, const XMFLOAT4X4& M2);
	// 두 행렬을 곱합니다.
	static XMFLOAT4X4 Mul_Matrix(const XMFLOAT4X4& M1, const XMFLOAT4X4& M2);
	// 두 행렬을 나눕니다.  벡터 대 벡터
	static XMFLOAT4X4 Div_Matrix(const XMFLOAT4X4& M1, const XMFLOAT4X4& M2);

	// 행렬과 float을 곱해줍니다.
	static XMFLOAT4X4 Mul_MatrixFloat(const XMFLOAT4X4& M, float S);
	// 행렬과 float를 나누어줍니다.
	static XMFLOAT4X4 Div_MatrixFloat(const XMFLOAT4X4& M, float S);

	// float 과 행렬을 곱합니다.
	static XMFLOAT4X4 Mul_FloatMatrix(float S, const XMFLOAT4X4& M);

	// 자신의 행렬을 넣으면 전치행렬을 리턴합니다.
	static XMFLOAT4X4 Transpose(const XMFLOAT4X4& mThis);

	// 자신의 행렬을 넣으면 역행렬을 리턴합니다.
	static XMFLOAT4X4 Invert(const XMFLOAT4X4& mThis);

	// 그 행렬식 있지요 그거요! 
	// 2x2 에서는 ad - bc 그런거
	static float	  Determinant(const XMFLOAT4X4& mThis);

	// 두 행렬이 일치하는가
	static bool		  Equal_Matrix(const XMFLOAT4X4& M1, const XMFLOAT4X4& M2);

	// 이동행렬를 만들어줍니다.
	static XMFLOAT4X4 CreateTranslation(const XMFLOAT3& vPosition);
	static XMFLOAT4X4 CreateTranslation(float x, float y, float z);

	// 크기행렬를 만들어줍니다.
	static XMFLOAT4X4 CreateScale(const XMFLOAT3& vScale);
	static XMFLOAT4X4 CreateScale(float xs, float ys, float zs);
	static XMFLOAT4X4 CreateScale(float scale);

	// X 축에 대한 회전행렬을 만들어줍니다.
	static XMFLOAT4X4 CreateRotationX(float radians);
	// Y 축에 대한 회전행렬을 만들어줍니다.
	static XMFLOAT4X4 CreateRotationY(float radians);
	// Z 축에 대한 회전행렬을 만들어줍니다.
	static XMFLOAT4X4 CreateRotationZ(float radians);

	// 객체의 위치를 받아주고 정방향을 넣어주고 업벡터를 넣어주면 월드행렬을 만들어줍니다.
	// 일반적으로 업벡터는 [0,1,0], 하지만 기울어진 상황에서는 이야기가 달라집니다.
	static XMFLOAT4X4 CreateWorld(const XMFLOAT3& position, const XMFLOAT3& forward, const XMFLOAT3& up);

	// 벡터 4개 행 모두 (M1 ~ M2) 다 보간을 진행합니다.
	static XMFLOAT4X4 Lerp(const XMFLOAT4X4& M1, const XMFLOAT4X4& M2, float t);
	static XMFLOAT4X4 MatrixAffineTransformation(const XMFLOAT4& vScale, const XMFLOAT4& vRotationOrigin, const XMFLOAT4& vRotation, const XMFLOAT4& vPosition);
#pragma endregion

#pragma region 추가적인 함수들
	// XMFLOAT3 끼리의 덧셈
	static XMFLOAT3 Add_Float3(const XMFLOAT3& v1, const XMFLOAT3& v2);
	// XMFLOAT3 끼리의 뺄셈
	static XMFLOAT3 Sub_Float3(const XMFLOAT3& v1, const XMFLOAT3& v2);

	static XMFLOAT4 MulNum_Float4(const float& f1, const XMFLOAT4& v1);
	static XMFLOAT3 MulNum_Float3(const float& f1, const XMFLOAT3& v1);

#pragma endregion

};