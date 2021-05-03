#include <cassert>
#include <iostream>
#include <iomanip>
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

  // these behave the same, but we need both versions
  // the compiler will call the appropriate one (depending on whether
  // the instance is a const instance or not)
  T& operator[](unsigned int index);
  const T& operator[](unsigned int index) const;

  // just return the # of slots allocated to the array
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
  resize(copy.numItems);

  // now the array has the proper size, so just copy the contents of the other
  // array into this array
  for (unsigned int i = 0; i < numItems; i++) {
    array[i] = copy.array[i];
  }
}

// different than the copy constructor, see the lecture slides for a discussion
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


template <typename T>
const T& DynamicArray<T>::operator[](unsigned int index) const {
  assert(index < numItems);
  return array[index];
}


// struct for holding an item and pointers to the next and previous node
template <typename T>
struct ListNode {
  // constructor, while we generally don't add to much functionality to a struct
  // (because it maybe should have been a class in that case), simple constructors
  // are perfectly fine
  // NOTE: there is more convenient syntax if a constructor just copies parameters
  // to its attributes, feel free to look it up if you are curious
  ListNode(const T& l_item, ListNode<T>* l_prev, ListNode<T>* l_next);

  T item;
  ListNode<T> *prev, *next;
};

template <typename T>
ListNode<T>::ListNode(const T& l_item, ListNode<T>* l_prev, ListNode<T>* l_next) {
  item = l_item;
  prev = l_prev;
  next = l_next;
}


// A linked list, just as discussed in the slides.
template <typename T>
class LinkedList {
public:
  LinkedList();

  // copy constructor
  LinkedList(const LinkedList<T>& rhs);

  ~LinkedList();

  // assignment operator
  LinkedList<T>& operator=(const LinkedList& rhs);

  // insert a new item to the front
  void insertFront(const T& item);

  // insert a new item to the back
  void insertBack(const T& item);

  // remove the first node
  void removeFront();

  // remove the last node
  void removeBack();

  // makes the linked list empty (deletes all nodes)
  void clear();

  // assumes the node is in this list
  // will insert the item just before this node in the list
  void insertBefore(const T& item, ListNode<T>* node);

  // assumes the node is in this list
  void removeNode(ListNode<T>* node);

  // obviously returns the size of the list
  unsigned int size() const;

  // SEE COMMENT AT THE TOP OF THE FILE STATING WHY THESE DO NOT
  // RETURN const ListNode<T>* pointers.

  // Get ListNode pointers to the first and last items in the list,
  // respectively. Both return a pointer to NULL if the list is empty.
  ListNode<T>* getFirst() const;
  ListNode<T>* getLast() const;

  // Find and return a pointer to the first node with the item.
  // Returns the NULL pointer if the item is not in the list.
  ListNode<T>* find(const T& item) const;

private:
  ListNode<T> *first, *last;
  unsigned int listSize;
};

template <typename T>
LinkedList<T>::LinkedList() {
  first = last = NULL;
  listSize = 0;
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList& rhs) {
  // first initialize this to an empty list
  first = last = NULL;
  listSize = 0;

  // and then just use the assignment operator
  *this = rhs;
}

template <typename T>
LinkedList<T>::~LinkedList() {
  // the clear method will delete all dynamically-allocated memory
  clear();
}

template <typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& rhs) {
  // empty out this list
  clear();

  // loop through all items in the rhs list and insert the item to the end
  // of this list
  for (ListNode<T>* node = rhs.first; node != NULL; node = node->next) {
    insertBack(node->item);
  }

  return *this;
}

template <typename T>
void LinkedList<T>::insertFront(const T& item) {
  // get a new ListNode to hold the item
  // it points back to NULL and ahead to the first node in current list
  ListNode<T> *node = new ListNode<T>(item, NULL, first);

  // if the list is not empty, have the first node point back to the new node.
  if (first != NULL) {
    first->prev = node;
  }
  else {
    // otherwise, this new node is also the last node
    last = node;
  }

  // update the first node
  first = node;
  listSize++;
}


template <typename T>
void LinkedList<T>::insertBack(const T& item) {
    ListNode<T> *node = new ListNode<T>(item, last, NULL);

    // if the list is not empty, have the last node point to the new node.
    if (last != NULL) {
      last->next = node;
    }
    else {
      // otherwise this new node is also the first node
      first = node;
    }

    // update the last node
    last = node;
    listSize++;
}

template <typename T>
void LinkedList<T>::clear() {
  // just iteratively remove the last node until the list is empty
  while (listSize > 0) {
    removeBack();
  }
}

template <typename T>
void LinkedList<T>::insertBefore(const T& item, ListNode<T> *link) {
  // if the link is the start of the list, just call insertFront
  if (link == first) {
    insertFront(item);
    return;
  }

  // get a new node to hold this item
  ListNode<T> *node = new ListNode<T>(item, link->prev, link);

  // redirect surrounding links, the order you do this is important!
  link->prev->next = node;
  link->prev = node;
  listSize++;
}

template <typename T>
void LinkedList<T>::removeFront() {
  assert(first != NULL);

  ListNode<T> *toDelete = first;

  // if this is not the only item in the list, redirect
  // the prev pointer of the 2nd item to NULL
  if (first != last) {
    first->next->prev = NULL;
  }
  else {
    // otherwise, the list will become empty
    last = NULL;
  }

  // works even if the list had size 1
  first = first->next;

  delete toDelete;
  listSize--;
}

template <typename T>
void LinkedList<T>::removeBack() {
  assert(first != NULL);

  ListNode<T> *toDelete = last;

  // if this is not the only item in the list, redirect
  // the next pointer of the 2nd last item to NULL
  if (first != last) {
    last->prev->next = NULL;
  }
  else {
    // the list will become empty
    first = NULL;
  }

  // works even if the list had size 1
  last = last->prev;

  delete toDelete;
  listSize--;
}


template <typename T>
void LinkedList<T>::removeNode(ListNode<T>* node) {
  // if we are removing the first or the last item, call that function
  if (node == first) {
    removeFront();
    return; // something followed the first node
  }

  if (node == last) {
    removeBack();
    return; // nothing followed the last node
  }

  // bypass the node we are deleting by redirecting pointers of surrounding nodes
  node->prev->next = node->next;
  node->next->prev = node->prev;

  delete node;
  listSize--;
}

template <typename T>
unsigned int LinkedList<T>::size() const {
  return listSize;
}

template <typename T>
ListNode<T>* LinkedList<T>::getFirst() const {
  return first;
}

template <typename T>
ListNode<T>* LinkedList<T>::getLast() const {
  return last;
}

template <typename T>
ListNode<T>* LinkedList<T>::find(const T& item) const {
  // crawl along the list until the item is found
  ListNode<T>* node = first;
  while (node != NULL && node->item != item) {
    node = node->next;
  }

  // returns NULL if the item was not found
  return node;
}

#include "linked_list.h"
#include "dynarray.h"
#include <cassert>

template <typename T>
class HashTable {
public:
  // creates an empty hash table with the given number of buckets.
  // HashTable(unsigned int tableSize);
  HashTable();

  HashTable(const HashTable<T>& copy);
  ~HashTable();

  HashTable<T>& operator=(const HashTable<T>& rhs);

  // void resize(unsigned int newSize);

  // Check if the item already appears in the table.
  bool contains(const T& item) const;

  // Insert the item, do nothing if it is already in the table.
  // Returns true iff the insertion was successful (i.e. the item was not there).
  bool insert(const T& item);

  // Removes the item after checking, via assert, that the item was in the table.
  void remove(const T& item);

  // Returns the number of items in the hash table.
  unsigned int size() const;

  // Returns a dynamic array containing the items in the hash table
  // (in no particular order).
  DynamicArray<T> getItemsArray() const;

private:
  void resize(unsigned int newSize);
  LinkedList<T> *table; // start of the array of linked lists (buckets)
  unsigned int numItems; // # of items in the table
  unsigned int tableSize; // # of buckets

  // Computes the hash table bucket that the item maps into
  // by calling it's .hash() method.
  unsigned int getBucket(const T& item) const;
};

template <typename T>
HashTable<T>::HashTable(){
  // Number of buckets by default
  unsigned int no_of_buckets = 10;

  // Calls the constructor for each linked list
  // So each is initialized properly as an empty list
  table = new LinkedList<T>[no_of_buckets];

  // we are not storing anything
  numItems = 0;
  this->tableSize = no_of_buckets;
}

// template <typename T>
// HashTable<T>::HashTable(unsigned int tableSize) {
//   // make sure there is at least one bucket
//   assert(tableSize > 0);

//   // calls the constructor for each linked list
//   // so each is initialized properly as an empty list
//   table = new LinkedList<T>[tableSize];

//   // we are not storing anything
//   numItems = 0;
//   this->tableSize = tableSize;
// }

template <typename T>
HashTable<T>::HashTable(const HashTable<T>& copy) {
  table = NULL;

  *this = copy;
}

template <typename T>
HashTable<T>::~HashTable() {
  // this will call the destructor for the linked list in each bucket
  delete[] table;
}

template <typename T>
HashTable<T>& HashTable<T>::operator=(const HashTable<T>& rhs) {
  // it could be table == NULL (if calling this from the copy constructor)
  // so best to double-check before we delete the old table
  if (table != NULL) {
    delete[] table;
  }

  tableSize = rhs.tableSize;
  numItems = rhs.numItems;

  table = new LinkedList<T>[tableSize];

  for (int i = 0; i < tableSize; i++) {
    // uses the = operator for the linked lists, so we truly get
    // a copy of each list in the rhs
    table[i] = rhs.table[i];
  }

  return *this;
}

template <typename T>
bool HashTable<T>::contains(const T& item) const {
  unsigned int bucket = getBucket(item);

  return table[bucket].find(item) != NULL;
}
/*
  Resize the number of buckets based on the newSize parameter
  Arguments:
    newSize(unsinged int): The new size of the linked list array or buckets
  Return:
    void
*/
template <typename T>
void HashTable<T>::resize(unsigned int newSize){
  unsigned int newTableSize = 1;
  // If the table size to be incresesed
  if (tableSize < newSize){
    newTableSize = max((tableSize)*2, 10u);
  }
  // If the table size to be reduced
  else if (tableSize > newSize){
    newTableSize = max(tableSize/2, 10u);
  }

  // New memory location to store new buckets
  LinkedList<T> *newTable = new LinkedList<T>[newTableSize];
  // Since hash table is nothing but a array of linked_lists
  for (unsigned int i = 0; i < tableSize; i++){
    // Replacing the linked_list items in the bucket to new bucket
    for (ListNode<T> *start = table[i].getFirst(); start != NULL;
    start = start->next){
      // To compute the bucket with new size
      unsigned int bucket = (start->item.hash() % newTableSize);
      
      newTable[bucket].insertFront((start->item));
    }
  }
  // Since the old array is no longer used
  delete[] table;
  table = NULL;
  // Assigning the new array pointer
  table = newTable;
  // Assigning the new size
  tableSize = newTableSize;
}


template <typename T>
bool HashTable<T>::insert(const T& item) {
  // if the item is here, return false
  if (contains(item)) {
    return false;
  }
  else {
    // Since this is the only method to add items to the hash_table
    if (numItems == tableSize){
      resize(tableSize+1);
    }
    // otherwise, insert it into the front of the list
    // in this bucket and return true
    unsigned int bucket = getBucket(item);
    table[bucket].insertFront(item);
    ++numItems;
    return true;
  }
}

template <typename T>
void HashTable<T>::remove(const T& item) {
  // Since this is the only place to remove the items in the hash_table
  if ((numItems < (tableSize/4u)) && tableSize > 10){
    resize(tableSize-1);
  }
  unsigned int bucket = getBucket(item);

  ListNode<T>* link = table[bucket].find(item);

  // make sure the item was in the list
  assert(link != NULL);

  table[bucket].removeNode(link);

  --numItems;
}

template <typename T>
unsigned int HashTable<T>::size() const {
  return numItems;
}

template <typename T>
DynamicArray<T> HashTable<T>::getItemsArray() const {
  DynamicArray<T> array;

  // go through each bucket
  for (int i = 0; i < tableSize; i++) {
    // and crawl along the list in the bucket to add the items to the array
    ListNode<T>* node = table[i].getFirst();
    while (node != NULL) {
      array.pushBack(node->item);
      node = node->next;
    }
  }

  // returns a copy of the array because it is a local variable
  return array;
}


template <typename T>
unsigned int HashTable<T>::getBucket(const T& item) const {
  return item.hash() % tableSize;
}


struct StudentRecord {
  char name[20];
  unsigned int id;
  unsigned int grade;

  // returns a hash of the student struct
  // in this case, we simply return the id which seems very natural
  // as student IDs are (roughly) consecutive so they will be distributed
  // roughly evenly between buckets
  unsigned int hash() const;

  // two records with the same ID will be regarded as the same
  // remember, we only need the != operator for an entry being added
  // to the table
  bool operator!=(const StudentRecord& rhs) const;
};

unsigned int StudentRecord::hash() const {
  return id;
}


bool StudentRecord::operator!=(const StudentRecord& rhs) const {
  return id != rhs.id;
}


void printHashTable(const HashTable<StudentRecord>& table) {
  DynamicArray<StudentRecord> array = table.getItemsArray();

  cout << "Table size: " << table.size() << endl;
  for (int i = 0; i < array.size(); i++) {
    cout << setw(20) << left << array[i].name
         << setw(7) << array[i].id
         << setw(3) << array[i].grade << endl;
  }
}


int main() {
  // create a new table with 20 buckets
  HashTable<StudentRecord> table(20);

  StudentRecord students[] = {
    {"Zac",  12345, 89},
    {"Omid", 87654, 89},
    {"Alexa", 80000, 34},
    {"Siri",  55545, 84},
    {"Google Home", 11111, 84}
  };

  cout << "Inserting 4 entries" << endl;
  table.insert(students[0]);
  table.insert(students[2]);
  table.insert(students[3]);
  table.insert(students[4]);
  printHashTable(table);
  cout << endl;

  cout << "Checking Alexa is there but Omid is not" << endl;
  assert(table.contains(students[2]) == true);
  assert(table.contains(students[1]) == false);
  cout << endl;

  cout << "Removing Zac and adding Omid" << endl;
  table.remove(students[0]);
  table.insert(students[1]);
  printHashTable(table);
  cout << endl;

  cout << "Double-checking we removed and added them" << endl;
  assert(table.contains(students[0]) == false);
  assert(table.contains(students[1]) == true);
  cout << endl;

  cout << "Adding Zac again" << endl;
  table.insert(students[0]);
  assert(table.contains(students[0]) == true);
  printHashTable(table);
  cout << endl;

  cout << "Changing Siri's Grade" << endl;
  // should use the same ID number
  StudentRecord newSiri = {"Siri", 55545, 75};
  // this actually removes the old entry with Siri (i.e. the one with the
  // matching student ID)
  table.remove(newSiri);
  table.insert(newSiri);
  // notice Siri and Zac print in a different order this time,
  // so there really is no natural "ordering" to the entries
  printHashTable(table);
  cout << endl;

  return 0;
}
