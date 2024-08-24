#pragma once
#include <entt/entt.hpp>
#include <printf.h>
#include <utility>
#include "Scene.h"
#include "Components.h"

class Entity {
public:
  Entity(entt::entity h, Scene* s)
    : handle(h), scene(s)
  {
    std::printf("Entity Created\n");
  }

  ~Entity() {
    scene->r.destroy(handle);
    std::printf("Entity Destroyed\n");
  }

  template<typename T>
  void addComponent(auto&&... args) {
    scene->r.emplace<T>(handle, std::forward<decltype(args)>(args)...);
  }

  template<typename T>
  auto& get(auto&&... args) {
    return scene->r.get_or_emplace<T>(handle, std::forward<decltype(args)>(args)...);
  }

private:
  entt::entity handle;
  Scene* scene;
};