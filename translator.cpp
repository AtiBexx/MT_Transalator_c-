#include <ctranslate2/translator.h>
#include "sentencepiece_processor.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Globális beállítások
bool saveToFile = false;
bool singleLineOutput = false;
string outputFile = "translations.txt";

// Globális SentencePiece processzorok
sentencepiece::SentencePieceProcessor sp_en;
sentencepiece::SentencePieceProcessor sp_hu;

// Képernyő törlése
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}
// Fordító függvény
void translateLoop(ctranslate2::Translator& translator, sentencepiece::SentencePieceProcessor& sp_source, sentencepiece::SentencePieceProcessor& sp_target) {

    cout << "\n--- FORDITO MOD ---\n";
    cout << "Irj be egy mondatot es nyomj Entert.\n";
    cout << "Parancsok: /back (vissza), /cls (kepernyo torles)\n\n";

    string line;
    while (true) {
        if (!singleLineOutput) cout << "> ";

        getline(cin, line);

        // Parancsok kezelése
        if (line == "/back") {
            clearScreen();
            break; // Egyszerűen kilépünk a ciklusból
        }
        if (line == "/cls") {
            clearScreen();
            cout << "\n--- FORDITO MOD ---\n";
            cout << "Irj be egy mondatot es nyomj Entert.\n";
            cout << "Parancsok: /back (vissza), /cls (kepernyo torles)\n\n";
            continue;
        }

        if (line.empty()) continue;

        // 1. Tokenizálás
        vector<string> tokens;
        auto status = sp_source.Encode(line, &tokens);

        if (!status.ok()) continue;
        // it is important!
        tokens.emplace_back("</s>");

        if (tokens.empty()) continue;

        // 2. Fordítás
        vector<vector<string>> input = {tokens};

        try {
            ctranslate2::TranslationOptions options;
            options.beam_size = 2;
            options.max_decoding_length = 200;
            options.repetition_penalty = 1.2;

            auto results = translator.translate_batch(input, options);

            // 3. Detokenizálás
            string out;
            sp_target.Decode(results[0].output(), &out);

            // Kiírás
            if (singleLineOutput) {
                cout << "\033[1A\033[2K\r" << line << " -> " << out << endl;
            } else {
                cout << line << " -> " << out << endl;
            }

            if (saveToFile) {
                ofstream f(outputFile, ios::app);
                if (f.is_open()) {
                    f << line << " -> " << out << "\n";
                    f.close();
                }
            }
        } catch (const std::exception& e) {
            cerr << "Hiba: " << e.what() << endl;
        }
    }
}

void loadSettings() {
    ifstream f("config.ini");
    if (f.is_open()) {
        string line;
        while (getline(f, line)) {
            if (line.find("saveToFile=1") != string::npos) saveToFile = true;
            else if (line.find("saveToFile=0") != string::npos) saveToFile = false;

            if (line.find("singleLineOutput=1") != string::npos) singleLineOutput = true;
            else if (line.find("singleLineOutput=0") != string::npos) singleLineOutput = false;
        }
        f.close();
    }
}

void saveSettings() {
    ofstream f("config.ini");
    if (f.is_open()) {
        f << "saveToFile=" << (saveToFile ? 1 : 0) << endl;
        f << "singleLineOutput=" << (singleLineOutput ? 1 : 0) << endl;
        f.close();
    }
}

void settingsMenu() {

    while (true) {
        cout << "\n--- BEALLITASOK ---\n";
        cout << "1. Fajlba mentes: " << (saveToFile ? "BE" : "KI") << endl;
        cout << "2. Egy soros kiiras (CMD-hez): " << (singleLineOutput ? "BE" : "KI") << endl;
        cout << "3. Vissza a fomenube\n";
        cout << "Valasztas: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            clearScreen();
            saveToFile = !saveToFile;
            saveSettings(); // Mentjük a beállításokat
        } else if (choice == "2") {
            clearScreen();
            singleLineOutput = !singleLineOutput;
            saveSettings(); // Mentjük a beállításokat
        } else if (choice == "3") {
            clearScreen();
            break;
        }
    }
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
    system("reg add HKCU\\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f > nul 2>&1");
#endif

    //a Beállítasok betöltése
    loadSettings();

    // Modellek betöltése
    string en_spm_path = R"(en-hu-f32\source.spm)";
    string hu_spm_path = R"(hu-en-f32\source.spm)";

    cout << "Modellek betoltese..." << endl;

    ifstream f_en(en_spm_path);
    if (!f_en.good()) {
        cerr << "HIBA: Nem talalhato a SentencePiece modellfajl: " << en_spm_path << endl;
        return 1;
    }

    const auto status_en = sp_en.Load(en_spm_path);
    if (!status_en.ok()) {
        cerr << "Hiba az angol SP modell betoltesekor." << endl;
        return 1;
    }

    ifstream f_hu(hu_spm_path);
    if (f_hu.good()) {
        sp_hu.Load(hu_spm_path);
    } else {
        sp_hu.Load(en_spm_path);
    }
    {
        ctranslate2::models::ModelLoader enhu_loader(R"(en-hu-f32)");
        ctranslate2::models::ModelLoader huen_loader(R"(hu-en-f32)");

        ctranslate2::Translator enhu(enhu_loader);
        ctranslate2::Translator huen(huen_loader);

        while (true) {
            cout << "\n--- FOMENU ---\n";
            cout << "1. EN -> HU\n";
            cout << "2. HU -> EN\n";
            cout << "3. Beallitasok\n";
            cout << "4. Kilepes\n";
            cout << "Valasztas: ";

            string choice;
            getline(cin, choice);

            if (choice == "1") {
                clearScreen();
                translateLoop(enhu, sp_en, sp_hu);
            }
            else if (choice == "2") {
                clearScreen();
                translateLoop(huen, sp_hu, sp_en);
            }
            else if (choice == "3") {
                clearScreen();
                settingsMenu();
            }
            else if (choice == "4") {
                cout << "Kilepes...\n";
                //cin.ignore(numeric_limits<streamsize>::max(), '\n');
                //break;
                exit(0);
            }
        }
    }
}