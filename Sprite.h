#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "SpriteCommon.h"


class Sprite
{
private: // エイリアス
// Microsoft::WRL::を省略
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    // DirectX::を省略
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
/// <summary>
/// 頂点データ構造体
/// </summary>
    struct VertexPosUv
    {
        XMFLOAT3 pos; // xyz座標
        XMFLOAT2 uv;  // uv座標
    };

    /// <summary>
    /// 定数バッファ用データ構造体
    /// </summary>
    struct ConstBufferData8
    {
        XMFLOAT4 color8;	// 色 (RGBA)
        XMMATRIX mat8;	// ３Ｄ変換行列
    };

private://メンバ変数
    SpriteCommon* spritecommon_ = nullptr;
    //頂点バッファ
    ComPtr<ID3D12Resource> vertBuff_;

    //頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView_;

    //定数バッファ
    ComPtr<ID3D12Resource> constBuff_;

    //XYZ軸周りの回転軸
    XMFLOAT3 rotation_ = { 0.0f,0.0f, 0.0f };

    //座標
    XMFLOAT3 position_ = { 0,0,0 };

    //ワールド行列
    XMMATRIX matWorld_;

    //色
    XMFLOAT4 color_ = { 1,1,1,1 };

    //テクスチャ番号
    UINT texnumber_ = 0;

    //大きさ
    XMFLOAT2 size_ = { 100, 100 };

    //アンカーポイント
    XMFLOAT2 anchorpoint_ = { 0.5f,0.5f };

    //左右反転
    bool isFlagX_ = false;

    //上下反転
    bool isFlagY_ = false;

    //テクスチャ左上座標
    XMFLOAT2 texLeftTop_ = { 0,0 };

    //テクスチャ切り出しサイズ
    XMFLOAT2 texSize_ = { 100,100 };

    //非表示
    bool isInvisible_ = false;

private:
   


public:
    void SpriteUpdate();
    void SpriteTransVertexBuffer();
    void SpriteDraw(ID3D12GraphicsCommandList* cmdList,ID3D12Device* dev);

    void SetPosition(const DirectX::XMFLOAT3& position) { position_ = position; }


    void Initialize(SpriteCommon* spriteCommon, UINT texnumber, XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlagX = false, bool isFlagY = false);
};

