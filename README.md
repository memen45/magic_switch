# magic_switch
C++ Single header dynamic, non-type template switch implementation. 

## Features
- C++17
- Header-only
- Dependency-free
- Compile-time
- Run-time template switching

## How to use
The magic_switch.hpp header file can be used any time you have a n-dimensional non-type templated class for an enum class and you want to dynamically switch based on a run-time enum variable. Let's see how this would be implemented normally.

### Example 0
First define the enum class for the example:
```cpp
enum class Color { 
  RED, 
  GREEN, 
  BLUE, 
  END,    // END is required for magic_switch to work!
};
```
The following class definition is assumed:
```cpp
template <Color color>
struct FunStore{ void operator()(){} };
```

Normally to switch a template variable at run-time, it is required to manually address each case:
```cpp
volatile Color color;   // may be externally modified
...
if (color == Color::RED)
  FunStore<Color::RED>{}();
else if (color == Color::GREEN)
  FunStore<Color::GREEN>{}();
else if (color == Color::BLUE)
  FunStore<Color::BLUE>{}();
else
  FunStore<Color::END>{}();   // optional if you want default behaviour
```
By including the single header file `magic_switch.hpp`, it is now possible to do all in a single line
```cpp
magic_switch<FunStore, Color>{}(color);
```

### Example 1
Let's start with the simplest form, just define a single non-type template parameter class with an operator()(). 
```cpp
template <Color color>
struct FunStore1 {
  void operator()() {
    std::cout << "Hi from FunStore1<";
    if constexpr (color == Color::RED)
      std::cout << "RED";
    else if (color == Color::GREEN)
      std::cout << "GREEN";
    else if (color == Color::BLUE)
      std::cout << "BLUE";
    else
      std::cout << "END";
    std::cout << ">" << std::endl;
  }
};
```
FunStore1 is a template class and as such, only compile time switching can occur:
```cpp
FunStore1<Color::RED>{}();    // valid c++, will output "Hi from FunStore1<RED>"

Color color = Color::RED;
FunStore1<color>{}();         // won't compile as color is runtime variable
```
Using magic_switch, we can switch at runtime anyways:
```cpp
Color color = Color::RED;
magic_switch<FunStore1, Color>{}(color);    // valid c++, will output "Hi from FunStore1<RED>"
```

### Example 2
Let's introduce a second enum in the non-type template list and again define an operator()().
```cpp
template <Color color1, Color color2>
struct FunStore2 {
  void operator()() {
    std::cout << "Hi from FunStore2<";
    if constexpr (color1 == Color::RED)
      std::cout << "RED";
    else if (color1 == Color::GREEN)
      std::cout << "GREEN";
    else if (color1 == Color::BLUE)
      std::cout << "BLUE";
    else
      std::cout << "END";
    std::cout << ", ";
    if constexpr (color2 == Color::RED)
      std::cout << "RED";
    else if (color2 == Color::GREEN)
      std::cout << "GREEN";
    else if (color2 == Color::BLUE)
      std::cout << "BLUE";
    else
      std::cout << "END";
    std::cout << ">" << std::endl;
  }
};
```
Just make sure you match the number of template arguments in the magic_switch function call as follows:
```cpp
Color color1 = Color::RED;.
Color color2 = Color::BLUE;
magic_switch<FunStore1, Color, Color>{}(color1, color2);    // valid c++, will output "Hi from FunStore2<RED, BLUE>"
```
Actually, the number of enums can be any number you need, so 
```cpp
template<Color color1, Color color2, Color color3, ...>
struct FunStoreN;
```
can be switched at runtime by 
```cpp
magic_switch<FunStoreN, Color, Color, Color, ...>{}(color1, color2, color3, ...);
```

### Example 3
Additionally, any number of arguments can be forwarded to the function call as well. Modifying the first example to accept two function arguments:
```cpp
template <Color color>
struct FunStore1 {
  void operator()(size_t number, size_t & number2) {
    std::cout << "Hi from FunStore1<";
    if constexpr (color == Color::RED)
      std::cout << "RED";
    else if (color == Color::GREEN)
      std::cout << "GREEN";
    else if (color == Color::BLUE)
      std::cout << "BLUE";
    else
      std::cout << "END";
    std::cout << ">{}(" << number << ", " << number2 << ")" << std::endl;
  }
};
```
Function arguments are appended at the end of the argument list, so first the run-time enum values, then the function arguments:
```cpp
Color color = Color::RED;
size_t number = 21;
size_t number2 = 1;
magic_switch<FunStore1, Color>{}(color, number, number2);    // valid c++, will output "Hi from FunStore1<RED>{}(21, 1)"
```
Naturally, it is possible to use any number of enums combined with any number of arguments as needed:

```cpp
magic_switch<FunStore1, Color, Color, Color, ...>{}(color1, color2, color3, ..., arg1, arg2, ...);
```

## Future improvements
Ideally, the syntax should be simplified to 
```cpp
magic_switch<FunStore>(enum1, enum2, ..., arg1, arg2, ...);
```
However, this requires an additional function definition where enum class types can be derived based on the FunStore<> template type, something like:
```cpp
template <Enums... nums> typename FunStore, typename ...Args>
auto magic_switch(Enums... nums, Args ... args) {
  return magic_switch<FunStore, Enums...>(nums..., std::forward<Args...>(args)...);
}
```
Which I have not been able to solve yet. If you have suggestions, feel free to open an issue!

## Licensed under the [MIT License](LICENSE)











