# RC-GC Project

## Overview

This project implements a **simple reference-counted garbage collection system** in C++17, distinguishing between **single-object** and **multi-object allocations**. It provides automatic memory management via **reference counting** while remaining lightweight, deterministic, and easy to integrate into low-latency or high-performance C++ projects.

This system is suitable for managing objects where **shared ownership** is expected but cyclic references are not present. It uses smart pointers (`Ref<T>`) to track ownership and deallocate objects automatically once the last reference goes out of scope.

---

## Significance of Garbage Collection

In C++, manual memory management (`new`/`delete`) is error-prone. Mistakes can lead to:

* **Memory leaks:** Objects never freed.
* **Use-after-free:** Accessing deleted memory.
* **Dangling pointers:** References to deleted objects.

A garbage collection system like this **automates memory cleanup**:

1. Keeps track of object usage with a **reference count**.
2. Deletes objects automatically when no references remain.
3. Reduces the risk of leaks and dangling pointers without a full tracing GC.

> Unlike tracing GCs (Mark-and-Sweep), reference counting provides **deterministic destruction**.

---

## Best Practices Without Garbage Collection

If you do **not use GC or smart pointers**, you must:

* Carefully pair every `new` with a `delete`.
* Avoid manual pointer sharing across multiple owners unless ownership is clear.
* Use RAII wrappers (like `std::unique_ptr` or `std::shared_ptr`) to ensure automatic destruction.

Reference counting is essentially a **manual implementation of shared ownership**, but you can customize it for:

* High-performance systems (no heap tracking overhead).
* Low-latency applications (deterministic destruction is faster than periodic GC).

---

## Code Explanation

### Core Classes

#### 1. `GCObject`

* Base class for all GC-managed objects.
* Holds the **reference count (`ref_count`)**.
* Provides methods:

  * `incRef()`: Increment reference count.
  * `decRef()`: Decrement reference count and destroy if zero.
* Virtual `destroy()` ensures **correct deletion** based on derived type.

#### 2. `GCSingleObject`

* For **single-object allocations** (`new`).
* `destroy()` calls `delete this`.
* Static template `allocate_impl()` creates a new single object.

#### 3. `GCMultiObject`

* For **multi-object allocations** (`new[]`).
* `destroy()` calls `delete[] this`.
* Static template `allocate_impl(size_t)` creates an array of objects.

#### 4. `Ref<T>`

* Template **smart pointer** managing `GCObject` lifetime.
* Automatically calls `incRef()` on copy, `decRef()` on destruction.
* Supports copy/move semantics.
* Ensures object is destroyed when the last `Ref<T>` goes out of scope.

---

### Example User Classes

```cpp
class MyObject : public GCSingleObject { ... };
class MyArray : public GCMultiObject { ... };
```

* `MyObject`: Demonstrates a single object with a value.
* `MyArray`: Demonstrates multi-object (array) allocation.
* Constructors and destructors print messages for demonstration.

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

* **Allocate objects** using `GCObject::allocate<T>()`.
* **Wrap in `Ref<T>`** to manage lifetime automatically.
* **Access object members** via `->` or `*`.
* **Automatic cleanup** occurs when `Ref<T>` goes out of scope.

---

### Class Diagram

```
          +----------------+
          |   GCObject     |
          +----------------+
          | - ref_count    |
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

1. **Low-latency systems:** Deterministic destruction avoids unpredictable GC pauses.
2. **Shared ownership:** Multiple `Ref<T>` instances can safely reference the same object.
3. **Lightweight custom GC:** Useful when you want fine control over memory management without full tracing.

---

## Notes and Limitations

* **No cycle detection:** Objects forming cyclic references will never be freed.
* **Single-threaded:** Current implementation is not thread-safe. Use `std::atomic<int>` for `ref_count` in multithreaded environments.
* **Polymorphic allocation:** Users must derive from `GCSingleObject` or `GCMultiObject` for proper allocation.

---

## How to Use

1. Include headers:

```cpp
#include "GCObject.h"
#include "GCMulti.h"
#include "Ref.h"
```

2. Define your objects deriving from `GCSingleObject` or `GCMultiObject`.
3. Allocate using `GCObject::allocate<T>()`.
4. Wrap in `Ref<T>` to manage references automatically.
5. Access members as usual.
6. Let objects go out of scope — cleanup is automatic.

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

This README provides a comprehensive explanation of the reference-counted GC system, how it works, its advantages, limitations, and best practices for usage in C++17 projects.
