#include "Building.h"

namespace game {

Building::Building(int spawn_interval)
    : spawn_interval_(spawn_interval), counter_(0), health_(10) {} // Инициализируем здоровье

bool Building::AdvanceTurnAndCheckSpawn() {
  counter_++;
  if (counter_ >= spawn_interval_) {
    counter_ = 0;
    return true;
  }
  return false;
}

int Building::GetSpawnInterval() const { return spawn_interval_; }

int Building::GetHealth() const {
    return health_;
}

void Building::TakeDamage(int damage) {
    health_ -= damage;
    if (health_ < 0) {
        health_ = 0;
    }
}

bool Building::IsAlive() const {
    return health_ > 0;
}

} // namespace game
