#pragma once
#include <type_traits>
#include <cstddef>

// Base class for all GC-managed objects.
class GCObject {
protected:
    int ref_count = 0;

public:
    virtual ~GCObject() = default;

    void incRef() noexcept { ++ref_count; }
    void decRef() noexcept {
        if (--ref_count == 0) {
            destroy();
        }
    }

protected:
    // Derived classes implement correct destruction semantics (delete vs delete[])
    virtual void destroy() noexcept = 0;

public:
    // Unified allocation entry point that dispatches to derived types'
    // allocate_impl. This is a static template because allocation is done
    // before any object instance exists.
    template <typename T, typename... Args>
    static T* allocate(Args&&... args) {
        static_assert(std::is_base_of<GCObject, T>::value,
                      "T must derive from GCObject");
        return T::allocate_impl(std::forward<Args>(args)...);
    }
};
