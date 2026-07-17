#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

int main()
{
    RenderWindow window(VideoMode(1920, 1080), "Crossing Game");
    window.setFramerateLimit(60);

    //----------------------------------
    // MAP
    //----------------------------------
    Texture mapTexture;

    if (!mapTexture.loadFromFile("D:/OOP_Project/OOP - CrossingGame/Map/City_map.png"))
    {
        cout << "Khong load duoc map\n";
        return -1;
    }

    Sprite map(mapTexture);

    map.setScale(
        1920.f / mapTexture.getSize().x,
        1080.f / mapTexture.getSize().y
    );

    //----------------------------------
    // CHICKEN
    //----------------------------------
    // QUAN TRONG: dung ban Chicken_character.png da CHUAN HOA (1040 x 1100),
    // luoi 4 cot x 5 hang DEU NHAU (moi o 260 x 220px), khong con dung
    // bang toa do rieng nua. Neu ban dang dung file Chicken_character.png
    // BAN GOC (1254x1254, chua chuan hoa) thi PHAI dung lai bang chickenFrames
    // cua lan truoc, KHONG duoc tron lan 2 kieu voi nhau - do chinh la
    // nguyen nhan gay ra vet do lem duoi chan ga trong anh ban gui.
    Texture chickenTexture;

    if (!chickenTexture.loadFromFile("D:/OOP_Project/OOP - CrossingGame/Character/Sky_character.png"))
    {
        cout << "Khong load duoc chicken\n";
        return -1;
    }

    chickenTexture.setSmooth(false);

    cout << "Texture = "
        << chickenTexture.getSize().x
        << " x "
        << chickenTexture.getSize().y
        << endl;

    // Anh chuan hoa co luoi DEU: 4 cot x 5 hang
    // -> chia deu la chinh xac, khong bi lech o nhu ban goc truoc day
    int frameWidth = chickenTexture.getSize().x / 4;   // = 260
    int frameHeight = chickenTexture.getSize().y / 5;  // = 220

    Sprite chicken(chickenTexture);

    int frame = 0;
    int direction = 1; // 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT, 4=DIE/DIZZY

    chicken.setTextureRect(IntRect(
        frame * frameWidth,
        direction * frameHeight,
        frameWidth,
        frameHeight
    ));

    // Anh da duoc can giua nhan vat trong tung o san, nen origin
    // = tam o se luon trung voi tam nhan vat, khong bi lech nua.
    chicken.setOrigin(frameWidth / 2.f, frameHeight / 2.f);

    chicken.setScale(0.5f, 0.5f);

    //----------------------------------
    // TINH VI TRI PHAN DAT TRONG (CO) TREN MAP
    //----------------------------------
    float grassStartYOriginal = 855.f;
    float grassStartOnScreen = grassStartYOriginal * map.getScale().y;
    float windowBottom = 1080.f;

    float chickenY = (grassStartOnScreen + windowBottom) / 2.f;
    float chickenX = 960.f;

    chicken.setPosition(chickenX, chickenY);

    //----------------------------------
    while (window.isOpen())
    {
        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();

        window.draw(map);
        window.draw(chicken);

        window.display();
    }

    return 0;
}