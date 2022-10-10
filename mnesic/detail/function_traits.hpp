#ifndef _FUNCTION_TRAITS_HPP_INCLUDED_
#define _FUNCTION_TRAITS_HPP_INCLUDED_

#include <tuple>

// Function signature
template <typename T> struct signature;

// Free function
template <typename Ret, typename... ArgsT> struct signature<Ret(ArgsT...)> {
  using return_t = Ret;
  using args_t = std::tuple<std::remove_reference_t<ArgsT>...>;
  using fun_t = Ret(ArgsT...);
};

// Function pointer
template <typename Ret, typename... ArgsT> struct signature<Ret (*)(ArgsT...)> {
  using return_t = Ret;
  using args_t = std::tuple<std::remove_reference_t<ArgsT>...>;
  using fun_t = Ret(ArgsT...);
};

// Member function
template <typename Ret, typename C, typename... ArgsT>
struct signature<Ret (C::*)(ArgsT...) const> {
  using return_t = Ret;
  using args_t = std::tuple<std::remove_reference_t<ArgsT>...>;
  using fun_t = Ret(ArgsT...);
};

// Functor
template <typename T> struct signature {
  // TODO some kind of static assert here if operator() isn't defined
  // This would lead to better error message.

  using func_t_internal = decltype(&T::operator());
  using return_t = typename signature<func_t_internal>::return_t;
  using args_t = typename signature<func_t_internal>::args_t;
  using fun_t = typename signature<func_t_internal>::fun_t;
};

template <typename T> using args_of_t = typename signature<T>::args_t;
template <typename T> using return_of_t = typename signature<T>::return_t;

#endif // !_FUNCTION_TRAITS_HPP_INCLUDED_
