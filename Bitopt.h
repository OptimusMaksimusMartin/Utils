#pragma once

#include <type_traits>
#include <cstdint>

// Установка маски
template <class T>
void set_mask(T & val, T mask) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  val |= mask;
}

// Сброс маски
template <class T>
void clr_mask(T & val, T mask) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  val &= ~mask;
}

// Получение признака установки маски
template <class T>
bool is_set_mask(T x, T m) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  return (((x) & (m)) == (m));
}

template <class T>
bool is_not_set_mask(T x, T m) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  return (((x) & (m)) != (m));
}

// Получение признака установки данных по маске
template <class T>
bool is_set_data(T x, T m, T d) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  return (((x) & (m)) == (d));
}

template <class T>
bool is_not_set_data(T x, T m, T d) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  return (((x) & (m)) != (d));
}

// Получение маски по номеру бита
template <class T>
bool bit_to_mask(std::size_t b) {
  static_assert(b < sizeof(T), "Bit is to large");
  return (T)(1 << (b));
}

// Установка бита
template <class T>
void set_bit(T & x, std::size_t b) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  ((x) |= bit_to_mask<T>(b));
}

// Сброс бита
template <class T>
void clr_bit(T & x, std::size_t b) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  ((x) &= ~bit_to_mask<T>(b));
}

// Получение признака установки бита
template <class T>
void is_set_bit(T x, std::size_t b) {
  static_assert(std::is_integral<T>::value, "Not integral type");
  return ((x) & bit_to_mask<T>(b));
}

// Вспомогательные макросы получения смещения поля
std::size_t __get_field_offset_byte(uint8_t x) {
  return (
      ((x) & 0x0F)
      ? (
          ((x) & 0x03)
          ? (((x) & 0x01) ? 0 : 1)
          : (((x) & 0x04) ? 2 : 3)
        )
      : (
          ((x) & 0xF0)
          ? (
              ((x) & 0x30)
              ? (((x) & 0x10) ? 4 : 5)
              : (((x) & 0x40) ? 6 : 7)
            )
          : (0)
        )
  );
}

std::size_t __get_field_offset_word(uint16_t x) {
  return (
      ((x) & 0x00FF) ? (__get_field_offset_byte((uint8_t )((uint16_t)(x)     ))    ) : (
      ((x) & 0xFF00) ? (__get_field_offset_byte((uint8_t )((uint16_t)(x) >> 8)) + 8) : (
      (0))));
}

std::size_t __get_field_offset_dword(uint32_t x) {
  return (
      ((x) & 0x0000FFFF) ? (__get_field_offset_word((uint16_t)((uint32_t)(x)      ))     ) : (
      ((x) & 0xFFFF0000) ? (__get_field_offset_word((uint16_t)((uint32_t)(x) >> 16)) + 16) : (
      (0)
  )) );
}

std::size_t __get_field_offset_qword(uint64_t x) {
  return (
          ((x) & 0x00000000FFFFFFFF) ? (__get_field_offset_dword((uint32_t)((uint64_t)(x)      ))     ) : (
          ((x) & 0xFFFFFFFF00000000) ? (__get_field_offset_dword((uint32_t)((uint64_t)(x) >> 32)) + 32) : (
          (0)
      )) );
}

template <class T>
std::size_t get_field_offset(T x) {
  return (
          (sizeof(x) == sizeof(uint32_t)) ? __get_field_offset_dword(x) : (
          (sizeof(x) == sizeof(uint8_t )) ? __get_field_offset_byte (x) : (
          (sizeof(x) == sizeof(uint16_t)) ? __get_field_offset_word (x) : (
          (sizeof(x) == sizeof(uint64_t)) ? __get_field_offset_qword(x) : (
          (0)
      )))) );
}
