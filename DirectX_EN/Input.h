#pragma once



#include <windows.h>
#include<wrl.h>
#include "WinApp.h"

#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include<dinput.h>



//入力
class Input
{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数
    //初期化
	void Intialize(WinApp* winApp);
	//更新
	void Update();

	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);

	

private://メンバ変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;
	//DirectInputのインスタンス
	ComPtr<IDirectInput8> dinput;

	//全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE oldkey[256] = {};

private:
	//WindowsAPI
	WinApp* winApp = nullptr;

};

