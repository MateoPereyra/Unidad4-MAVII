#include "Ragdoll.h"

Ragdoll::Ragdoll(b2World* world, const b2Vec2& position, float angulo)
	: world(world)
{
	b2Body* headBody = Box2DHelper::CreateCircularDynamicBody(world, 2.0f, 1.0f, 0.3f, 0.2f);
	headBody->SetTransform(b2Vec2(10, 85), angulo);
	b2Body* torsoBody = Box2DHelper::CreateRectangularDynamicBody(world, 8, 4, 1.0f, 0.3f, 0.2f);
	torsoBody->SetTransform(b2Vec2(10, 90.5), angulo);
	b2Body* rLegBody = Box2DHelper::CreateRectangularDynamicBody(world, 2, 3, 1.0f, 0.3f, 0.2f);
	rLegBody->SetTransform(b2Vec2(8.0, 94.5), angulo);
	b2Body* lLegBody = Box2DHelper::CreateRectangularDynamicBody(world, 2, 3, 1.0f, 0.3f, 0.2f);
	lLegBody->SetTransform(b2Vec2(12.0, 94.5), angulo);
	b2Body* rArmBody = Box2DHelper::CreateRectangularDynamicBody(world, 2, 5, 1.0f, 0.3f, 0.2f);
	rArmBody->SetTransform(b2Vec2(14, 90.5), angulo);
	b2Body* lArmBody = Box2DHelper::CreateRectangularDynamicBody(world, 2, 5, 1.0f, 0.3f, 0.2f);
	lArmBody->SetTransform(b2Vec2(6, 90.5), angulo);

	partes = { headBody, torsoBody, lLegBody, rLegBody, lArmBody, rArmBody };

	CreateJoint(headBody, torsoBody, headBody->GetWorldCenter() + b2Vec2(0.0f, 0.5f)); 
	CreateJoint(torsoBody, lLegBody, lLegBody->GetWorldCenter() - b2Vec2(0.0f, 0.5f)); 
	CreateJoint(torsoBody, rLegBody, rLegBody->GetWorldCenter() - b2Vec2(0.0f, 0.5f)); 
	CreateJoint(torsoBody, lArmBody, lArmBody->GetWorldCenter() - b2Vec2(0.0f, 0.5f)); 
	CreateJoint(torsoBody, rArmBody, rArmBody->GetWorldCenter() - b2Vec2(0.0f, 0.5f)); 


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
