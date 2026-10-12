#include <ps4.h>
#include "types.h"
#include "natives.h"

// ==========================================
// 1. VARIABEL NAVIGASI & KONTROL MENU
// ==========================================
bool menu_opened = false;
int current_option = 0;
int max_options = 7; // Total baris pilihan di menu

// Variabel status fitur (Toggle)
bool toggled_money = false;
bool toggled_invisible = false;

// Variabel index untuk ganti pakaian manual (bisa digeser angkanya)
int baju_index = 0;
int celana_index = 0;
int sepatu_index = 0;

// ==========================================
// 2. FUNGSI GRAFIS (MENGGAMBAR KOTAK & TEKS)
// ==========================================
void DrawMenuText(const char* text, int font, float x, float y, float scaleX, float scaleY, int r, int g, int b, int a, bool center) {
    UI::SET_TEXT_FONT(font);
    UI::SET_TEXT_SCALE(scaleX, scaleY);
    UI::SET_TEXT_COLOUR(r, g, b, a);
    UI::SET_TEXT_WRAP(0.0f, 1.0f);
    UI::SET_TEXT_CENTRE(center);
    UI::_SET_TEXT_ENTRY("STRING");
    UI::_ADD_TEXT_COMPONENT_STRING(text);
    UI::_DRAW_TEXT(x, y);
}

void DrawMenuRect(float x, float y, float width, float height, int r, int g, int b, int a) {
    GRAPHICS::DRAW_RECT(x, y, width, height, r, g, b, a);
}

// ==========================================
// 3. FUNGSI FITUR UTAMA (GTA ONLINE v1.27)
// ==========================================
void DropMoneyOnline(Player targetPlayer) {
    Ped targetPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(targetPlayer);
    if (ENTITY::DOES_ENTITY_EXIST(targetPed) && !ENTITY::IS_ENTITY_DEAD(targetPed)) {
        Vector3 coords = ENTITY::GET_ENTITY_COORDS(targetPed, TRUE);
        Hash cashHash = GAMEPLAY::GET_HASH_KEY("PICKUP_MONEY_CASE"); 
        STREAMING::REQUEST_MODEL(cashHash);
        if (STREAMING::HAS_MODEL_LOADED(cashHash)) {
            OBJECT::CREATE_AMBIENT_PICKUP(cashHash, coords.x, coords.y, coords.z + 2.0f, 0, 2500, cashHash, FALSE, TRUE);
            STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(cashHash);
        }
    }
}

void TeleportToPlayer(Player targetPlayer) {
    Ped myPed = PLAYER::PLAYER_PED_ID();
    Ped targetPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(targetPlayer);
    if (ENTITY::DOES_ENTITY_EXIST(targetPed)) {
        Vector3 targetCoords = ENTITY::GET_ENTITY_COORDS(targetPed, TRUE);
        if (PED::IS_PED_IN_ANY_VEHICLE(myPed, FALSE)) {
            Vehicle myVehicle = PED::GET_VEHICLE_PED_IS_IN(myPed, FALSE);
            ENTITY::SET_ENTITY_COORDS(myVehicle, targetCoords.x, targetCoords.y, targetCoords.z + 1.0f, 0, 0, 0, TRUE);
        } else {
            ENTITY::SET_ENTITY_COORDS(myPed, targetCoords.x, targetCoords.y, targetCoords.z + 1.0f, 0, 0, 0, TRUE);
        }
    }
}

// Mengatur komponen pakaian berdasarkan ID Komponen GTA V
void ChangeOutfitComponent(int componentId, int drawableId) {
    Ped myPed = PLAYER::PLAYER_PED_ID();
    if (ENTITY::DOES_ENTITY_EXIST(myPed) && !ENTITY::IS_ENTITY_DEAD(myPed)) {
        PED::SET_PED_COMPONENT_VARIATION(myPed, componentId, drawableId, 0, 2);
    }
}

void MakeBodyInvisible(bool toggle) {
    Ped myPed = PLAYER::PLAYER_PED_ID();
    if (toggle) {
        PED::SET_PED_COMPONENT_VARIATION(myPed, 3, 15, 0, 2);  // Tangan hilang
        PED::SET_PED_COMPONENT_VARIATION(myPed, 11, 15, 0, 2); // Dada/Baju hilang
        PED::SET_PED_COMPONENT_VARIATION(myPed, 4, 15, 0, 2);  // Kaki/Celana hilang
    } else {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(myPed);
    }
}

void ApplyTrashmanPreset() {
    Ped myPed = PLAYER::PLAYER_PED_ID();
    PED::SET_PED_COMPONENT_VARIATION(myPed, 11, 44, 0, 2); // Baju Petugas Sampah
    PED::SET_PED_COMPONENT_VARIATION(myPed, 4, 36, 0, 2);  // Celana Petugas Sampah
    PED::SET_PED_COMPONENT_VARIATION(myPed, 6, 24, 0, 2);  // Sepatu Petugas Sampah
}

// ==========================================
// 4. LOGIKA UTAMA & TAMPILAN MENU (LOOP)
// ==========================================
void Menu_Logic_Loop() {
    // Tombol Buka/Tutup Menu: KOTAK + D-PAD KANAN
    if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, CONTROL_FRONTEND_X) && 
        CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, CONTROL_FRONTEND_RIGHT)) {
        menu_opened = !menu_opened;
    }

    // Fitur pasif yang berjalan otomatis di background
    if (toggled_money) {
        DropMoneyOnline(0); // Drop ke diri sendiri (ID 0)
        SYSTEM::WAIT(100);
    }

    if (!menu_opened) return;

    // ------------------------------------------
    // GAMBAR VISUAL MENU (STRUKTUR UI XANZY)
    // ------------------------------------------
    // Header Menu (Kotak Atas)
    DrawMenuRect(0.15f, 0.18f, 0.22f, 0.06f, 200, 50, 50, 255); // Warna Merah Khas
    DrawMenuText("XANZY MENU v1.27", 0, 0.15f, 0.16f, 0.45f, 0.45f, 255, 255, 255, 255, true);

    // Background Menu (Kotak Isi)
    DrawMenuRect(0.15f, 0.42f, 0.22f, 0.42f, 0, 0, 0, 200); // Hitam Transparan

    // Teks Pilihan Menu (Daftar Opsi)
    DrawMenuText(toggled_money ? "1. Money Drop: ~g~ON" : "1. Money Drop: ~r~OFF", 0, 0.05f, 0.24f, 0.35f, 0.35f, 255, 255, 255, 255, false);
    DrawMenuText("2. Teleport to Player", 0, 0.05f, 0.28f, 0.35f, 0.35f, 255, 255, 255, 255, false);
    DrawMenuText(toggled_invisible ? "3. Invisible Body: ~g~ON" : "3. Invisible Body: ~r~OFF", 0, 0.05f, 0.32f, 0.35f, 0.35f, 255, 255, 255, 255, false);
    
    // Teks Opsi Pakaian Manual (Menampilkan nomor baju/celana)
    char baju_txt[50], celana_txt[50], sepatu_txt[50];
    GRAPHICS::MINI_SUB_STRING_CELL_SET_FORMAT_X("4. Ganti Baju: < %d >", baju_index); // v1.27 text formatting
    DrawMenuText(baju_txt, 0, 0.05f, 0.36f, 0.35f, 0.35f, 255, 255, 255, 255, false);
    
    GRAPHICS::MINI_SUB_STRING_CELL_SET_FORMAT_X("5. Ganti Celana: < %d >", celana_index);
    DrawMenuText(celana_txt, 0, 0.05f, 0.40f, 0.35f, 0.35f, 255, 255, 255, 255, false);
    
    GRAPHICS::MINI_SUB_STRING_CELL_SET_FORMAT_X("6. Ganti Sepatu: < %d >", sepatu_index);
    DrawMenuText(sepatu_txt, 0, 0.05f, 0.44f, 0.35f, 0.35f, 255, 255, 255, 255, false);

    DrawMenuText("7. Preset: Trashman Outfit", 0, 0.05f, 0.48f, 0.35f, 0.35f, 255, 255, 255, 255, false);

    // Batang Sorotan (Scroller Pilihan)
    DrawMenuRect(0.15f, 0.255f + (current_option * 0.04f), 0.22f, 0.035f, 255, 255, 255, 80);

    // ------------------------------------------
    // NAVIGASI TOMBOL (D-PAD)
    // ------------------------------------------
    if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, CONTROL_FRONTEND_UP)) {
        current_option--;
        if (current_option < 0) current_option = max_options - 1;
    }
    if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, CONTROL_FRONTEND_DOWN)) {
        current_option++;
        if (current_option >= max_options) current_option = 0;
    }

    // Tombol Kiri/Kanan D-Pad untuk mengubah angka indeks pakaian
    if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, CONTROL_FRONTEND_LEFT)) {
        if (current_option == 3 && baju_index > 0) { baju_index--; ChangeOutfitComponent(11, baju_index); }
        if (current_option == 4 && celana_index > 0) { celana_index--; ChangeOutfitComponent(4, celana_index); }
        if (current_option == 5 && sepatu_index > 0) { sepatu_index--; ChangeOutfitComponent(6, sepatu_index); }
    }
    if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, CONTROL_FRONTEND_RIGHT)) {
        if (current_option == 3) { baju_index++; ChangeOutfitComponent(11, baju_index); }
        if (current_option == 4) { celana_index++; ChangeOutfitComponent(4, celana_index); }
        if (current_option == 5) { sepatu_index++; ChangeOutfitComponent(6, sepatu_index); }
    }

    // Tombol Konfirmasi (Tombol X / Silang)
    if (CONTROLS::IS_DISABLED_CONTROL_JUST_PRESSED(0, CONTROL_FRONTEND_ACCEPT)) {
        switch (current_option) {
            case 0: toggled_money = !toggled_money; break;
            case 1: TeleportToPlayer(0); break;
            case 2: 
                toggled_invisible = !toggled_invisible; 
                MakeBodyInvisible(toggled_invisible); 
                break;
            case 6: ApplyTrashmanPreset(); break;
        }
    }
}
