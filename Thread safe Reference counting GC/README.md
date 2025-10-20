# RC_GC_Project

Minimal C++17 project demonstrating a reference-counted GC-style system
that differentiates between single-object and multi-object allocations.

Structure:
```
rc_gc_project/
├─ CMakeLists.txt
├─ include/
│  └─ gc/
│     ├─ GCObject.h
│     └─ GCMulti.h
│     └─ Ref.h
└─ src/
   └─ main.cpp
```

Build:
```
mkdir build
cd build
cmake ..
make
./rc_gc
```

Notes:
- Uses `GCObject::allocate<T>(...)` to create objects. The allocation
  dispatches to the derived type's `allocate_impl` (single vs multi).
- No cycle detection or compaction included. Those will be covered later.
- C++17 required.

Sample terminal output (see docs/sample_output.txt)
