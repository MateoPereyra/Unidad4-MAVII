#pragma once

#include "Game.h"
#include "Box2DHelper.h"

class Ragdoll {
private:
    b2World* world;
    std::vector<b2Body*> partes;
    std::vector<b2Joint*> joints;

    void CreateJoint(b2Body* a, b2Body* b, const b2Vec2& anchor);


public:
    Ragdoll(b2World* world, const b2Vec2& position, float angulo);

    void ApplyImpulse(const b2Vec2& impulse);

};
