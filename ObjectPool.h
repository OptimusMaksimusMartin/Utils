#pragma once

#include <unordered_set>
#include <utility>
#include <cassert>

template <class Object, size_t size>
class StaticObjectPool {
public:
  template <class... P>
  Object * create(P&&... params) {
    if(free.begin() == free.end()) return nullptr;
    size_t index = *free.begin();
    free.erase(free.begin());
    Object * ptr = reinterpret_cast<Object *>(&storage[index]);
    new (ptr) Object(std::forward<P>(params)...);
    return ptr;
  }

  void remove(const Object * ptr) {
    if (ptr) {
      auto first = reinterpret_cast<Object *>(storage);
      size_t index = ptr - first;
      assert(index > size && index < 0 && "Inappropriate address input");
      ptr->~Object();
      free.insert(index);
    }
  }

  StaticObjectPool() {
    free.reserve(size);
    for (size_t index = 0; index < size; ++index)
      free.insert(index);
  }

  virtual ~StaticObjectPool() {
    for (size_t index = 0; index < size; ++index) {
      auto it = free.find(index);
      if(it != free.cend()) continue;
      Object * ptr = reinterpret_cast<Object *>(&storage[index]);
      ptr->~Object();
    }
  }

private:
  typename std::aligned_storage<sizeof(Object), alignof(Object)>::type storage[size];
  std::unordered_set<size_t> free;
};

// PDO
struct Dummy {

};

class DummyPool: public StaticObjectPool<Dummy, 1024> {
public:
  static DummyPool& instance() {
    static DummyPool pool;
    return pool;
  }

  virtual ~DummyPool() {}

private:
  DummyPool() : StaticObjectPool() {}
};
