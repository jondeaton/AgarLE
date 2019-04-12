#include <iostream>

#include <Engine.hpp>

int main(int argc, char **argv) {

  std::cout << "Agario server not yet implemented!" << std::endl;

  agario::Engine engine(100, 100);

  for (int i = 0; i < 100; i++)
    engine.tick();

  return 0;
}