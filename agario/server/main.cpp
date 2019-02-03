#include <iostream>

#include <Engine.hpp>

int main(int argc, char **argv) {

  std::cout << "Agario server not yet implemented!" << std::endl;

  AgarioEngine engine;

  for (int i = 0; i < 100; i++)
    engine.tick();

  return 0;
}