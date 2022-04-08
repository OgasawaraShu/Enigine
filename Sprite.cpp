#include "Sprite.h"
#include <cassert>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "DirectXCommon.h"

DirectXCommon* dxCommon = nullptr;



#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;


void Sprite::SpriteUpdate()
{
     //ワールド行列の更新
    matWorld_ = XMMatrixIdentity();

    //Z軸回転
    matWorld_ *= XMMatrixRotationZ(XMConvertToRadians(rotation_.z));

    //平行移動
    matWorld_ *= XMMatrixTranslation(position_.x,position_.y,position_.z);

    //定数バッファの転送
    ConstBufferData8* constMap = nullptr;
    HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap);
    constMap->color8 = color_;
    constMap->mat8 = matWorld_ * spritecommon_->matProjection;



    constBuff_->Unmap(0, nullptr);
}

void Sprite::SpriteTransVertexBuffer()
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

    float left = (0.0f - anchorpoint_.x) * size_.x;

    float right = (1.0f - anchorpoint_.x) * size_.x;

    float top = (0.0f - anchorpoint_.y) * size_.y;

    float bottom = (1.0f - anchorpoint_.y) * size_.y;

    if (isFlagX_ == true)
    {
        left = -left;
        right = -right;
    }

    if (isFlagY_ == true)
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
    if (spritecommon_->texBuff[texnumber_])
    {
        //テクスチャ情報取得
        D3D12_RESOURCE_DESC resDesc = spritecommon_->texBuff[texnumber_]->GetDesc();

        float tex_left = texLeftTop_.x / resDesc.Width;

        float tex_right = (texLeftTop_.x + texSize_.x) / resDesc.Width;

        float tex_top = texLeftTop_.y / resDesc.Height;

        float tex_bottom = (texLeftTop_.y + texSize_.y) / resDesc.Height;

        vertices[LB].uv = { tex_left,	tex_bottom };
        vertices[LT].uv = { tex_left,	tex_top };
        vertices[RB].uv = { tex_right,	tex_bottom };
        vertices[RT].uv = { tex_right,	tex_top };

    }

    //頂点バッファへのデータ転送
    VertexPosUv* vertMap = nullptr;
    result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
    memcpy(vertMap, vertices, sizeof(vertices));
    vertBuff_->Unmap(0, nullptr);
}

void Sprite::SpriteDraw(ID3D12GraphicsCommandList* cmdList,ID3D12Device* dev)
{
    if (isInvisible_ == true)
    {
        return;
    }
    //頂点バッファをセット
    cmdList->IASetVertexBuffers(0, 1, &vbView_);

    //定数バッファをセット
    cmdList->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

    //
    cmdList->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(spritecommon_->descHeap->GetGPUDescriptorHandleForHeapStart(),
            texnumber_,
            dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

    //ポリゴンの描画
    cmdList->DrawInstanced(4, 1, 0, 0);
}

void Sprite::Initialize(SpriteCommon* spriteCommon, UINT texnumber, XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlagX = false, bool isFlagY = false)
{
    HRESULT result = S_FALSE;

  //  Sprite sprite{};

    spritecommon_ = spriteCommon;

    //アンカーポイントをコピー
    anchorpoint_ = anchorpoint;

    //反転フラグをコピー
    isFlagX_ = isFlagX;

    isFlagY_ = isFlagY;

    VertexPosUv vertices[] = {
        {{0.0f	,100.0f	,0.0f},{0.0f,1.0f} },
        {{0.0f	,0.0f	,0.0f},{0.0f,0.0f} },
        {{100.0f,100.0f	,0.0f},{1.0f,1.0f} },
        {{100.0f,0.0f	,0.0f},{1.0f,0.0f} },


    };




    //頂点バッファの生成
    result = spriteCommon->device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff_)
    );

    this->texnumber_ = texnumber;

    if (spritecommon_->texBuff[texnumber_])
    {
        //テクスチャ情報の画像が読み込み済みなら
        D3D12_RESOURCE_DESC resDesc = spriteCommon->texBuff[texnumber_]->GetDesc();

        //スプライトの大きさを画像の解像度に合わせる
        size_ = { (float)resDesc.Width,(float)resDesc.Height };
    }

    //頂点バッファデータ転送
    SpriteTransVertexBuffer();




    //頂点バッファビューへのデータ転送
    VertexPosUv* vertMap = nullptr;
    result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
    memcpy(vertMap, vertices, sizeof(vertices));
    vertBuff_->Unmap(0, nullptr);

    //頂点バッファビューの生成
    vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
    vbView_.SizeInBytes = sizeof(vertices);
    vbView_.StrideInBytes = sizeof(vertices[0]);

    //定数バッファの生成
    result = dxCommon->GetDev()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData8) + 0xff) & ~0xff),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constBuff_));

    //定数バッファにデータを転送
    ConstBufferData8* constMap = nullptr;
    result = constBuff_->Map(0, nullptr, (void**)&constMap);
    constMap->color8 = XMFLOAT4(1, 1, 1, 1);//色指定(R G B A)

    //平行投影法
    constMap->mat8 = XMMatrixOrthographicOffCenterLH(
        0.0f, WinApp::window_width, WinApp::window_height, 0.0f, 0.0f, 1.0f);
    constBuff_->Unmap(0, nullptr);

    //return sprite;

    //return Sprite();
}


