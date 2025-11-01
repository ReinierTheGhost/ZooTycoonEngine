#include "Audio.h"
#include <Windows.h>
#include <fstream>

static bool loadPCM16(const std::wstring& path, std::vector<unsigned char>& out, WAVEFORMATEX& wfx) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    auto readU32 = [&]() { uint32_t v; f.read((char*)&v, 4); return v; };
    auto readU16 = [&]() { uint16_t v; f.read((char*)&v, 2); return v; };

    char riff[4]; f.read(riff, 4); if (strncmp(riff, "RIFF", 4) != 0) return false;
    (void)readU32(); char wave[4]; f.read(wave, 4); if (strncmp(wave, "WAVE", 4) != 0) return false;

    bool gotFmt = false, gotData = false; uint32_t dataSize = 0; std::streampos dataPos{};
    while (f && !(gotFmt && gotData)) {
        char id[4]; if (!f.read(id, 4)) break;
        uint32_t sz = readU32();
        if (strncmp(id, "fmt ", 4) == 0) {
            uint16_t audioFormat = readU16();
            wfx.wFormatTag = audioFormat;
            wfx.nChannels = readU16();
            wfx.nSamplesPerSec = readU32();
            wfx.nAvgBytesPerSec = readU32();
            wfx.nBlockAlign = readU16();
            wfx.wBitsPerSample = readU16();
            if (sz > 16) f.seekg(sz - 16, std::ios::cur);
            gotFmt = (audioFormat == WAVE_FORMAT_PCM && wfx.wBitsPerSample == 16);
        }
        else if (strncmp(id, "data", 4) == 0) {
            dataSize = sz; dataPos = f.tellg(); f.seekg(sz, std::ios::cur);
            gotData = true;
        }
        else {
            f.seekg(sz, std::ios::cur);
        }
    }
    if (!gotFmt || !gotData) return false;
    out.resize(dataSize);
    f.clear(); f.seekg(dataPos);
    f.read((char*)out.data(), dataSize);
    return true;
}

void Audio::init() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(XAudio2Create(xa_.ReleaseAndGetAddressOf(), 0))) throw std::runtime_error("XAudio2Create failed");
    xa_->CreateMasteringVoice(&master_);
}

bool Audio::playLoopPCM(const std::wstring& wavPath) {
    if (!loadPCM16(wavPath, data_, wfx_)) return false;
    if (FAILED(xa_->CreateSourceVoice(&src_, &wfx_))) return false;

    XAUDIO2_BUFFER buf{}; buf.pAudioData = data_.data();
    buf.AudioBytes = (UINT32)data_.size();
    buf.Flags = XAUDIO2_END_OF_STREAM;
    buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    if (FAILED(src_->SubmitSourceBuffer(&buf))) return false;
    return SUCCEEDED(src_->Start(0));
}

void Audio::shutdown() {
    if (src_) { src_->Stop(); src_->DestroyVoice(); src_ = nullptr; }
    if (master_) { master_->DestroyVoice(); master_ = nullptr; }
    xa_.Reset();
    CoUninitialize();
}