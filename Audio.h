#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

class Audio
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    struct ChunkHeader
    {
        char id[4];
        int  size;
    };

    struct RiffHeader
    {
        ChunkHeader chunk;
        char type[4];

    };

    struct FormatChunk
    {
        ChunkHeader chunk;
        WAVEFORMATEX fmt;
    };

    struct SoundData
    {
        WAVEFORMATEX wfex;

        BYTE* pBuffer;

        unsigned int bufferSize;


    };

public : //�����o�֐�

    SoundData SoundLoadWave(const char* filename);
    void SoundUnload(SoundData* soundData);
    void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);
    void SoundPlayWaveLoop(IXAudio2* xAudio2, const SoundData& soundData);
    bool Initialize();

public://�����o�ϐ�

    ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* masterVoice;
};

