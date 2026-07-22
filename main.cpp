#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <unordered_map>

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;

class Player {
public:
    void Move(float dx, float dy) {
        x_ = std::clamp(x_ + dx, 0.0f, kWindowWidth - width_);
        y_ = std::clamp(y_ + dy, 0.0f, kWindowHeight - height_);
    }

    void Draw(SDL_Renderer* renderer) const {
        const SDL_FRect rect{ x_, y_, width_, height_ };
        SDL_SetRenderDrawColor(renderer, 80, 180, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

private:
    float x_ = 375.0f;
    float y_ = 275.0f;
    float width_ = 50.0f;
    float height_ = 50.0f;
};

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
};

// ConcreteCommand：保存 Receiver 和這次操作需要的參數。
class MoveCommand final : public ICommand {
public:
    MoveCommand(Player& player, float dx, float dy)
        : player_(player), dx_(dx), dy_(dy) {}

    void Execute() override {
        player_.Move(dx_, dy_);
    }

private:
    Player& player_;
    float dx_;
    float dy_;
};


class InputHandler {
public:
    void Bind(SDL_Keycode key, ICommand& command) {
        bindings_[key] = &command;
    }

    void HandleEvent(const SDL_Event& event) const {
        if (event.type != SDL_EVENT_KEY_DOWN || event.key.repeat) {
            return;
        }

        const auto it = bindings_.find(event.key.key);
        if (it != bindings_.end()) {
            it->second->Execute();
        }
    }

private:
    std::unordered_map<SDL_Keycode, ICommand*> bindings_;
};

int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL3 - Command Pattern",
        kWindowWidth,
        kWindowHeight,
        0
    );

    if (window == nullptr) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Client：建立 Receiver、Command，並完成按鍵綁定。
    Player player;
    MoveCommand moveLeft(player, -20.0f, 0.0f);
    MoveCommand moveRight(player, 20.0f, 0.0f);
    MoveCommand moveUp(player, 0.0f, -20.0f);
    MoveCommand moveDown(player, 0.0f, 20.0f);

    InputHandler input;
    input.Bind(SDLK_LEFT, moveLeft);
    input.Bind(SDLK_RIGHT, moveRight);
    input.Bind(SDLK_UP, moveUp);
    input.Bind(SDLK_DOWN, moveDown);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
                running = false;
                continue;
            }

            input.HandleEvent(event);
        }

        SDL_SetRenderDrawColor(renderer, 25, 25, 30, 255);
        SDL_RenderClear(renderer);
        player.Draw(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
