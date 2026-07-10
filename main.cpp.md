#include "Component/Button.hpp"
#include "Component/Image.hpp"
#include "Component/Text.hpp"
#include "Core/Config.hpp"
#include "Core/ResourceManager.hpp"
#include "Game/EnemyManager.hpp"
#include "Game/GameSession.hpp"
#include "Game/Input.hpp"
#include "Game/PlayerManager.hpp"

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
