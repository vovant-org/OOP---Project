// ModManager.cpp
#include "ModManager.h"

#include <Windows.h>
#include <Commdlg.h>
#pragma comment(lib, "Comdlg32.lib")

#include <fstream>
#include <algorithm>

namespace
{
    // Copy file don gian bang fstream (khong dung <filesystem> de tranh
    // phu thuoc them - du dung cho 1 file anh).
    bool CopyFileBinary(const std::string& from, const std::string& to)
    {
        std::ifstream src(from, std::ios::binary);
        if (!src) return false;

        std::ofstream dst(to, std::ios::binary);
        if (!dst) return false;

        dst << src.rdbuf();
        return true;
    }

    bool FileExists(const std::string& path)
    {
        std::ifstream f(path);
        return f.good();
    }
}

std::string ModManager::ComputeBackupPath(const std::string& targetPathIn)
{
    // vd "Character/Chicken_character.png" -> "Character/Chicken_character_original.png"
    std::string base = targetPathIn;
    std::string ext;

    std::size_t dot = base.find_last_of('.');
    if (dot != std::string::npos)
    {
        ext = base.substr(dot);
        base = base.substr(0, dot);
    }

    return base + "_original" + ext;
}

ModManager::ModManager(const std::string& targetPathIn)
    : targetPath(targetPathIn)
{
    backupPath = ComputeBackupPath(targetPath);
}

// ===== ADDED: chuyen ModManager sang thao tac tren 1 nhan vat khac -
// dung khi nguoi choi bam < > trong ModMenu de chon nhan vat can doi
// skin. Backup rieng cho tung nhan vat nen khong bi lan sang nhau =====
void ModManager::SetTarget(const std::string& targetPathIn)
{
    targetPath = targetPathIn;
    backupPath = ComputeBackupPath(targetPath);
}

//==================================================
// Open dialog (Windows Common Dialog)
//==================================================

std::string ModManager::OpenImageDialog()
{
    char fileBuffer[MAX_PATH] = "";

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter =
        "Image Files (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0"
        "All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Chon anh de lam skin (MOD)";

    // OFN_NOCHANGEDIR: QUAN TRONG - toan bo game load texture bang
    // duong dan TUONG DOI ("Character/...", "Map/...", "ui/..."). Neu
    // khong co co nay, chon 1 file o thu muc khac se doi luon current
    // working directory cua CA CHUONG TRINH, khien moi lan load
    // texture sau do bi loi (khong tim thay file).
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn))
        return std::string(fileBuffer);

    return "";
}

//==================================================
// Resize / Fit helpers
//==================================================

sf::Image ModManager::ResizeImage(const sf::Image& src, unsigned int newW, unsigned int newH)
{
    sf::Image dst;
    dst.create(newW, newH, sf::Color::Transparent);

    sf::Vector2u sz = src.getSize();
    if (sz.x == 0 || sz.y == 0 || newW == 0 || newH == 0)
        return dst;

    for (unsigned int y = 0; y < newH; y++)
    {
        unsigned int sy = std::min(sz.y - 1, (unsigned int)((float)y * sz.y / newH));

        for (unsigned int x = 0; x < newW; x++)
        {
            unsigned int sx = std::min(sz.x - 1, (unsigned int)((float)x * sz.x / newW));
            dst.setPixel(x, y, src.getPixel(sx, sy));
        }
    }

    return dst;
}

sf::Image ModManager::FitToFrame(const sf::Image& src, unsigned int frameW, unsigned int frameH)
{
    sf::Image tile;
    tile.create(frameW, frameH, sf::Color::Transparent);

    sf::Vector2u sz = src.getSize();
    if (sz.x == 0 || sz.y == 0)
        return tile;

    // Giu nguyen ti le anh goc, chi thu/phong cho VUA khop trong 1 o
    // frame - tranh bi meo hinh.
    float scale = std::min((float)frameW / sz.x, (float)frameH / sz.y);
    unsigned int tw = std::max(1u, (unsigned int)(sz.x * scale));
    unsigned int th = std::max(1u, (unsigned int)(sz.y * scale));

    sf::Image resized = ResizeImage(src, tw, th);

    tile.copy(resized, (frameW - tw) / 2, (frameH - th) / 2,
        sf::IntRect(0, 0, tw, th), true);

    return tile;
}

//==================================================
// Apply / Reset
//==================================================

bool ModManager::ApplySkin(const std::string& sourceImagePath, std::string& outMessage)
{
    // 1) Backup ban goc - CHI 1 LAN DAU TIEN (tranh viec Apply nhieu
    // lan lien tiep lai backup nham de anh mod cu thay vi anh Chicken
    // that su ban dau).
    if (!FileExists(backupPath))
    {
        if (!FileExists(targetPath) || !CopyFileBinary(targetPath, backupPath))
        {
            outMessage = "Loi: khong backup duoc anh goc, da huy import.";
            return false;
        }
    }

    // 2) Doc kich thuoc spritesheet GOC (tu file backup) de biet
    // frameW/frameH can dung.
    sf::Image original;
    if (!original.loadFromFile(backupPath))
    {
        outMessage = "Loi: khong doc duoc anh goc de lay kich thuoc.";
        return false;
    }

    unsigned int sheetW = original.getSize().x;
    unsigned int sheetH = original.getSize().y;
    unsigned int frameW = sheetW / SHEET_COLS;
    unsigned int frameH = sheetH / SHEET_ROWS;

    if (frameW == 0 || frameH == 0)
    {
        outMessage = "Loi: kich thuoc anh goc khong hop le.";
        return false;
    }

    // 3) Doc anh nguoi choi vua chon
    sf::Image picked;
    if (!picked.loadFromFile(sourceImagePath))
    {
        outMessage = "Loi: khong doc duoc file anh da chon.";
        return false;
    }

    // 4) Ghep anh do vao 1 spritesheet moi (tile vao ca 20 o - don
    // gian, khong co animation, chi doi "skin")
    sf::Image tile = FitToFrame(picked, frameW, frameH);

    sf::Image sheet;
    sheet.create(sheetW, sheetH, sf::Color::Transparent);

    for (unsigned int row = 0; row < SHEET_ROWS; row++)
    {
        for (unsigned int col = 0; col < SHEET_COLS; col++)
        {
            sheet.copy(tile, col * frameW, row * frameH,
                sf::IntRect(0, 0, frameW, frameH), true);
        }
    }

    // 5) Ghi de len file game dang doc - CharacterSelection,
    // LeaderboardMenu va CGAME deu se thay skin moi tu lan load ke tiep.
    if (!sheet.saveToFile(targetPath))
    {
        outMessage = "Loi: khong ghi duoc file skin moi.";
        return false;
    }

    outMessage = "Da doi skin thanh cong!";
    return true;
}

bool ModManager::ResetSkin(std::string& outMessage)
{
    if (!FileExists(backupPath))
    {
        outMessage = "Chua co ban goc de khoi phuc (chua Import lan nao).";
        return false;
    }

    if (!CopyFileBinary(backupPath, targetPath))
    {
        outMessage = "Loi: khong khoi phuc duoc anh goc.";
        return false;
    }

    outMessage = "Da khoi phuc skin goc.";
    return true;
}
