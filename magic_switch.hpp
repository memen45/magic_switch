/*

MIT License

Copyright (c) 2021 memen45 <memen45@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#ifndef MAGIC_SWITCH_HPP
#define MAGIC_SWITCH_HPP

#include <utility>
#include <array>
#include <type_traits>

template <template <auto...> class Z, class... Enums>
struct magic_switch
{

  // return value of a call to magic_switch(Args...)
  template <class... Args>
  using R = std::result_of_t<Z<(Enums(0))...>(Args...)>;

  // A function pointer for a jump table:
  template <class... Args>
  using F = R<Args...> (*)(Args &&...);

  template <size_t... Args>
  class value_list
  {
    enum
    {
      size = sizeof...(Args),
    };
  };
  template <typename... Args>
  class type_list
  {
    enum
    {
      size = sizeof...(Args),
    };
  };
  template <class T>
  struct wrapper
  {
  };

  // Produces a single function pointer for index I and args Args...
  template <template <auto...> class Indices, size_t... I,
            template <typename...> typename ArgHolder, typename... Args>
  F<Args...> f(wrapper<ArgHolder<Args...>>, wrapper<Indices<I...>>) const
  {
    using ret = R<Args...>;
    return +[](Args &&...args) -> ret
    {
      using Invoke = Z<Enums(I)...>;
      return Invoke{}(std::forward<Args>(args)...);
    };
  }

  // helper class to determine return type of nested array
  template <typename... Args>
  struct arr_type_helper;

  // returns the nested array type
  template <template <typename...> typename EnumHolder, typename Enum, typename... Nums,
            template <typename...> typename ArgHolder, typename... Args>
  struct arr_type_helper<EnumHolder<Enum, Nums...>, ArgHolder<Args...>>
  {
    arr_type_helper(wrapper<EnumHolder<Enum, Nums...>>, wrapper<ArgHolder<Args...>>);
    using type = std::array<typename arr_type_helper<type_list<Nums...>, type_list<Args...>>::type, size_t(Enum::END)>;
  };

  // returns the array element type
  template <template <typename...> typename EnumHolder,
            template <typename...> typename ArgHolder, typename... Args>
  struct arr_type_helper<EnumHolder<>, ArgHolder<Args...>>
  {
    arr_type_helper(wrapper<ArgHolder<Args...>>);
    using type = F<Args...>;
  };

  template <typename... Args>
  using arr_type_helper_t = arr_type_helper<Args...>::type;

  // middle table calls
  template <template <typename...> typename ArgHolder, typename... Args,
            template <typename...> typename EnumHolder, typename Enum, typename... Nums,
            template <auto...> class Indices, size_t... I,
            size_t... Is>
  std::array<typename arr_type_helper<type_list<Enum, Nums...>, type_list<Args...>>::type, sizeof...(Is)>
      table(wrapper<ArgHolder<Args...>>, wrapper<EnumHolder<Enum, Nums...>>, wrapper<Indices<I...>>, std::index_sequence<Is...>)
  {
    return {{table(wrapper<type_list<Args...>>{}, wrapper<type_list<Nums...>>{}, wrapper<value_list<I..., Is>>{}, std::make_index_sequence<size_t(Enum::END)>{})...}};
  };

  // final table call
  template <template <typename...> typename ArgHolder, typename... Args,
            template <typename...> typename EnumHolder, typename... Nums,
            template <auto...> class Indices, size_t... I,
            size_t... Is>
  std::array<typename arr_type_helper<type_list<>, type_list<Args...>>::type, sizeof...(Is)>
      table(wrapper<ArgHolder<Args...>>, wrapper<EnumHolder<Nums...>>, wrapper<Indices<I...>>, std::index_sequence<Is...>)
  {
    return {{f(wrapper<type_list<Args...>>{}, wrapper<value_list<I..., Is>>{})...}};
  };

  // first table call
  template <template <typename...> typename ArgHolder, typename... Args,
            template <typename...> typename EnumHolder, typename Enum, typename... Nums>
  typename arr_type_helper<type_list<Enum, Nums...>, type_list<Args...>>::type
      table(wrapper<ArgHolder<Args...>>, wrapper<EnumHolder<Enum, Nums...>>)
  {
    return table(wrapper<type_list<Args...>>{}, wrapper<type_list<Nums...>>{}, wrapper<value_list<>>{}, std::make_index_sequence<size_t(Enum::END)>{});
  };

  template <size_t todo, typename Ar, typename Enum, typename... Args>
  auto indexer(Ar jump, Enum val, Args &&...args)
  {
    Enum end = Enum(size_t(Enum::END) - 1); // default to the one before END. Other option: make_index_sequence to Enum::END + 1,
    val = (val > end) ? end : val;
    size_t idx = size_t(val);
    if constexpr (todo != 1)
      return indexer<todo - 1>(jump[idx], std::forward<Args>(args)...);
    else
      return jump[idx](std::forward<Args>(args)...);
  }

  template <class... Args>
  R<Args...> operator()(Enums... nums, Args &&...args)
  {
    // a static jump table for this case of Args...:
    static auto jump = table(wrapper<type_list<Args...>>{}, wrapper<type_list<Enums...>>{});
    // Look up the nth entry in the jump table, and invoke it:
    return indexer<sizeof...(Enums)>(jump, nums..., std::forward<Args>(args)...);
  }
};

#endif /* MAGIC_SWITCH_HPP */
