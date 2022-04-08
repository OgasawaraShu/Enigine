#pragma once
//当たり判定

#include <DirectXMath.h>

//球

struct Sphere
{
	//中心座標
	DirectX::XMVECTOR center = { 0,0,0,1 };
	//半径
	float redius = 1.0f;
};

//平面

struct Plane
{
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	//原点からの九折
	float distance = 0.0f;
};

//法線付き三角形

class Triangle
{
public:
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;

	DirectX::XMVECTOR normal;

	void ComputeNormal();
};

//レイ

struct Ray
{
	//始点座標
	DirectX::XMVECTOR start = { 0,0,0,1 };

	//方向
	DirectX::XMVECTOR dir = { 1,0,0,0 };
};
