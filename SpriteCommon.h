#pragma once
#include <Windows.h>
#include <d3d12.h>
#include<wrl.h>
#include <DirectXMath.h>


#include"PipeLineSet.h"


class SpriteCommon
{
public:
	//スプライト用テクスチャ枚数
	static const int spriteSRVCount = 512;

    //テクスチャリソース(テクスチャバッファ)の配列
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[spriteSRVCount];

    //射影行列
    DirectX::XMMATRIX matProjection{};

    //テクスチャ用デスクリプタヒープの生成
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;

    ID3D12Device* device_ = nullptr;


private:
  

    //パイプラインセット
    PipeLineSet pipelineSet;

  
   
    

    

public ://メンバ関数
    void Initialize(ID3D12Device* device);
    void SpriteCommonCreate();
    void SpriteCommonLoadTexture(UINT texnumber, const wchar_t* filename, ID3D12Device* dev);
    void SpriteCommonBeginDraw(ID3D12GraphicsCommandList* cmdList);

private:
    void CreateGraphicPipeLine();

};



