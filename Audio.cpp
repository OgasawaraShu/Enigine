#include "Audio.h"
#include <fstream>
#include <cassert>
#include <Windows.h>


#pragma comment(lib,"xaudio2.lib")
using namespace Microsoft::WRL;


ComPtr<IXAudio2> xAudio2;
IXAudio2MasteringVoice* masterVoice;

Audio::SoundData SoundLoadWave(const char* filename)
{
    HRESULT result;

    //File open
    std::ifstream file;

    file.open(filename, std::ios_base::binary);

    assert(file.is_open());

    //wavData Load
    Audio::RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));

    if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
    {
        assert(0);
    }

    if (strncmp(riff.type, "WAVE", 4) != 0)
    {
        assert(0);
    }

    Audio::FormatChunk format = {};

    file.read((char*)&format, sizeof(Audio::ChunkHeader));
    if (strncmp(format.chunk.id, "fmt ", 4) != 0)
    {
        assert(0);
    }

    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);



    Audio::ChunkHeader data;
    file.read((char*)&data, sizeof(data));

    if (strncmp(data.id, "JUNK", 4) == 0)
    {
        file.seekg(data.size, std::ios_base::cur);

        file.read((char*)&data, sizeof(data));
    }

    if (strncmp(data.id, "LIST", 4) == 0)
    {
        file.seekg(data.size, std::ios_base::cur);

        file.read((char*)&data, sizeof(data));
    }

    if (strncmp(data.id, "data", 4) != 0)
    {
        assert(0);
    }

    char* pBuffer = new char[data.size];
    file.read(pBuffer, data.size);

    file.close();
    //File close

    //return
    Audio::SoundData soundData = {};

    soundData.wfex = format.fmt;
    soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
    soundData.bufferSize = data.size;



    return soundData;
}

void Audio::SoundUnload(SoundData* soundData)
{
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}

void SoundPlayWave(IXAudio2* xAudio2, const Audio:: SoundData& soundData)
{
    HRESULT result;

    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;
    // buf.AudioBytes = size;

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}

void SoundPlayWaveLoop(IXAudio2* xAudio2, const Audio::SoundData& soundData)
{
    HRESULT result;

    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // buf.AudioBytes = size;

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}

bool Audio::Initialize()
{
    HRESULT result;

    result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    //マスターボイスを作成
    result = xAudio2->CreateMasteringVoice(&masterVoice);

    return true;
}