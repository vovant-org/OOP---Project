// AudioManager.cpp
#include "AudioManager.h"

#include <algorithm>
#include <iostream>

//==================================================
// Background Music
//==================================================

bool AudioManager::loadMusic(const std::string& path)
{
    if (!music.openFromFile(path))
    {
        std::cout << "[ERROR] AudioManager: cannot load music: "
            << path << "\n";
        return false;
    }

    music.setVolume(static_cast<float>(musicVolume));

    return true;
}

void AudioManager::playMusic(bool loop)
{
    music.setLoop(loop);
    music.play();
}

void AudioManager::stopMusic()
{
    music.stop();
}

void AudioManager::pauseMusic()
{
    music.pause();
}

void AudioManager::resumeMusic()
{
    music.play();
}

void AudioManager::setMusicVolume(int volume)
{
    if (volume < 0)
        volume = 0;

    if (volume > 100)
        volume = 100;

    musicVolume = volume;

    music.setVolume(static_cast<float>(musicVolume));
}

int AudioManager::getMusicVolume() const
{
    return musicVolume;
}

//==================================================
// Sound Effects
//==================================================

bool AudioManager::loadSound(const std::string& name, const std::string& path)
{
    sf::SoundBuffer buffer;

    if (!buffer.loadFromFile(path))
    {
        std::cout << "[ERROR] AudioManager: cannot load sound: "
            << path << " (" << name << ")\n";
        return false;
    }

    // map::operator[] tao/ghi de entry; sf::Sound sau nay se tro toi
    // buffer nam trong map nay, nen map phai song lau hon moi sf::Sound
    // dang tham chieu toi no (AudioManager la member song suot game
    // nen dieu nay luon dung).
    soundBuffers[name] = buffer;

    return true;
}

void AudioManager::playSound(const std::string& name)
{
    auto it = soundBuffers.find(name);

    if (it == soundBuffers.end())
    {
        std::cout << "[ERROR] AudioManager: sound not loaded: "
            << name << "\n";
        return;
    }

    activeSounds.emplace_back();

    sf::Sound& s = activeSounds.back();
    s.setBuffer(it->second);
    s.setVolume(static_cast<float>(soundVolume));
    s.play();
}

// ===== ADDED: SFX "co dieu khien" - dung 1 instance sf::Sound duy nhat
// cho moi `name` (tao luc goi lan dau, tai su dung cac lan sau), cho phep
// loop va stop rieng - hop voi tieng gio/tieng tau chay dai/lien tuc =====
void AudioManager::playControlledSound(const std::string& name, bool loop)
{
    auto it = soundBuffers.find(name);

    if (it == soundBuffers.end())
    {
        std::cout << "[ERROR] AudioManager: sound not loaded: "
            << name << "\n";
        return;
    }

    // map::operator[] tao entry moi (mac dinh) neu chua co, hoac tra ve
    // entry cu de restart dung instance do
    sf::Sound& s = controlledSounds[name];
    s.setBuffer(it->second);
    s.setLoop(loop);
    s.setVolume(static_cast<float>(soundVolume));
    s.play();
}

void AudioManager::stopControlledSound(const std::string& name)
{
    auto it = controlledSounds.find(name);

    if (it != controlledSounds.end())
        it->second.stop();
}

void AudioManager::setSoundVolume(int volume)
{
    if (volume < 0)
        volume = 0;

    if (volume > 100)
        volume = 100;

    soundVolume = volume;

    // Cap nhat tuc thi cho ca cac sound dang phat (neu co)
    for (auto& s : activeSounds)
        s.setVolume(static_cast<float>(soundVolume));

    // ===== ADDED: ap dung ca cho cac SFX "co dieu khien" (vd tieng gio/
    // tau dang loop) - de keo thanh truot Sound trong SettingMenu tang/
    // giam duoc NGAY LAP TUC ca voi am thanh dang phat, khong can doi den
    // lan play() ke tiep =====
    for (auto& kv : controlledSounds)
        kv.second.setVolume(static_cast<float>(soundVolume));
}

int AudioManager::getSoundVolume() const
{
    return soundVolume;
}

//==================================================
// Update
//==================================================

void AudioManager::update()
{
    // Don cac sound da phat xong de deque khong phinh to vo han
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](const sf::Sound& s)
            {
                return s.getStatus() == sf::Sound::Stopped;
            }),
        activeSounds.end());
}