#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>

class Model
{

public:
	//読み込む
	static Model* LoadFromOBJ(const std::string& modelname);


	
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// DirectX::を省
	/*
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	*/
public: // サブクラス
// 頂点データ構造体
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos; // xyz座標
		DirectX::XMFLOAT3 normal; // 法線ベクトル
		DirectX::XMFLOAT2 uv;  // uv座標
	};

	//マテリアル
	struct Material
	{
		std::string name;//マテリアル名
		DirectX::XMFLOAT3 ambient;//アンビエント影響度
		DirectX::XMFLOAT3 diffuse;//ディフューズ影響度
		DirectX::XMFLOAT3 specular;//スペキュラー影響度
		float alpha;//α
		std::string textureFilename;//テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	//マテリアル
	 Material material;

	 //定数バッファ用データ構造体B１
	 struct ConstBufferDataB1
	 {
		 DirectX::XMFLOAT3 ambient;//アンビエント係数
		 float pad1;//パディング
		 DirectX::XMFLOAT3 diffuse;//ディフューズ係数
		 float pad2;//パディング
		 DirectX::XMFLOAT3 specular;//スぺキュラー係数
		 float alpha;//α
	 };

public:


	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <returns>成否</returns>
	bool LoadTexture(const std::string& directoryPath, const std::string& filename);

	 static ID3D12Device* device;

	static void SetDevice(ID3D12Device* device) { Model::device = device; }

	//bool InitializeDescriptorHeap();

	void CreateBuffers();

	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	/// <returns></returns>
	bool InitializeDescriptorHeap();

	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);




	std::vector<unsigned short>indices;

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;


private://メンバ変数
	
	
	//ID3D12Device* device;
	
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;

	//非公開
	void LoadFromOBJInternal(const std::string& modelname);

	
	
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB1; // 定数バッファ
	
};


