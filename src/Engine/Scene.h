#pragma once

#include <string>
#include <entt/entt.hpp>
#include <SDL2/SDL.h>

class Entity;
class SetupSystem;  // loading
class EventSystem;  // for events + keyboard
class RenderSystem;  // render
class UpdateSystem;  // updates variables

class Scene {
public:
  std::vector<SetupSystem*> setupSystems;
  std::vector<EventSystem*> eventSystems;
  std::vector<RenderSystem*> renderSystems;
  std::vector<UpdateSystem*> updateSystems;

  Scene(const std::string&, entt::registry&);
  ~Scene();

  Entity* createEntity(const std::string&);
  Entity* createEntity(const std::string&,int,int);

  void destroyEntity(entt::entity entity) {
        r.destroy(entity);
    }

  void setup();
  void update(float dT);
  void render(SDL_Renderer* renderer);
  void processEvents(SDL_Event e);

  private:
    int brickCount;

  public:
    void setBrickCount(int count) {
      brickCount = count;
    }

    int getBrickCount() const {
      return brickCount;
    }

    void decreaseBrickCount() {
      brickCount--;
    }

  entt::registry& r;
  std::string name;
};