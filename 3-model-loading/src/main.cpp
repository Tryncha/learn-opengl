#include <assimp/version.h>

#include <assimp/Importer.hpp>
#include <iostream>

int main() {
  Assimp::Importer importer{};

  std::cout << "Assimp loaded correctly!\n";
  std::cout << "Version: " << aiGetVersionMajor() << '.' << aiGetVersionMinor()
            << '.' << aiGetVersionRevision() << '\n';

  return 0;
}
