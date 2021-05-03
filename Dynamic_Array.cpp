/*
  Implementation of an array that can be resized during run time.
*/

#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;

// A dynamic array that can be resized when desired.
template <typename T>
class DynamicArray {
public:
  // create a new array with the given size
  DynamicArray(unsigned int size = 0);
  ~DynamicArray();

  // copy constructor
  DynamicArray(const DynamicArray& copy);

  // assignment operator overload
  DynamicArray& operator=(const DynamicArray& rhs);

  // add a new entry to the end of the array
  void pushBack(const T& item);

  // resize the array, keeping the items in the current array
  // except for ones that are indexed >= size (if any)
  void resize(unsigned int newSize);

  T& operator[](unsigned int index);

  // returns the # of slots allocated to the array
  unsigned int size() const;

  void insert(unsigned int index, const T& item);

  void erase(unsigned int index);

  T popBack();

  int find(const T& item);

  unsigned int size() const;

private:
  T *array; // the actual array allocated in the heap
  unsigned int numItems;  // number of items in the array, for the user
  unsigned int arraySize; // size of the underlying array in the heap
};

template <typename T>
DynamicArray<T>::DynamicArray(unsigned int size) {
  // just point array to NULL and let resize do the work
  array = NULL;
  resize(size);
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
  delete[] array;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& copy) {
  // first get an array of the appropriate size,
  // since this is a constructor, we treat it as if the array pointer
  // was not initialized at all
  // FURTHER STUDY FOR THE CURIOUS: constructor delegation
  array = NULL;
  resize(numItems);

  // now the array has the proper size, so just copy the contents of the other
  // array into this array
  for (unsigned int i = 0; i < numItems; i++) {
    array[i] = copy.array[i];
  }
}


template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray& rhs) {
  resize(rhs.numItems);

  for (unsigned int i = 0; i < numItems; i++) {
    array[i] = rhs.array[i];
  }

  return *this;
}

template <typename T>
void DynamicArray<T>::resize(unsigned int newSize) {
  // get an array from the heap with twice the new user array size
  // (or 10, if the new size is really small)
  unsigned int newArraySize = max(newSize*2, 10u);

  // get the new array
  T *newArray = new T[newArraySize];

  // if we had an old array (i.e. this was not called from the constructor),
  // copy the contents over to the new array and then delete this array
  if (array != NULL) {
    // copy the old array over until we fill the new array (according to user size)
    // or we copied all contents from the old array
    for (unsigned int i = 0; i < min(numItems, newSize); i++) {
      newArray[i] = array[i];
    }
    delete[] array;
  }

  // update the class members for this new array and point to it now
  numItems = newSize;
  arraySize = newArraySize;
  array = newArray;
}

template <typename T>
unsigned int DynamicArray<T>::size() const {
  return numItems;
}

template <typename T>
void DynamicArray<T>::pushBack(const T& item) {
  // if the dynamic array is already full, resize it
  if (numItems == arraySize) {
    // resize to get enough space for more items
    resize(numItems+1);
    // but we haven't actually put the new item in yet
    numItems--;
  }

  // either way, we now have space to add the item
  // to the end of the user's array
  array[numItems] = item;
  numItems++;
}

template <typename T>
T& DynamicArray<T>::operator[](unsigned int index) {
  assert(index < numItems);
  return array[index];
}

///
template <typename T>
void DynamicArray<T>::insert(unsigned int index, const T& item) {

  assert(index <= numItems);

  if (numItems == arraySize) {
    resize(numItems+1);
    // remember that after resizing we still haven't added the new item yet
    numItems--;
  }

  // next we want to shift everything in our array one item to the right
  for (int i = numItems; i >= index; i--) {
    array[i] = array[i-1];
  }

  // finally, we can add our new element
  array[index] = item;
  numItems++;
}

///
template <typename T>
void DynamicArray<T>::erase(unsigned int index) {
  // first lets make sure the index they gave us is in range of our array
  assert(0 <= index < numItems);

  // now that we know we are able to get rid of an item, we just have to shift everything left and overwrite the old values
  for (int i = index; i < numItems - 1; i++) {
    array[i] = array[i+1];
  }

  // finally we can reduce the number of items
  numItems--;

  // check to see if the user's array is much smaller than the array in the heap
  if (numItems < arraySize && arraySize > 10) {
    resize(numItems);
  }
}

///
template <typename T>
T DynamicArray<T>::popBack() {
  // first thing we want to do is ensure there are actually items in the array
  assert(numItems > 0);
  // we should copy the item we want so that it can be returned
  T item = array[numItems - 1];

  // new get rid of the item: can you reuse any code?
  erase(numItems - 1);

  // finally, return the item that you created before
  return item;
}

///
template <typename T>
int DynamicArray<T>::find(const T& item) {
  // lets first just look for the item in the array
  for (int i = 0; i < numItems; i++) {
    // if we find it, then we should return that index
    if (item == array[i]) {
      return i;
    }
  }
  // if we haven't found it, we should return -1
  return -1;
}


void dumpArray(DynamicArray<int> &a) {
  cout << "Size is: " << a.size() << endl;
  cout << "Items:";
  for (unsigned int i = 0; i < a.size(); i++) {
    cout << ' ' << a[i];
  }
  cout << endl << endl;
}

int main() {
  DynamicArray<int> a(10);

  cout << "Might be nonsense since entries are not initialized yet" << endl;
  dumpArray(a);

  for (unsigned int i = 0; i < a.size(); i++) {
    a[i] = 2*i;
  }

  cout << "Should be even numbers" << endl;
  dumpArray(a);

  a.resize(7);

  for (int i = 0; i < 10; i++) {
    a.pushBack(101+i);
  }

  cout << "After resizing to 7 and then pushing 101, 102, ..., 110 to the back" << endl;
  dumpArray(a);

  // testing copy constructor
  DynamicArray<int> b(a);

  a[0] = -4;
  cout << "Printing the original array again, but with a[0] = -4" << endl;
  dumpArray(a);
  cout << "Printing the copy of a[] that was made before a[0] was set to -4" << endl;
  dumpArray(b);

  // testing assignment operator
  b = a;
  a.resize(3);

  cout << "Printing the new 'a', which was resized to 3" << endl;
  dumpArray(a);
  cout << "Now printing b, which was assigned to be a copy of the old 'a'" << endl;
  dumpArray(b);

  return 0;
}
