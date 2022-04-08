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
     //���[���h�s��̍X�V
    matWorld_ = XMMatrixIdentity();

    //Z����]
    matWorld_ *= XMMatrixRotationZ(XMConvertToRadians(rotation_.z));

    //���s�ړ�
    matWorld_ *= XMMatrixTranslation(position_.x,position_.y,position_.z);

    //�萔�o�b�t�@�̓]��
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

    //	 ���� ���� �E�� �E��
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
    //�w��ԍ��̉摜���ǂݍ��ݍς݂Ȃ�
    if (spritecommon_->texBuff[texnumber_])
    {
        //�e�N�X�`�����擾
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

    //���_�o�b�t�@�ւ̃f�[�^�]��
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
    //���_�o�b�t�@���Z�b�g
    cmdList->IASetVertexBuffers(0, 1, &vbView_);

    //�萔�o�b�t�@���Z�b�g
    cmdList->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

    //
    cmdList->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(spritecommon_->descHeap->GetGPUDescriptorHandleForHeapStart(),
            texnumber_,
            dxCommon->GetDev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

    //�|���S���̕`��
    cmdList->DrawInstanced(4, 1, 0, 0);
}

void Sprite::Initialize(SpriteCommon* spriteCommon, UINT texnumber, XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlagX = false, bool isFlagY = false)
{
    HRESULT result = S_FALSE;

  //  Sprite sprite{};

    spritecommon_ = spriteCommon;

    //�A���J�[�|�C���g���R�s�[
    anchorpoint_ = anchorpoint;

    //���]�t���O���R�s�[
    isFlagX_ = isFlagX;

    isFlagY_ = isFlagY;

    VertexPosUv vertices[] = {
        {{0.0f	,100.0f	,0.0f},{0.0f,1.0f} },
        {{0.0f	,0.0f	,0.0f},{0.0f,0.0f} },
        {{100.0f,100.0f	,0.0f},{1.0f,1.0f} },
        {{100.0f,0.0f	,0.0f},{1.0f,0.0f} },


    };




    //���_�o�b�t�@�̐���
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
        //�e�N�X�`�����̉摜���ǂݍ��ݍς݂Ȃ�
        D3D12_RESOURCE_DESC resDesc = spriteCommon->texBuff[texnumber_]->GetDesc();

        //�X�v���C�g�̑傫�����摜�̉𑜓x�ɍ��킹��
        size_ = { (float)resDesc.Width,(float)resDesc.Height };
    }

    //���_�o�b�t�@�f�[�^�]��
    SpriteTransVertexBuffer();




    //���_�o�b�t�@�r���[�ւ̃f�[�^�]��
    VertexPosUv* vertMap = nullptr;
    result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
    memcpy(vertMap, vertices, sizeof(vertices));
    vertBuff_->Unmap(0, nullptr);

    //���_�o�b�t�@�r���[�̐���
    vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
    vbView_.SizeInBytes = sizeof(vertices);
    vbView_.StrideInBytes = sizeof(vertices[0]);

    //�萔�o�b�t�@�̐���
    result = dxCommon->GetDev()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData8) + 0xff) & ~0xff),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constBuff_));

    //�萔�o�b�t�@�Ƀf�[�^��]��
    ConstBufferData8* constMap = nullptr;
    result = constBuff_->Map(0, nullptr, (void**)&constMap);
    constMap->color8 = XMFLOAT4(1, 1, 1, 1);//�F�w��(R G B A)

    //���s���e�@
    constMap->mat8 = XMMatrixOrthographicOffCenterLH(
        0.0f, WinApp::window_width, WinApp::window_height, 0.0f, 0.0f, 1.0f);
    constBuff_->Unmap(0, nullptr);

    //return sprite;

    //return Sprite();
}


