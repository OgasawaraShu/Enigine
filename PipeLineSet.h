#pragma once

#include <wrl.h>
#include<d3d12.h>

//�p�C�v���C���Z�b�g
struct PipeLineSet
{
    ComPtr<ID3D12PipelineState>pipelinestate;

    ComPtr<ID3D12RootSignature>rootsignature;
};
