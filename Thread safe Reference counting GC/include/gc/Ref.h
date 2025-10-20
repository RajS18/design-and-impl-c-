#pragma once
#include "gc/GCObject.h"
#include <utility>
#include <type_traits>

template <typename T>
class Ref {
    static_assert(std::is_base_of<GCObject, T>::value,
                  "T must derive from GCObject");

    T* ptr = nullptr;

public:
    Ref() = default;

    explicit Ref(T* p) : ptr(p) {
        if (ptr) ptr->incRef();
    }

    Ref(const Ref& other) : ptr(other.ptr) {
        if (ptr) ptr->incRef();
    }

    Ref(Ref&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    Ref& operator=(const Ref& other) {
        if (this == &other) return *this;
        if (ptr) ptr->decRef();
        ptr = other.ptr;
        if (ptr) ptr->incRef();
        return *this;
    }

    Ref& operator=(Ref&& other) noexcept {
        if (this == &other) return *this;
        if (ptr) ptr->decRef();
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    ~Ref() {
        if (ptr) ptr->decRef();
    }

    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* get() const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }
};
