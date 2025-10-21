#ifndef _Eon_Draw_Prefabs_h_
#define _Eon_Draw_Prefabs_h_


struct Gun : EntityPrefab<Transform, ModelComponent, ToolComponent>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);

        components.Get<ModelComponentPtr>()->SetPrefabModel(KnownModelNames::Gun);

        return components;
    }
};

struct PaintBrush : EntityPrefab<Transform, ModelComponent, ToolComponent>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);

        components.Get<ModelComponentPtr>()->SetPrefabModel(KnownModelNames::PaintBrush);

        return components;
    }
};

struct DummyToolModel : EntityPrefab<Transform, ModelComponent, ToolComponent>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);

        components.Get<ModelComponentPtr>()->MakeCylinder(vec3(0,0,0), 0.2f, 1.0f);

        return components;
    }
};

struct PaintStroke : EntityPrefab<Transform, ModelComponent, PaintStrokeComponent>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);

        components.Get<ModelComponentPtr>()->Create();

        return components;
    }
};

struct Bullet : EntityPrefab<Transform, ModelComponent, RigidBody, PhysicsBody>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);

        components.Get<RigidBodyPtr>()->acceleration = e.val.FindOwner<Engine>()->Get<PhysicsSystem>()->gravity;
        components.Get<ModelComponentPtr>()->MakeBall(vec3(0,0,0), 0.2f);
        components.Get<ModelComponentPtr>()->color = vec4(0, 0, 1, 1);
        components.Get<TransformPtr>()->size = vec3(0.025f);

        return components;
    }
};

struct Baseball : EntityPrefab<Transform, ModelComponent, RigidBody, PhysicsBody>
{
    static Components Make(Entity& e, const WorldState& ws)
    {
        auto components = EntityPrefab::Make(e, ws);

        components.Get<RigidBodyPtr>()->acceleration = e.val.FindOwner<Engine>()->Get<PhysicsSystem>()->gravity;
        components.Get<ModelComponentPtr>()->SetPrefabModel(KnownModelNames::Baseball);

        return components;
    }
};


#endif
