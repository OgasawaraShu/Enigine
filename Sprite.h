#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "SpriteCommon.h"


class Sprite
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    // DirectX::���ȗ�
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMMATRIX = DirectX::XMMATRIX;

public: // �T�u�N���X
/// <summary>
/// ���_�f�[�^�\����
/// </summary>
    struct VertexPosUv
    {
        XMFLOAT3 pos; // xyz���W
        XMFLOAT2 uv;  // uv���W
    };

    /// <summary>
    /// �萔�o�b�t�@�p�f�[�^�\����
    /// </summary>
    struct ConstBufferData8
    {
        XMFLOAT4 color8;	// �F (RGBA)
        XMMATRIX mat8;	// �R�c�ϊ��s��
    };

private://�����o�ϐ�
    SpriteCommon* spritecommon_ = nullptr;
    //���_�o�b�t�@
    ComPtr<ID3D12Resource> vertBuff_;

    //���_�o�b�t�@�r���[
    D3D12_VERTEX_BUFFER_VIEW vbView_;

    //�萔�o�b�t�@
    ComPtr<ID3D12Resource> constBuff_;

    //XYZ������̉�]��
    XMFLOAT3 rotation_ = { 0.0f,0.0f, 0.0f };

    //���W
    XMFLOAT3 position_ = { 0,0,0 };

    //���[���h�s��
    XMMATRIX matWorld_;

    //�F
    XMFLOAT4 color_ = { 1,1,1,1 };

    //�e�N�X�`���ԍ�
    UINT texnumber_ = 0;

    //�傫��
    XMFLOAT2 size_ = { 100, 100 };

    //�A���J�[�|�C���g
    XMFLOAT2 anchorpoint_ = { 0.5f,0.5f };

    //���E���]
    bool isFlagX_ = false;

    //�㉺���]
    bool isFlagY_ = false;

    //�e�N�X�`��������W
    XMFLOAT2 texLeftTop_ = { 0,0 };

    //�e�N�X�`���؂�o���T�C�Y
    XMFLOAT2 texSize_ = { 100,100 };

    //��\��
    bool isInvisible_ = false;

private:
   


public:
    void SpriteUpdate();
    void SpriteTransVertexBuffer();
    void SpriteDraw(ID3D12GraphicsCommandList* cmdList,ID3D12Device* dev);

    void SetPosition(const DirectX::XMFLOAT3& position) { position_ = position; }


    void Initialize(SpriteCommon* spriteCommon, UINT texnumber, XMFLOAT2 anchorpoint = { 0.5f,0.5f }, bool isFlagX = false, bool isFlagY = false);
};

