#include "Enemy.h"

namespace game {

Enemy::Enemy(int hp, int damage) : health_(hp), damage_(damage) {}

void Enemy::TakeDamage(int d) {
  health_ -= d;
  if (health_ < 0) {
    health_ = 0;
  }
}

bool Enemy::IsAlive() const { return health_ > 0; }

int Enemy::GetDamage() const { return damage_; }

int Enemy::GetHealth() const { return health_; }

} // namespace game
