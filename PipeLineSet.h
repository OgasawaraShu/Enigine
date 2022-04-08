#pragma once

#include <wrl.h>
#include<d3d12.h>

//パイプラインセット
struct PipeLineSet
{
    ComPtr<ID3D12PipelineState>pipelinestate;

    ComPtr<ID3D12RootSignature>rootsignature;
};
