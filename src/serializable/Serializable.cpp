//
// Created by franciscolopez on 02/08/2023.
//

#include "serializable/Serializable.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

Serializable::operator const std::vector<uint8_t>() const {
  return serializedData;
}

Serializable::Serializable(const std::vector<uint8_t> &data)
    : serializedData(data) {}

void Serializable::setVector(std::vector<uint8_t> vector) {
  serializedData = vector;
}
int Serializable::size() {
  return serializedData.size();
}

std::ostream &operator<<(std::ostream &os, const Serializable &serializable) {
  os << "serializedData: ";

  // To print spanish characters and any other character
  setlocale(LC_ALL, "english");

  for (uint8_t i : serializable.serializedData) {
    os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i)
       << " ";
  }

  // Restaura el formato de salida a decimal si es necesario
  os << std::dec << " Parsedinfo: ";
  for (uint8_t i : serializable.serializedData) {
    os << i;
  }

  return os;
}
