// Minimal complex number wrappers

template <class T = double>
using Complex = std::complex<T>;

template <class T>
inline T Real(const std::complex<T>& c) { return std::real(c); }

template <class T>
inline T Imag(const std::complex<T>& c) { return std::imag(c); }

template <class T>
inline T Abs(const std::complex<T>& c) { return std::abs(c); }

