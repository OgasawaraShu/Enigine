#include "Input.h"

//#include<wrl.h>
//#include<dinput.h>
//using namespace Microsoft::WRL;

//#define DIRECTINPUT_VERSION 0x0800//DirectInput�̃o�[�W�����w��

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


void Input::Intialize(WinApp* winApp)
{
    HRESULT result;


    //DirectInput�̃C���X�^���X����
    //ComPtr<IDirectInput8> dinput = nullptr;
    //�؂�Ă���App�̃C���X�^���X���L�^
    this->winApp = winApp;

    result = DirectInput8Create(
        winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
    //�L�[�{�[�h����
    result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
    //���̓f�[�^�̌`���Z�b�g
    result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
    //�r�����䃌�x���̃Z�b�g
    result = devkeyboard->SetCooperativeLevel(
        winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

}

void Input::Update()
{
    HRESULT result;

    //�O��̃L�[���͂�ۑ�
   // memcpy(oldkey, key, sizeof(key));

    //�L�[�{�[�h���̎擾
    result = devkeyboard->Acquire();
    //�S�L�[�̓��͏����擾
   // BYTE key[256] = {};
    memcpy(oldkey, key, sizeof(key));

    result = devkeyboard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber)
{
    //�w��L�[�������Ă����true��Ԃ�
    if (key[keyNumber])
    {
        return true;
    }

    //�����łȂ����false��Ԃ�
    return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
    //�O�񉟂��Ă��Ȃ��A���񉟂��Ă����true��Ԃ�
    if (key[keyNumber] && !oldkey[keyNumber])
    {
        return true;
    }

    //�����łȂ��ꍇ�Afalse��Ԃ�
    return false;
}
