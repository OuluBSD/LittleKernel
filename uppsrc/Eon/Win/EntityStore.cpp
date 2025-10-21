#if 0
#include "EcsWin.h"




EntityPtr EntityStore::CreateFromComponentMap(ComponentMap components)
{
    return AddEntity(std::make_shared<Entity>(std::move(components), GetNextId(), m_engine));
}

void EntityStore::Update(double)
{
    Destroyable::PruneFromContainer(&m_objects);
}

EntityPtr EntityStore::AddEntity(EntityPtr obj)
{
    m_objects.push_back(obj);
    return obj;
}

Entity::EntityId EntityStore::GetNextId()
{
    return ++m_nextId;
}

#endif
