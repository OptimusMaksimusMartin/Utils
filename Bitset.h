#pragma once

#include <cstdint>
#include <type_traits>
#include <bitset>
#include <iterator>
#include <cassert>
#include <tuple>
#include <algorithm>

static constexpr size_t bits_to_bytes(size_t bits) {
  size_t words = (bits/32) + ((bits % 32 == 0) ? 0 : 1);
  return words*4;
};

template <size_t Nb>
class bitset : public std::bitset<Nb> {
private:
  static constexpr size_t sizeBytes = bits_to_bytes(Nb);
public:

template <class T>
class iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
using iterator_category = std::bidirectional_iterator_tag;
using value_type = std::remove_cv_t<T>;
using difference_type = std::ptrdiff_t;
using pointer = T*;
using reference = T&;

iterator() = default;
iterator( pointer begin, pointer end, pointer current) :
  begin_(begin), end_(end), current_(current) {
  assert(begin != end);
}

iterator(const iterator & it) = default;

void operator=(const iterator & it) {
  begin_ = it.begin_;
  end_ = it.end_;
  current_ = it.current_;
}

T& operator*() {
  return *current_;
}

const T& operator*() const {
  return *current_;
}

bool operator==(const iterator & rhs) const {
  return std::tie(current_, begin_, end_) == std::tie(rhs.current_, rhs.begin_, rhs.end_);
}

bool operator!=(const iterator & rhs) const {
  return std::tie(current_, begin_, end_) != std::tie(rhs.current_, rhs.begin_, rhs.end_);
}

iterator & operator++(){
  assert(current_ !=  end_);
  ++current_;
  return *this;
}

iterator operator++(int) {
  iterator it(*this);
  operator++();
  return it;
}

iterator & operator--(){
  assert(current_ !=  begin_);
  --current_;
  return *this;
}

iterator operator--(int) {
  iterator it(*this);
  operator--();
  return it;
}

iterator & operator+=(ssize_t value) {
  current_ += value;
  assert(current_ > begin_ && current_ < end_);
  return *this;
}

iterator & operator+(ssize_t value) {
  current_ += value;
  assert(current_ > begin_ && current_ < end_);
  return *this;
}

private:
  pointer begin_ { nullptr };
  pointer end_ { nullptr };
  pointer current_ { nullptr };
};

using bitset_it = iterator<uint8_t>;
using const_bitset_it = const iterator<uint8_t>;

bitset_it begin() {
  uint8_t * beg = reinterpret_cast<uint8_t*>(this);
  return bitset_it(beg, beg + sizeBytes, beg);
}

bitset_it end() {
  uint8_t * beg = reinterpret_cast<uint8_t*>(this);
  return bitset_it(beg, beg + sizeBytes, beg + sizeBytes);
}

// Инициализация битсета значением
template <class valType>
bitset & set(valType val) {
  std::bitset<Nb>::reset();
  size_t valSize = sizeof(val);
  const size_t bytes = std::min(valSize, sizeBytes);
  uint8_t * valBeg = reinterpret_cast<uint8_t*>(&val);
  auto copyTo = begin();
  std::copy_n(valBeg, bytes, copyTo);
  return *this;
}

};
