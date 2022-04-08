#pragma once
#include <Windows.h>
#include <d3d12.h>
#include<wrl.h>
#include <DirectXMath.h>


#include"PipeLineSet.h"


class SpriteCommon
{
public:
	//�X�v���C�g�p�e�N�X�`������
	static const int spriteSRVCount = 512;

    //�e�N�X�`�����\�[�X(�e�N�X�`���o�b�t�@)�̔z��
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[spriteSRVCount];

    //�ˉe�s��
    DirectX::XMMATRIX matProjection{};

    //�e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;

    ID3D12Device* device_ = nullptr;


private:
  

    //�p�C�v���C���Z�b�g
    PipeLineSet pipelineSet;

  
   
    

    

public ://�����o�֐�
    void Initialize(ID3D12Device* device);
    void SpriteCommonCreate();
    void SpriteCommonLoadTexture(UINT texnumber, const wchar_t* filename, ID3D12Device* dev);
    void SpriteCommonBeginDraw(ID3D12GraphicsCommandList* cmdList);

private:
    void CreateGraphicPipeLine();

};



