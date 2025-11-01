#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <vector>
#include <string>

class Audio {
public:
    void init();
    bool playLoopPCM(const std::wstring& wavPath); // 16-bit PCM WAV only
    void shutdown();
private:
    Microsoft::WRL::ComPtr<IXAudio2> xa_;
    IXAudio2MasteringVoice* master_ = nullptr;
    IXAudio2SourceVoice* src_ = nullptr;

    std::vector<unsigned char> data_;
    WAVEFORMATEX wfx_{};
};

