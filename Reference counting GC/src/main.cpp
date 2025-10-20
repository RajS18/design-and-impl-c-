#include <iostream>
#include "gc/GCObject.h"
#include "gc/GCMulti.h"
#include "gc/Ref.h"

// Example user classes (kept minimal, no Node example)
class MyObject : public GCSingleObject {
public:
    int x;
    MyObject(int v) : x(v) {
        std::cout << "MyObject(" << x << ") constructed\n";
    }
    ~MyObject() {
        std::cout << "MyObject(" << x << ") destroyed\n";
    }
};

class MyArray : public GCMultiObject {
public:
    int data;
    MyArray() : data(0) {
        std::cout << "MyArray element constructed\n";
    }
    ~MyArray() {
        std::cout << "MyArray element destroyed\n";
    }
};

int main() {
    std::cout << "=== Single Object Example ===\n";
    {
        MyObject* obj_ptr = GCObject::allocate<MyObject>(42);
        Ref<MyObject> obj(obj_ptr);
        std::cout << "Value: " << obj->x << "\n";
    } // obj goes out of scope -> decRef() -> deletes object

    std::cout << "\n=== Multi Object Example ===\n";
    {
        MyArray* arr_ptr = GCObject::allocate<MyArray>(3);
        Ref<MyArray> arr(arr_ptr);
        arr.get()[0].data = 10;
        arr.get()[1].data = 20;
        std::cout << "arr[0]=" << arr.get()[0].data
                  << ", arr[1]=" << arr.get()[1].data << "\n";
    } // arr goes out of scope -> decRef() -> delete[] array

    std::cout << "\n=== End ===\n";
    return 0;
}
