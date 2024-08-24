#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <printf.h>
#include <entt/entt.hpp>
#include <cstdlib>
#include <ctime>

#define WIDTH 1024
#define HEIGHT 768

#define SPEED_LIMIT 600
#define PADDLE_SPEED 550

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};       // Rojo
SDL_Color orange = {0xFF, 0xA5, 0x00, 0xFF};    // Naranja
SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};    // Amarillo
SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};     // Verde
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};      // Azul
SDL_Color purple = {0x80, 0x00, 0x80, 0xFF};    // Morado
SDL_Color cyan = {0x00, 0xFF, 0xFF, 0xFF};      // Cian
SDL_Color magenta = {0xFF, 0x00, 0xFF, 0xFF};   // Magenta
SDL_Color lime = {0x32, 0xCD, 0x32, 0xFF};      // Lima
SDL_Color pink = {0xFF, 0xC0, 0xCB, 0xFF};      // Rosado
SDL_Color brown = {0xA5, 0x2A, 0x2A, 0xFF};     // Marrón
SDL_Color gray = {0x80, 0x80, 0x80, 0xFF};      // Gris



struct SpriteComponent {
  int width;
  int height;
  SDL_Color color;
};

class PaddleSpawnSetypSystem : public SetupSystem {
  void run() {

    int paddle_width = 120;
    int paddle_height = 35;

    Entity* paddle = scene->createEntity("PADDLE", WIDTH / 2, HEIGHT-110); 
    paddle->addComponent<VelocityComponent>(500, 500);
    paddle->addComponent<SpriteComponent>(paddle_width, paddle_height, SDL_Color{255, 255, 255});
    paddle->addComponent<PlayerControlledComponent>();
  }
};



class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    // Inicializar el generador de números aleatorios
    srand(static_cast<unsigned int>(time(nullptr)));

    int brick_width = 120;
    int brick_height = 40;
    int brick_spacing = 5;

    int brickCount = 0;
    SDL_Color arrayColors[] = {red, orange, yellow, green, blue, purple, cyan, magenta, lime, pink, brown, gray};

    for (int row = 0; row < 3; ++row) {
      for (int col = 0; col < 8; ++col) {
        int posX = col * (brick_width + brick_spacing) + WIDTH / 2 - (8 * (brick_width + brick_spacing) / 2);
        int posY = row * (brick_height + brick_spacing) + 20;

        // Seleccionar un color aleatorio
        SDL_Color randomColor = arrayColors[rand() % (sizeof(arrayColors) / sizeof(arrayColors[0]))];

        Entity* square = scene->createEntity("BRICK", posX, posY); 
        square->addComponent<SpriteComponent>(brick_width, brick_height, randomColor);
        square->addComponent<BrickComponent>();
        brickCount++;
      }
    }

    scene->setBrickCount(brickCount);
  }
};

class BallSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* ball = scene->createEntity("BALL", WIDTH/2, HEIGHT/2); 
    ball->addComponent<VelocityComponent>(250, 250);
    ball->addComponent<SpriteComponent>(20, 20, SDL_Color{255, 255, 255});
    ball->addComponent<BallComponent>();
  }
};

class MovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      pos.x += vel.x * dT;
      pos.y += vel.y * dT;
    }
  }
};

class CollisionSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<PositionComponent, VelocityComponent>();

        if (scene->getBrickCount() == 0) {
          printf("¡Ganaste!\n");
          exit(0);
        }

        int paddle_width = 120;
        int paddle_height = 35;
            int brick_width = 120;
    int brick_height = 40;

        for (auto entity : view) {
            auto &pos = view.get<PositionComponent>(entity);
            auto &vel = view.get<VelocityComponent>(entity);

            if (scene->r.any_of<BallComponent>(entity)) {

                limitSpeed(vel);
                
                auto paddleView = scene->r.view<PositionComponent, PlayerControlledComponent>();
                for (auto paddleEntity : paddleView) {
                    auto &paddlePos = paddleView.get<PositionComponent>(paddleEntity);
                    if (checkCollision(pos, paddlePos, paddle_width, paddle_height)) { 
                        vel.y = -vel.y;
                    }
                }

                auto brickView = scene->r.view<PositionComponent, BrickComponent>();
                for (auto brickEntity : brickView) {
                    auto &brickPos = brickView.get<PositionComponent>(brickEntity);
                    if (checkCollision(pos, brickPos, brick_width, brick_height)) {
                        scene->destroyEntity(brickEntity);
                        scene->decreaseBrickCount();

                        vel.y = -vel.y;
                        break;
                    }
                }
            }
        }
    }

    void setScene(Scene *s) {
        scene = s;
    }

private:
    Scene *scene;

    bool checkCollision(const PositionComponent &a, const PositionComponent &b, int width, int height) {
        return !(a.x + width < b.x ||
                 a.x > b.x + width ||
                 a.y + height < b.y ||
                 a.y > b.y + height);
    }

    void limitSpeed(VelocityComponent &vel) {
        if (vel.x > SPEED_LIMIT) vel.x = SPEED_LIMIT;
        if (vel.x < -SPEED_LIMIT) vel.x = -SPEED_LIMIT;

        if (vel.y > SPEED_LIMIT) vel.y = SPEED_LIMIT;
        if (vel.y < -SPEED_LIMIT) vel.y = -SPEED_LIMIT;
    }
};


class WallHitSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();

    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);
      
      int newPosX = pos.x + vel.x * dT;
      int newPosY = pos.y + vel.y * dT;

      if (newPosX < 0 || newPosX + spr.width > 1024) {
        vel.x *= -1.1;
        pos.x = 0 + (newPosX < 0 ? 0 + spr.width : 1024 - spr.width);

      }

      if (newPosY + spr.height > 768) {
        exit(0);
      }
      
      if(newPosY < 0) {
        vel.y *= -1.1;
        pos.y = 0;
      }
      
    }
  }
};

class SquareRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<PositionComponent, SpriteComponent>();
    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);

      SDL_SetRenderDrawColor(renderer, spr.color.r, spr.color.g, spr.color.b, spr.color.a);
      SDL_Rect r = { pos.x, pos.y, spr.width, spr.height };
      SDL_RenderFillRect(renderer, &r);
    }
  }
}; 

class InputSystem : public UpdateSystem {
  void run(float dT) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    auto view = scene->r.view<VelocityComponent, PlayerControlledComponent>();

    for (auto e : view) {
      auto& vel = view.get<VelocityComponent>(e);

      vel.x = 0;
      vel.y = 0;

      if (state[SDL_SCANCODE_LEFT]) {
        vel.x = -PADDLE_SPEED;
      }
      if (state[SDL_SCANCODE_RIGHT]) {
        vel.x = PADDLE_SPEED;
      }
    }
  }
};


class DemoGame : public Game {
  public:
    Scene* sampleScene;
    entt::registry r;

  public:
    DemoGame()
      : Game("SAMPLE", WIDTH, HEIGHT)
    { }

    void setup() {
      
      sampleScene = new Scene("SAMPLE", r);
      addSetupSystem<PaddleSpawnSetypSystem>(sampleScene);
      addSetupSystem<BallSpawnSetupSystem>(sampleScene);
      addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
      addUpdateSystem<MovementSystem>(sampleScene);
      addUpdateSystem<WallHitSystem>(sampleScene);
      addUpdateSystem<InputSystem>(sampleScene);
      addRenderSystem<SquareRenderSystem>(sampleScene);
      addUpdateSystem<CollisionSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 