#pragma once
#include "gc/GCObject.h"

// Single-object allocation: uses delete this
class GCSingleObject : public GCObject {
protected:
    void destroy() noexcept override {
        delete this;
    }

public:
    template <typename T, typename... Args>
    static T* allocate_impl(Args&&... args) {
        static_assert(std::is_base_of<GCSingleObject, T>::value,
                      "T must derive from GCSingleObject");
        return new T(std::forward<Args>(args)...);
    }
};

// Multi-object allocation: uses delete[] this
class GCMultiObject : public GCObject {
protected:
    void destroy() noexcept override {
        delete[] this;
    }

public:
    template <typename T>
    static T* allocate_impl(size_t count) {
        static_assert(std::is_base_of<GCMultiObject, T>::value,
                      "T must derive from GCMultiObject");
        return new T[count];
    }
};
