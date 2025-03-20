#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

//hello
// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Player class
class Player {
public:
    Player(SDL_Renderer* renderer) : speed(5) {
        rect.x = WINDOW_WIDTH / 2;
        rect.y = WINDOW_HEIGHT / 2;
        rect.w = 50;
        rect.h = 50;

        // Load texture (replace with your own character.png)
        texture = IMG_LoadTexture(renderer, "assets/character.png");
        if (!texture) {
            std::cerr << "Failed to load player texture! IMG_Error: " << IMG_GetError() << std::endl;
            // Fallback to a green rectangle if texture fails
            texture = nullptr;
        }
    }

    ~Player() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    void update() {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_LEFT]) rect.x -= speed;
        if (state[SDL_SCANCODE_RIGHT]) rect.x += speed;
        if (state[SDL_SCANCODE_UP]) rect.y -= speed;
        if (state[SDL_SCANCODE_DOWN]) rect.y += speed;

        // Keep player within bounds
        if (rect.x < 0) rect.x = 0;
        if (rect.x + rect.w > WINDOW_WIDTH) rect.x = WINDOW_WIDTH - rect.w;
        if (rect.y < 0) rect.y = 0;
        if (rect.y + rect.h > WINDOW_HEIGHT) rect.y = WINDOW_HEIGHT - rect.h;
    }

    void render(SDL_Renderer* renderer) {
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for player
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_Rect getRect() const { return rect; }

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
};

// Zombie class
class Zombie {
public:
    Zombie(SDL_Renderer* renderer, int x, int y) : speed(2) {
        rect.x = x;
        rect.y = y;
        rect.w = 40;
        rect.h = 40;

        // Load texture (replace with your own zombie.png)
        texture = IMG_LoadTexture(renderer, "assets/zombie.png");
        if (!texture) {
            std::cerr << "Failed to load zombie texture! IMG_Error: " << IMG_GetError() << std::endl;
            // Fallback to a red rectangle if texture fails
            texture = nullptr;
        }
    }

    ~Zombie() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    void update(int playerX, int playerY) {
        // Move towards the player
        if (rect.x < playerX) rect.x += speed;
        if (rect.x > playerX) rect.x -= speed;
        if (rect.y < playerY) rect.y += speed;
        if (rect.y > playerY) rect.y -= speed;
    }

    void render(SDL_Renderer* renderer) {
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for zombies
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_Rect getRect() const { return rect; }

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
};

// Game class
class Game {
public:
    Game() : window(nullptr), renderer(nullptr), game_over(false), player(nullptr) {}

    ~Game() {
        delete player;
        for (auto zombie : zombies) {
            delete zombie;
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
    }

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
            std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow(
            "SDL2 Zombie Game (Single File)",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN
        );
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Initialize player
        player = new Player(renderer);

        // Initialize a few zombies
        zombies.push_back(new Zombie(renderer, 100, 100));
        zombies.push_back(new Zombie(renderer, 700, 500));

        return true;
    }

    void run() {
        if (!init()) {
            return;
        }

        SDL_Event event;
        while (!game_over) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    game_over = true;
                }
            }

            // Update
            player->update();
            SDL_Rect playerRect = player->getRect();
            for (auto zombie : zombies) {
                zombie->update(playerRect.x + playerRect.w / 2, playerRect.y + playerRect.h / 2);
                SDL_Rect zombieRect = zombie->getRect();
                if (SDL_HasIntersection(&playerRect, &zombieRect)) {
                    game_over = true;
                    std::cout << "Game Over! A zombie caught you!" << std::endl;
                }
            }

            // Render
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            player->render(renderer);
            for (auto zombie : zombies) {
                zombie->render(renderer);
            }

            SDL_RenderPresent(renderer);

            SDL_Delay(16); // Roughly 60 FPS
        }
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool game_over;
    Player* player;
    std::vector<Zombie*> zombies;
};

// Main function
int main(int argc, char* argv[]) {
    Game game;
    game.run();
    return 0;
}