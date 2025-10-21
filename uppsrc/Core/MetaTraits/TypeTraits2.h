#ifndef _Core_MetaTraits_TypeTraits2_h_
#define _Core_MetaTraits_TypeTraits2_h_



struct Component;
struct Entity;
class System;


template <typename Base, typename... Ts>
using AllPtrBaseOf = IsAllTrue<std::is_base_of<Base, typename Ts::Type>::value...>;

template<typename T>
using IsComponent = std::is_base_of<Component, T>;

template<typename T>
using IsSystem = std::is_base_of<System, T>;

template<typename... Ts>
using AllComponents = IsAllBaseOf<Component, Ts...>;

template<typename... Ts>
using AllSystems = IsAllBaseOf<System, Ts...>;

template <typename Tuple>
struct TupleAllComponents : std::false_type {};

template <typename... Ts>
struct TupleAllComponents<Tuple<Ts...>> : AllPtrBaseOf<Component, Ts...> {};

template <typename Tuple>
struct RTupleAllComponents : std::false_type {};

template <typename... Ts>
struct RTupleAllComponents<RTuple<Ts...>> : AllPtrBaseOf<Component, Ts...> {};


#endif
