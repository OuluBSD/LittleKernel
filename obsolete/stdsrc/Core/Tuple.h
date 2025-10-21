// Minimal tuple wrappers

template <class... Ts>
using Tuple = std::tuple<Ts...>;

template <class... Ts>
inline Tuple<Ts...> MakeTuple(Ts&&... xs) { return std::make_tuple(std::forward<Ts>(xs)...); }

template <int I, class... Ts>
inline decltype(auto) Get(std::tuple<Ts...>& t) { return std::get<I>(t); }

template <int I, class... Ts>
inline decltype(auto) Get(const std::tuple<Ts...>& t) { return std::get<I>(t); }

