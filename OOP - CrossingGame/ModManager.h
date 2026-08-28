// ModManager.h
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// ==================================================
// ModManager
//
// "Mod / Outsource" don gian (tinh nang KHONG bat buoc, giang vien cho
// phep them): cho nguoi choi tu chon 1 file anh tu may (PNG/JPG/BMP...)
// de thay the skin nhan vat - CO THE chon bat ky nhan vat nao trong 4
// nhan vat (Chicken/Knight/Dog/Luffy), xem SetTarget().
//
// Cach hoat dong:
//  - File dang duoc game su dung, vd "Character/Chicken_character.png",
//    la 1 spritesheet 4 cot x 5 hang (moi o la 1 frame animation).
//  - Lan Apply DAU TIEN cho MOI nhan vat se backup file goc sang
//    "Character/<Ten>_character_original.png" (chi backup 1 lan duy
//    nhat MOI nhan vat, cac lan Apply sau khong ghi de len backup nay
//    nua). Moi nhan vat co file backup RIENG.
//  - Anh nguoi choi chon duoc co dan/can giua vao TUNG o cua 1
//    spritesheet moi CUNG kich thuoc voi file goc, roi ghi de len file
//    dang dung. Vi CharacterSelection, LeaderboardMenu va CGAME deu doc
//    LAI file nay tu dia (theo duong dan tuong doi), ca 3 cho se tu
//    dong hien thi skin moi ma KHONG can sua gi them trong code cua
//    chung.
//  - He qua: neu anh nguoi choi chon KHONG phai la spritesheet 4x5
//    chuan (vd 1 tam anh tinh binh thuong), nhan vat se mat animation
//    (dung yen 1 dang) nhung van hien dung skin da chon. Day la danh
//    doi chap nhan duoc de giu tinh nang "mod" don gian dung yeu cau.
//    (CPEOPLE con TU DONG nhan dien truong hop nay va lat ngang skin
//    theo huong di chuyen trai/phai - xem CPEOPLE::loadTexture)
// ==================================================
class ModManager
{
public:

    // targetPath: duong dan spritesheet dang duoc game su dung, vd
    // "Character/Chicken_character.png"
    explicit ModManager(const std::string& targetPath);

    // ===== ADDED: doi nhan vat dang duoc MOD (vd tu Chicken sang Knight)
    // - cap nhat lai targetPath + backupPath (backup rieng cho TUNG nhan
    // vat, khong dung chung 1 file backup). Goi khi nguoi choi bam < >
    // trong ModMenu de chuyen nhan vat can doi skin =====
    void SetTarget(const std::string& targetPath);
    const std::string& GetTargetPath() const { return targetPath; }

    // Mo hop thoai "Open File" cua Windows de nguoi choi chon anh tu
    // may. Tra ve "" neu ho bam Cancel.
    static std::string OpenImageDialog();

    // Ap dung 1 anh moi lam skin (ghi de targetPath).
    // outMessage: mo ta ket qua, dung de hien thi len ModMenu.
    bool ApplySkin(const std::string& sourceImagePath, std::string& outMessage);

    // Khoi phuc lai anh GOC (tu file backup), neu co.
    bool ResetSkin(std::string& outMessage);

private:

    std::string targetPath;
    std::string backupPath;

    // ===== ADDED: tinh duong dan backup ("..._original.ext") tu 1
    // targetPath - dung chung cho constructor va SetTarget() =====
    static std::string ComputeBackupPath(const std::string& targetPath);

    static const unsigned int SHEET_COLS = 4;
    static const unsigned int SHEET_ROWS = 5;

    // Resize anh src ve dung newW x newH (nearest-neighbor - du dung
    // cho 1 tinh nang mod don gian, khong can toi uu chat luong).
    static sf::Image ResizeImage(const sf::Image& src, unsigned int newW, unsigned int newH);

    // Dua anh src vao giua 1 "o" kich thuoc frameW x frameH, giu nguyen
    // ti le (khong bi meo), phan con lai de trong suot.
    static sf::Image FitToFrame(const sf::Image& src, unsigned int frameW, unsigned int frameH);
};
