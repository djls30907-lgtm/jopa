#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <raylib.h> 
enum GameState { MENU, PLAYING, SETTINGS, EXIT, SLOT_SELECT };

std::vector<std::string> inventory;

int currentSlot = 1; // По умолчанию первый слот

struct Scene {
    const char* StoryText;
    const char* choice1;
    const char* choice2;
    int nextScene1;
    int nextScene2;
    const char* backgroundPath; // <-- ТЕПЕРЬ ТУТ ПРОСТО ПУТЬ К ФАЙЛУ
    const char* musicPath;
};

Scene scenes[] = {
    { u8"Ты смотришь в обледенелое окно автобуса.За стеклом — сплошная белая стена.\nБуран начался внезапно, отрезав рейс №402 от цивилизации.\n",
     u8"", u8"", 1, -1,  "resources/scene1.png","resources/music2.mp3"},
    { u8"В салоне холодно, изо рта идет пар.Рядом с тобой нервно трясет ногой твой лучший друг, Макс.\nЧуть позади тихо переговариваются брат и сестра, Денис и Аня,\nстараясь согреть друг другу руки.",
     u8"", u8"", 2, -1 , "resources/scene1.png"},
    { u8"Внезапно автобус резко дергается.\nКолеса скользят по льду, мотор издает натужный вой, захлебывается и глохнет.\nВ салоне гаснет свет..",
    u8"", u8"", 3, -1 , "resources/scene2.png"},
    { u8"Маркус, грузный чернокожий водитель с сединой на висках, бьет кулаком по рулю и тяжело выдыхает.\n — Приехали, — басит он, оборачиваясь к вам.— Двигатель сдох.Печка сейчас остынет.\nЕсли останемся здесь — замерзнем насмерть. Я видел свет в паре десятков метров отсюда.\nКафе.Идем туда, держимся вместе.",
    u8"", u8"", 4, -1 , "resources/scene2.png"},
    { u8"Вы кутаетесь в куртки и вываливаетесь в ревущую метель. Ветер сбивает с ног,\nно сквозь пелену снега действительно пробивается теплый, спасительный неоновый свет вывески:\n«У ОБОЧИНЫ»",
    u8"", u8"", 5, -1 , "resources/scene3.png"},
{   u8"Ты толкаешь тяжелую дубовую дверь. Над головой звякает колокольчик, и рев метели отрезает тяжелая створка.\nВнутри невероятно тепло.Пахнет выпечкой, старым деревом и крепким кофе.\nСвет мягкий, приглушенный.Но первое, что бросается в глаза — вы здесь не одни.\nНа вас устремляются взгляды людей, которые уже пережидают бурю.",
    u8"", u8"", 6, -1, "resources/scene4.png"},
{
    u8"За барной стойкой стоит мужчина в чистом фартуке — Илья, работник кафе.\nОн невозмутимо протирает стакан, словно группа замерзших людей из бурана — обычное дело.\n— Места хватит всем.Кофе за счет заведения, — спокойно говорит он, кивая на пустые столики.\nВ углу, напряженно обняв маленькую дочку, сидит хмурый мужчина.",
        u8"", u8"", 7, -1, "resources/scene4.png"
},
{
    u8"Девочка с любопытством смотрит на вас поверх папиного плеча.\nЗа соседним столиком сидит парень, старательно копающийся с фотоаппаратом,\n начинающий дружелюбно поворачивоть голову в вашу сторону.\nА у окна, закинув ноги на соседний стул, сидит девушка в кожаной куртке (Рита) и лениво листает журнал.",
        u8"", u8"", 8, -1, "resources/scene4.png"
},
{
    u8"Макс с облегчением выдыхает и падает на ближайший стул. Денис и Аня стягивают мокрые шапки.\nВодитель Маркус идет прямиком к бармену, чтобы спросить про телефон.",
        u8"", u8"", 9, -1, "resources/scene4.png"
}, {
    u8"Ты подходишь к столику, за которым сидят мужчина и девочка. Они выглядят измотанными.\nМужина крепко обнимает дочь, словно пытаясь защитить её от самого холода,\nпробирающегося сквозь иней на стеклах. Перед ними стоят пустые стаканы\n— кажется, они выпили свой чай уже очень давно.",
        u8"", u8"", 10, -1, "resources/sc5otec.png"
}, {
    u8"Маленькая девочка испуганно прижимается к отцу, глядя на тебя из-под челки.\nМужчина поднимает на тебя тяжелый, усталый взгляд. В его глазах читается не враждебность,\nа скорее бесконечная усталость человека, который отвечает за кого-то еще.\n— Тяжелая выдалась ночка, верно? — негромко говорит мужчина, поправляя куртку на плечах дочери.",
        u8"", u8"", 11, -1, "resources/sc5otec.png"
},
{
    u8"— Ваш водитель сказал, автобус совсем заглох. Нам повезло чуть больше\n— у нашей машины просто пробило колесо в паре километров отсюда,\nно в такой буран менять его было самоубийством.\nПришлось бросить её на обочине и идти сюда пешком. Юлька совсем закоченела.",
        u8"", u8"", 12, -1, "resources/sc5otec.png"
},
{
    u8"Он делает паузу и чуть слышно вздыхает.\n— Мы едем к моей сестре в соседний город, обещали быть к ужину...\nА теперь вот, сидим здесь, ждем, пока небо перестанет падать на землю.",
        u8"", u8"", 13, -1, "resources/sc5otec.png"
},
{
    u8"Мы тоже ехали в ту сторону. Вы давно здесь сидите?\nНа трассе за это время проезжал кто-нибудь еще, кроме нашего автобуса?",
        u8"", u8"", 14, -1, "resources/sc5otec.png"
},{
    u8"С тех пор как повалил этот проклятый снег — ни души. Мы добрались сюда около пяти вечера, кажется.\nИли в шесть? Знаете, когда свет так мигает, а за окном сплошная темень, время путается.\nМои часы встали — наверное, батарейка на холоде сдохла.\nС тех пор, как мы зашли, по трассе никто не проезжал... до вашего автобуса.",
        u8"", u8"", 15, -1, "resources/sc5otec.png"
},
{
    u8"Главное, что вы успели добраться до тепла. Юля, ты как, согрелась?\nХочешь, я попрошу у бармена еще горячего шоколада или чаю",
        u8"", u8"", 16, -1, "resources/sc5otec.png"
},{
    u8"Мужчина чуть заметно улыбается — впервые за вечер. «Это было бы здорово, спасибо. Юль, слышишь?\nДобрый человек хочет угостить тебя. Она просто перепугана, обычно она куда болтливее.\nИлья, бармен, вроде говорил, что у него есть какао. Если принесете — будем очень признательны».",
        u8"", u8"", 17, -1, "resources/sc5otec.png"
},{
    u8"А те двое? Они были здесь, когда вы пришли, или приехали позже?",
        u8"", u8"", 18, -1, "resources/sc5otec.png"
},
{
    u8"Этот фотограф, Костя, был уже здесь, когда мы пришли. Говорит, «кадры года» ловил.\nА девушка на мотоцикле, Рита, приехала минут через сорок после нас.\nВесь комбинезон в снегу, злая как чёрт. Сказала, байк завалился в кювет.\nМы тут теперь как в Ноевом ковчеге, только вместо зверей — случайные прохожие.",
        u8"", u8"", 19, -1, "resources/sc5otec.png"
}, {
    u8"— Нас просто выкинуло с трассы. Один резкий занос — и автобус замер прямо над обрывом.\nМотор заглох мгновенно, свет погас, и в этой кромешной тьме стало слышно только, как завывает ветер.\nЕсли бы не наш водитель Маркус, мы бы сейчас уже не грелись здесь, а лежали в сугробах на дне оврага.",
        u8"", u8"", 20, -1, "resources/sc5otec.png"
}, {
    u8"Понимаю... Мы сами едва не улетели в овраг. Я видел ваши фары в окно пару минут назад.\nОни мигнули и погасли. Я уж грешным делом подумал, что мне померещилось, или это волки...\nРад, что вы добрались.\nВ такую погоду человек человеку — единственный шанс не сойти с ума от тишины",
        u8"", u8"", 21, -1, "resources/sc5otec.png"
},
{
    u8"Ты подходишь к столику парня в ушанке. Он не спеша протирает объектив фланелевой тряпочкой,\nно, заметив тебя, расплывается в дружелюбной улыбке.\n— О, привет!Присаживайся, тут у фонаря теплее, — он отодвигает один из своих блокнотов, освобождая место.",
        u8"", u8"", 22, -1, "resources/sc5ushanka.png"
},
{
    u8"— Я Костя. Фотограф-документалист на вынужденном отдыхе. Вы, ребята, эффектно зашли — снег, пар, драма...\nЕсли бы у меня не села основная вспышка, получился бы кадр для обложки.\nОн кивает на свою камеру и стопку тетрадей.",
        u8"", u8"", 23, -1, "resources/sc5ushanka.png"
}, {
    u8"— Я приехал еще засветло. Думал переждать часок, выпить кофе — и вот я здесь.\nЗнаешь, в этом есть что-то уютное...пока не посмотришь в окно и не поймешь, что мы заперты.\nКостя внимательно смотрит на тебя, ожидая реакции.\nНа его столе лежит раскрытый блокнот с какими - то пометками.",
        u8"", u8"", 24, -1, "resources/sc5ushanka.png"
}, {
    u8"Значит, ты был здесь совсем один? Расскажи, как выглядело это место до нашего приезда.\nИлья, бармен, вел себя так же... спокойно?",
        u8"", u8"", 25, -1, "resources/sc5ushanka.png"
},{
    u8"О, в первые часы тут было совсем глухо. Только я и Илья.\nОн... парень своеобразный, да? Протирал эти стаканы сорок минут кряду,\nпока я не попросил его остановиться — скрип по нервам бил.\nОн почти не разговаривал, просто кивал.",
        u8"", u8"", 26, -1, "resources/sc5ushanka.png"
},{
    u8"Можно взглянуть на фото?",
        u8"", u8"", 27, -1, "resources/sc5ushanka.png"
},{
    u8"Костя заминается и прикрывает ладонью дисплей камеры.\n«Ох, с радостью бы показал, но я сейчас экономлю заряд батареи на максимум — на морозе аккумулятор тает на глазах.\nСделал буквально пару кадров, когда Андрей и Юля ввалились,\nи когда Рита свой шлем на стойку бросила.",
        u8"", u8"", 28, -1, "resources/sc5ushanka.png"
}, {
    u8"Хочу оставить пару щелчков на случай,\nесли буря вдруг стихнет и луна выйдет — свет будет фантастический.\nТак что пока это «закрытый показ», извини, дружище».",
        u8"", u8"", 29, -1, "resources/sc5ushanka.png"
}, {
    u8"Я вижу, ты что-то записываешь.\nВедешь дневник выживания или это просто путевые заметки?",
        u8"", u8"", 30, -1, "resources/sc5ushanka.png"
}, {
    u8"А, это? Да так, привычка. Я записываю экспозицию, время заката... Ну и так, по мелочи.\nНапример, во сколько свет моргнул или кто что заказал.\nПомогает не терять связь с реальностью, когда за окном сплошное белое ничто.",
        u8"", u8"", 31, -1, "resources/sc5ushanka.png"
}, {

    u8"Если хочешь, могу и твое прибытие записать:\n«Рейс №402, куча замерзших, но живых людей».\nЗвучит как начало хорошего репортажа, а?»",
        u8"", u8"", 0, -1, "resources/sc5ushanka.png"
}, };




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
    if (scenes[currentScene].backgroundPath != nullptr && scenes[currentScene].backgroundPath[0] != '\0') {
        currentBackground = LoadTexture(scenes[currentScene].backgroundPath);
    }

    // --- Логика музыки ---
    if (scenes[currentScene].musicPath != nullptr && scenes[currentScene].musicPath[0] != '\0') {
        std::string newPath = scenes[currentScene].musicPath;

        // Меняем музыку ТОЛЬКО если указан новый трек
        if (newPath != currentMusicPath) {
            if (currentMusicPath != "") UnloadMusicStream(currentMusic); // Удаляем старую

            currentMusic = LoadMusicStream(newPath.c_str());             // Грузим новую
            PlayMusicStream(currentMusic);                               // Запускаем
            currentMusicPath = newPath;                                  // Запоминаем путь
        }
    }
    //автосохранение
    SaveGame(currentSlot);
}

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
        SetMusicVolume(currentMusic, musicVolume);
        UpdateMusicStream(currentMusic);
        SetSoundVolume(typeSound, sfxVolume);
        SetSoundVolume(selectSound, sfxVolume);


        // Включение / выключение окна выхода
        if (IsKeyPressed(KEY_ESCAPE)) {
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
                int maxLen = TextLength(scenes[currentScene].StoryText);
                bool hasChoices = (TextLength(scenes[currentScene].choice1) > 0);

                // Логика печати текста
                framesCounter2 += 0.4f;
                framesCounter += 0.7f;

                if (framesCounter >= 2) {
                    letterCount += 2;
                    framesCounter = 0;
                }
                if (letterCount > maxLen) letterCount = maxLen;

                if (framesCounter2 >= 5) {
                    if (letterCount < maxLen) PlaySound(typeSound);
                    framesCounter2 = 0;
                }

                pauseCounter += 1;
                if (pauseCounter > 200) framesCounter2 = 0;
                if (pauseCounter > 250) pauseCounter = -4;

                // Управление в игре
                if (IsKeyPressed(KEY_ENTER) && letterCount < maxLen) {
                    letterCount = maxLen; // Скип текста
                }
                else if (IsKeyPressed(KEY_ENTER) && letterCount >= maxLen) {
                    PlaySound(selectSound);
                    if (hasChoices) {
                        int next = (selectedOption == 0) ? scenes[currentScene].nextScene1 : scenes[currentScene].nextScene2;
                        if (next == -1) SwitchToMenu(gameState);
                        else ChangeScene(next); // <-- Загружаем фон новой сцены
                    }
                    else {
                        if (scenes[currentScene].nextScene1 == -1) SwitchToMenu(gameState);
                        else ChangeScene(scenes[currentScene].nextScene1); // <-- Загружаем фон новой сцены
                    }
                    selectedOption = 0;
                    letterCount = 0;
                    framesCounter = 0;
                }

                if (hasChoices && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP))) {
                    selectedOption = 1 - selectedOption;
                    PlaySound(selectSound);
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
                    else { // Если это новая игр
                        inventory.clear();
                        currentScene = 0;
                        currentSlot = selectedSlot + 1; // <--- ДОБАВЬ ЭТУ СТРОКУ
                        SaveGame(currentSlot);
                        ChangeScene(0);
                        gameState = PLAYING;
                    }
                }
                break;

            case EXIT:
                break; // Сюда мы не должны попадать из-за условия цикла, но switch требует
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
            
            DrawTexture(currentBackground, 0, 0, WHITE);
            DrawRectangle(0, 860, 1920,180, ColorAlpha(BLACK, 0.3f));
            DrawTextEx(horrorFont, TextSubtext(scenes[currentScene].StoryText, 0, letterCount), { 50, 880 }, 35, 2, LIGHTGRAY);
            
            

            bool hasChoices = (TextLength(scenes[currentScene].choice1) > 0);
            int maxLen = TextLength(scenes[currentScene].StoryText);

            if (hasChoices) {
                if (selectedOption == 0) DrawTextGlow(horrorFont, scenes[currentScene].choice1, { 100, 950 }, 35, WHITE, RED, 2);
                else DrawTextEx(horrorFont, scenes[currentScene].choice1, { 100, 950 }, 35, 2, GRAY);

                if (selectedOption == 1) DrawTextGlow(horrorFont, scenes[currentScene].choice2, { 100, 1010 }, 35, WHITE, RED, 2);
                else DrawTextEx(horrorFont, scenes[currentScene].choice2, { 100, 1010 }, 35, 2, GRAY);
            }
            else {
                float alpha = (sinf((float)GetTime() * 5.0f) + 1.0f) / 2.0f;
                Color blinkColor = ColorAlpha(WHITE, alpha);
                if (letterCount >= maxLen) {
                    DrawTextEx(horrorFont, u8"Нажмите Enter >>>", { 1500, 980 }, 30, 2, blinkColor);
                }           
            } 
            
           
            break;
        }

        case SETTINGS:
        {
            DrawRectangle(0, 0, 1920, 1080, ColorAlpha(BLACK, 0.8f));
            DrawTextEx(horrorFont, u8"НАСТРОЙКИ", { 700, 150 }, 100, 2, GRAY);

            Color mColor = (settingsItem == 0) ? activeColor : WHITE;
            DrawTextEx(horrorFont, TextFormat(u8"< Музыка: %d%% >", (int)(musicVolume * 100)), { 650, 350 }, 60, 2, mColor);

            Color sColor = (settingsItem == 1) ? activeColor : WHITE;
            DrawTextEx(horrorFont, TextFormat(u8"< Звуки эффектов: %d%% >", (int)(sfxVolume * 100)), { 550, 450 }, 60, 2, sColor);

            Color bColor = (settingsItem == 2) ? activeColor : WHITE;
            DrawTextEx(horrorFont, u8"Назад в меню", { 720, 600 }, 60, 2, bColor);
            break;
        }
        case SLOT_SELECT:
        {
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
            break;
        }
        case EXIT:
            break;
        }

        // --- ОКНО ПОДТВЕРЖДЕНИЯ ВЫХОДА ПОВЕРХ ВСЕГО ---
        if (isExitDialogActive) {
            DrawRectangle(0, 0, 1920, 1080, ColorAlpha(BLACK, 0.6f));
            DrawRectangle(660, 400, 600, 300, Fade(DARKGRAY, 0.9f));
            DrawRectangleLines(660, 400, 600, 300, WHITE);

            const char* question = (gameState == MENU) ? u8"Выйти из игры?" : u8"Выйти в меню?";
            DrawTextEx(horrorFont, question, { 750, 450 }, 40, 2, WHITE);

            Color yesColor = (exitChoice == 0) ? YELLOW : WHITE;
            Color noColor = (exitChoice == 1) ? YELLOW : WHITE;

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