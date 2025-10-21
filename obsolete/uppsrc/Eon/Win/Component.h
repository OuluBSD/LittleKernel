#if 0
#pragma once


NAMESPACE_UPP


class ComponentStore;
struct Component;
class ComponentMap;
using SharedComponent = std::shared_ptr<Component>;

////////////////////////////////////////////////////////////////////////////////
// Component
// Base abstract class for all Components
// A component is a piece of data/state that can be attached to any Entity
// Components do not contain any significant logic, but helpers/convenience functions are okay
//   e.g. Transform::GetMatrix
struct Component abstract : Destroyable, Enableable
{
    virtual ~Component() = default;

    virtual detail::type_id type() const = 0;
    virtual void CopyTo(Component* component) const = 0;
};

// CRTP implementation helper 
// Usage: struct MyComponent : Component<MyComponent> { /* data members */ };
template<typename T>
struct Component : Component
{
    detail::type_id type() const override {
        return typeid(T);
    }

    void CopyTo(Component* target) const override {
        fail_fast_if(target->type() != type());

        *static_cast<T*>(target) = *static_cast<const T*>(this);
    }
};

////////////////////////////////////////////////////////////////////////////////
// ComponentMap
// An unordered_map of type_id -> SharedComponent
// As well as convenience functions for Adding, Removing, and Getting components
class ComponentMap : public detail::unordered_type_map<SharedComponent>
{
public:
    using detail::unordered_type_map<SharedComponent>::unordered_type_map;

    template<typename ComponentT>
    ComponentT* Get()
    {
        static_assert(detail::is_component<ComponentT>::value, "T should derive from Component");

        auto it = find(typeid(ComponentT));
        fail_fast_if(it == end());
        return static_cast<ComponentT*>(it->second.get());
    }

    template<typename ComponentT>
    ComponentT* TryGet()
    {
        static_assert(detail::is_component<ComponentT>::value, "T should derive from Component");

        auto it = find(typeid(ComponentT));
        if (it != end())
        {
            return static_cast<ComponentT*>(it->second.get());
        }

        return nullptr;
    }

    template<typename ComponentT>
    ComponentT* Add(SharedComponent component)
    {
        static_assert(detail::is_component<ComponentT>::value, "T should derive from Component");

        const detail::type_id componentType = typeid(ComponentT);

        fail_fast_if(component->type() != componentType, "SharedComponent type does not match T");

        auto it = find(componentType);
        fail_fast_if(it != end(), "Can't have duplicate componnets");
        it = insert(it, { componentType, std::move(component) });

        return static_cast<ComponentT*>(it->second.get());
    }

    template<typename ComponentT>
    void Remove()
    {
        static_assert(detail::is_component<ComponentT>::value, "T should derive from Component");

        auto it = find(typeid(ComponentT));
        fail_fast_if(it == end(), "Tried to remove non-existent component");
        it->second->Destroy();
        erase(it);
    }
};


END_UPP_NAMESPACE

#endif
