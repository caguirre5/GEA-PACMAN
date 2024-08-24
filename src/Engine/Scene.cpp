#include "Scene.h"
#include <printf.h>

#include "Entity.h"
#include "Components.h"
#include "Systems.h"

Scene::Scene(const std::string& n, entt::registry& r)
  : name(n), r(r)
{
  std::printf("Scene Initialized: %s\n", n.c_str());
}

Scene::~Scene() {
  setupSystems.clear();
  std::printf("Scene Destroyed: %s\n", name.c_str());
}

Entity* Scene::createEntity(const std::string& n) {
  Entity* entity = new Entity(r.create(), this);
  entity->addComponent<NameComponent>(n);
  return entity;
}

Entity* Scene::createEntity(const std::string& n, int x, int y) {
  Entity* entity = new Entity(r.create(), this);
  entity->addComponent<NameComponent>(n);
  entity->addComponent<PositionComponent>(x, y);

  return entity;
}

void Scene::setup() {
  for (auto sys: setupSystems) {
    sys->run();
  }
}

void Scene::update(float dT) {
  for (auto sys: updateSystems) {
    sys->run(dT);
  }
}

void Scene::processEvents(SDL_Event e) {
  for (auto sys: eventSystems) {
    sys->run(e);
  }
}

void Scene::render(SDL_Renderer* render) {
  for (auto sys: renderSystems) {
    sys->run(render);
  }
}