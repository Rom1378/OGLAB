#pragma once
#include "PhysicsComponent.hpp"

class SpherePhysics : public PhysicsComponent {
public:
    SpherePhysics(Type t = Type::STATIC);

    void createDynamic(const PxTransform& transform, const PxSphereGeometry& geometry, PxMaterial* material, float mass);


    void init() override;
};
