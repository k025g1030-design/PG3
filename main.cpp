#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Novice.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace Core {
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 768;
    constexpr char kWindowTitle[] = "GC1B_10_ラ_ケツブン_ザ．ラストシップ";
    constexpr int INPUT_LOCK_FRAME = 30;
    constexpr int MAX_SP_COUNT = 2;
    constexpr int BG_SPEED = 2;
    constexpr int MOVE_SPEED = 5;
    constexpr int BULLET_SPEED = 10;
    constexpr int BOSS_MAX_HP = 20;
    constexpr int PLAYER_MAX_HP = 3;
    constexpr int FIRE_INTERVAL = 15;
    constexpr int PLAYER_AMMO = 3;
    constexpr int BOSS_AMMO = 10;
    constexpr int DEATH_WAIT_TIME = 60;

    enum MouseCursor { HIDE_MOUSE = 0, SHOW_MOUSE = 1 };
    enum MouseButton { LEFT_BUTTON = 0, RIGHT_BUTTON = 1, MIDDLE_BUTTON = 2 };
    enum Life { ALIVE, DEATH, RECEIVE };
    enum Role { PLAYER, BOSS };
    enum SceneState { MAIN, GAMEPLAY, PAUSE, GAMEOVER_WIN, GAMEOVER_LOSE };

    struct ActiveState {
        Life life;
        Role role;
    };

    class Vector2 {
    public:
        float x{ 0.0f };
        float y{ 0.0f };

        Vector2() = default;
        Vector2(float _x, float _y) : x(_x), y(_y) {}
        Vector2 operator+(const Vector2& o) const { return { x + o.x, y + o.y }; }
        Vector2 operator-(const Vector2& o) const { return { x - o.x, y - o.y }; }
        Vector2& operator+=(const Vector2& o) { x += o.x; y += o.y; return *this; }
        Vector2& operator-=(const Vector2& o) { x -= o.x; y -= o.y; return *this; }
    };

    struct AudioResource {
        int handle;
        bool isPlaying;
        bool isLoop;
        float volume;
        AudioResource() : handle(-1), isPlaying(false), isLoop(false), volume(1.0f) {}
    };

    struct AnimeResource {
        int textureHandle;
        int frameCount;
        Vector2 size;
        AnimeResource() : textureHandle(-1), frameCount(0), size({ 0, 0 }) {}
        AnimeResource(int textureHandle, int frameCount, Vector2 size)
            : textureHandle(textureHandle), frameCount(frameCount), size(size) {}
    };

    struct AnimeHandle {
        Vector2 posi;
        Vector2 size;
        AnimeResource resource;
        AnimeHandle() : posi({ 0, 0 }), size({ 0, 0 }), resource() {}
        AnimeHandle(Vector2 posi, Vector2 size, AnimeResource resource) : posi(posi), size(size), resource(resource) {}
    };

    class ResourceManager {
    public:
        void LoadAll() {
            gameMapRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Map/Game_Play[1280x768].png"), 1, { 1280, 768 });
            playerBodyRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Role/Player-Body[96x96].png"), 1, { 96, 96 });
            playerBulletRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Role/Player-Bullet[32x36][16x36_2].png"), 2, { 32, 36 });
            playerDeathRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Role/Player-Death[384x96][96x96_4].png"), 4, { 384, 96 });
            bossIdelRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Role/Boss-Idel[440x216][220x216_2].png"), 2, { 440, 216 });
            bossBulletRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Role/Boss-Bullet[96x96].png"), 1, { 96, 96 });
            bossDeathRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Role/Boss-Death[880x216][220x216_4].png"), 4, { 880, 216 });
            gameTitleRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Game_Title[740x100].png"), 1, { 740, 100 });
            startBtnRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Main_Start[140x40].png"), 1, { 140, 40 });
            exitBtnRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Main_End[140x40].png"), 1, { 140, 40 });
            reTextRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Main_Re[180x40].png"), 1, { 180, 40 });
            yes2noBtnRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Main_Y2N[180x30][90x30_2].png"), 2, { 180, 30 });
            winTextRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Player_Win[260x110].png"), 1, { 260, 110 });
            loseTextRes_ = AnimeResource(Novice::LoadTexture("./Assets/Images/Text/Player_Faild[260x110].png"), 1, { 260, 110 });
        }

        AnimeHandle MainBGHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 1280.0f, 768.0f }, gameMapRes_); }

        AnimeHandle CharacterHandle(ActiveState state) {
            AnimeHandle result;
            if (state.life == Life::ALIVE) {
                if (state.role == Role::PLAYER) result = AnimeHandle({ 0.0f, 0.0f }, { 96.0f, 96.0f }, playerBodyRes_);
                else if (state.role == Role::BOSS) result = AnimeHandle({ 0.0f, 0.0f }, { 220.0f, 216.0f }, bossIdelRes_);
            } else if (state.life == Life::DEATH) {
                if (state.role == Role::PLAYER) result = AnimeHandle({ 0.0f, 0.0f }, { 96.0f, 96.0f }, playerDeathRes_);
                else if (state.role == Role::BOSS) result = AnimeHandle({ 0.0f, 0.0f }, { 220.0f, 216.0f }, bossDeathRes_);
            } else {
                if (state.role == Role::PLAYER) result = AnimeHandle({ 0.0f, 0.0f }, { 96.0f, 96.0f }, playerBodyRes_);
                else if (state.role == Role::BOSS) result = AnimeHandle({ 0.0f, 0.0f }, { 220.0f, 216.0f }, bossIdelRes_);
            }
            return result;
        }

        AnimeHandle BulletHandle(Role role) {
            AnimeHandle result;
            if (role == Role::PLAYER) result = AnimeHandle({ 0.0f, 0.0f }, { 16.0f, 36.0f }, playerBulletRes_);
            else if (role == Role::BOSS) result = AnimeHandle({ 0.0f, 0.0f }, { 96.0f, 96.0f }, bossBulletRes_);
            return result;
        }

        AnimeHandle GetYes2NoBtnHandle(bool isYes) { return AnimeHandle({ isYes ? 0.0f : 90.0f, 0.0f }, { 90.0f, 30.0f }, yes2noBtnRes_); }
        AnimeHandle GetStartBtnHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 140.0f, 40.0f }, startBtnRes_); }
        AnimeHandle GetExitBtnHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 140.0f, 40.0f }, exitBtnRes_); }
        AnimeHandle GetReTextHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 180.0f, 40.0f }, reTextRes_); }
        AnimeHandle GetGameTitleHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 740.0f, 100.0f }, gameTitleRes_); }
        AnimeHandle GetWinTextHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 260.0f, 110.0f }, winTextRes_); }
        AnimeHandle GetLoseTextHandle() { return AnimeHandle({ 0.0f, 0.0f }, { 260.0f, 110.0f }, loseTextRes_); }

    private:
        AnimeResource gameMapRes_;
        AnimeResource playerBodyRes_;
        AnimeResource playerDeathRes_;
        AnimeResource playerBulletRes_;
        AnimeResource bossIdelRes_;
        AnimeResource bossDeathRes_;
        AnimeResource bossBulletRes_;
        AnimeResource gameTitleRes_;
        AnimeResource startBtnRes_;
        AnimeResource exitBtnRes_;
        AnimeResource reTextRes_;
        AnimeResource yes2noBtnRes_;
        AnimeResource winTextRes_;
        AnimeResource loseTextRes_;
    };
}

namespace Component {
    class Button {
    public:
        explicit Button(Core::Vector2 size, Core::Vector2 position, std::string name) : size_(size), position_(position), name_(name) {}
        explicit Button(Core::Vector2 position, std::string name, Core::AnimeHandle cursorHandle) : size_(cursorHandle.size), position_(position), name_(name), cursorHandle_(cursorHandle) {}
        explicit Button() {}

        bool Contains(Core::Vector2 p, Core::Vector2 sz, int mx, int my) { return mx >= p.x && mx <= p.x + sz.x && my >= p.y && my <= p.y + sz.y; }

        bool HandleMouse(int mx, int my, bool mouseDown, bool prevMouseDown) {
            if (!isEnabled_ || !isVisible_) { pressed = false; return false; }
            hovered = Contains(position_, size_, mx, my);
            if (hovered && mouseDown && !prevMouseDown) pressed = true;
            bool clicked = false;
            if (pressed && !mouseDown && prevMouseDown) {
                if (hovered) { isSelected_ = !isSelected_; clicked = true; }
                pressed = false;
            }
            if (pressed && mouseDown && !hovered) pressed = false;
            return clicked;
        }

        Core::Vector2 GetSize() const { return size_; }
        Core::Vector2 GetPosition() const { return position_; }
        void SetPosition(Core::Vector2 pos) { position_ = pos; }
        void Open() { isEnabled_ = true; isVisible_ = true; }
        void Close() { isEnabled_ = false; isVisible_ = false; }
        bool IsEnabled() const { return isEnabled_; }
        void SetEnabled(bool flag) { isEnabled_ = flag; }
        bool IsVisible() const { return isVisible_; }
        void SetVisible(bool flag) { isVisible_ = flag; }
        bool IsSelected() const { return isSelected_; }
        void SetSelected(bool flag) { isSelected_ = flag; }
        void SetValue(int value) { value_ = value; }
        int GetValue() const { return value_; }
        Core::AnimeHandle GetCursorHandle() { return cursorHandle_; }
        void SetCursorHandle(Core::AnimeHandle handle) { cursorHandle_ = handle; }

        void Render() const {
            if (!isVisible_) return;
            int x = static_cast<int>(position_.x);
            int y = static_cast<int>(position_.y);
            int w = static_cast<int>(size_.x);
            int h = static_cast<int>(size_.y);
            if (cursorHandle_.resource.textureHandle == -1) {
                Novice::DrawBox(x, y, w, h, 0, 0x404040FF, kFillModeSolid);
                Novice::DrawBox(x, y, w, h, 0, 0xFFFFFFFF, kFillModeWireFrame);
                Novice::ScreenPrintf(x + 10, y + h / 2 - 8, "%s", name_.c_str());
                if (isSelected_) Novice::DrawBox(x, y, w, h, 0, RED, kFillModeWireFrame);
            } else {
                Novice::DrawBox(x, y, static_cast<int>(cursorHandle_.size.x), static_cast<int>(cursorHandle_.size.y), 0, 0x40404066, kFillModeSolid);
                Novice::DrawSpriteRect(x, y, static_cast<int>(cursorHandle_.posi.x), static_cast<int>(cursorHandle_.posi.y), static_cast<int>(cursorHandle_.size.x), static_cast<int>(cursorHandle_.size.y), cursorHandle_.resource.textureHandle, cursorHandle_.size.x / cursorHandle_.resource.size.x, 1, 0.0f, WHITE);
                if (isSelected_) Novice::DrawBox(x, y, static_cast<int>(cursorHandle_.size.x), static_cast<int>(cursorHandle_.size.y), 0, 0x00000099, kFillModeSolid);
            }
        }

    private:
        Core::Vector2 size_;
        Core::Vector2 position_;
        bool isEnabled_{ true };
        bool isVisible_{ true };
        bool isSelected_{ false };
        bool hovered{ false };
        bool pressed{ false };
        int value_{ -1 };
        std::string name_;
        Core::AnimeHandle cursorHandle_;
    };

    class ImageSpan {
    public:
        explicit ImageSpan(Core::Vector2 size, Core::Vector2 position) : size_(size), position_(position) {}
        explicit ImageSpan(Core::Vector2 position, Core::AnimeHandle cursorHandle) : size_(cursorHandle.size), position_(position), cursorHandle_(cursorHandle) {}
        explicit ImageSpan() {}

        void Render() const {
            if (!isVisible_) return;
            int x = static_cast<int>(position_.x);
            int y = static_cast<int>(position_.y);
            int w = static_cast<int>(size_.x);
            int h = static_cast<int>(size_.y);
            if (cursorHandle_.resource.textureHandle == -1) {
                Novice::DrawBox(x, y, w, h, 0, 0x404040FF, kFillModeSolid);
                Novice::DrawBox(x, y, w, h, 0, 0xFFFFFFFF, kFillModeWireFrame);
            } else {
                Novice::DrawSpriteRect(x, y, static_cast<int>(cursorHandle_.posi.x), static_cast<int>(cursorHandle_.posi.y), static_cast<int>(cursorHandle_.size.x), static_cast<int>(cursorHandle_.size.y), cursorHandle_.resource.textureHandle, cursorHandle_.size.x / cursorHandle_.resource.size.x, 1, 0.0f, WHITE);
            }
        }

        void SetCursorHandle(Core::AnimeHandle handle) { cursorHandle_ = handle; }
        void SetSize(Core::Vector2 size) { size_ = size; }
        Core::Vector2 GetSize() const { return size_; }
        void Open() { isVisible_ = true; }
        void Close() { isVisible_ = false; }
        void SetPosition(Core::Vector2 position) { position_ = position; }
        Core::Vector2 GetPosition() const { return position_; }
        bool IsVisible() const { return isVisible_; }
        void SetVisible(bool flag) { isVisible_ = flag; }

    private:
        Core::Vector2 size_;
        Core::Vector2 position_;
        bool isVisible_{ true };
        Core::AnimeHandle cursorHandle_;
    };

    class TextSpan {
    public:
        explicit TextSpan(Core::Vector2 size, Core::Vector2 position, std::string name) : size_(size), position_(position), name_(name) {}
        explicit TextSpan(Core::Vector2 position, std::string name, Core::AnimeHandle cursorHandle) : size_(cursorHandle.size), position_(position), name_(name), cursorHandle_(cursorHandle) {}
        explicit TextSpan() {}

        void Render() const {
            if (!isVisible_) return;
            int x = static_cast<int>(position_.x);
            int y = static_cast<int>(position_.y);
            int w = static_cast<int>(size_.x);
            int h = static_cast<int>(size_.y);
            if (cursorHandle_.resource.textureHandle == -1) {
                Novice::DrawBox(x, y, w, h, 0, 0x404040FF, kFillModeSolid);
                Novice::DrawBox(x, y, w, h, 0, 0xFFFFFFFF, kFillModeWireFrame);
                Novice::ScreenPrintf(x + 10, y + h / 2 - 8, "%s", name_.c_str());
            } else {
                Novice::DrawSpriteRect(x, y, static_cast<int>(cursorHandle_.posi.x), static_cast<int>(cursorHandle_.posi.y), static_cast<int>(cursorHandle_.size.x), static_cast<int>(cursorHandle_.size.y), cursorHandle_.resource.textureHandle, cursorHandle_.size.x / cursorHandle_.resource.size.x, 1, 0.0f, WHITE);
            }
        }

        void SetCursorHandle(Core::AnimeHandle handle) { cursorHandle_ = handle; }
        void SetName(std::string name) { name_ = name; }
        std::string GetName() { return name_; }
        void SetSize(Core::Vector2 size) { size_ = size; }
        void SetPosition(Core::Vector2 position) { position_ = position; }
        bool IsVisible() const { return isVisible_; }
        void SetVisible(bool flag) { isVisible_ = flag; }
        void Open() { isVisible_ = true; }
        void Close() { isVisible_ = false; }

    private:
        Core::Vector2 size_;
        Core::Vector2 position_;
        bool isVisible_{ true };
        std::string name_;
        Core::AnimeHandle cursorHandle_;
    };
}

namespace Game {
    struct Boss { Core::Vector2 position; Core::Vector2 velocity; Core::AnimeHandle bossAnime_; Core::AnimeHandle bossDeathAnime_; bool isActive; int hp; int ammoCount; int score; };
    struct Player { Core::Vector2 position; Core::Vector2 velocity; Core::AnimeHandle playerAnime_; Core::AnimeHandle playerDeathAnime_; bool isActive; int hp; int ammoCount; int score; };
    struct Bullet { Core::Vector2 position; Core::Vector2 velocity; Core::AnimeHandle bulletAnime_; bool isActive; };

    class GameSetting {
    public:
        int currentFrame{ 0 };
        int inputLockFrame{ Core::INPUT_LOCK_FRAME };
        bool isGameOver{ false };
        bool isGameClear{ false };
        bool isDebugMode{ false };
        void IncrementFrame() { if (currentFrame < INT_MAX) ++currentFrame; else currentFrame = 0; }
        void ResetInputLock(int frames) { inputLockFrame = frames; }
        bool DetectInputLock() { if (inputLockFrame <= 0) return false; --inputLockFrame; return true; }
    };

    struct KeyTimestamp { int up = 0, down = 0, left = 0, right = 0, aBtn = 0, bBtn = 0, xBtn = 0, yBtn = 0, enterBtn = 0, escBtn = 0; };

    class KeyBoard {
    public:
        void PollKeyboard(KeyBoard& kb, Game::GameSetting setting);
        char keys[256]{ 0 };
        char preKeys[256]{ 0 };
        KeyTimestamp ktt{};
    };

    class Mouse {
    public:
        void Init(Core::ResourceManager* rm);
        void Update();
        void Render() const;
        const Core::Vector2& GetPosition() const { return position_; }
        bool IsLeftButtonPressed() const;
        bool IsRightButtonPressed() const;
        void PlayClickSound();
        bool IsVisible() const { return isVisible_; }
        void Open() { isEnabled_ = true; isVisible_ = true; }
        void Close() { isEnabled_ = false; isVisible_ = false; }
        void SetVisible(bool flag) { isVisible_ = flag; }
        bool IsEnabled() const { return isEnabled_; }
        bool isSelected_{ false };

    private:
        bool isVisible_{ true };
        bool isEnabled_{ true };
        Core::Vector2 position_{ 0.0f, 0.0f };
        Core::AnimeHandle cursorHandle_;
        Core::AudioResource clickHandle_;
        bool leftPressed_{ false };
        bool leftTriggered_{ false };
        bool rightPressed_{ false };
        bool rightTriggered_{ false };
    };

    void RefreshInputDirTimestamp(KeyBoard& keyBoard, GameSetting setting) {
        auto updateTimestamp = [&keyBoard, &setting](int key, int& inputKey) {
            if (keyBoard.keys[key] && inputKey == 0) inputKey = setting.currentFrame;
            else if (!keyBoard.keys[key]) inputKey = 0;
            };
        updateTimestamp(DIK_W, keyBoard.ktt.up);
        updateTimestamp(DIK_S, keyBoard.ktt.down);
        updateTimestamp(DIK_A, keyBoard.ktt.left);
        updateTimestamp(DIK_D, keyBoard.ktt.right);
        updateTimestamp(DIK_J, keyBoard.ktt.aBtn);
        updateTimestamp(DIK_K, keyBoard.ktt.bBtn);
        updateTimestamp(DIK_RETURN, keyBoard.ktt.enterBtn);
        updateTimestamp(DIK_ESCAPE, keyBoard.ktt.escBtn);
    }

    void KeyBoard::PollKeyboard(KeyBoard& kb, GameSetting setting) {
        std::memcpy(kb.preKeys, kb.keys, 256);
        Novice::GetHitKeyStateAll(kb.keys);
        RefreshInputDirTimestamp(kb, setting);
    }

    void Mouse::Init(Core::ResourceManager* rm) {
        (void)rm;
        Novice::SetMouseCursorVisibility(Core::MouseCursor::HIDE_MOUSE);
        isVisible_ = true;
        isEnabled_ = true;
    }

    void Mouse::Update() {
        if (!isVisible_ || !isEnabled_) return;
        int x = 0, y = 0;
        Novice::GetMousePosition(&x, &y);
        position_.x = static_cast<float>(x);
        position_.y = static_cast<float>(y);
    }

    void Mouse::Render() const {
        if (!isVisible_ || !isEnabled_) return;
        Novice::DrawBox(static_cast<int>(position_.x - 32 / 2), static_cast<int>(position_.y - 32 / 2), 32, 32, 0.0f, WHITE, kFillModeWireFrame);
    }

    bool Mouse::IsLeftButtonPressed() const { return Novice::IsPressMouse(Core::MouseButton::LEFT_BUTTON); }
    bool Mouse::IsRightButtonPressed() const { return Novice::IsPressMouse(Core::MouseButton::RIGHT_BUTTON); }
    void Mouse::PlayClickSound() { if (clickHandle_.handle != -1) {} }

    class GameSession {
    public:
        GameSession() : resources(), setting(), mouse(), keyboard(), currentState(Core::SceneState::MAIN) {}
        Core::ResourceManager resources;
        GameSetting setting;
        Mouse mouse;
        KeyBoard keyboard;
        Core::SceneState currentState;
        void ResetInputLock() { setting.ResetInputLock(Core::INPUT_LOCK_FRAME); }
        bool DetectInputLock() { return setting.DetectInputLock(); }
    };

    class EnemyManager {
    public:
        explicit EnemyManager(Game::GameSession* session) : session_(session) {}
        void Init();
        void Update();
        void Render();
        void Shutdown();
        Boss& GetBoss() { return boss_; }
        std::vector<Bullet>& GetBullets() { return bullets_; }

    private:
        Game::GameSession* session_{ nullptr };
        int fireCooldown_{ 0 };
        Boss boss_;
        std::vector<Bullet> bullets_;
    };

    class PlayerManager {
    public:
        explicit PlayerManager(Game::GameSession* session) : session_(session) {}
        void Init();
        void Input(Game::KeyBoard& kb);
        void Update();
        void Render();
        void Shutdown();
        Player& GetPlayer() { return player_; }
        std::vector<Bullet>& GetBullets() { return bullets_; }

    private:
        Game::GameSession* session_{ nullptr };
        int fireCooldown_{ 0 };
        Player player_;
        std::vector<Bullet> bullets_;
    };

    void EnemyManager::Init() {
        boss_.bossAnime_ = session_->resources.CharacterHandle({ Core::Life::ALIVE, Core::Role::BOSS });
        boss_.bossDeathAnime_ = session_->resources.CharacterHandle({ Core::Life::DEATH, Core::Role::BOSS });
        boss_.position = { Core::kWindowWidth / 2 - boss_.bossAnime_.size.x / 2, 20 };
        boss_.velocity = { 0.0f, 0.0f };
        boss_.isActive = true;
        boss_.hp = Core::BOSS_MAX_HP;
        boss_.ammoCount = Core::BOSS_AMMO;
        bullets_.clear();
        for (int i = 0; i < boss_.ammoCount; ++i) {
            Bullet bullet;
            bullet.position = { -100.0f, -100.0f };
            bullet.velocity = { 0.0f, static_cast<float>(Core::BULLET_SPEED) };
            bullet.bulletAnime_ = session_->resources.BulletHandle(Core::Role::BOSS);
            bullet.isActive = false;
            bullets_.push_back(bullet);
        }
    }

    void EnemyManager::Update() {
        if (session_->currentState != Core::SceneState::GAMEPLAY) return;
        if (boss_.isActive) {
            boss_.position.x += boss_.velocity.x;
            boss_.position.y += boss_.velocity.y;
            if (boss_.position.x <= 3 * 64 || boss_.position.x >= Core::kWindowWidth - (boss_.bossAnime_.size.x + 3 * 64)) boss_.velocity.x = -boss_.velocity.x;
            if (boss_.position.y <= 0 || boss_.position.y >= Core::kWindowHeight / 2 - boss_.bossAnime_.size.y) boss_.velocity.y = -boss_.velocity.y;
            if (boss_.velocity.x == 0 && boss_.velocity.y == 0) {
                int dirX = (std::rand() % 2 == 0) ? -1 : 1;
                int dirY = (std::rand() % 2 == 0) ? -1 : 1;
                boss_.velocity.x = static_cast<float>(dirX * (std::rand() % Core::MOVE_SPEED + 1));
                boss_.velocity.y = static_cast<float>(dirY * (std::rand() % Core::MOVE_SPEED + 1));
            }
            int fireTime = session_->setting.currentFrame / 30 % 2;
            if (fireTime == 0 && fireCooldown_ == 0) {
                for (auto& bullet : bullets_) {
                    if (!bullet.isActive) { bullet.isActive = true; fireCooldown_ = Core::FIRE_INTERVAL; break; }
                }
            }
            if (fireCooldown_ > 0) --fireCooldown_; else fireCooldown_ = 0;
            for (auto& bullet : bullets_) {
                if (bullet.isActive) {
                    bullet.position.x += bullet.velocity.x;
                    bullet.position.y += bullet.velocity.y;
                    if (bullet.position.x > Core::kWindowWidth || bullet.position.x < -bullet.bulletAnime_.size.x || bullet.position.y > Core::kWindowHeight || bullet.position.y < -bullet.bulletAnime_.size.y) {
                        bullet.isActive = false;
                        bullet.position = { -100.0f, -100.0f };
                    }
                } else {
                    bullet.position = { boss_.position.x + boss_.bossAnime_.size.x / 2 - bullet.bulletAnime_.size.x / 2, boss_.position.y - bullet.bulletAnime_.size.y };
                }
            }
        }
    }

    void EnemyManager::Render() {
        if (session_->currentState != Core::SceneState::GAMEPLAY) return;
        if (boss_.isActive) {
            int hpRenderX = 200;
            int hpRenderY = 20;
            int remainHp = Core::BOSS_MAX_HP - boss_.hp;
            Novice::DrawBox(static_cast<int>(boss_.position.x + boss_.bossAnime_.size.x), static_cast<int>(boss_.position.y), hpRenderX + 2, hpRenderY + 2, 0, WHITE, kFillModeSolid);
            Novice::DrawBox(static_cast<int>(boss_.position.x + boss_.bossAnime_.size.x) + 1, static_cast<int>(boss_.position.y) + 1, hpRenderX - ((hpRenderX / Core::BOSS_MAX_HP) * remainHp), hpRenderY, 0, RED, kFillModeSolid);
            int animeFrame = session_->setting.currentFrame / 30 % boss_.bossAnime_.resource.frameCount;
            Novice::DrawSpriteRect(static_cast<int>(boss_.position.x), static_cast<int>(boss_.position.y), static_cast<int>(boss_.bossAnime_.posi.x) + (static_cast<int>(boss_.bossAnime_.resource.size.x) / boss_.bossAnime_.resource.frameCount) * animeFrame, static_cast<int>(boss_.bossAnime_.posi.y), static_cast<int>(boss_.bossAnime_.resource.size.x) / boss_.bossAnime_.resource.frameCount, static_cast<int>(boss_.bossAnime_.resource.size.y), boss_.bossAnime_.resource.textureHandle, boss_.bossAnime_.size.x / boss_.bossAnime_.resource.size.x, 1.0f, 0.0f, WHITE);
            for (auto& bullet : bullets_) {
                if (bullet.isActive) Novice::DrawSpriteRect(static_cast<int>(bullet.position.x), static_cast<int>(bullet.position.y), static_cast<int>(bullet.bulletAnime_.posi.x), static_cast<int>(bullet.bulletAnime_.posi.y), static_cast<int>(bullet.bulletAnime_.resource.size.x), static_cast<int>(bullet.bulletAnime_.resource.size.y), bullet.bulletAnime_.resource.textureHandle, bullet.bulletAnime_.size.x / bullet.bulletAnime_.resource.size.x, 1.0f, 0.0f, WHITE);
            }
        } else {
            int animeFrame = session_->setting.currentFrame / 30 % boss_.bossDeathAnime_.resource.frameCount;
            Novice::DrawSpriteRect(static_cast<int>(boss_.position.x), static_cast<int>(boss_.position.y), static_cast<int>(boss_.bossDeathAnime_.posi.x) + (static_cast<int>(boss_.bossDeathAnime_.resource.size.x) / boss_.bossDeathAnime_.resource.frameCount) * animeFrame, static_cast<int>(boss_.bossDeathAnime_.posi.y), static_cast<int>(boss_.bossDeathAnime_.resource.size.x) / boss_.bossDeathAnime_.resource.frameCount, static_cast<int>(boss_.bossDeathAnime_.resource.size.y), boss_.bossDeathAnime_.resource.textureHandle, boss_.bossDeathAnime_.size.x / boss_.bossDeathAnime_.resource.size.x, 1.0f, 0.0f, WHITE);
        }
    }

    void EnemyManager::Shutdown() { bullets_.clear(); }

    void PlayerManager::Init() {
        player_.playerAnime_ = session_->resources.CharacterHandle({ Core::Life::ALIVE, Core::Role::PLAYER });
        player_.playerDeathAnime_ = session_->resources.CharacterHandle({ Core::Life::DEATH, Core::Role::PLAYER });
        player_.position = { Core::kWindowWidth / 2 - player_.playerAnime_.size.x / 2, Core::kWindowHeight - player_.playerAnime_.size.y - 20 };
        player_.velocity = { 0.0f, 0.0f };
        player_.isActive = true;
        player_.hp = Core::PLAYER_MAX_HP;
        player_.ammoCount = Core::PLAYER_AMMO;
        bullets_.clear();
        for (int i = 0; i < player_.ammoCount; ++i) {
            Bullet bullet;
            bullet.position = { -100.0f, -100.0f };
            bullet.velocity = { 0.0f, -static_cast<float>(Core::BULLET_SPEED / 2) };
            bullet.bulletAnime_ = session_->resources.BulletHandle(Core::Role::PLAYER);
            bullet.isActive = false;
            bullets_.push_back(bullet);
        }
    }

    void PlayerManager::Input(Game::KeyBoard& kb) {
        if (!player_.isActive) return;
        if (session_->currentState != Core::SceneState::GAMEPLAY) return;
        Core::Vector2 inputKey = { 0, 0 };
        KeyTimestamp ts = kb.ktt;
        if (ts.up > ts.down) inputKey.y = -1; else if (ts.down > ts.up) inputKey.y = 1; else inputKey.y = 0;
        if (ts.right > ts.left) inputKey.x = 1; else if (ts.left > ts.right) inputKey.x = -1; else inputKey.x = 0;
        double angle = std::atan2(inputKey.y, inputKey.x);
        if (inputKey.x != 0 || inputKey.y != 0) {
            player_.velocity = { static_cast<float>(std::cos(angle) * Core::MOVE_SPEED), static_cast<float>(std::sin(angle) * Core::MOVE_SPEED) };
            player_.position.x += player_.velocity.x;
            player_.position.y += player_.velocity.y;
            player_.position.x = std::min((Core::kWindowWidth - player_.playerAnime_.size.x) - 3 * 64, std::max(static_cast<float>(3 * 64), player_.position.x));
            player_.position.y = std::min(Core::kWindowHeight - player_.playerAnime_.size.y, std::max(0.0f, player_.position.y));
        }
        if (ts.aBtn > 0 && fireCooldown_ == 0) {
            for (auto& bullet : bullets_) {
                if (!bullet.isActive) { bullet.isActive = true; fireCooldown_ = Core::FIRE_INTERVAL; break; }
            }
        }
    }

    void PlayerManager::Update() {
        if (session_->currentState != Core::SceneState::GAMEPLAY) return;
        if (fireCooldown_ > 0) --fireCooldown_; else fireCooldown_ = 0;
        for (auto& bullet : bullets_) {
            if (bullet.isActive) {
                bullet.position.x += bullet.velocity.x;
                bullet.position.y += bullet.velocity.y;
                if (bullet.position.x > Core::kWindowWidth || bullet.position.x < -bullet.bulletAnime_.size.x || bullet.position.y > Core::kWindowHeight || bullet.position.y < -bullet.bulletAnime_.size.y) {
                    bullet.isActive = false;
                    bullet.position = { -100.0f, -100.0f };
                }
            } else {
                bullet.position = { player_.position.x + player_.playerAnime_.size.x / 2 - bullet.bulletAnime_.size.x / 2, player_.position.y - bullet.bulletAnime_.size.y };
            }
        }
    }

    void PlayerManager::Render() {
        if (session_->currentState != Core::SceneState::GAMEPLAY) return;
        if (player_.isActive) {
            int animeFrame = session_->setting.currentFrame / 30 % player_.playerAnime_.resource.frameCount;
            Novice::DrawSpriteRect(static_cast<int>(player_.position.x), static_cast<int>(player_.position.y), static_cast<int>(player_.playerAnime_.posi.x) + (static_cast<int>(player_.playerAnime_.resource.size.x) / player_.playerAnime_.resource.frameCount) * animeFrame, static_cast<int>(player_.playerAnime_.posi.y), static_cast<int>(player_.playerAnime_.resource.size.x) / player_.playerAnime_.resource.frameCount, static_cast<int>(player_.playerAnime_.resource.size.y), player_.playerAnime_.resource.textureHandle, player_.playerAnime_.size.x / player_.playerAnime_.resource.size.x, 1.0f, 0.0f, WHITE);
            for (auto& bullet : bullets_) {
                if (bullet.isActive) Novice::DrawSpriteRect(static_cast<int>(bullet.position.x), static_cast<int>(bullet.position.y), static_cast<int>(bullet.bulletAnime_.posi.x) + (static_cast<int>(bullet.bulletAnime_.resource.size.x) / bullet.bulletAnime_.resource.frameCount) * (session_->setting.currentFrame % bullet.bulletAnime_.resource.frameCount), static_cast<int>(bullet.bulletAnime_.posi.y), static_cast<int>(bullet.bulletAnime_.resource.size.x) / bullet.bulletAnime_.resource.frameCount, static_cast<int>(bullet.bulletAnime_.resource.size.y), bullet.bulletAnime_.resource.textureHandle, bullet.bulletAnime_.size.x / bullet.bulletAnime_.resource.size.x, 1.0f, 0.0f, WHITE);
            }
        } else {
            int animeFrame = session_->setting.currentFrame / 30 % player_.playerDeathAnime_.resource.frameCount;
            Novice::DrawSpriteRect(static_cast<int>(player_.position.x), static_cast<int>(player_.position.y), static_cast<int>(player_.playerDeathAnime_.posi.x) + (static_cast<int>(player_.playerDeathAnime_.resource.size.x) / player_.playerDeathAnime_.resource.frameCount) * animeFrame, static_cast<int>(player_.playerDeathAnime_.posi.y), static_cast<int>(player_.playerDeathAnime_.resource.size.x) / player_.playerDeathAnime_.resource.frameCount, static_cast<int>(player_.playerDeathAnime_.resource.size.y), player_.playerDeathAnime_.resource.textureHandle, player_.playerDeathAnime_.size.x / player_.playerDeathAnime_.resource.size.x, 1.0f, 0.0f, WHITE);
        }
    }

    void PlayerManager::Shutdown() { bullets_.clear(); }

} // namespace Game

namespace Game {

    enum class GameEventType {
        None,
        SceneSwitch,
        GameExit,
    };

    struct GameEvent {
        GameEventType type{ GameEventType::None };
        Core::SceneState nextState{ Core::SceneState::MAIN };
    };

    class GameEventQueue {
    public:
        void PushSceneSwitch(Core::SceneState nextState) {
            if (count_ >= kMaxEvents) {
                return;
            }

            events_[count_++] = GameEvent{ GameEventType::SceneSwitch, nextState };
        }

        void PushGameExit() {
            if (count_ >= kMaxEvents) {
                return;
            }

            events_[count_++] = GameEvent{ GameEventType::GameExit, Core::SceneState::MAIN };
        }

        bool Pop(GameEvent& outEvent) {
            if (count_ == 0) {
                outEvent = GameEvent{};
                return false;
            }

            outEvent = events_[0];
            for (int i = 1; i < count_; ++i) {
                events_[i - 1] = events_[i];
            }
            --count_;
            return true;
        }

        void Clear() {
            count_ = 0;
        }

    private:
        static constexpr int kMaxEvents = 64;
        GameEvent events_[kMaxEvents];
        int count_{ 0 };
    };

    class SceneHandler {
    public:
        virtual ~SceneHandler() = default;
        virtual void OnEnter(Core::SceneState previousState) { (void)previousState; }
        virtual void OnExit(Core::SceneState nextState) { (void)nextState; }
        virtual void InitResources(Core::ResourceManager* rm) = 0;
        virtual void OnInput(Game::KeyBoard& kb, Game::Mouse& mo, Game::GameSetting& setting, bool& prevMouseDown) = 0;
        virtual void Update(Game::GameSetting& setting) = 0;
        virtual void Render() = 0;
    };

    class MainSceneHandler final : public SceneHandler {
    public:
        MainSceneHandler(Game::GameSession* session, Game::GameEventQueue* eventQueue)
            : session_(session), eventQueue_(eventQueue) {}

        void InitResources(Core::ResourceManager* rm) override {
            titleText_ = Component::TextSpan({ (Core::kWindowWidth - rm->GetGameTitleHandle().size.x) / 2.0f, 100.0f }, "Title", rm->GetGameTitleHandle());
            startButton_ = Component::Button({ (Core::kWindowWidth - rm->GetStartBtnHandle().size.x) / 2.0f, 500.0f }, "Start", rm->GetStartBtnHandle());
            exitButton_ = Component::Button({ (Core::kWindowWidth - rm->GetExitBtnHandle().size.x) / 2.0f, 580.0f }, "Exit", rm->GetExitBtnHandle());
        }

        void OnInput(Game::KeyBoard& kb, Game::Mouse& mo, Game::GameSetting& setting, bool& prevMouseDown) override {
            (void)kb;
            (void)setting;

            const bool mouseDown = mo.IsLeftButtonPressed();
            const int moX = static_cast<int>(mo.GetPosition().x);
            const int moY = static_cast<int>(mo.GetPosition().y);

            if (startButton_.HandleMouse(moX, moY, mouseDown, prevMouseDown)) {
                eventQueue_->PushSceneSwitch(Core::SceneState::GAMEPLAY);
                startButton_.SetSelected(false);
                mo.PlayClickSound();
            } else if (exitButton_.HandleMouse(moX, moY, mouseDown, prevMouseDown)) {
                exitButton_.SetSelected(false);
                eventQueue_->PushGameExit();
                mo.PlayClickSound();
            }

            prevMouseDown = mouseDown;
        }

        void Update(Game::GameSetting& setting) override {
            (void)setting;
        }

        void Render() override {
            session_->mouse.Open();
            titleText_.Open();
            startButton_.Open();
            exitButton_.Open();

            titleText_.Render();
            startButton_.Render();
            exitButton_.Render();
        }

    private:
        Game::GameSession* session_{ nullptr };
        Game::GameEventQueue* eventQueue_{ nullptr };
        Component::TextSpan titleText_;
        Component::Button startButton_;
        Component::Button exitButton_;
    };

    class GameplaySceneHandler final : public SceneHandler {
    public:
        GameplaySceneHandler(Game::GameSession* session, Game::GameEventQueue* eventQueue, Game::PlayerManager* playerMgr, Game::EnemyManager* enemyMgr)
            : session_(session), eventQueue_(eventQueue), playerMgr_(playerMgr), enemyMgr_(enemyMgr) {}

        void OnEnter(Core::SceneState previousState) override {
            if (previousState != Core::SceneState::PAUSE) {
                InitManager();
            }
        }

        void OnExit(Core::SceneState nextState) override {
            if (nextState == Core::SceneState::MAIN || nextState == Core::SceneState::GAMEOVER_WIN || nextState == Core::SceneState::GAMEOVER_LOSE) {
                Shutdown();
            }
        }

        void InitResources(Core::ResourceManager* rm) override {
            (void)rm;
        }

        void InitManager() {
            deathWaitFrame_ = 0;
            enemyMgr_->Init();
            playerMgr_->Init();
        }

        void Shutdown() {
            enemyMgr_->Shutdown();
            playerMgr_->Shutdown();
        }

        void OnInput(Game::KeyBoard& kb, Game::Mouse& mo, Game::GameSetting& setting, bool& prevMouseDown) override {
            (void)mo;
            (void)setting;
            (void)prevMouseDown;

            playerMgr_->Input(kb);
        }

        void Update(Game::GameSetting& setting) override {
            (void)setting;

            playerMgr_->Update();
            enemyMgr_->Update();

            Game::Player& player = playerMgr_->GetPlayer();
            Game::Boss& boss = enemyMgr_->GetBoss();

            if (player.isActive && boss.isActive) {
                for (int i = 0; i < playerMgr_->GetBullets().size(); i++) {
                    Game::Bullet& pBullet = playerMgr_->GetBullets()[i];
                    if (!pBullet.isActive) continue;

                    for (int j = 0; j < enemyMgr_->GetBullets().size(); j++) {
                        Game::Bullet& eBullet = enemyMgr_->GetBullets()[j];
                        if (!eBullet.isActive) continue;
                        if (CheckAABB(pBullet.position, pBullet.bulletAnime_.size, eBullet.position, eBullet.bulletAnime_.size)) {
                            pBullet.isActive = false;
                            pBullet.position = { -100.0f, -100.0f };
                            eBullet.isActive = false;
                            eBullet.position = { -100.0f, -100.0f };
                            break;
                        }
                    }

                    if (boss.isActive && CheckAABB(boss.position, boss.bossAnime_.size, pBullet.position, pBullet.bulletAnime_.size)) {
                        pBullet.isActive = false;
                        pBullet.position = { -100.0f, -100.0f };
                        boss.hp -= 1;
                        if (boss.hp <= 0) {
                            boss.isActive = false;
                            deathWaitFrame_ = Core::DEATH_WAIT_TIME;
                        }
                        break;
                    }
                }

                for (int j = 0; j < enemyMgr_->GetBullets().size(); j++) {
                    Game::Bullet& eBullet = enemyMgr_->GetBullets()[j];
                    if (!eBullet.isActive) continue;
                    if (CheckAABB(player.position, player.playerAnime_.size, eBullet.position, eBullet.bulletAnime_.size)) {
                        eBullet.isActive = false;
                        eBullet.position = { -100.0f, -100.0f };

                        player.hp -= 1;
                        if (player.hp <= 0) {
                            player.isActive = false;
                            deathWaitFrame_ = Core::DEATH_WAIT_TIME;
                        }
                        break;
                    }
                }
            } else {
                if (deathWaitFrame_ > 0) {
                    deathWaitFrame_--;
                } else {
                    deathWaitFrame_ = 0;
                    if (!player.isActive) {
                        eventQueue_->PushSceneSwitch(Core::SceneState::GAMEOVER_LOSE);
                    } else if (!boss.isActive) {
                        eventQueue_->PushSceneSwitch(Core::SceneState::GAMEOVER_WIN);
                    }
                }
            }
        }

        void Render() override {
            session_->mouse.Close();
            playerMgr_->Render();
            enemyMgr_->Render();
        }

    private:
        bool CheckAABB(Core::Vector2 posA, Core::Vector2 sizeA, Core::Vector2 posB, Core::Vector2 sizeB) {
            return (posA.x < posB.x + sizeB.x &&
                posA.x + sizeA.x > posB.x &&
                posA.y < posB.y + sizeB.y &&
                posA.y + sizeA.y > posB.y);
        }

        Game::GameSession* session_{ nullptr };
        Game::GameEventQueue* eventQueue_{ nullptr };
        Game::PlayerManager* playerMgr_{ nullptr };
        Game::EnemyManager* enemyMgr_{ nullptr };
        int deathWaitFrame_{ 0 };
    };

    class ResultSceneHandler final : public SceneHandler {
    public:
        ResultSceneHandler(Game::GameSession* session, Game::GameEventQueue* eventQueue)
            : session_(session), eventQueue_(eventQueue) {}

        void InitResources(Core::ResourceManager* rm) override {
            reText_ = Component::TextSpan({ (Core::kWindowWidth - rm->GetReTextHandle().size.x) / 2.0f, 300.0f }, "ReGame", rm->GetReTextHandle());
            yesButton_ = Component::Button({ (Core::kWindowWidth - rm->GetYes2NoBtnHandle(true).resource.size.x) / 2.0f - rm->GetYes2NoBtnHandle(true).size.x / 2, 400.0f }, "Yes", rm->GetYes2NoBtnHandle(true));
            noButton_ = Component::Button({ (Core::kWindowWidth - rm->GetYes2NoBtnHandle(false).resource.size.x) / 2.0f + rm->GetYes2NoBtnHandle(false).size.x * 2, 400.0f }, "No", rm->GetYes2NoBtnHandle(false));
            winText_ = Component::TextSpan({ (Core::kWindowWidth - rm->GetWinTextHandle().size.x) / 2.0f, 100.0f }, "Win", rm->GetWinTextHandle());
            loseText_ = Component::TextSpan({ (Core::kWindowWidth - rm->GetLoseTextHandle().size.x) / 2.0f, 100.0f }, "Lose", rm->GetLoseTextHandle());
        }

        void OnInput(Game::KeyBoard& kb, Game::Mouse& mo, Game::GameSetting& setting, bool& prevMouseDown) override {
            (void)kb;
            (void)setting;

            const bool mouseDown = mo.IsLeftButtonPressed();
            const int moX = static_cast<int>(mo.GetPosition().x);
            const int moY = static_cast<int>(mo.GetPosition().y);

            if (yesButton_.HandleMouse(moX, moY, mouseDown, prevMouseDown)) {
                yesButton_.SetSelected(false);
                if (session_->currentState == Core::SceneState::PAUSE) {
                    eventQueue_->PushSceneSwitch(Core::SceneState::GAMEPLAY);
                } else {
                    eventQueue_->PushSceneSwitch(Core::SceneState::MAIN);
                }
                mo.PlayClickSound();
            } else if (noButton_.HandleMouse(moX, moY, mouseDown, prevMouseDown)) {
                noButton_.SetSelected(false);
                if (session_->currentState == Core::SceneState::PAUSE) {
                    eventQueue_->PushSceneSwitch(Core::SceneState::MAIN);
                } else {
                    eventQueue_->PushGameExit();
                }
                mo.PlayClickSound();
            }

            prevMouseDown = mouseDown;
        }

        void Update(Game::GameSetting& setting) override {
            (void)setting;
        }

        void Render() override {
            session_->mouse.Open();

            if (session_->currentState == Core::SceneState::GAMEOVER_WIN) {
                winText_.Open();
                loseText_.Close();
            } else if (session_->currentState == Core::SceneState::GAMEOVER_LOSE) {
                winText_.Close();
                loseText_.Open();
            } else {
                winText_.Close();
                loseText_.Close();
            }

            reText_.Open();
            yesButton_.Open();
            noButton_.Open();

            winText_.Render();
            loseText_.Render();
            reText_.Render();
            yesButton_.Render();
            noButton_.Render();
        }

    private:
        Game::GameSession* session_{ nullptr };
        Game::GameEventQueue* eventQueue_{ nullptr };
        Component::TextSpan reText_;
        Component::Button yesButton_;
        Component::Button noButton_;
        Component::TextSpan winText_;
        Component::TextSpan loseText_;
    };

    class GameManager {
    public:
        explicit GameManager(Game::GameSession* session) : session_(session) {
            enemyMgr_ = new Game::EnemyManager(session_);
            playerMgr_ = new Game::PlayerManager(session_);
            mainScene_ = new Game::MainSceneHandler(session_, &eventQueue_);
            gameplayScene_ = new Game::GameplaySceneHandler(session_, &eventQueue_, playerMgr_, enemyMgr_);
            resultScene_ = new Game::ResultSceneHandler(session_, &eventQueue_);
            currentHandler_ = mainScene_;
        }

        ~GameManager() {
            delete resultScene_;
            delete gameplayScene_;
            delete mainScene_;
            delete enemyMgr_;
            delete playerMgr_;
        }

        void Reset() {
            prevMouseDown_ = false;
            eventQueue_.Clear();
            session_->setting = Game::GameSetting();
            session_->mouse.Close();
            session_->keyboard = Game::KeyBoard();
            gameplayScene_->Shutdown();
            SwitchToScene(Core::SceneState::MAIN);
        }

        void InitResources(Core::ResourceManager* rm) {
            session_->mouse.Init(rm);
            bgAnime_[0] = Component::ImageSpan({ 0, 0 }, rm->MainBGHandle());
            bgAnime_[1] = Component::ImageSpan({ 0, -Core::kWindowHeight }, rm->MainBGHandle());

            mainScene_->InitResources(rm);
            gameplayScene_->InitResources(rm);
            resultScene_->InitResources(rm);
            currentHandler_ = ResolveSceneHandler(session_->currentState);
        }

        void InitManager() {
            gameplayScene_->InitManager();
        }

        void OnInput(Game::KeyBoard& kb, Game::Mouse& mo, Game::GameSetting& setting) {
            if (setting.DetectInputLock()) {
                return;
            }

            if (kb.ktt.escBtn > 0) {
                eventQueue_.PushSceneSwitch(Core::SceneState::PAUSE);
                ProcessEvents(setting);
                return;
            }

            currentHandler_->OnInput(kb, mo, setting, prevMouseDown_);
            ProcessEvents(setting);
        }

        void Update(Game::GameSetting& setting) {
            session_->mouse.Update();
            currentHandler_->Update(setting);
            ProcessEvents(setting);
        }

        void Render() {
            for (auto& bg : bgAnime_) {
                int ly = static_cast<int>(bg.GetPosition().y);
                ly += Core::BG_SPEED;
                bg.SetPosition({ 0, static_cast<float>(ly) });
                bg.Render();
                if (ly >= Core::kWindowHeight) {
                    bg.SetPosition({ 0, -Core::kWindowHeight });
                }
            }

            session_->mouse.Close();
            currentHandler_->Render();
            session_->mouse.Render();
        }

        void Shutdown() {
            gameplayScene_->Shutdown();
        }

    private:
        Game::SceneHandler* ResolveSceneHandler(Core::SceneState state) {
            if (state == Core::SceneState::MAIN) {
                return mainScene_;
            }
            if (state == Core::SceneState::GAMEPLAY) {
                return gameplayScene_;
            }
            return resultScene_;
        }

        void SwitchToScene(Core::SceneState nextState) {
            if (session_->currentState == nextState) {
                return;
            }

            Core::SceneState previousState = session_->currentState;
            SceneHandler* nextHandler = ResolveSceneHandler(nextState);
            if (currentHandler_ != nullptr) {
                currentHandler_->OnExit(nextState);
            }

            session_->currentState = nextState;
            currentHandler_ = nextHandler;
            prevMouseDown_ = false;
            session_->ResetInputLock();

            if (currentHandler_ != nullptr) {
                currentHandler_->OnEnter(previousState);
            }
        }

        void ProcessEvents(Game::GameSetting& setting) {
            GameEvent event;
            while (eventQueue_.Pop(event)) {
                if (event.type == GameEventType::SceneSwitch) {
                    SwitchToScene(event.nextState);
                } else if (event.type == GameEventType::GameExit) {
                    setting.isGameOver = true;
                }
            }
        }

        Game::GameSession* session_{ nullptr };
        Game::EnemyManager* enemyMgr_{ nullptr };
        Game::PlayerManager* playerMgr_{ nullptr };
        Game::MainSceneHandler* mainScene_{ nullptr };
        Game::GameplaySceneHandler* gameplayScene_{ nullptr };
        Game::ResultSceneHandler* resultScene_{ nullptr };
        Game::SceneHandler* currentHandler_{ nullptr };
        Game::GameEventQueue eventQueue_;
        bool prevMouseDown_{ false };
        Component::ImageSpan bgAnime_[2];
    };

} // namespace Game

using namespace Core;

void DebugMode(bool flag) {
    if (!flag) return;

    for (int y = 0; y < Core::kWindowHeight / 10; ++y) {
        int setpCount = y * 10;
        int length = 10;
        if (setpCount % 100 == 0 && y != 0) {
            length = 20;
            Novice::ScreenPrintf(25, setpCount - 10, "%d", y);
        }
        Novice::DrawLine(0, setpCount, length, setpCount, RED);
    }

    for (int x = 0; x < Core::kWindowWidth / 10; ++x) {
        int setpCount = x * 10;
        int length = 10;
        if (setpCount % 100 == 0 && x != 0) {
            length = 20;
            Novice::ScreenPrintf(setpCount - 10, 25, "%d", x);
        }
        Novice::DrawLine(setpCount, 0, setpCount, length, RED);
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Novice::Initialize(Core::kWindowTitle, Core::kWindowWidth, Core::kWindowHeight);

    Game::GameSession session;
    session.resources.LoadAll();
    session.currentState = Core::SceneState::MAIN;

    Game::GameManager mainProcess(&session);
    mainProcess.InitResources(&session.resources);

    while (Novice::ProcessMessage() == 0 && !session.setting.isGameOver) {
        Novice::BeginFrame();

        session.keyboard.PollKeyboard(session.keyboard, session.setting);

        mainProcess.OnInput(session.keyboard, session.mouse, session.setting);
        mainProcess.Update(session.setting);
        mainProcess.Render();

        DebugMode(session.setting.isDebugMode);
        session.setting.IncrementFrame();

        Novice::EndFrame();
    }

    Novice::Finalize();
    return 0;
}
