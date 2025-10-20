# RC-GC Project (Thread-Safe Version)

## Overview

This project implements a **thread-safe reference-counted garbage collection system** in C++17, distinguishing between **single-object** and **multi-object allocations**. It provides automatic memory management via **atomic reference counting**, making it safe to use in **multithreaded environments** while remaining lightweight and deterministic.

This system is suitable for managing objects where **shared ownership across threads** is expected but cyclic references are not present. It uses smart pointers (`Ref<T>`) to track ownership and deallocate objects automatically once the last reference goes out of scope.

---

## Significance of Garbage Collection

In C++, manual memory management (`new`/`delete`) is error-prone. Mistakes can lead to:

* **Memory leaks:** Objects never freed.
* **Use-after-free:** Accessing deleted memory.
* **Dangling pointers:** References to deleted objects.

A thread-safe reference-counting system automates cleanup and adds **concurrency safety**:

1. Keeps track of object usage with **atomic reference counts**.
2. Deletes objects automatically when no references remain.
3. Prevents race conditions when multiple threads increment or decrement the reference count.

> Unlike traditional tracing GCs, reference counting provides **deterministic destruction**, and the atomic version ensures correctness in multithreaded scenarios.

---

## Best Practices Without Garbage Collection

If you do **not use GC or smart pointers**, you must:

* Carefully pair every `new` with a `delete`.
* Avoid manual pointer sharing across threads unless ownership is clear.
* Use RAII wrappers (like `std::unique_ptr` or `std::shared_ptr`) to ensure automatic destruction.

Thread-safe reference counting allows **safe shared ownership across threads** without explicit locking.

---

## Code Explanation

### Core Classes

#### 1. `GCObject`

* Base class for all GC-managed objects.
* Holds the **atomic reference count (`ref_count`)**.
* Methods:

  * `incRef()`: Atomically increments the reference count.
  * `decRef()`: Atomically decrements the reference count and destroys the object if zero.
* Virtual `destroy()` ensures **correct deletion** based on derived type.

```cpp
#include <atomic>
class GCObject {
protected:
    std::atomic<int> ref_count{0};
public:
    virtual ~GCObject() = default;

    void incRef() noexcept {
        ref_count.fetch_add(1, std::memory_order_relaxed);
    }

    void decRef() noexcept {
        if (ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            destroy();
        }
    }

protected:
    virtual void destroy() noexcept = 0;

public:
    template <typename T, typename... Args>
    static T* allocate(Args&&... args) {
        static_assert(std::is_base_of_v<GCObject, T>, "T must derive from GCObject");
        return T::allocate_impl(std::forward<Args>(args)...);
    }
};
```

#### 2. `GCSingleObject` & `GCMultiObject`

* `GCSingleObject`: For single-object allocations, `destroy()` calls `delete this`.
* `GCMultiObject`: For arrays, `destroy()` calls `delete[] this`.
* Allocation is polymorphic via `allocate_impl()`.

#### 3. `Ref<T>`

* Template smart pointer managing lifetime.
* Automatically calls atomic `incRef()` and `decRef()`.
* Safe to use across threads without additional locks.

---

### Example User Classes

```cpp
class MyObject : public GCSingleObject { ... };
class MyArray : public GCMultiObject { ... };
```

* `MyObject`: Single-object example.
* `MyArray`: Multi-object example.
* Constructors/destructors print messages for demonstration.

---

### Usage Example

```cpp
MyObject* obj_ptr = GCObject::allocate<MyObject>(42);
Ref<MyObject> obj(obj_ptr);
std::cout << obj->x << "\n";

MyArray* arr_ptr = GCObject::allocate<MyArray>(3);
Ref<MyArray> arr(arr_ptr);
arr.get()[0].data = 10;
arr.get()[1].data = 20;
```

* Allocation via `GCObject::allocate<T>()`.
* Wrap in `Ref<T>` for automatic reference tracking.
* Thread-safe: multiple threads can safely share `Ref<T>` instances.

---

### Class Diagram

```
          +----------------+
          |   GCObject     |
          +----------------+
          | - atomic ref_count |
          | + incRef()     |
          | + decRef()     |
          | + allocate<T>()|
          +----------------+
                 / \
                /   \
   +----------------+   +----------------+
   | GCSingleObject |   | GCMultiObject  |
   +----------------+   +----------------+
   | + allocate_impl |   | + allocate_impl|
   | - destroy()    |   | - destroy()    |
   +----------------+   +----------------+
                 ^
                 |
              Ref<T> (template smart pointer)
```

---

## Use-Cases

1. **Multithreaded low-latency systems:** Deterministic destruction with concurrency safety.
2. **Shared ownership:** Multiple threads can safely hold references to the same object.
3. **Lightweight GC:** Fine control over memory management without periodic GC overhead.

---

## Notes and Limitations

* **No cycle detection:** Objects forming cyclic references will never be freed.
* **Thread-safe:** Reference counts are atomic, but cycles are still a limitation.
* **Polymorphic allocation:** Users must derive from `GCSingleObject` or `GCMultiObject`.

---

## How to Use

1. Include headers:

```cpp
#include "GCObject.h"
#include "GCMulti.h"
#include "Ref.h"
```

2. Derive your objects from `GCSingleObject` or `GCMultiObject`.
3. Allocate using `GCObject::allocate<T>()`.
4. Wrap in `Ref<T>` to manage references automatically.
5. Safe for multiple threads.

---

## Sample Terminal Output

```
=== Single Object Example ===
MyObject(42) constructed
Value: 42
MyObject(42) destroyed

=== Multi Object Example ===
MyArray element constructed
MyArray element constructed
MyArray element constructed
arr[0]=10, arr[1]=20
MyArray element destroyed
MyArray element destroyed
MyArray element destroyed

=== End ===
```

---

This README explains the thread-safe reference-counted GC system, its usage, advantages, limitations, and best practices for multithreaded C++ projects.
