#include "Ragdoll.h"

Ragdoll::Ragdoll(b2World* world, const b2Vec2& position, float angulo)
    : world(world)
{
    b2Rot rot(angulo);

    // Helper para rotar y trasladar una posición
    auto localToWorld = [&](float x, float y) {
        b2Vec2 local(x, y);
        return position + b2Mul(rot, local);
        };

    b2Body* torso = Box2DHelper::CreateRectangularDynamicBody(world, 4.0f, 8.0f, 1.0f, 0.3f, 0.2f);
    torso->SetTransform(position, angulo);

    b2Body* head = Box2DHelper::CreateCircularDynamicBody(world, 2.0f, 1.0f, 0.3f, 0.2f);
    head->SetTransform(localToWorld(0.0f, -6.0f), angulo);

    b2Body* lLeg = Box2DHelper::CreateRectangularDynamicBody(world, 2.0f, 4.0f, 1.0f, 0.3f, 0.2f);
    lLeg->SetTransform(localToWorld(-1.0f, 7.0f), angulo);

    b2Body* rLeg = Box2DHelper::CreateRectangularDynamicBody(world, 2.0f, 4.0f, 1.0f, 0.3f, 0.2f);
    rLeg->SetTransform(localToWorld(1.0f, 7.0f), angulo);

    b2Body* lArm = Box2DHelper::CreateRectangularDynamicBody(world, 2.0f, 6.0f, 1.0f, 0.3f, 0.2f);
    lArm->SetTransform(localToWorld(-4.0f, -2.0f), angulo);

    b2Body* rArm = Box2DHelper::CreateRectangularDynamicBody(world, 2.0f, 6.0f, 1.0f, 0.3f, 0.2f);
    rArm->SetTransform(localToWorld(4.0f, -2.0f), angulo);

    partes = { head, torso, lLeg, rLeg, lArm, rArm };

    // Joints
    CreateJoint(head, torso, localToWorld(0.0f, -4.0f));       // cuello
    CreateJoint(torso, lLeg, localToWorld(-1.0f, 4.5f));       // pierna izq
    CreateJoint(torso, rLeg, localToWorld(1.0f, 4.5f));        // pierna der
    CreateJoint(torso, lArm, localToWorld(-3.0f, -2.0f));      // brazo izq
    CreateJoint(torso, rArm, localToWorld(3.0f, -2.0f));       // brazo der
}

void Ragdoll::CreateJoint(b2Body* a, b2Body* b, const b2Vec2& anchor) {
	b2RevoluteJointDef jd;
	jd.bodyA = a;
	jd.bodyB = b;
	jd.localAnchorA = a->GetLocalPoint(anchor);
	jd.localAnchorB = b->GetLocalPoint(anchor);
	jd.collideConnected = false;
	b2Joint* joint = world->CreateJoint(&jd);
	joints.push_back(joint);
}

void Ragdoll::ApplyImpulse(const b2Vec2& impulse) {
	for (b2Body* body : partes) {
		body->ApplyLinearImpulseToCenter(impulse, true);
	}
}
