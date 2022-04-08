#pragma once



#include <windows.h>
#include<wrl.h>
#include "WinApp.h"

#define DIRECTINPUT_VERSION 0x0800//DirectInput�̃o�[�W�����w��
#include<dinput.h>



//����
class Input
{
public:
	//namespace�ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://�����o�֐�
    //������
	void Intialize(WinApp* winApp);
	//�X�V
	void Update();

	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);

	

private://�����o�ϐ�
	//�L�[�{�[�h�̃f�o�C�X
	ComPtr<IDirectInputDevice8> devkeyboard;
	//DirectInput�̃C���X�^���X
	ComPtr<IDirectInput8> dinput;

	//�S�L�[�̏��
	BYTE key[256] = {};
	//�O��̑S�L�[�̏��
	BYTE oldkey[256] = {};

private:
	//WindowsAPI
	WinApp* winApp = nullptr;

};

