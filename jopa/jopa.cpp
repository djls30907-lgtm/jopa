#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#include <raylib.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include "json.hpp" // Подключаем библиотеку JSON

using json = nlohmann::json;

enum GameState { MENU, PLAYING, SETTINGS, EXIT, SLOT_SELECT };

std::vector<std::string> inventory;
int currentSlot = 1; // По умолчанию первый слот

struct Choice {
    std::string text;
    int nextScene;
};

struct Scene {
    std::string StoryText;
    std::vector<Choice> choices;
    std::string backgroundPath;
    std::string musicPath;
    int nextSceneDefault; // <-- Добавь это поле
};
std::vector<Scene> scenes; // Теперь это не фиксированный массив, а гибкий вектор!

// Функция загрузки сцен из JSON
void LoadScenesFromJSON() {
    std::ifstream file("resources/scenes.json");
    if (!file.is_open()) {
        std::cout << "ОШИБКА: Файл не найден!" << std::endl;
        return;
    }

    json j;
    file >> j;
    scenes.clear();

    for (const auto& item : j) {
        Scene s;
        s.StoryText = item.value("StoryText", "");
        s.backgroundPath = item.value("backgroundPath", "");
        s.musicPath = item.value("musicPath", "");

        // Читаем переход по умолчанию (если выборов нет)
        // Если в JSON этого поля нет, запишем -1
        s.nextSceneDefault = item.value("nextScene", -1);

        if (item.contains("choices") && item["choices"].is_array()) {
            for (const auto& c : item["choices"]) {
                s.choices.push_back({
                    c.value("text", ""),
                    c.value("next", -1)
                    });
            }
        }
        scenes.push_back(s);
    }
}

int currentScene = 0;
int selectedOption = 0;

void DrawTextGlow(Font font, const char* text, Vector2 pos, float fontSize, Color textColor, Color glowColor, int glowSize)
{
    float spacing = 2.0f;
    for (int x = -glowSize; x <= glowSize; x++) {
        for (int y = -glowSize; y <= glowSize; y++) {
            if (x != 0 || y != 0)
                DrawTextEx(font, text, { pos.x + x, pos.y + y }, fontSize, spacing, glowColor);
        }
    }
    DrawTextEx(font, text, pos, fontSize, spacing, textColor);
}

Texture2D currentBackground; // Глобальная переменная для текущего фона
Music currentMusic;             // Глобальная переменная музыки
std::string currentMusicPath = ""; // Запоминаем, что сейчас играет
//<<<<<<<<функция перехода в меню
void SwitchToMenu(GameState& state) {
    state = MENU;
    std::string menuMusicPath = "resources/music.mp3"; // Путь к музыке твоего меню

    if (currentMusicPath != menuMusicPath) {
        if (currentMusicPath != "") UnloadMusicStream(currentMusic);
        currentMusic = LoadMusicStream(menuMusicPath.c_str());
        PlayMusicStream(currentMusic);
        currentMusicPath = menuMusicPath;
    }
}


///функция сохранений
void SaveGame(int slot) {
    std::string fileName = "save_" + std::to_string(slot) + ".txt";
    std::ofstream file(fileName);
    if (file.is_open()) {
        // Получаем текущее время
        time_t now = time(0);
        char* dt = ctime(&now);
        std::string dateStr(dt);
        if (!dateStr.empty()) dateStr.pop_back(); // Удаляем символ переноса строки в конце

        file << currentScene << "\n";
        file << dateStr << "\n"; // Записываем дату
        file << inventory.size() << "\n";
        for (const auto& item : inventory) {
            file << item << "\n";
        }
        file.close();
    }
}

// В LoadGame добавь чтение даты (просто пропускаем её или сохраняем)
bool LoadGame(int slot) {
    std::string fileName = "save_" + std::to_string(slot) + ".txt";
    std::ifstream file(fileName);
    if (file.is_open()) {
        file >> currentScene;
        file.ignore();
        std::string dateTrash;
        std::getline(file, dateTrash); // Пропускаем строку с датой

        int invSize;
        file >> invSize;
        file.ignore();
        inventory.clear();
        for (int i = 0; i < invSize; i++) {
            std::string item;
            std::getline(file, item);
            inventory.push_back(item);
        }
        file.close();

        currentSlot = slot;

        return true;
    }
    return false;
}

///<<<<<< функция смены картинок музыки сцен
void ChangeScene(int newSceneID) {
    currentScene = newSceneID;

    // --- Логика картинок ---
    UnloadTexture(currentBackground);
    if (!scenes[currentScene].backgroundPath.empty()) {
        currentBackground = LoadTexture(scenes[currentScene].backgroundPath.c_str());
    }

    // --- Логика музыки ---
    // 1. ДОСТАЕМ ПУТЬ К МУЗЫКЕ ИЗ ТЕКУЩЕЙ СЦЕНЫ
    std::string newPath = scenes[currentScene].musicPath;

    // 2. Меняем музыку ТОЛЬКО если указан новый трек и он не пустой
    if (!newPath.empty() && newPath != currentMusicPath) {
        if (currentMusicPath != "") UnloadMusicStream(currentMusic); // Удаляем старую

        currentMusic = LoadMusicStream(newPath.c_str());             // Грузим новую
        PlayMusicStream(currentMusic);                               // Запускаем
        currentMusicPath = newPath;                                  // Запоминаем путь
    }

    //автосохранение
    SaveGame(currentSlot);
} // <--- Теперь функция закрывается правильно, здесь!

std::string GetSavePreview(int slot) {
    std::string fileName = "save_" + std::to_string(slot) + ".txt";
    if (!FileExists(fileName.c_str())) return u8"ПУСТОЙ СЛОТ";

    std::ifstream file(fileName);
    std::string sceneNum, dateStr;
    std::getline(file, sceneNum);
    std::getline(file, dateStr);
    return u8"ИГРА " + std::to_string(slot) + " - " + dateStr;
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    InitWindow(1920, 1080, "Text Quest");
    LoadScenesFromJSON(); // ЗАГРУЖАЕМ СЮЖЕТ ИЗ ФАЙЛА
    SetWindowMinSize(640, 360);
    SetExitKey(KEY_NULL);

    RenderTexture2D target = LoadRenderTexture(1920, 1080);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    if (!IsWindowFullscreen()) {
        ToggleFullscreen();
    }

    InitAudioDevice();
    currentMusicPath = "resources/music.mp3";
    currentMusic = LoadMusicStream(currentMusicPath.c_str());
    PlayMusicStream(currentMusic);
    // --- ЗАГРУЗКА РЕСУРСОВ ---
    Texture2D background = LoadTexture("resources/menu.png");

    Texture2D buttonStart = LoadTexture("resources/butStart.png");
    Texture2D buttonCont = LoadTexture("resources/butCont.png");
    Texture2D buttonSet = LoadTexture("resources/butSet.png");
    Texture2D buttonExit = LoadTexture("resources/butExit.png");
    Texture2D buttonStart2 = LoadTexture("resources/butStart2.png");
    Texture2D buttonCont2 = LoadTexture("resources/butCont2.png");
    Texture2D buttonSet2 = LoadTexture("resources/butSet2.png");
    Texture2D buttonExit2 = LoadTexture("resources/butExit2.png");
    Texture2D buttonYes = LoadTexture("resources/butYes.png");
    Texture2D buttonNo = LoadTexture("resources/butNo.png");


    Sound typeSound = LoadSound("resources/m2.wav");
    Sound selectSound = LoadSound("resources/m1.wav");

    SetTraceLogLevel(LOG_WARNING);

    // --- ПЕРЕМЕННЫЕ ЛОГИКИ ---
    bool isLoadingMode = false; // true - загружаем, false - начинаем новую игру в этот слот
    int selectedSlot = 0;
    int confirmStage = 0; // 0 - нет окна, 1 - первый вопрос, 2 - предупреждение об удалении

    int pauseCounter = 0;
    float framesCounter = 0;
    int letterCount = 0;
    float framesCounter2 = 0;

    int codepoints[350] = { 0 };
    for (int i = 0; i < 95; i++) codepoints[i] = 32 + i;
    for (int i = 0; i < 255; i++) codepoints[95 + i] = 0x0400 + i;

    Font horrorFont = LoadFontEx("resources/Alice-Regular.ttf", 64, codepoints, 350);
    SetTextureFilter(horrorFont.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);

    GameState gameState = MENU;
    int selectedItem = 0;
    float musicVolume = 0.6f;
    float sfxVolume = 0.6f;
    int settingsItem = 0;

    bool isExitDialogActive = false;
    int exitChoice = 0;
    // цвета
    Color activeColor = { 247, 189, 52, 255 };

    //  Цикл работает, пока мы не укажем gameState = EXIT
    while (!WindowShouldClose() && gameState != EXIT) {

        // ==========================================
        // 1. UPDATE (ОБНОВЛЕНИЕ ЛОГИКИ)
        // ==========================================

        // Масштабирование окна
        if (IsWindowResized()) {
            int width = GetScreenWidth();
            int height = GetScreenHeight();
            int targetHeight = (int)(width / 1.777f);
            if (height != targetHeight) {
                SetWindowSize(width, targetHeight);
            }
        }

        // Полноэкранный режим
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
            if (!IsWindowFullscreen()) SetWindowSize(1280, 720);
            else SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
        }

        // Музыка обновляется всегда, даже на паузе
        if (currentMusicPath == "resources/music.mp3") {
            SetMusicVolume(currentMusic, musicVolume*0.5);
        }else{
            SetMusicVolume(currentMusic, musicVolume);
        }
        UpdateMusicStream(currentMusic);
        SetSoundVolume(typeSound, sfxVolume*0.3);
        SetSoundVolume(selectSound, sfxVolume*0.4);


        // Включение / выключение окна выхода
        if (IsKeyPressed(KEY_ESCAPE)&&((gameState == MENU) || (gameState == PLAYING))) {
            isExitDialogActive = !isExitDialogActive;
            exitChoice = 1; // Сброс на "НЕТ"
        }

        if (isExitDialogActive) {
            // --- ЛОГИКА ОКНА ВЫХОДА ---
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
                exitChoice = 1 - exitChoice;
                PlaySound(selectSound);
            }

            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(selectSound);
                if (exitChoice == 0) { // ДА
                    if (gameState == MENU) gameState = EXIT;
                    else SwitchToMenu(gameState);
                    isExitDialogActive = false;
                }
                else { // НЕТ
                    isExitDialogActive = false;
                }
            }
        }
        else {
            // --- ОСНОВНАЯ ИГРОВАЯ ЛОГИКА (ЕСЛИ НЕТ ПАУЗЫ) ---
            switch (gameState) {
            case MENU:
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) PlaySound(selectSound);
                if (IsKeyPressed(KEY_ENTER)) PlaySound(selectSound);

                if (IsKeyPressed(KEY_DOWN)) { selectedItem++; if (selectedItem > 3) selectedItem = 0; }
                if (IsKeyPressed(KEY_UP)) { selectedItem--; if (selectedItem < 0) selectedItem = 3; }

                if (IsKeyPressed(KEY_ENTER)) {
                    if (selectedItem == 0) { // НОВАЯ ИГРА
                        isLoadingMode = false;
                        gameState = SLOT_SELECT; // Идем в меню слотов
                    }
                    else if (selectedItem == 1) { // ПРОДОЛЖИТЬ ИГРУ
                        isLoadingMode = true;
                        gameState = SLOT_SELECT; // Идем в меню слотов
                    }
                    else if (selectedItem == 2) gameState = SETTINGS;
                    else if (selectedItem == 3) gameState = EXIT;
                }

                break;

            case PLAYING:
            {
                // 1. Подготовка данных текущей сцены
                const Scene& activeScene = scenes[currentScene];
                int maxLen = (int)activeScene.StoryText.length();
                int numChoices = (int)activeScene.choices.size(); // Сколько вариантов выбора в этой сцене
                bool textFinished = (letterCount >= maxLen);    // Закончилась ли печать текста

                // --- Логика печати текста (оставляем твою) ---
                framesCounter2 += 0.4f;
                framesCounter += 1.7f;

                if (framesCounter >= 2) {
                    if (letterCount < maxLen) letterCount += 2;
                    framesCounter = 0;
                }
                if (letterCount > maxLen) letterCount = maxLen;

                if (framesCounter2 >= 5) {
                    if (letterCount < maxLen) PlaySound(typeSound);
                    framesCounter2 = 0;
                }

                // --- Управление в игре ---

                // 2. Навигация по выборам (только если текст допечатан и варианты есть)
                if (textFinished && numChoices > 0) {
                    if (IsKeyPressed(KEY_DOWN)) {
                        selectedOption++;
                        if (selectedOption >= numChoices) selectedOption = 0;
                        PlaySound(selectSound);
                    }
                    if (IsKeyPressed(KEY_UP)) {
                        selectedOption--;
                        if (selectedOption < 0) selectedOption = numChoices - 1;
                        PlaySound(selectSound);
                    }
                }

                // 3. Обработка нажатия ENTER
                if (IsKeyPressed(KEY_ENTER)) {
                    if (!textFinished) {
                        // Скип текста: если еще печатается, мгновенно показываем всё
                        letterCount = maxLen;
                    }
                    else {
                        // Если текст уже показан, переходим к следующей сцене
                        PlaySound(selectSound);

                        if (numChoices > 0) {
                            // Вариант А: Есть список выборов
                            int next = activeScene.choices[selectedOption].nextScene;

                            if (next == -1) SwitchToMenu(gameState);
                            else ChangeScene(next);
                        }
                        else {
                            // Вариант Б: Выборов нет (линейный переход или конец)
                            // Здесь можно либо переходить на ID + 1, либо выходить в меню
                            if (currentScene + 1 < scenes.size()) {
                                ChangeScene(currentScene + 1);
                            }
                            else {
                                SwitchToMenu(gameState);
                            }
                        }

                        // Сброс параметров для новой сцены
                        selectedOption = 0;
                        letterCount = 0;
                        framesCounter = 0;
                    }
                }
                break;
            }

            case SETTINGS:
                if (IsKeyPressed(KEY_DOWN)) { settingsItem++; if (settingsItem > 2) settingsItem = 0; PlaySound(selectSound); }
                if (IsKeyPressed(KEY_UP)) { settingsItem--; if (settingsItem < 0) settingsItem = 2; PlaySound(selectSound); }

                if (settingsItem == 0) {
                    if (IsKeyDown(KEY_RIGHT) && musicVolume < 1.0f) musicVolume += 0.01f;
                    if (IsKeyDown(KEY_LEFT) && musicVolume > 0.0f) musicVolume -= 0.01f;
                }
                if (settingsItem == 1) {
                    if (IsKeyDown(KEY_RIGHT) && sfxVolume < 1.0f) sfxVolume += 0.01f;
                    if (IsKeyDown(KEY_LEFT) && sfxVolume > 0.0f) sfxVolume -= 0.01f;
                }

                if ((IsKeyPressed(KEY_ENTER) && settingsItem == 2) || IsKeyPressed(KEY_ESCAPE)) {
                    PlaySound(selectSound);
                    SwitchToMenu(gameState);
                    settingsItem = 0;
                }
                break;

            case SLOT_SELECT:
                if (confirmStage == 0) {
                    // Обычная навигация по слотам
                    if (IsKeyPressed(KEY_DOWN)) { selectedSlot++; if (selectedSlot > 2) selectedSlot = 0; PlaySound(selectSound); }
                    if (IsKeyPressed(KEY_UP)) { selectedSlot--; if (selectedSlot < 0) selectedSlot = 2; PlaySound(selectSound); }
                    if (IsKeyPressed(KEY_ESCAPE)) gameState = MENU;

                    if (IsKeyPressed(KEY_ENTER)) {
                        PlaySound(selectSound);
                        if (isLoadingMode) {
                            if (LoadGame(selectedSlot + 1)) {
                                ChangeScene(currentScene);
                                gameState = PLAYING;
                            }
                        }
                        else {
                            // НОВАЯ ИГРА: ВАЖНО - проверь путь! Должен совпадать с тем, что в SaveGame()
                            currentSlot = selectedSlot + 1;
                            std::string path = "save_" + std::to_string(currentSlot) + ".txt"; // Убрал папку "saves/", проверь у себя!
                            std::ifstream f(path);

                            if (f.good()) {
                                // Файл ЕСТЬ -> запускаем первое окно подтверждения
                                confirmStage = 1;
                            }
                            else {
                                // Файла НЕТ -> чистый старт (обнуляем ВСЁ)
                                inventory.clear();
                                currentScene = 0;
                                letterCount = 0;        // <-- Сбрасываем печать текста
                                selectedOption = 0;     // <-- Сбрасываем выбор кнопки
                                framesCounter = 0;      // <-- Сбрасываем таймеры
                                framesCounter2 = 0;

                                SaveGame(currentSlot);
                                ChangeScene(0);
                                gameState = PLAYING;
                            }
                        }
                    }
                }
                else if (confirmStage == 1) {
                    // Первое окно
                    if (IsKeyPressed(KEY_ENTER)) { confirmStage = 2; PlaySound(selectSound); }
                    if (IsKeyPressed(KEY_ESCAPE)) confirmStage = 0; // Отмена
                }
                else if (confirmStage == 2) {
                    // Второе окно (финальное)
                    if (IsKeyPressed(KEY_ENTER)) {
                        confirmStage = 0;
                        PlaySound(selectSound);

                        // Игрок всё подтвердил -> чистый старт (обнуляем ВСЁ)
                        inventory.clear();
                        currentScene = 0;
                        letterCount = 0;        // <-- Сбрасываем печать текста
                        selectedOption = 0;     // <-- Сбрасываем выбор кнопки
                        framesCounter = 0;      // <-- Сбрасываем таймеры
                        framesCounter2 = 0;

                        SaveGame(currentSlot); // Перезаписываем старый файл чистым сейвом
                        ChangeScene(0);
                        gameState = PLAYING;
                    }
                    if (IsKeyPressed(KEY_ESCAPE)) confirmStage = 0; // Отмена
                }
                break;

            case EXIT:
                break; 
            }
        }

        // ==========================================
        // 2. DRAW (ОТРИСОВКА)
        // ==========================================
        BeginDrawing();
        ClearBackground(BLACK);

        BeginTextureMode(target);
        ClearBackground(BLACK);

        switch (gameState) {
        case MENU:
        {
            DrawTexture(background, 0, 0, WHITE);

            Color colorStart = (selectedItem == 0) ? activeColor : WHITE;
            Color colorContinue = (selectedItem == 1) ? activeColor : WHITE;
            Color colorSettings = (selectedItem == 2) ? activeColor : WHITE;
            Color colorExit = (selectedItem == 3) ? activeColor : WHITE;

            DrawTexture(buttonStart, 320, 260, WHITE);
            DrawTexture(buttonCont, 319, 350, WHITE);
            DrawTexture(buttonSet, 320, 440, WHITE);
            DrawTexture(buttonExit, 319, 530, WHITE);

            DrawTexture(buttonStart2, 349, 275, colorStart);
            DrawTexture(buttonCont2, 340, 366, colorContinue);
            DrawTexture(buttonSet2, 343, 453, colorSettings);
            DrawTexture(buttonExit2, 344, 545, colorExit);
            break;
        }

        case PLAYING:
        {
            const Scene& activeScene = scenes[currentScene];
            int numChoices = (int)activeScene.choices.size();
            int maxLen = (int)activeScene.StoryText.length();

            // 1. Рисуем фон и подложку для текста
            DrawTexture(currentBackground, 0, 0, WHITE);
            DrawRectangle(0, 860, 1920, 220, ColorAlpha(BLACK, 0.4f)); // Немного увеличил высоту для списка

            // 2. Рисуем основной текст сцены (с эффектом печати)
            DrawTextEx(horrorFont, TextSubtext(activeScene.StoryText.c_str(), 0, letterCount), { 50, 880 }, 35, 2, LIGHTGRAY);

            // 3. Логика отрисовки вариантов выбора
            if (numChoices > 0) {
                // Рисуем варианты только когда основной текст допечатан
                if (letterCount >= maxLen) {
                    for (int i = 0; i < numChoices; i++) {
                        // Рассчитываем Y для каждого варианта (база 950 + отступ по 45 пикселей)
                        float yPos = 950.0f + (i * 45.0f);

                        if (selectedOption == i) {
                            // Подсвечиваем выбранный пункт
                            DrawTextGlow(horrorFont, activeScene.choices[i].text.c_str(), { 100, yPos }, 35, WHITE, RED, 2);
                        }
                        else {
                            // Остальные рисуем серым
                            DrawTextEx(horrorFont, activeScene.choices[i].text.c_str(), { 100, yPos }, 35, 2, GRAY);
                        }
                    }
                }
            }
            else {
                // 4. Если выборов нет — рисуем мигающую подсказку "Enter"
                float alpha = (sinf((float)GetTime() * 5.0f) + 1.0f) / 2.0f;
                Color blinkColor = ColorAlpha(WHITE, alpha);

                if (letterCount >= maxLen) {
                    DrawTextEx(horrorFont, u8"Нажмите Enter >>>", { 1500, 1000 }, 30, 2, blinkColor);
                }
            }

            break;
        }

        case SETTINGS:
        {
            DrawTexture(background, 0, 0, WHITE);
            DrawRectangle(0, 0, 1920, 1080, ColorAlpha(BLACK, 0.8f));

            // Заголовок (чуть левее, так как он большой)
            DrawTextEx(horrorFont, u8"НАСТРОЙКИ", { 670, 150 }, 100, 2, GRAY);

            // Музыка (самая длинная строка, ее ставим по центру)
            Color mColor = (settingsItem == 0) ? activeColor : WHITE;
            DrawTextEx(horrorFont, TextFormat(u8"< Музыка: %d%% >", (int)(musicVolume * 100)), { 730, 350 }, 60, 2, mColor);

            // Звуки (сдвинул чуть левее, чтобы < > были на одном уровне с музыкой)
            Color sColor = (settingsItem == 1) ? activeColor : WHITE;
            DrawTextEx(horrorFont, TextFormat(u8"< Звуки эффектов: %d%% >", (int)(sfxVolume * 100)), { 620, 450 }, 60, 2, sColor);

            // Назад (короткая фраза, поэтому x побольше)
            Color bColor = (settingsItem == 2) ? activeColor : WHITE;
            DrawTextEx(horrorFont, u8"Назад в меню", { 765, 600 }, 60, 2, bColor);

            break;
        }
        case SLOT_SELECT:
        {
            DrawTexture(background, 0, 0, WHITE);
            DrawRectangle(0, 0, 1920, 1080, ColorAlpha(BLACK, 0.8f));
            DrawTextEx(horrorFont, isLoadingMode ? u8"ВЫБЕРИТЕ СОХРАНЕНИЕ" : u8"НАЧАТЬ НОВУЮ ИГРУ В...", { 550, 150 }, 60, 2, WHITE);

            for (int i = 0; i < 3; i++) {
                Color boxColor = (selectedSlot == i) ? ColorAlpha(activeColor, 0.3f) : ColorAlpha(BLACK, 0.5f);
                Color textColor = (selectedSlot == i) ? activeColor : WHITE;

                // Рисуем плашку
                DrawRectangleRec({ 460, (float)300 + i * 150, 1000, 120 }, boxColor);
                DrawRectangleLinesEx({ 460, (float)300 + i * 150, 1000, 120 }, 2, textColor);

                // Текст внутри плашки
                DrawTextEx(horrorFont, GetSavePreview(i + 1).c_str(), { 500, (float)335 + i * 150 }, 35, 2, textColor);
            }
            if (confirmStage > 0) {
                DrawRectangle(0, 0, 1920, 1080, ColorAlpha(BLACK, 0.7f)); // Затемнение
                DrawRectangleLinesEx({ 660, 440, 600, 200 }, 3, RED);
                DrawRectangle(660, 440, 600, 200, ColorAlpha(BLACK, 0.8f));

                if (confirmStage == 1) {
                    DrawTextEx(horrorFont, u8"Начать игру в этом слоте?", { 770, 480 }, 30, 2, WHITE);
                    DrawTextEx(horrorFont, u8"[ENTER] - Да            [ESC] - Отмена", { 750, 560 }, 25, 2, GRAY);
                }
                else if (confirmStage == 2) {
                    DrawTextEx(horrorFont, u8"ВНИМАНИЕ!", { 860, 460 }, 35, 2, RED);
                    DrawTextEx(horrorFont, u8"Старое сохранение будет стерто!", { 760, 510 }, 25, 2, WHITE);
                    DrawTextEx(horrorFont, u8"[ENTER] - Стереть      [ESC] - Отмена", { 740, 560 }, 25, 2, LIGHTGRAY);
                }
            }
            break;
        }
        case EXIT:
            break;
        }

        // --- ОКНО ПОДТВЕРЖДЕНИЯ ВЫХОДА ПОВЕРХ ВСЕГО ---
        if (isExitDialogActive) {
            DrawRectangle(0, 0, 1920, 1080, ColorAlpha(BLACK, 0.7f));
            
            DrawRectangleLinesEx({ 660, 400, 600, 300 }, 3, RED);
            DrawRectangle(660, 400, 600, 300, Fade(BLACK, 0.8f));

            const char* question = (gameState == MENU) ? u8"Выйти из игры?" : u8"Выйти в меню?";
            DrawTextEx(horrorFont, question, { 815, 460 }, 40, 2, GRAY);

            Color yesColor = (exitChoice == 0) ? activeColor : WHITE;
            Color noColor = (exitChoice == 1) ? activeColor : WHITE;

            DrawTextEx(horrorFont, u8"ДА", { 800, 580 }, 50, 2, yesColor);
            //DrawTexture(buttonYes, 800, 580, WHITE);
            DrawTextEx(horrorFont, u8"НЕТ", { 1030, 580 }, 50, 2, noColor);
            // DrawTexture(buttonNo, 1030, 580, WHITE);
        }

        EndTextureMode();

        // Отрисовка виртуального экрана на реальный
        DrawTexturePro(target.texture,
            { 0, 0, (float)target.texture.width, -(float)target.texture.height },
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    // ==========================================
    // 3. ОЧИСТКА ПАМЯТИ
    // ==========================================
    UnloadTexture(background);
    UnloadSound(typeSound);
    UnloadSound(selectSound);
    UnloadFont(horrorFont);
    UnloadMusicStream(currentMusic);
    CloseAudioDevice();
    UnloadTexture(buttonStart);
    UnloadTexture(buttonCont);
    UnloadTexture(buttonSet);
    UnloadTexture(buttonExit);
    UnloadTexture(buttonStart2);
    UnloadTexture(buttonCont2);
    UnloadTexture(buttonSet2);
    UnloadTexture(buttonExit2);
    UnloadTexture(buttonYes);
    UnloadTexture(buttonNo);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}