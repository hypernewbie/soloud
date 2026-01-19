#include "utest.h"
#include "soloud.h"
#include "soloud_speech.h"
#include "soloud_sfxr.h"
#include "soloud_monotone.h"
#include "soloud_noise.h"
#include "soloud_bus.h"
#include "soloud_echofilter.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"
#include "soloud_lofifilter.h"
#include "soloud_tedsid.h"
#include "soloud_vic.h"
#include "soloud_vizsn.h"
#include "soloud_queue.h"
#include <thread>
#include <chrono>

UTEST_MAIN();

struct SoloudFixture {
    SoLoud::Soloud soloud;
};

UTEST_F_SETUP(SoloudFixture) {
    ASSERT_EQ(0, utest_fixture->soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::LEFT_HANDED_3D, SoLoud::Soloud::NULLDRIVER));
}

UTEST_F_TEARDOWN(SoloudFixture) {
    utest_fixture->soloud.deinit();
}

UTEST_F(SoloudFixture, CoreInit) {
    // Already handled in Setup, but checking state
    EXPECT_EQ(SoLoud::Soloud::NULLDRIVER, utest_fixture->soloud.getBackendId());
}

UTEST_F(SoloudFixture, SpeechSynthesis) {
    SoLoud::Speech speech;
    speech.setText("Hello world");
    SoLoud::handle handle = utest_fixture->soloud.play(speech);
    EXPECT_TRUE(handle != 0);
    EXPECT_TRUE(utest_fixture->soloud.isValidVoiceHandle(handle));
    
    // Simulate a few frames
    float buffer[256 * 2]; // Stereo
    utest_fixture->soloud.mix(buffer, 256);
    utest_fixture->soloud.stop(handle);
}

UTEST_F(SoloudFixture, SfxrPresets) {
    SoLoud::Sfxr sfxr;
    
    // Test Coin Preset
    sfxr.loadPreset(SoLoud::Sfxr::COIN, 0);
    SoLoud::handle h1 = utest_fixture->soloud.play(sfxr);
    EXPECT_TRUE(h1 != 0);
    utest_fixture->soloud.stop(h1);

    // Test Laser Preset
    sfxr.loadPreset(SoLoud::Sfxr::LASER, 0);
    SoLoud::handle h2 = utest_fixture->soloud.play(sfxr);
    EXPECT_TRUE(h2 != 0);
    utest_fixture->soloud.stop(h2);
    
    // Test Explosion Preset
    sfxr.loadPreset(SoLoud::Sfxr::EXPLOSION, 0);
    SoLoud::handle h3 = utest_fixture->soloud.play(sfxr);
    EXPECT_TRUE(h3 != 0);
    utest_fixture->soloud.stop(h3);
}

UTEST_F(SoloudFixture, MonotoneBasic) {
    SoLoud::Monotone monotone;
    monotone.setParams(1, SoLoud::Soloud::WAVE_SQUARE);
    SoLoud::handle h = utest_fixture->soloud.play(monotone);
    EXPECT_TRUE(h != 0);
    
    utest_fixture->soloud.setVolume(h, 0.5f);
    EXPECT_NEAR(0.5f, utest_fixture->soloud.getVolume(h), 0.001f);
    
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, NoiseGenerator) {
    SoLoud::Noise noise;
    SoLoud::handle h = utest_fixture->soloud.play(noise);
    EXPECT_TRUE(h != 0);
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, BusAndFilters) {
    SoLoud::Bus bus;
    SoLoud::EchoFilter echo;
    SoLoud::Noise noise;
    
    // Set up filter
    echo.setParams(0.2f, 0.5f, 0.1f);
    bus.setFilter(0, &echo);
    
    // Play bus
    SoLoud::handle busHandle = utest_fixture->soloud.play(bus);
    EXPECT_TRUE(busHandle != 0);
    
    // Play noise through bus
    SoLoud::handle noiseHandle = bus.play(noise);
    EXPECT_TRUE(noiseHandle != 0);
    
    // Tick
    float buffer[256 * 2];
    utest_fixture->soloud.mix(buffer, 256);
    
    utest_fixture->soloud.stop(busHandle);
}

// --- Asset Loading Tests ---

UTEST_F(SoloudFixture, WavFileLoading) {
    SoLoud::Wav wav;
    // Assuming running from build_ninja/ so path is ../audio/
    SoLoud::result res = wav.load("../audio/Pew_Pew-DKnight556-1379997159.wav");
    ASSERT_EQ(SoLoud::SO_NO_ERROR, res);
    EXPECT_GT(wav.getLength(), 0.0);
    
    SoLoud::handle h = utest_fixture->soloud.play(wav);
    EXPECT_TRUE(h != 0);
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256); // Mix a bit
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, Mp3FileLoading) {
    SoLoud::Wav wav;
    SoLoud::result res = wav.load("../audio/Pew_Pew-DKnight556-1379997159.mp3");
    ASSERT_EQ(SoLoud::SO_NO_ERROR, res);
    EXPECT_GT(wav.getLength(), 0.0);
    
    SoLoud::handle h = utest_fixture->soloud.play(wav);
    EXPECT_TRUE(h != 0);
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, WavStreaming) {
    SoLoud::WavStream stream;
    SoLoud::result res = stream.load("../audio/Pew_Pew-DKnight556-1379997159.wav");
    ASSERT_EQ(SoLoud::SO_NO_ERROR, res);
    EXPECT_GT(stream.getLength(), 0.0);
    
    SoLoud::handle h = utest_fixture->soloud.play(stream);
    EXPECT_TRUE(h != 0);
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256);
    utest_fixture->soloud.stop(h);
}

// --- Advanced Engine Features ---

UTEST_F(SoloudFixture, FadersAndOscillators) {
    SoLoud::Monotone tone;
    tone.setParams(1, SoLoud::Soloud::WAVE_SQUARE);
    SoLoud::handle h = utest_fixture->soloud.play(tone);
    
    utest_fixture->soloud.setVolume(h, 1.0f);
    utest_fixture->soloud.fadeVolume(h, 0.0f, 0.02f); // Fade to 0 in ~0.02s
    
    // Mix enough frames to cover the fade duration
    // 0.02s at 44100Hz is ~882 samples.
    // mix(256) multiple times.
    float buffer[512];
    for (int i = 0; i < 10; i++) {
        utest_fixture->soloud.mix(buffer, 256);
    }
    
    EXPECT_NEAR(0.0f, utest_fixture->soloud.getVolume(h), 0.05f); // Allow some margin
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, Audio3DLogic) {
    utest_fixture->soloud.update3dAudio(); // Init 3D math
    
    SoLoud::Monotone tone;
    tone.setParams(1, SoLoud::Soloud::WAVE_SQUARE);
    SoLoud::handle h = utest_fixture->soloud.play3d(tone, 10.0f, 0.0f, 0.0f);
    
    EXPECT_TRUE(h != 0);
    
    // Just verify the logic doesn't crash on update
    utest_fixture->soloud.set3dListenerParameters(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    utest_fixture->soloud.update3dAudio();
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256);
    
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, VoiceGroups) {
    SoLoud::handle group = utest_fixture->soloud.createVoiceGroup();
    SoLoud::Sfxr sfxr; 
    sfxr.loadPreset(SoLoud::Sfxr::COIN, 0);
    
    SoLoud::handle h1 = utest_fixture->soloud.play(sfxr);
    SoLoud::handle h2 = utest_fixture->soloud.play(sfxr);
    
    utest_fixture->soloud.addVoiceToGroup(group, h1);
    utest_fixture->soloud.addVoiceToGroup(group, h2);
    
    utest_fixture->soloud.stop(group);
    
    EXPECT_FALSE(utest_fixture->soloud.isValidVoiceHandle(h1));
    EXPECT_FALSE(utest_fixture->soloud.isValidVoiceHandle(h2));
    
    utest_fixture->soloud.destroyVoiceGroup(group);
}

UTEST_F(SoloudFixture, Queueing) {
    SoLoud::Queue queue;
    SoLoud::Sfxr sfxr;
    sfxr.loadPreset(SoLoud::Sfxr::COIN, 0);
    
    // Queue must be playing to accept sounds
    SoLoud::handle h = utest_fixture->soloud.play(queue);
    EXPECT_TRUE(h != 0);
    
    // Add same source twice
    queue.play(sfxr);
    queue.play(sfxr);
    
    EXPECT_EQ(2, queue.getQueueCount());
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256);
    
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, FilterLiveParameters) {
    SoLoud::LofiFilter lofi;
    SoLoud::Monotone tone;
    tone.setParams(1, SoLoud::Soloud::WAVE_SQUARE);
    
    // Set filter on the source, not the engine
    tone.setFilter(0, &lofi);
    
    SoLoud::handle h = utest_fixture->soloud.play(tone);
    
    // Set parameter
    utest_fixture->soloud.setFilterParameter(h, 0, SoLoud::LofiFilter::BITDEPTH, 4.0f);
    
    float val = utest_fixture->soloud.getFilterParameter(h, 0, SoLoud::LofiFilter::BITDEPTH);
    EXPECT_NEAR(4.0f, val, 0.01f);
    
    utest_fixture->soloud.stop(h);
}

// --- Additional Synthesizers ---

UTEST_F(SoloudFixture, TedSidBasic) {
    SoLoud::TedSid tedsid;
    SoLoud::handle h = utest_fixture->soloud.play(tedsid);
    EXPECT_TRUE(h != 0);
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256);
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, VicBasic) {
    SoLoud::Vic vic;
    vic.setModel(SoLoud::Vic::PAL);
    SoLoud::handle h = utest_fixture->soloud.play(vic);
    EXPECT_TRUE(h != 0);
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256);
    utest_fixture->soloud.stop(h);
}

UTEST_F(SoloudFixture, VizsnBasic) {
    SoLoud::Vizsn vizsn;
    vizsn.setText((char*)"ABC");
    SoLoud::handle h = utest_fixture->soloud.play(vizsn);
    EXPECT_TRUE(h != 0);
    
    float buffer[512];
    utest_fixture->soloud.mix(buffer, 256);
    utest_fixture->soloud.stop(h);
}