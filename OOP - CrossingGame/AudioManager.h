// AudioManager.h
#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <deque>
#include <string>

//==================================================
// AudioManager
//
// Quan ly tap trung nhac nen (Music) va hieu ung am
// thanh (SFX) cho toan bo game. Chi can 1 instance
// duy nhat, song suot vong doi chuong trinh (tao trong
// main(), truyen tham chieu vao noi can dung).
//==================================================

class AudioManager
{
public:
    AudioManager() = default;

    //----------------------------------
    // Background Music (sf::Music - streaming, dung cho file dai)
    //----------------------------------

    bool loadMusic(const std::string& path);

    void playMusic(bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void setMusicVolume(int volume); // 0-100
    int  getMusicVolume() const;

    //----------------------------------
    // Sound Effects (sf::Sound - nap san vao bo nho, dung cho file ngan)
    //----------------------------------

    bool loadSound(const std::string& name, const std::string& path);

    void playSound(const std::string& name);

    // ===== ADDED: ban "co dieu khien" cua SFX - dung khi can LOOP va/hoac
    // can STOP dung 1 instance cu the theo ten (vd tieng tau chay, tieng
    // gio thoi keo dai) - khac voi playSound() (ban fire-and-forget, moi
    // lan goi tao 1 instance moi trong activeSounds, khong loop duoc va
    // khong the stop rieng tung cai). Goi lai playControlledSound() voi
    // cung 1 name se restart dung instance do (khong tao them ban moi).
    void playControlledSound(const std::string& name, bool loop = false);
    void stopControlledSound(const std::string& name);

    void setSoundVolume(int volume); // 0-100
    int  getSoundVolume() const;

    //----------------------------------
    // Goi moi frame trong game loop de don cac sound da phat xong
    //----------------------------------

    void update();

private:

    //----------------------------------
    // Music
    //----------------------------------

    sf::Music music;
    int musicVolume = 100;

    //----------------------------------
    // Sound
    //----------------------------------
    // dung deque thay vi vector: deque khong bao gio "di doi" cac
    // phan tu da ton tai khi them phan tu moi (khac vector se realloc
    // va copy/destroy lam gian doan am thanh dang phat).

    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::deque<sf::Sound> activeSounds;

    // ===== ADDED: cac SFX "co dieu khien" (loop duoc/stop duoc rieng tung
    // cai), khoa theo ten - khac activeSounds (vo danh, fire-and-forget).
    // sf::Sound trong map khong bi "di doi" khi them entry moi (map giu
    // reference/iterator on hop le tru khi entry do bi erase), nen an toan
    // de AudioManager tu quan ly rieng.
    std::map<std::string, sf::Sound> controlledSounds;

    int soundVolume = 100;
};
