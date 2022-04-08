//#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <cstdlib>      // srand,rand
#include <time.h>
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//#define DIRECTINPUT_VERSION 0x0800
//#include<dinput.h>
#include<DirectXTex.h>
//#pragma comment(lib,"dinput8.lib")
//#pragma comment(lib,"dxguid.lib")
#include <wrl.h>

using namespace Microsoft::WRL;

#include<d3dx12.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"

#include "3d/Object3d.h"
#include"Model.h"


#include "Collision.h"
#include "CollisionPrimitive.h"


#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<fstream>

#include "WinApp.h"
#include "Audio.h"

#include "SpriteCommon.h"
#include "Sprite.h"

Sphere sphere;

Plane plane;

Triangle triangle;

Ray ray;

DirectXCommon* dxCommon = nullptr;

//ComPtr<ID3D12Device> dev;

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


#ifdef _DEBUG
#   define MyOutputDebugString( str, ... ) \
      { \
        TCHAR c[256]; \
        _stprintf( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#    define MyOutputDebugString( str, ... ) // 空実装
#endif


//定数バッファデータ構造体
struct ConstBufferData8 {
    XMFLOAT4 color8;//色
    XMMATRIX mat8;//3D変換行列
};

struct VertexPosUv
{
    XMFLOAT3 pos;//x y z　座標
    XMFLOAT2 uv;//u v 座標
};



//スプライト用テクスチャ枚数

//

/*
const int spriteSRVCount = 512;

struct PipeLineSet
{
    ComPtr<ID3D12PipelineState>pipelinestate;

    ComPtr<ID3D12RootSignature>rootsignature;
};

PipeLineSet object3dPipelineSet(ID3D12Device* dev);


struct PipelineSet//パイプラインセット
{
    ComPtr<ID3D12PipelineState> pipelinestate;//パイプラインステートオブジェクト
    ComPtr<ID3D12RootSignature> rootsignature;//ルートシグネチャ
};
*/
/*
#pragma region 3dpipeline
PipelineSet Create3dpipe(ID3D12Device* dev)
{
    HRESULT result = S_OK;
    ComPtr<ID3DBlob> vsBlob;// ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
    ComPtr<ID3DBlob> psBlob;//ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
    ComPtr<ID3DBlob> errorBlob;//ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

    // 頂点シェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/BasicVS.hlsl",  // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &vsBlob, &errorBlob);
    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // ピクセルシェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/BasicPS.hlsl",   // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &psBlob, &errorBlob);
    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }


    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }, // (1行で書いたほうが見やすい)

        {
            "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }, // (1行で書いたほうが見やすい)
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }, // (1行で書いたほうが見やすい)
    };
    CD3DX12_DESCRIPTOR_RANGE descRangeCBV, descRangeSRV;
    //descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//b0 レジスタ
    descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0 レジスタ


    CD3DX12_ROOT_PARAMETER rootparams[2];
    rootparams[0].InitAsConstantBufferView(0);//定数バッファビューとして初期化(b0　レジスタ)
    rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

    //頂点シェーダ　ピクセルシェーダ
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    //標準的な設定(背面をカリング ポリゴン内塗りつぶし 深度クリッピングを有効に)
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    //ブレンド
//gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画
    D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
    blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    //共通設定(BLEND)
    blenddesc.BlendEnable = true;					//ブレンドを有効にする
    blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
    blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
    blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を0%使う

    //加算合成
    //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	 //加算
    //blenddesc.SrcBlend = D3D12_BLEND_ONE;	 //ソースの値を100%使う
    //blenddesc.DestBlend = D3D12_BLEND_ONE;	 //デストの値を100%使う

    //半透明合成
    blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
    blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

    gpipeline.InputLayout.pInputElementDescs = inputLayout;
    gpipeline.InputLayout.NumElements = _countof(inputLayout);
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    gpipeline.NumRenderTargets = 1; // 描画対象は1つ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
    gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

    //デプスステンシルステートの設定

    //標準的な設定(深度テストを行う 書き込み許可 小さければ合格)
    gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

    //デスクリプタテーブルの設定
    D3D12_DESCRIPTOR_RANGE descTblRange{};
    descTblRange.NumDescriptors = 1;
    descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    descTblRange.BaseShaderRegister = 0;
    descTblRange.OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //ルートパラメータ
    D3D12_ROOT_PARAMETER rootparam = {};
    rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
    rootparam.DescriptorTable.NumDescriptorRanges = 1;
    rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;



    //ルートシグネチャ生成
    ComPtr<ID3D12RootSignature> rootsignature;



    //ルートシグネチャの設定
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

    rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    PipelineSet pipelineSet;

    ComPtr<ID3DBlob> rootSigBlob;

    result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
        &rootSigBlob, &errorBlob);

    result = dxCommon->GetDev()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
        IID_PPV_ARGS(&pipelineSet.rootsignature));

    gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

    ComPtr<ID3D12PipelineState> pipelinestate;// ID3D12PipelineState* pipelinestate = nullptr;
    result = dxCommon->GetDev()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

    return pipelineSet;
}
#pragma endregion
*/

#pragma region 2dpipeline

/*
PipelineSet CreateSprite2dpipe(ID3D12Device* dev)
{
    HRESULT result = S_OK;
    ComPtr<ID3DBlob> vsBlob;// ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
    ComPtr<ID3DBlob> psBlob;//ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
    ComPtr<ID3DBlob> errorBlob;//ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

    // 頂点シェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/SpriteVS.hlsl",  // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &vsBlob, &errorBlob);
    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // ピクセルシェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/SpritePS.hlsl",   // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &psBlob, &errorBlob);
    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }


    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }, // (1行で書いたほうが見やすい)


        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }, // (1行で書いたほうが見やすい)
    };
    CD3DX12_DESCRIPTOR_RANGE  descRangeSRV;
    //descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//b0 レジスタ
    descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0 レジスタ


    CD3DX12_ROOT_PARAMETER rootparams[2];
    rootparams[0].InitAsConstantBufferView(0);//定数バッファビューとして初期化(b0　レジスタ)
    rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

    //頂点シェーダ　ピクセルシェーダ
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());


    //ブレンド
//gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画
    D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
    blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    //共通設定(BLEND)
    blenddesc.BlendEnable = true;					//ブレンドを有効にする
    blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
    blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
    blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を0%使う

    //加算合成
    //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	 //加算
    //blenddesc.SrcBlend = D3D12_BLEND_ONE;	 //ソースの値を100%使う
    //blenddesc.DestBlend = D3D12_BLEND_ONE;	 //デストの値を100%使う

    //半透明合成
    blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
    blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

    gpipeline.InputLayout.pInputElementDescs = inputLayout;
    gpipeline.InputLayout.NumElements = _countof(inputLayout);
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    gpipeline.NumRenderTargets = 1; // 描画対象は1つ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
    gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        //標準的な設定(ポリゴン内塗りつぶし 深度クリッピングを有効に)
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;  // 背面をカリングなし

    //デプスステンシルステートの設定

    //標準的な設定(書き込み許可)
    gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//一旦標準値をセット
    gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書き
    gpipeline.DepthStencilState.DepthEnable = false;//深度テストしない

    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

    //デスクリプタテーブルの設定
    D3D12_DESCRIPTOR_RANGE descTblRange{};
    descTblRange.NumDescriptors = 1;
    descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    descTblRange.BaseShaderRegister = 0;
    descTblRange.OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //ルートパラメータ
    D3D12_ROOT_PARAMETER rootparam = {};
    rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
    rootparam.DescriptorTable.NumDescriptorRanges = 1;
    rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;



    //ルートシグネチャ生成
    ComPtr<ID3D12RootSignature> rootsignature;



    //ルートシグネチャの設定
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

    rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    PipelineSet pipelineSet;

    ComPtr<ID3DBlob> rootSigBlob;

    result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
        &rootSigBlob, &errorBlob);

    result = dxCommon->GetDev()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
        IID_PPV_ARGS(&pipelineSet.rootsignature));

    gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

    ComPtr<ID3D12PipelineState> pipelinestate;// ID3D12PipelineState* pipelinestate = nullptr;
    result = dxCommon->GetDev()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

    return pipelineSet;
}
#pragma endregion 
*/
//


/*
struct Sprite
{
    //頂点バッファ
    ComPtr<ID3D12Resource> vertBuff;

    //頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView;

    //定数バッファ
    ComPtr<ID3D12Resource> constBuff;

    //XYZ軸周りの回転軸
    XMFLOAT3 rotation = { 0.0f,0.0f, 0.0f };

    //座標
    XMFLOAT3 position = { 0,0,0 };

    //ワールド行列
    XMMATRIX matWorld;

    //色
    XMFLOAT4 color = { 1,1,1,1 };

    //テクスチャ番号
    UINT texnumber = 0;

    //大きさ
    XMFLOAT2 size = { 100, 100 };

    //アンカーポイント
    XMFLOAT2 anchorpoint = { 0.5f,0.5f };

    //左右反転
    bool isFlagX = false;

    //上下反転
    bool isFlagY = false;

    //テクスチャ左上座標
    XMFLOAT2 texLeftTop = { 0,0 };

    //テクスチャ切り出しサイズ
    XMFLOAT2 texSize = { 100,100 };

    //非表示
    bool isInvisible = false;
};

//
struct SpriteCommon
{
    //パイプラインセット
    PipelineSet pipelineSet;

    //射影行列
    XMMATRIX matProjection{};

    //テクスチャ用デスクリプタヒープの生成
    ComPtr<ID3D12DescriptorHeap> descHeap;

    //テクスチャリソース(テクスチャバッファ)の配列
    ComPtr<ID3D12Resource> texBuff[spriteSRVCount];
};


//
void SpriteTransVertexBuffer(const Sprite& sprite, const SpriteCommon& spriteCommon)
{
    HRESULT result = S_FALSE;

    VertexPosUv vertices[] = {
        {{},{} },
        {{},{} },
        {{},{} },
        {{},{} },

    };

    //	 左下 左上 右下 右上
    enum { LB, LT, RB, RT };

    float left = (0.0f - sprite.anchorpoint.x) * sprite.size.x;

    float right = (1.0f - sprite.anchorpoint.x) * sprite.size.x;

    float top = (0.0f - sprite.anchorpoint.y) * sprite.size.y;

    float bottom = (1.0f - sprite.anchorpoint.y) * sprite.size.y;

    if (sprite.isFlagX == true)
    {
        left = -left;
        right = -right;
    }

    if (sprite.isFlagY == true)
    {
        top = -top;
        bottom = -bottom;
    }

    vertices[LB].pos = { left,	bottom, 0.0f };
    vertices[LT].pos = { left,	top,	0.0f };
    vertices[RB].pos = { right,	bottom, 0.0f };
    vertices[RT].pos = { right,	top,	0.0f };

    //UV
    //指定番号の画像が読み込み済みなら
    if (spriteCommon.texBuff[sprite.texnumber])
    {
        //テクスチャ情報取得
        D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[sprite.texnumber]->GetDesc();

        float tex_left = sprite.texLeftTop.x / resDesc.Width;

        float tex_right = (sprite.texLeftTop.x + sprite.texSize.x) / resDesc.Width;

        float tex_top = sprite.texLeftTop.y / resDesc.Height;

        float tex_bottom = (sprite.texLeftTop.y + sprite.texSize.y) / resDesc.Height;

        vertices[LB].uv = { tex_left,	tex_bottom };
        vertices[LT].uv = { tex_left,	tex_top };
        vertices[RB].uv = { tex_right,	tex_bottom };
        vertices[RT].uv = { tex_right,	tex_top };

    }

    //頂点バッファへのデータ転送
    VertexPosUv* vertMap = nullptr;
    result = sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
    memcpy(vertMap, vertices, sizeof(vertices));
    sprite.vertBuff->Unmap(0, nullptr);
}


//
Sprite SpriteCreate(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const SpriteCommon& spriteCommon,
    XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlagX = false, bool isFlagY = false)
{
    HRESULT result = S_FALSE;

    Sprite sprite{};

    //アンカーポイントをコピー
    sprite.anchorpoint = anchorpoint;

    //反転フラグをコピー
    sprite.isFlagX = isFlagX;

    sprite.isFlagY = isFlagY;

    VertexPosUv vertices[] = {
        {{0.0f	,100.0f	,0.0f},{0.0f,1.0f} },
        {{0.0f	,0.0f	,0.0f},{0.0f,0.0f} },
        {{100.0f,100.0f	,0.0f},{1.0f,1.0f} },
        {{100.0f,0.0f	,0.0f},{1.0f,0.0f} },


    };




    //頂点バッファの生成
    result = dev->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&sprite.vertBuff)
    );

    sprite.texnumber = texnumber;

    if (spriteCommon.texBuff[sprite.texnumber])
    {
        //テクスチャ情報の画像が読み込み済みなら
        D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[sprite.texnumber]->GetDesc();

        //スプライトの大きさを画像の解像度に合わせる
        sprite.size = { (float)resDesc.Width,(float)resDesc.Height };
    }

    //頂点バッファデータ転送
    SpriteTransVertexBuffer(sprite, spriteCommon);




    //頂点バッファビューへのデータ転送
    VertexPosUv* vertMap = nullptr;
    result = sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
    memcpy(vertMap, vertices, sizeof(vertices));
    sprite.vertBuff->Unmap(0, nullptr);

    //頂点バッファビューの生成
    sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
    sprite.vbView.SizeInBytes = sizeof(vertices);
    sprite.vbView.StrideInBytes = sizeof(vertices[0]);

    //定数バッファの生成
    result = dxCommon->GetDev()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData8) + 0xff) & ~0xff),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&sprite.constBuff));

    //定数バッファにデータを転送
    ConstBufferData8* constMap = nullptr;
    result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
    constMap->color8 = XMFLOAT4(1, 1, 1, 1);//色指定(R G B A)

    //平行投影法
    constMap->mat8 = XMMatrixOrthographicOffCenterLH(
        0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);
    sprite.constBuff->Unmap(0, nullptr);

    return sprite;
}


//
SpriteCommon SpriteCommonCreate(ID3D12Device* dev, int window_width, int window_height)
{
    HRESULT result = S_FALSE;




    //新たなスプライト共通データを生成
    SpriteCommon spriteCommon{};

    //設定構造
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見える
    descHeapDesc.NumDescriptors = spriteSRVCount;//テクスチャ枚数

    //デスクリプタヒープの生成
    result = dxCommon->GetDev()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteCommon.descHeap));//生成

    //スプライト用パイプライン生成
    spriteCommon.pipelineSet = CreateSprite2dpipe(dxCommon->GetDev());

    //並行投影の射影行列変換
    spriteCommon.matProjection = XMMatrixOrthographicOffCenterLH(
        0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f);


    return spriteCommon;
}

//
void SpriteCommonLoadTexture(SpriteCommon& spritecommon, UINT texnumber, const wchar_t* filename, ID3D12Device* dev)
{

    assert(texnumber <= spriteSRVCount - 1);

    HRESULT result;

    TexMetadata metadata{};
    ScratchImage scratchImg{};


    result = LoadFromWICFile(
        filename,
        WIC_FLAGS_NONE,
        &metadata, scratchImg);

    const Image* img = scratchImg.GetImage(0, 0, 0);

    const int texWidth = 256;//横方向ピクセル
    const int imageDataCount = texWidth * texWidth;//配列の要素数


    CD3DX12_HEAP_PROPERTIES texheapProp{};//テクスチャヒープ
    texheapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    texheapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texheapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;


    CD3DX12_RESOURCE_DESC texresDesc{};
    texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
    texresDesc.Format = metadata.format;
    texresDesc.Width = metadata.width;//幅
    texresDesc.Height = (UINT)metadata.height;//高さ
    texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
    texresDesc.MipLevels = (UINT16)metadata.mipLevels;
    texresDesc.SampleDesc.Count = 1;


    result = dxCommon->GetDev()->CreateCommittedResource(//GPUリソースの生成
        &texheapProp,
        D3D12_HEAP_FLAG_NONE,
        &texresDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
        nullptr,
        IID_PPV_ARGS(&spritecommon.texBuff[texnumber]));

    //リソース設定
    texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        metadata.format,
        metadata.width,
        (UINT)metadata.height,
        (UINT16)metadata.arraySize,
        (UINT16)metadata.mipLevels);

    result = dxCommon->GetDev()->CreateCommittedResource(//GPUリソースの生成
        &CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
        D3D12_HEAP_FLAG_NONE,
        &texresDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
        nullptr,
        IID_PPV_ARGS(&spritecommon.texBuff[texnumber]));


    //テクスチャバッファにデータ転送
    result = spritecommon.texBuff[texnumber]->WriteToSubresource(
        0,
        nullptr,							//全領域にコピー
        img->pixels,						//元データアドレス
        (UINT)img->rowPitch,				//1ラインサイズ
        (UINT)img->slicePitch);			//全サイズ


    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(spritecommon.descHeap->GetCPUDescriptorHandleForHeapStart(),
            texnumber,
            dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
        CD3DX12_GPU_DESCRIPTOR_HANDLE(spritecommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
            texnumber,
            dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

    //シェーダーリソースビュー設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
    //srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    //ヒープの?番目にシェーダーリソースビューを生成
    dxCommon->GetDev()->CreateShaderResourceView(spritecommon.texBuff[texnumber].Get(),//ビューと関連付けるバッファ
        &srvDesc,//テクスチャ設定情報
        CD3DX12_CPU_DESCRIPTOR_HANDLE(spritecommon.descHeap->GetCPUDescriptorHandleForHeapStart(), texnumber,
            dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

}


//
void SpriteUpdate(Sprite& sprite, const SpriteCommon& spriteCommon)
{
    //ワールド行列の更新
    sprite.matWorld = XMMatrixIdentity();

    //Z軸回転
    sprite.matWorld *= XMMatrixRotationZ(XMConvertToRadians(sprite.rotation.z));

    //平行移動
    sprite.matWorld *= XMMatrixTranslation(sprite.position.x, sprite.position.y, sprite.position.z);

    //定数バッファの転送
    ConstBufferData8* constMap = nullptr;
    HRESULT result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
    constMap->color8 = sprite.color;
    constMap->mat8 = sprite.matWorld * spriteCommon.matProjection;



    sprite.constBuff->Unmap(0, nullptr);
}


//
void SpriteCommonBeginDraw(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon)
{
    //パイプラインステートの設定
    cmdList->SetPipelineState(spriteCommon.pipelineSet.pipelinestate.Get());

    //ルートシグネチャの設定
    cmdList->SetGraphicsRootSignature(spriteCommon.pipelineSet.rootsignature.Get());

    //プリミティブ形状を設定
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //テクスチャ用デスクリプタヒープ
    ID3D12DescriptorHeap* ppHeaps[] = { spriteCommon.descHeap.Get() };
    cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}


//
void  SpriteDraw(const Sprite& sprite, ID3D12GraphicsCommandList* cmdList, SpriteCommon& spriteCommon, ID3D12Device* dev)
{

    if (sprite.isInvisible == true)
    {
        return;
    }
    //頂点バッファをセット
    cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);

    //定数バッファをセット
    cmdList->SetGraphicsRootConstantBufferView(0, sprite.constBuff->GetGPUVirtualAddress());

    //
    cmdList->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(spriteCommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
            sprite.texnumber,
            dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

    //ポリゴンの描画
    cmdList->DrawInstanced(4, 1, 0, 0);
}
*/




//デバックテキスト
/*
class DebugText
{
public:
    static const int maxCharCount = 256;

    static const int fontWidth = 80;

    static const int fontHeight = 85;

    static const int fontLineCount = 16;

    Sprite sprites[maxCharCount];

    int spriteIndex = 0;

    void debugTextInit(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const SpriteCommon& spriteCommon);

    void Print(const SpriteCommon& spritecommon, const std::string& text, float x, float y, float scale = 1.0f);

    void DrawAll(ID3D12GraphicsCommandList* cmdList, SpriteCommon& spriteCommon, ID3D12Device* dev);
};

void DebugText::debugTextInit(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const SpriteCommon& spritecommon)
{
    for (int i = 0; i < _countof(sprites); i++)
    {
        sprites[i] = Sprite::Sprite::SpriteCreate(dev, window_width, window_height, texnumber, spritecommon, { 0,0 });
    }
}

void DebugText::Print(const SpriteCommon& spritecommon, const std::string& text, float x, float y, float scale)
{
    for (int i = 0; i < text.size(); i++)
    {
        if (spriteIndex >= maxCharCount)
        {
            break;
        }

        const unsigned char& charctor = text[i];

        int fontIndex = charctor - 32;

        if (charctor >= 0x7f)
        {
            fontIndex = 0;
        }

        int fontIndexX = fontIndex % fontLineCount;

        int fontIndexY = fontIndex / fontLineCount;

        sprites[spriteIndex].position = { x + fontWidth * scale * i,y,0 };
        sprites[spriteIndex].texLeftTop = { (float)fontIndexX * fontWidth,(float)fontIndexY * fontHeight };
        sprites[spriteIndex].texSize = { fontWidth,fontHeight };
        sprites[spriteIndex].size = { fontWidth * scale,fontHeight * scale };

        SpriteTransVertexBuffer(sprites[spriteIndex], spritecommon);

        SpriteUpdate(sprites[spriteIndex], spritecommon);

        spriteIndex++;


    }
}

void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList, SpriteCommon& spriteCommon, ID3D12Device* dev)
{
    for (int i = 0; i < spriteIndex; i++)
    {
        SpriteDraw(sprites[i], cmdList, spriteCommon, dev);
    }

    spriteIndex = 0;
}
*/


//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    //ポインタ置き場
    Input* input = nullptr;
    WinApp* winApp = nullptr;
    Audio* audio = nullptr;
    //DirectXCommon* dxCommon = nullptr;

    // コンソールへの文字出力
    OutputDebugStringA("Hello,DirectX!!\n");

   
  //  MSG msg{};  // メッセージ
    
    //WindowsAPIの初期化
    winApp = new WinApp();
    winApp->Initialize();
    


// DirectX初期化処理　ここから
 
    //DirectXの初期化
    dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);
    HRESULT result;
  
    //入力の初期化
    input = new Input();
    input->Intialize(winApp);
   
    //3D初期化
    Object3d::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height);
    //読み込み
    Model* model = Model::LoadFromOBJ("Mato");
    Model* model_2 = Model::LoadFromOBJ("Player");
    Model* model_3 = Model::LoadFromOBJ("Shou");
    Model* model_4 = Model::LoadFromOBJ("Tama");
    Model* model_5 = Model::LoadFromOBJ("Cloud");

    //生成
    Object3d* object3d = Object3d::Create();
    Object3d* object3d_2 = Object3d::Create();
    Object3d* object3d_3 = Object3d::Create();
    Object3d* object3d_4 = Object3d::Create();
    Object3d* object3d_5 = Object3d::Create();

    Object3d* object3d_6 = Object3d::Create();
    Object3d* object3d_7 = Object3d::Create();
    Object3d* object3d_8 = Object3d::Create();

    Object3d* object3d_9 = Object3d::Create();
    Object3d* object3d_10 = Object3d::Create();
    Object3d* object3d_11 = Object3d::Create();


    //ひもづけ
    object3d->SetModel(model);
    object3d_2->SetModel(model_2);
    object3d_3->SetModel(model_3);
    object3d_4->SetModel(model_4);
    object3d_5->SetModel(model_5);

    object3d_6->SetModel(model_5);
    object3d_7->SetModel(model_5);
    object3d_8->SetModel(model_5);

    object3d_9->SetModel(model_5);
    object3d_10->SetModel(model_5);
    object3d_11->SetModel(model_5);


    //位置指定
    object3d->SetPosition({ 0,0,-9 });
    object3d_2->SetPosition({ 0,-5,-30 });
    object3d_5->SetPosition({ 0,15,10 });

    object3d_6->SetPosition({ 20,-10,-10 });
    object3d_7->SetPosition({ -10,-5,0 });
    object3d_8->SetPosition({ 5,4,-10 });

    object3d_9->SetPosition({ -43,-20,5 });
    object3d_10->SetPosition({ -40,12,0 });
    object3d_11->SetPosition({ 39,0,0 });

    //Audio初期化
    audio->Initialize();

    
    //スプライト初期化
    SpriteCommon* spriteCommon= new SpriteCommon();
    spriteCommon->Initialize(dxCommon->GetDev());

    Sprite* sprite_ = new Sprite();
    //sprite_->Initialize(spriteCommon,);

    //sprite->

    spriteCommon->SpriteCommonCreate();

    spriteCommon->SpriteCommonLoadTexture( 0, L"Resources/title.png", dxCommon->GetDev());
   

    Sprite sprite;

     sprite_->Initialize(spriteCommon, 0);

    //sprite_->texnumber_ = 0;




   spriteCommon->SpriteCommonLoadTexture(1, L"Resources/TIME.png", dxCommon->GetDev());


    Sprite sprite1;

    sprite_-> Initialize(spriteCommon, 1);

    //sprite1.texnumber = 1;


    spriteCommon->SpriteCommonLoadTexture(4, L"Resources/GameOver.png", dxCommon->GetDev());


    Sprite sprite4;

    sprite_ -> Initialize(spriteCommon, 4);

    //sprite4.texnumber = 4;

    //デバックテキスト
 //   DebugText debugtext;

  //  const int debugTextTexNumber = 2;

  //  spriteCommon->SpriteCommonLoadTexture(debugTextTexNumber, L"Resources/ASC_white1280.png", dxCommon->GetDev());

  //  debugtext.debugTextInit(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height, debugTextTexNumber, spriteCommon);

  //  DebugText debugtext2;
///
  //  const int debugTextTexNumber2 = 3;

  //  spriteCommon->SpriteCommonLoadTexture(debugTextTexNumber2, L"Resources/ASC_white1280.png", dxCommon->GetDev());

  //  debugtext2.debugTextInit(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height, debugTextTexNumber2, spriteCommon);



   


    // DirectX初期化処理　ここまで

    float x = 0;
    float y = 0;
    int Color = 0.1;
    float Green = 0;
    float Red = 1;

    float r = 1;
    float g = 1;
    float b = 1;

    int Flag = 0;

    int ShotFlag = 0;
    int EnemyFlag = 0;
    int Enemy_Move = 0;
    int Time = 0;

    float angle = 0.0f;//カメラの回転角度
    //描画初期化処理


        //定数バッファデータ構造体
    struct ConstBufferData {
        XMFLOAT4 color;//色
        XMMATRIX mat;//3D変換行列
    };

   
    ComPtr<ID3D12Resource> constBuff;
    result = dxCommon->GetDev()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(&constBuff));
  

    
    ConstBufferData* constMap = nullptr;
    result = constBuff->Map(0, nullptr, (void**)&constMap);//マッピング
    constMap->color = XMFLOAT4(1, 0, 0, 0.5f);//半透明の赤
    constBuff->Unmap(0, nullptr);//マッピング削除

    constMap->mat = XMMatrixIdentity();

    //constMap->mat.r[0].m128_f32[0] = 2.0f / 1280;
    //constMap->mat.r[1].m128_f32[1] = -2.0f / 720;

    constMap->mat = XMMatrixOrthographicOffCenterLH(
        200.0f, 1080.0f,
        620.0f, 100.0f,
        256.0f, 0.0f
    );

    constMap->mat = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(60.0f),//上下角度
        (float)1280 / 720,//アスペクト比
        0.1f, 1000.0f//前端、奥端
    );


    XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(60.0f),
        (float)WinApp::window_width / WinApp::window_height,
        0.1f, 1000.0f
    );
    constMap->mat = matProjection;

    //ビュー変換行列
    XMMATRIX matView;
    XMFLOAT3 eye(0, 0, -100);
    XMFLOAT3 target(0, 0, 0);
    XMFLOAT3 up(0, 1, 0);
    matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

    constMap->mat = matView * matProjection;



    //ワールド変換
    XMMATRIX matWorld;
    matWorld = XMMatrixIdentity();


    XMMATRIX matScale;//スケーリング行列
    matScale = XMMatrixScaling(1.0f, 0.5f, 1.0f);
    matWorld *= matScale;

    XMMATRIX matRot;//回転行列
    matRot = XMMatrixIdentity();
    matRot *= XMMatrixRotationZ(XMConvertToRadians(45.0f));
    matRot *= XMMatrixRotationX(XMConvertToRadians(45.0f));
    matRot *= XMMatrixRotationY(XMConvertToRadians(45.0f));
    matWorld *= matRot;



    XMMATRIX matTrans;//平行移動行列
    matTrans = XMMatrixTranslation(70.0f, 0, 0);
    matWorld *= matTrans;

    XMFLOAT3 scale;
    XMFLOAT3 rotation;
    XMFLOAT3 position;

    scale = { 20.0f,20.0f,1.0f };
    rotation = { 0.0f,0.0f,0.0f };
    position = { 0.0f,0.0f,0.0f };

    // XMFLOAT3  position = { 50.0f,0.0f,0.0f };

    constMap->mat = matWorld * matView * matProjection;
    /*

    */
    //定数バッファ用
    ComPtr< ID3D12DescriptorHeap>basicDescHeap;
    //設定構造体
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//
    descHeapDesc.NumDescriptors = 2;//シェーダ
    //デスクリフヒープの生成
    result = dxCommon->GetDev()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

    //でスクリプタヒープの戦闘バンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle =
        basicDescHeap->GetCPUDescriptorHandleForHeapStart();

    //CD3DX12_CPU_DESCRIPTOR_HANDLE basicHeapHandle =
      //  CD3DX12_CPU_DESCRIPTOR_HANDLE(basicDescHeap->GetCPUDescriptorHandleForHeapStart(),
            
        //    dev->GetDescriptorHandleIncrementSize(/*D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV*/heapDesc.Type));

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
    dxCommon->GetDev()->CreateConstantBufferView(
        &cbvDesc, basicHeapHandle);

    //デスクリスタテーブルの設定
    //
    /*
    D3D12_DESCRIPTOR_RANGE descTblRangeCBV{};
    descTblRangeCBV.NumDescriptors = 1;//定数一つ
    descTblRangeCBV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//種別は定数
    descTblRangeCBV.BaseShaderRegister = 0;//スロットから
    descTblRangeCBV.OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//標準
     //デスクリスタテーブルの設定
    D3D12_DESCRIPTOR_RANGE descTblRangeSRV{};
    descTblRangeSRV.NumDescriptors = 1;//定数一つ
    descTblRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//種別は定数
    descTblRangeSRV.BaseShaderRegister = 0;//スロットから
    descTblRangeSRV.OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//標準
    //
    D3D12_ROOT_PARAMETER rootparams[2] = {};
    //定数用
    rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//種類
    rootparams[0].DescriptorTable.pDescriptorRanges = &descTblRangeCBV;//でスクリプタレンジ
    rootparams[0].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
    rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
    //テクスチャ用
    rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//種類
    rootparams[1].DescriptorTable.pDescriptorRanges = &descTblRangeSRV;//でスクリプタレンジ
    rootparams[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
    rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
    */
    CD3DX12_DESCRIPTOR_RANGE descRangeCBV, descRangeSRV;
    descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER rootparams[2];
    rootparams[0].InitAsDescriptorTable(1, &descRangeCBV);
    rootparams[1].InitAsDescriptorTable(1, &descRangeSRV);

    //テクスチャバッファ
    /*const int texWidth = 256;//横方向ピクセル
    const int imageDataCount = texWidth * texWidth;//配列の要素数
    //画像イメージデータ配列
    XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];//必ず後で開放する

    //全ピクセルの色を初期化
    for (int i = 0; i < imageDataCount; i++)
    {
        imageData[i].x = 1.0f;//R
        imageData[i].y = 0.0f;//G
        imageData[i].z = 0.0f;//B
        imageData[i].w = 1.0f;//A
    }*/

    TexMetadata   metadata{};
    ScratchImage scratchImg{};

    result = LoadFromWICFile(
        L"Resources/sky.png",
        WIC_FLAGS_NONE,
        &metadata, scratchImg
    );

    const Image* img = scratchImg.GetImage(0, 0, 0);


    //
    /*
    D3D12_HEAP_PROPERTIES texHeapProp{};//テクスチャ―ヒープ設定
    texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    D3D12_RESOURCE_DESC texresDesc{};//リソース設定
    texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);//2Dテクスチャ要
    texresDesc.Format = metadata.format;//RGBAフォーマット
    texresDesc.Width = metadata.width;//幅
    texresDesc.Height = (UINT)metadata.height;//高さ
    texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
    texresDesc.MipLevels = (UINT16)metadata.mipLevels;
    texresDesc.SampleDesc.Count = 1;
    */
    //
    CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        metadata.format,
        metadata.width,
        (UINT)metadata.height,
        (UINT16)metadata.arraySize,
        (UINT16)metadata.mipLevels
    );

    //テクスチャバッファの生成
    /*
    ComPtr<ID3D12Resource>texbuff;
    result = dev->CreateCommittedResource(//GPUリソース設定
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &texresDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
        nullptr,
        IID_PPV_ARGS(&texbuff)
    );
    */
    ComPtr<ID3D12Resource>texbuff=nullptr;
    result = dxCommon->GetDev()->CreateCommittedResource(//GPUリソース設定
        &CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
        D3D12_HEAP_FLAG_NONE,
        &texresDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&texbuff));




    //でスクリプタ1個分アドレスをまとめる
    basicHeapHandle.ptr +=
        dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //シェーダリソースビュー設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
    srvDesc.Format = metadata.format;//DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    //ヒープの2番目にシェーダーリソースビュー作成
    dxCommon->GetDev()->CreateShaderResourceView(texbuff.Get(),//ビューと関連づけるバッファ
        &srvDesc,//テクスチャ設定情報
        basicHeapHandle
    );




    //テクスチャバッファにデータ転送
    result = texbuff->WriteToSubresource(
        0,
        nullptr,//全領域へコピー
        img->pixels,//元データアドレス
        (UINT)img->rowPitch,//1ラインサイズ
        (UINT)img->slicePitch//全サイズ
    );

    //元データ開放
   

    struct Vertex
    {
        XMFLOAT3 pos;//xyz座標
        XMFLOAT3 normal;//法線ベクトル
        XMFLOAT2 uv;//uv座標
    };
    const float topHeight = 10.0f;//天井の高さ

    //頂点データ
    Vertex vertices[] = {

        {{-5.0f,-5.0f,-5.0f},{}, {0.0f,1.0f}},//左下
        {{-5.0f,+5.0f,-5.0f},{}, {0.0f,0.0f}},//左上
        {{+5.0f,-5.0f,-5.0f},{}, {1.0f,1.0f}},//右下
        {{+5.0f,+5.0f,-5.0f},{}, {1.0f,0.0f}},//右上
        //後

        {{-5.0f,+5.0f,5.0f},{}, {0.0f,0.0f}},//左上
        {{-5.0f,-5.0f,5.0f},{}, {0.0f,1.0f}},//左下
        {{+5.0f,+5.0f,5.0f},{}, {1.0f,0.0f}},//右上
        {{+5.0f,-5.0f,5.0f},{}, {1.0f,1.0f}},//右下
        //左
        {{-5.0f,-5.0f,-5.0f},{}, {0.0f,1.0f}},//左下
        {{-5.0f,-5.0f,5.0f},{}, {0.0f,0.0f}},//左上
        {{-5.0f, 5.0f,-5.0f},{}, {1.0f,1.0f}},//右下
        {{-5.0f,+5.0f,5.0f},{}, {1.0f,0.0f}},//右上
        //右

        {{5.0f,-5.0f,5.0f},{}, {0.0f,0.0f}},//左上
        {{5.0f,-5.0f,-5.0f},{}, {0.0f,1.0f}},//左下
        {{5.0f,+5.0f,5.0f},{}, {1.0f,0.0f}},//右上
        {{5.0f, 5.0f,-5.0f},{}, {1.0f,1.0f}},//右下
        //下
        {{-5.0f,5.0f,5.0f},{}, {0.0f,1.0f}},//左下
        {{5.0f,5.0f,5.0f},{}, {0.0f,0.0f}},//左上
        {{-5.0f,5.0f,-5.0f},{}, {1.0f,1.0f}},//右下
        {{5.0f,+5.0f,-5.0f},{}, {1.0f,0.0f}},//右上
        //上
        {{-5.0f,-5.0f,5.0f},{}, {0.0f,1.0f}},//左下
        {{-5.0f,-5.0f,-5.0f},{}, {0.0f,0.0f}},//左上
        {{5.0f,-5.0f,5.0f},{}, {1.0f,1.0f}},//右下
        {{5.0f,-5.0f,-5.0f},{}, {1.0f,0.0f}},//右上

        
    };


    constMap->mat.r[3].m128_f32[0] = -1.0f;
    constMap->mat.r[3].m128_f32[1] = 1.0f;
    //インデックスデータ
    unsigned short indices[] = {

        0,1,2,//三角形一つ目
        2,1,3,//二つ目
        //
        4,5,6,
        6,5,7,
        //
        8,9,10,
        10,9,11,
        //
        12,13,14,
        14,13,15,
        //
        16,17,18,
        18,17,19,
        //
        20,21,22,
        22,21,23

       
    };

   

    for (int i = 0; i < 36 / 3; i++)
    {
        unsigned short indices0 = indices[i * 3 + 0];
        unsigned short indices1 = indices[i * 3 + 1];
        unsigned short indices2 = indices[i * 3 + 2];

        XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
        XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
        XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);

        XMVECTOR v1 = XMVectorSubtract(p1, p0);
        XMVECTOR v2 = XMVectorSubtract(p2, p0);

        XMVECTOR normal = XMVector3Cross(v1, v2);

        normal = XMVector3Normalize(normal);

        XMStoreFloat3(&vertices[indices0].normal, normal);
        XMStoreFloat3(&vertices[indices1].normal, normal);
        XMStoreFloat3(&vertices[indices2].normal, normal);
    }

    // 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
    UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));

    // 頂点バッファの設定
    /*
    D3D12_HEAP_PROPERTIES heapprop{};   // ヒープ設定
    heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

    D3D12_RESOURCE_DESC resdesc{};  // リソース設定
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resdesc.Width = sizeVB; // 頂点データ全体のサイズ
    resdesc.Height = 1;
    resdesc.DepthOrArraySize = 1;
    resdesc.MipLevels = 1;
    resdesc.SampleDesc.Count = 1;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 頂点バッファの生成
    ComPtr< ID3D12Resource>vertBuff;
    result = dev->CreateCommittedResource(
        &heapprop, // ヒープ設定
        D3D12_HEAP_FLAG_NONE,
        &resdesc, // リソース設定
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));
    *///
    ComPtr< ID3D12Resource>vertBuff;
    result = dxCommon->GetDev()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff)
    );

    // GPU上のバッファに対応した仮想メモリを取得
    Vertex* vertMap = nullptr;
    result = vertBuff->Map(0, nullptr, (void**)&vertMap);

    // 全頂点に対して
    for (int i = 0; i < _countof(vertices); i++)
    {
        vertMap[i] = vertices[i];   // 座標をコピー
    }

    // マップを解除
    vertBuff->Unmap(0, nullptr);

    // 頂点バッファビューの作成
    D3D12_VERTEX_BUFFER_VIEW vbView{};

    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeVB;
    vbView.StrideInBytes = sizeof(Vertex);

    ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
    ComPtr< ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
    ComPtr< ID3DBlob> errorBlob; // エラーオブジェクト


    // 頂点シェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/BasicVS.hlsl",  // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &vsBlob, &errorBlob);
    // ピクセルシェーダの読み込みとコンパイル
    result = D3DCompileFromFile(
        L"Resources/shaders/BasicPS.hlsl",   // シェーダファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
        "main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &psBlob, &errorBlob);

    if (FAILED(result)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string errstr;
        errstr.resize(errorBlob->GetBufferSize());

        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            errstr.begin());
        errstr += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(errstr.c_str());
        exit(1);
    }

    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }, // (1行で書いたほうが見やすい)
        {
            "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        },
        {
            "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        },
    };

    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

    //デプスステンシルステートの設定
    /*
    gpipeline.DepthStencilState.DepthEnable = true;
    gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    */
    gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    /*
    gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
    gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
    gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
    gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
    */
    //頂点シェーダ、ピクセルシェーダ
    gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
    gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

    
    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
    /*
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;  // カリングしない
    gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
    gpipeline.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に
    */
    //ラスタライザ
    gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

   // gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画
    //レンダ―ターゲットのブレンド設定
    D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
    blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定
    blenddesc.BlendEnable = true;//ブレンドを友好にする
    blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
    blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
    blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
    /*加算*/
    //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
    //blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
    //blenddesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
    /*減算*/
    //blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//デストからソースを減算
    //blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
    //blenddesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
    /*色反転*/
    //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
    //blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//デストカラーの値
    //blenddesc.DestBlend = D3D12_BLEND_ZERO;//使わない
    /*半透明*/
    blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
    blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのα
    blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0fのソースのα

    gpipeline.InputLayout.pInputElementDescs = inputLayout;
    gpipeline.InputLayout.NumElements = _countof(inputLayout);

    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    gpipeline.NumRenderTargets = 1; // 描画対象は1つ
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
    gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

    //スタティックサンプラー
    /*
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};

    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    */
    CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);



    ComPtr<ID3D12RootSignature>rootsignature;

    //
    /*
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootparams;
    rootSignatureDesc.NumParameters = _countof(rootparams);

    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;

    ComPtr<ID3DBlob>rootSigBlob;
    result = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1_0,
        &rootSigBlob,
        &errorBlob
    );

    result = dev->CreateRootSignature(
        0,
        rootSigBlob->GetBufferPointer(),
        rootSigBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootsignature)
    );
    //rootSigBlob->Release();

    // パイプラインにルートシグネチャをセット
    gpipeline.pRootSignature = rootsignature.Get();
    */
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


    ComPtr<ID3DBlob>rootSigBlob;
    result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob,
        &errorBlob);

    result = dxCommon->GetDev()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootsignature));

    gpipeline.pRootSignature = rootsignature.Get();

    ComPtr<ID3D12PipelineState>pipelinestate;
    result = dxCommon->GetDev()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
    //インデックスデータのサイズ
    UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * _countof(indices));
    //インデックスバッファの生成
    //
    /*
    ComPtr< ID3D12Resource>indexBuff;
    resdesc.Width = sizeIB;//インデックス情報が入る分のサイズ
    //インデックスバッファの生成
    result = dev->CreateCommittedResource(
        &heapprop,//ヒープ生成
        D3D12_HEAP_FLAG_NONE,
        &resdesc,//リソース設定
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuff));
        */
    //
    ComPtr< ID3D12Resource>indexBuff;
    result = dxCommon->GetDev()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuff));

    //GPUのバッファに対応した仮想メモリを取得
    unsigned short* indexMap = nullptr;
    result = indexBuff->Map(0, nullptr, (void**)&indexMap);

    //全イデックスに対して
    for (int i = 0; i < _countof(indices); i++)
    {
        indexMap[i] = indices[i];//インデックスをコピー
    }
    //つながりを解除
    indexBuff->Unmap(0, nullptr);

    D3D12_INDEX_BUFFER_VIEW ibview{};
    ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
    ibview.Format = DXGI_FORMAT_R16_UINT;
    ibview.SizeInBytes = sizeIB;


    //2D
    struct VertexPosUv
    {
        XMFLOAT3 pos;
        XMFLOAT2 uv;
    };

    //3D初期値設定
    sphere.center = XMVectorSet(0, 2, 0, 1);
    sphere.redius = 1.0f;

    plane.normal = XMVectorSet(0, 1, 0, 0);
    plane.distance = 0.0f;

    ray.start = XMVectorSet(0, 1, 0, 1);
    ray.dir = XMVectorSet(0, -1, 0, 0);

    triangle.p0 = XMVectorSet(-1.0f, 0, 1.0f, 1);
    triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1);
    triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1);
    triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0);

    //的のフラグ
    int TargetFlag = 1;

    //音声読み込み
    Audio::SoundData soundData1 = audio->SoundLoadWave("Resources/Shot.wav");//正解の音
    Audio::SoundData soundData2 = audio->SoundLoadWave("Resources/Push.wav");//正解の音

    //パイプライン生成
    //PipelineSet object3dPipelineSet = Create3dpipe(dxCommon->GetDev());
   // PipelineSet spritePipelineSet = CreateSprite2dpipe(dxCommon->GetDev());

    BYTE key[256] = {};
    BYTE olds[256] = {};

    //スプライトサイズ等
    //sprite.rotation = { 0,0,0 };
    //sprite.position = { 640, 360, 0 };
    //sprite.size = { 1280.0f,720.0f };
    //sprite.texSize = { 1280.0f,720.0f };

    sprite_->SetPosition({ 640,360,0 });

   // sprite1.rotation = { 0,0,0 };
   // sprite1.position = { 640, 360, 0 };
   // sprite1.size = { 1280.0f,720.0f };
   // sprite1.texSize = { 1280.0f,720.0f };

   // sprite4.rotation = { 0,0,0 };
   // sprite4.position = { 640, 360, 0 };
   // sprite4.size = { 1280.0f,720.0f };
   // sprite4.texSize = { 1280.0f,720.0f };

    sprite_->SpriteTransVertexBuffer();
   // sprite->SpriteTransVertexBuffer();
   // sprite->SpriteTransVertexBuffer();


    int debug_x = 600;
    int debug_y = 80;


    int debug2_x = 300;
    int debug2_y = 24;


    //ゲームシーン処理関連

    int GameScene = 0;
    int Target_Hit = 0;
    char moji[64];
    char moji2[64];

    int Time_Count = 0;

    int TimeRimit = 20;

    Audio* audio = new Audio();
   
    while (true)  // ゲームループ
    {

        //3d更新
        object3d->Update();
        object3d_2->Update();
        object3d_3->Update();
        object3d_4->Update();
        object3d_5->Update();
        object3d_6->Update();
        object3d_7->Update();
        object3d_8->Update();
        object3d_9->Update();
        object3d_10->Update();
        object3d_11->Update();



        //スプライト
       sprite_->SpriteUpdate();
       //sprite->SpriteUpdate(sprite1, spriteCommon);
       //sprite->SpriteUpdate(sprite4, spriteCommon);


        sprintf_s(moji, "%d", Target_Hit);
        sprintf_s(moji2, "%d", TimeRimit);


       
        if (winApp->ProcessMessage())
        {
            //ゲームループを抜ける
            break;
        }

        // DirectX毎フレーム処理　ここから
      
        input->Update();
    
        //立体1つ目（自機）      
        XMFLOAT3 position3D = object3d_2->GetPosition();
        //弾
        XMFLOAT3 position3D_3 = object3d_4->GetPosition();
        //的
        XMFLOAT3 position3D_4 = object3d->GetPosition();


        //当たり判定更新
        sphere.center = XMVectorSet(position3D_3.x, position3D_3.y, position3D_3.z, 1);
        ray.start = XMVectorSet(0, 1, 0, 1);
        ray.dir = XMVectorSet(0, -1, 0, 0);

        //初期化処理
        if (GameScene == 0)
        {
            Target_Hit = 0;
            TimeRimit = 20;
            Time_Count = 0;
           

        }

        if (GameScene == 1)
        {
            Time_Count++;

            if (Time_Count >= 60)
            {
                TimeRimit -= 1;
                Time_Count = 0;
            }

            if (TimeRimit <= 0)
            {
                GameScene = 2;
            }
        }


        //操作　上下移動　発射処理
        if (input->TriggerKey(DIK_SPACE) && GameScene == 0)
        {
            GameScene = 1;

            position3D.x = 0.0f;
            position3D.y = -5.0f;


            position3D_4.x = 0.0f;
            position3D_4.y = 0.0f;

            audio->SoundPlayWave(audio->xAudio2.Get(), soundData2);
            // 座標の変更を反映
            object3d_2->SetPosition(position3D);
        }
        else if (input->TriggerKey(DIK_SPACE) || input->PushKey(DIK_A) || input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) && GameScene == 1)
        {
            //position.y += 1.0f;

            //matTrans = XMMatrixTranslation(position.x, position.y, position.z);//
            //matWorld = XMMatrixIdentity();
            //matWorld *= matTrans;

            //縦&横
            if (input->PushKey(DIK_W) && GameScene == 1)
            {
                position3D.y += 1.0f;
               // position3D.y += 0.25f;
            }
            else if (input->PushKey(DIK_S) && GameScene == 1)
            {
               position3D.y -= 1.0f;
                //position3D.y -= 0.25f;
            }

            if (input->PushKey(DIK_A) && GameScene == 1)
            {
                position3D.x -= 1.0f;
               // position3D.x -= 0.25f;
            }
            else if (input->PushKey(DIK_D) && GameScene == 1)
            {
                position3D.x += 1.0f;
              //  position3D.x += 0.25f;
            }

            //弾          
            if (input->TriggerKey(DIK_SPACE) && ShotFlag == 0 && GameScene == 1)
            {
                ShotFlag = 1;
            }

            // 座標の変更を反映
            object3d_2->SetPosition(position3D);
        }
        
        if (input->TriggerKey(DIK_SPACE) && GameScene == 2)
        {
            GameScene = 0;
            audio->SoundPlayWave(audio->xAudio2.Get(), soundData2);
        }

        //照準
        XMFLOAT3 position3D_2 = object3d_3->GetPosition();
        position3D_2 = position3D;
        position3D_2.z = -12;
        object3d_3->SetPosition(position3D_2);

        //弾
        if (ShotFlag == 1)
        {
            position3D_3.z += 0.8f;
        }
        else
        {
            position3D_3.z = position3D.z;
            position3D_3.x = position3D.x;
            position3D_3.y = position3D.y+1;
        }
        //奥まで行ったら弾のフラグを0にする
        if (position3D_3.z > 10.0f)
        {
            ShotFlag = 0;
        }

        object3d_4->SetPosition(position3D_3);

        //的

        //弾が当たったらランダムな位置に生成
        if (TargetFlag == 0)
        {
            position3D_4.x = rand() % 41 - 20;
            position3D_4.y = rand() % 24 - 12;
            TargetFlag = 1;
        }

        object3d->SetPosition(position3D_4);


        //当たり判定
        XMVECTOR position_sub2 = XMVectorSet(
            position3D_3.x - position3D_4.x,
            position3D_3.y - position3D_4.y,
            position3D_3.z - position3D_4.z,
            0
        );

        position_sub2 = XMVector3Length(position_sub2);
        float distance2 = position_sub2.m128_f32[0];

        if (distance2 <= 3.5f && ShotFlag == 1)
        {
            ShotFlag = 0;     
            TargetFlag = 0;
            Target_Hit += 1;

            audio->SoundPlayWave(audio->xAudio2.Get(), soundData1);
        }

        //10回当てたらクリア
        if (Target_Hit >= 10)
        {
            GameScene = 0;
        }

      //  debugtext.Print(spriteCommon, moji, debug_x, debug_y);
      //  debugtext2.Print(spriteCommon, moji2, debug2_x, debug2_y,1.0f);


        //画像の回転
        rotation.z = 0.0f;
        rotation.x = 0.0f;
        rotation.y = 0.0f;
        matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
        matTrans = XMMatrixTranslation(position.x, position.y, position.z);
        matRot = XMMatrixIdentity();
        matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
        matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
        matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

        matWorld = XMMatrixIdentity();
        matWorld *= matScale;
        matWorld *= matRot;
        matWorld *= matTrans;

       
        // GPU上のバッファに対応した仮想メモリを取得
        Vertex* vertMap = nullptr;
        result = vertBuff->Map(0, nullptr, (void**)&vertMap);
        constMap->mat = matWorld * matView * matProjection;
        // 全頂点に対して
        for (int i = 0; i < _countof(vertices); i++)
        {
            vertMap[i] = vertices[i];   // 座標をコピー
        }
        constMap->mat = matWorld * matView * matProjection;
        // マップを解除
        vertBuff->Unmap(0, nullptr);


        char str[128];
        //  sprintf_s(str, "x=%.2f,y=%.2f",x,-y);
        OutputDebugStringA(str);
        OutputDebugStringA("\n");
    
        // バックバッファの番号を取得（2つなので0番か1番）

        //
        dxCommon->PreDraw();



        //3D描画前処理
        Object3d::PreDraw(dxCommon->GetCmdList());

        //3D描画
        // 
        // 
        if(TargetFlag==1)
        object3d->Draw();
        object3d_2->Draw();
        object3d_3->Draw();
        object3d_4->Draw();
        object3d_5->Draw();

        object3d_6->Draw();
        object3d_7->Draw();
        object3d_8->Draw();

        object3d_9->Draw();
        object3d_10->Draw();
        object3d_11->Draw();
        //ここに処理追加できる

        //3D描画後処理
        Object3d::PostDraw();

        dxCommon->GetCmdList()->IASetIndexBuffer(&ibview);
        // cmdList->IASetIndexBuffer(&ibview2);
         // ４．描画コマンドここから

        //一つ目
        //cmdList->DrawInstanced(3, 1, 0, 0);
        constMap->color = XMFLOAT4(r, g, b, 1);//半透明の赤
        dxCommon->GetCmdList()->SetPipelineState(pipelinestate.Get());
        dxCommon->GetCmdList()->SetGraphicsRootSignature(rootsignature.Get());
        //デスクリプタヒープをセット
        ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
        dxCommon->GetCmdList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
        //定数バッファビューをセット
      //  cmdList->SetGraphicsRootDescriptorTable(0, basicdescHeap->GetGPUDescriptorHandleForHeapStart());
        //デスクリプタヒープの先頭ハンドルを取得
        D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
        //ヒープの先頭にあるCBVを√パラメータ0番に設定
        dxCommon->GetCmdList()->SetGraphicsRootDescriptorTable(0, gpuDescHandle);
        //ハンドル江尾1つ進める
        gpuDescHandle.ptr += dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        //ヒープの2番目にあるSRVを√パラメータ1番に設定
        dxCommon->GetCmdList()->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

       
        dxCommon->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        dxCommon->GetCmdList()->IASetVertexBuffers(0, 1, &vbView);

        dxCommon->GetCmdList()->DrawIndexedInstanced(36, 1, 0, 0, 0);

        ////スプライト共通コマンド
        spriteCommon->SpriteCommonBeginDraw(dxCommon->GetCmdList());

        //スプライト表示

        //タイトル
        if(GameScene==0)
       sprite_-> SpriteDraw(dxCommon->GetCmdList(),dxCommon->GetDev());//タイトル

        //ゲームオーバー
        if (GameScene == 2)
        {
            //SpriteDraw(sprite4, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());//タイトル
        }

        //メイン
        if (GameScene == 1)
        {
           // SpriteDraw(sprite1, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());//タイトル


          //  debugtext.DrawAll(dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());//的カウント

           // debugtext2.DrawAll(dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());//時間カウント

        }

        

        // ４．描画コマンドここまで
     
        // DirectX毎フレーム処理　ここまで
        dxCommon->PostDraw();
    }
    //3D解放
    delete object3d;
    delete object3d_2;
    delete object3d_3;
    delete object3d_4;
    delete object3d_5;

    delete object3d_6;
    delete object3d_7;
    delete object3d_8;

    delete object3d_9;
    delete object3d_10;
    delete object3d_11;

    //sprite
    delete spriteCommon;

    //入力開放
    delete input;

    //DirectX解放
    delete dxCommon;


    winApp->Finalize();

    //WindowsAPI解放
    delete winApp;
    winApp = nullptr;

   //3Dモデル解放
    delete model;
    delete model_2;
    delete model_3;
    delete model_4;
    delete model_5;
    

    // ウィンドウクラスを登録解除
   // UnregisterClass(w.lpszClassName, w.hInstance);
    audio->xAudio2.Reset();
    audio->SoundUnload(&soundData1);
    audio->SoundUnload(&soundData2);

    return 0;
}
