#include "utest.h"
#include "soloud.h"
#include "soloud_speech.h"
#include "soloud_sfxr.h"
#include "soloud_monotone.h"
#include "soloud_noise.h"
#include "soloud_bus.h"
#include "soloud_echofilter.h"
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