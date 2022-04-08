#include "DirectXCommon.h"

#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;



void DirectXCommon::Initialize(WinApp*winApp)
{
	//NULL検出
	assert(winApp);

	//メンバ変数に記録
	this->winApp = winApp;

	//デバイスの生成
	InitializeDevice();
	//コマンド関連の初期化
	InitializeCommand();
	//すぁっぷチェーンの初期化
	InitializeSwapchain();
	//レンダ―ターゲットビューの初期化
	InitializeRenderTargetView();
	//深度バッファの初期化
	InitializeDepthBuffer();
	//フェンスの初期化K
	InitializeFence();

}



void DirectXCommon::InitializeDevice()
{
	HRESULT result;

	//ComPtr<IDXGIFactory6> dxgiFactory;

#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

    // DXGIファクトリーの生成
    result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    // アダプターの列挙用
    std::vector<ComPtr<IDXGIAdapter1>> adapters;
    // ここに特定の名前を持つアダプターオブジェクトが入る
    ComPtr<IDXGIAdapter1>tmpAdapter;
    for (int i = 0;
        dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
        i++)
    {
        adapters.push_back(tmpAdapter); // 動的配列に追加する
    }

    for (int i = 0; i < adapters.size(); i++)
    {
        DXGI_ADAPTER_DESC1 adesc;
        adapters[i]->GetDesc1(&adesc);

        if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        std::wstring strDesc = adesc.Description;
        if (strDesc.find(L"Intel") == std::wstring::npos)
        {
            tmpAdapter = adapters[i];
            break;
        }
    }

    D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;

    for (int i = 0; i < _countof(levels); i++)
    {
        // 採用したアダプターでデバイスを生成
        result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
        if (result == S_OK)
        {
            // デバイスを生成できた時点でループを抜ける
            featureLevel = levels[i];
            break;
        }
    }

}

void DirectXCommon::InitializeCommand()
{
    HRESULT result;

    // コマンドアロケータを生成
    result = dev->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&cmdAllocator));

    // コマンドリストを生成
    result = dev->CreateCommandList(0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        cmdAllocator.Get(), nullptr,
        IID_PPV_ARGS(&cmdList));

    // 標準設定でコマンドキューを生成
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

    dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
}

void DirectXCommon::InitializeSwapchain()
{
    HRESULT result;

    // 各種設定をしてスワップチェーンを生成
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
    swapchainDesc.Width = 1280;
    swapchainDesc.Height = 720;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 色情報の書式
    swapchainDesc.SampleDesc.Count = 1; // マルチサンプルしない
    swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
    swapchainDesc.BufferCount = 2;  // バッファ数を２つに設定
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ComPtr<IDXGISwapChain1> swapchain1;
    HWND hwnd = winApp->GetHwnd();
    result = dxgiFactory->CreateSwapChainForHwnd(
        cmdQueue.Get(),
        winApp->GetHwnd(),
        &swapchainDesc,
        nullptr,
        nullptr,
        &swapchain1);

    swapchain1.As(&swapchain);
}

void DirectXCommon::InitializeRenderTargetView()
{
	//裏表の2つ分
	backBuffers.resize(2);
    HRESULT result;
	for (int i = 0; i < 2; i++)
	{
        // 各種設定をしてデスクリプタヒープを生成
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
        heapDesc.NumDescriptors = 2;    // 裏表の２つ
        dev->CreateDescriptorHeap(&heapDesc,
            IID_PPV_ARGS(&rtvHeaps));

        // 裏表の２つ分について
        //std::vector<ComPtr<ID3D12Resource>> backBuffers(2);
        for (int i = 0; i < 2; i++)
        {
            // スワップチェーンからバッファを取得
            result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
            // デスクリプタヒープのハンドルを取得
            D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
            // 裏か表かでアドレスがずれる
            handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
            // レンダーターゲットビューの生成
            dev->CreateRenderTargetView(
                backBuffers[i].Get(),
                nullptr,
                handle);
        }
	}
}

void DirectXCommon::InitializeDepthBuffer()
{
    HRESULT result;

    CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D32_FLOAT,
        WinApp::window_width,
        WinApp::window_height,
        1, 0,
        1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    );

    result = dev->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthResDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
        IID_PPV_ARGS(&depthBuffer)
    );





    //深度ビューでスクリプタヒープ
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  //  ComPtr< ID3D12DescriptorHeap>dsvHeap;
    result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

    //深度ビュー作成
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dev->CreateDepthStencilView(
        depthBuffer.Get(),
        &dsvDesc,
        dsvHeap->GetCPUDescriptorHandleForHeapStart()
    );


}

void DirectXCommon::InitializeFence()
{
    HRESULT result;
   

    result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

void DirectXCommon::PreDraw()
{
   
    UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
   
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),bbIndex,
    dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));


    // cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE (dsvHeap->GetCPUDescriptorHandleForHeapStart());
    cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

    // ３．画面クリア           R     G     B    A
    float clearColor[] = { 0,0, 1,1 }; // 青っぽい色
    cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
    cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


   

    cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f,0.0f,WinApp::window_width,WinApp::window_height));

   

    cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0,0, WinApp::window_width, WinApp::window_height));
}

void DirectXCommon::PostDraw()
{

    UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
    // ５．リソースバリアを戻す
        //barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画
        //barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;   // 表示に
        //cmdList->ResourceBarrier(1, &barrierDesc);
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // 命令のクローズ
    cmdList->Close();
    // コマンドリストの実行
    ID3D12CommandList* cmdLists[] = { cmdList.Get() }; // コマンドリストの配列
    cmdQueue->ExecuteCommandLists(1, cmdLists);
    // コマンドリストの実行完了を待つ
    cmdQueue->Signal(fence.Get(), ++fenceVal);
    if (fence->GetCompletedValue() != fenceVal) {
        HANDLE event = CreateEvent(nullptr, false, false, nullptr);
        fence->SetEventOnCompletion(fenceVal, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }

    cmdAllocator->Reset(); // キューをクリア
    cmdList->Reset(cmdAllocator.Get(), nullptr);  // 再びコマンドリストを貯める準備

    // バッファをフリップ（裏表の入替え）
    swapchain->Present(1, 0);
}