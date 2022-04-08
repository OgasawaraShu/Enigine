#include "Input.h"

//#include<wrl.h>
//#include<dinput.h>
//using namespace Microsoft::WRL;

//#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


void Input::Intialize(WinApp* winApp)
{
    HRESULT result;


    //DirectInputのインスタンス生成
    //ComPtr<IDirectInput8> dinput = nullptr;
    //借りてきたAppのインスタンスを記録
    this->winApp = winApp;

    result = DirectInput8Create(
        winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
    //キーボード生成
    result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
    //入力データの形式セット
    result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
    //排他制御レベルのセット
    result = devkeyboard->SetCooperativeLevel(
        winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

}

void Input::Update()
{
    HRESULT result;

    //前回のキー入力を保存
   // memcpy(oldkey, key, sizeof(key));

    //キーボード情報の取得
    result = devkeyboard->Acquire();
    //全キーの入力情報を取得
   // BYTE key[256] = {};
    memcpy(oldkey, key, sizeof(key));

    result = devkeyboard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber)
{
    //指定キーを押していればtrueを返す
    if (key[keyNumber])
    {
        return true;
    }

    //そうでなければfalseを返す
    return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
    //前回押していなく、今回押していればtrueを返す
    if (key[keyNumber] && !oldkey[keyNumber])
    {
        return true;
    }

    //そうでない場合、falseを返す
    return false;
}
