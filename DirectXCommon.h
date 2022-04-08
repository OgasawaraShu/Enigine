#pragma once

#include<Windows.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<dxgi1_6.h>
#include<wrl.h>
#include "WinApp.h"

//DirectX基盤
class DirectXCommon
{
public://メンバ関数

	//初期化
	void Initialize(WinApp*win);

	void PreDraw();

	void PostDraw();

	//Microsoft::WRL::ComPtr<ID3D12Device> GetDev() { return dev; }

	ID3D12Device* GetDev() { return dev.Get(); }

	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }

	

private:

	

	void InitializeDevice();

	void InitializeCommand();

	void InitializeSwapchain();

	void InitializeRenderTargetView();

	void InitializeDepthBuffer();

	void InitializeFence();



	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> dev;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	//WindowsAPI
	WinApp* winApp = nullptr;
    //バックバッファ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;

	



private: // メンバ変数
// ウィンドウズアプリケーション管理
	//WinApp* winApp;

	// Direct3D関連
	//ComPtr<IDXGIFactory6> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4>swapchain;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>cmdQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>rtvHeaps;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12Fence>fence;

	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	//UINT64 fenceVal = 0;


	UINT64 fenceVal = 0;


};

