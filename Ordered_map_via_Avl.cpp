#include <cassert>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

// forward declaration of class, so AVLNode can establish it's "friends" :)
template <typename K, typename T> class AVLMap;
template <typename K, typename T> class AVLIterator;

/*
  Node for holding the key, item, and pointers for a node in the AVL.
  Everything is private, only AVLMap abd AVLIterator have access.
*/
template <typename K, typename T>
class AVLNode {
private:
  AVLNode(const K& key, const T& item,
    AVLNode<K,T>* left, AVLNode<K,T>* right, AVLNode<K,T>* parent, int height) {
      this->key = key;
      this->item = item;
      this->left = left;
      this->right = right;
      this->parent = parent;
      this->height = height;
  }

  // recursively delete the left and right subtree, if they exist
  ~AVLNode() {
    if (this->left) {
      delete this->left;
    }
    if (this->right) {
      delete this->right;
    }
  }

  // recalculate the height of this node
  // assumes the heights of the children are correct
  void recalcHeight() {
    int lh, rh;
    childHeights(lh, rh);
    height = 1+std::max(lh, rh);
  }

  // get the heights of the children and store
  // in the pass-by-reference arguments
  void childHeights(int& lh, int& rh) {
    lh = rh = -1;
    if (left) {
      lh = left->height;
    }
    if (right) {
      rh = right->height;
    }
  }

  K key;
  T item;
  AVLNode<K,T> *left, *right, *parent;
  int height;

  // give access to the AVL map class itself and its iterators
  friend class AVLMap<K,T>;
  friend class AVLIterator<K,T>;
};

/*
  Iterator class for the AVLMap class.

  Supports:
  - key()
  - item(), as an l-value as well
  - prefix and postfix increment
  - == and !=
*/
template <typename K, typename T>
class AVLIterator {
public:
  const K& key() const {
    return this->node->key;
  }

  // allows assignment to the item, eg. iter.item() = 17
  // this will update the item held at by the corresponding key

  const T& item() const {
    return this->node->item;
  }

  T& item(){
    return this->node->item;
  }

  // prefix operator: ++iter
  AVLIterator<K,T> operator++() {
    advance();
    return *this;
  }

  // postfix operator: iter++
  AVLIterator<K,T>& operator++(int) {
    // uses the default copy constructor to copy this->node
    AVLIterator<K,T> tmp(*this);
    advance();
    return tmp;
  }

  bool operator==(const AVLIterator<K,T>& rhs) const {
    return node == rhs.node;
  }

  bool operator!=(const AVLIterator<K,T>& rhs) const {
    return node != rhs.node;
  }

private:
  AVLIterator(AVLNode<K,T> *root) {
    this->node = root;
    if (node != NULL) {
      // if the root of the tree is not empty, go to leftmost node
      while (this->node->left) {
        this->node = node->left;
      }
    }
  }

  void advance() {
    assert(node != NULL);
    if (this->node->right) {
      // if there is a right subtree, go to its leftmost/minimum-key node
      this->node = this->node->right;
      while (this->node->left) {
        this->node = this->node->left;
      }
    }
    else {
      // crawl up parent pointers while this node is the right child of the parent
      const AVLNode<K,T> *old;
      do {
        old = this->node;
        this->node = this->node->parent;
      } while (this->node != NULL && this->node->right == old);
    }
  }

  AVLNode<K,T> *node;

  // needed so AVLMap can access the constructor
  friend class AVLMap<K,T>;
};


/*
  An associative container (map/dict) using an AVL tree.
    The update, remove, [], at, and hasKey operations take O(log n)
    time (using O(log n) comparisons) where n = # entries
    in the tree.

  Assumes:
    - K is totally ordered and can be compared via <, !=, and ==
    - T has a default constructor (i.e. T())
*/

template <typename K, typename T>
class AVLMap {
public:
    // creates an empty AVLMap with 0 items
    AVLMap();

    // deletes all nodes in the AVLMap
    ~AVLMap();

    // add the item with the given key, replacing
    // the old item at that key if the key already exists
    void update(const K& key, const T& item);

    // remove the key and its associated item
    void remove(const K& key);

    // returns true iff the key exists
    bool hasKey(const K& key) const;

    // access the item at the given key, allows assignment
    // as an l-value, eg. tree["Zac"] = 20;
    // where tree is an instance of AVLMap<string, int>
    T& operator[](const K& key);

    // does not create the entry if it does not exist
    const T& at(const K& key) const;

    // returns the size of the tree
    unsigned int size() const;

    // returns an iterator to the first item (ordered by key)
    AVLIterator<K,T> begin() const;

    // returns an iterator signalling the end iterator
    AVLIterator<K,T> end() const;

private:
    AVLNode<K,T> *root;
    unsigned int avlSize;

    // returns a pointer to the node containing the key,
    // or to what its parent node would be if the key does not exist,
    // or NULL if the tree is currently empty
    AVLNode<K,T>* findNode(const K& key) const;

    // assumes at least one child of node is NULL, will delete
    // the node and move its only child (if any) to its place
    void pluckNode(AVLNode<K,T>* node);

    // fix the AVL property at this node and, recursively, all
    // nodes above it
    void fixUp(AVLNode<K,T>* node);

    // recalculate the height of the node, assuming its children's heights
    // are correct
    void recalcHeight(AVLNode<K,T>* node);

    // left and right rotations, return a pointer to the new root
    // of the subtree after the rotation, assumes the corresponding
    // node->left or node->right are not null
    AVLNode<K,T>* rotateLeft(AVLNode<K,T>* node);
    AVLNode<K,T>* rotateRight(AVLNode<K,T>* node);
};


template <typename K, typename T>
AVLMap<K,T>::AVLMap() {
    this->root = NULL;
    this->avlSize = 0;
}

template <typename K, typename T>
AVLMap<K,T>::~AVLMap() {
    if (this->root != NULL) {
        delete this->root;

        // point to NULL
        this->root = NULL;
        this->avlSize = 0;
    }
}

template <typename K, typename T>
void AVLMap<K,T>::update(const K& key, const T& item) {
    AVLNode<K,T>* node = findNode(key);

    // if there was no node in the tree with this key, create one
    if (node == NULL || node->key != key) {
        AVLNode<K,T> *newNode = new AVLNode<K,T>(key, item, NULL, NULL, node, 0);
        assert(newNode != NULL);

        // change the left or right pointer of the parent node
        // whichever is appropriate to preserve the AVL property
        if (node == NULL) {
            // the tree was empty, so this is the new root
            root = newNode;
        }
        else {
            // the tree was not empty, so put it as the appropriate child of "node"
            if (key < node->key) {
                node->left = newNode;
            }
            else {
                node->right = newNode;
            }
        }
        ++avlSize;

        // now fix the AVL property up the tree
        fixUp(newNode);
    }
    else {
        // the key existed, so just update the item
        node->item = item;
    }
}

template <typename K, typename T>
void AVLMap<K,T>::remove(const K& key) {
    AVLNode<K,T>* node = findNode(key);

    // make sure the key is in the tree
    // we only assume < is implemented for the key type, not necessarily ==
    assert(node != NULL && !(node->key < key || key < node->key));

    // find the maximum-key node in the left subtree of the node to remove
    AVLNode<K,T> *tmp = node->left, *pluck = node;
    while (tmp != NULL) {
        pluck = tmp;
        tmp = tmp->right;
    }
    // pluck is the one to remove, it might be pluck == node if node has
    // no left child

    // copy contents from node we are deleting
    node->key = pluck->key;
    node->item = pluck->item;

    AVLNode<K,T> *pluckParent = pluck->parent;

    // this function will delete a node with no left child and
    // restructure the tree
    pluckNode(pluck);

    // now fix the AVL tree up starting from the parent
    // of the recently-deleted node
    fixUp(pluckParent);
}

template <typename K, typename T>
bool AVLMap<K,T>::hasKey(const K& key) const {

    // "find" the node, and then check it really has the key
    AVLNode<K,T> *node = findNode(key);
    return node != NULL && !(node->key != key);
}

template <typename K, typename T>
T& AVLMap<K,T>::operator[](const K& key) {

    // "find" the node, if not found then create an entry
    // using the default constructor for the item type
    if (!hasKey(key)) {
        update(key, T());
    }

    return findNode(key)->item;
}

template <typename K, typename T>
const T& AVLMap<K,T>::at(const K& key) const {
    const AVLNode<K,T> *node = findNode(key);
    assert(node != NULL && !(node->key != key));

    return node->item;
}

template <typename K, typename T>
unsigned int AVLMap<K,T>::size() const {
    return this->avlSize;
}

template <typename K, typename T>
AVLNode<K,T>* AVLMap<K,T>::findNode(const K& key) const {
    AVLNode<K,T> *node = this->root, *parent = NULL;

    // traverse down the tree, going left and right as appropriate,
    // until the key is found or we fall off of the leaf node
    while (node != NULL && node->key != key) {
        parent = node;
        if (key < node->key) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }

    if (node == NULL) {
        // key not found
        // here, parent == NULL if and only if the tree is empty
        return parent;
    }
    else {
        // key found
        return node;
    }
}

// an AVLIterator is just a wrapper for a pointer to a node
template <typename K, typename T>
AVLIterator<K,T> AVLMap<K,T>::begin() const {
    return AVLIterator<K,T>(this->root);
}

// the NULL pointer represents the end iterator
template <typename K, typename T>
AVLIterator<K,T> AVLMap<K,T>::end() const {
    return AVLIterator<K,T>(NULL);
}


template <typename K, typename T>
void AVLMap<K,T>::pluckNode(AVLNode<K,T>* node) {

    // first find the only child (if any) of "node"
    AVLNode<K,T> *child;
    if (node->left) {
        child = node->left;
        // make sure the node does not have two children
        assert(child->right == NULL);
    }
    else {
        // might still be NULL, meaning we are plucking a leaf node
        child = node->right;
    }

    // adjust the appropriate child pointer of the node's parent
    if (node->parent == NULL) {
        // in this case, we are deleting the root node
        // so set the new root to the child
        this->root = child;
    }
    else if (node->parent->left == node) {
        node->parent->left = child;
    }
    else {
        node->parent->right = child;
    }

    // if we are not deleting a leaf, the child also gets a new parent
    if (child) {
        child->parent = node->parent;
    }

    // ensures ~AVLNode() does not recursively delete other parts of the tree
    node->left = node->right = NULL;

    // the node is now gone!
    delete node;
    --avlSize;
}

template <typename K, typename T>
void AVLMap<K,T>::fixUp(AVLNode<K,T> *node) {
    // keep climbing up the tree until we are past the root
    while (node != NULL) {
        // first make sure the height of node is correctly computed
        node->recalcHeight();

        // now compare the heights of the children
        int lh, rh;
        node->childHeights(lh, rh);

        // should never differ by more than 2, otherwise
        // there was a bug in the code
        assert(abs(lh-rh) <= 2);

        // if there is a violation of the AVL property, perform the
        // appropriate rotation(s)
        // see eClass notes for the different rules for applying rotations
        if (lh == rh+2) {
            // left child is higher

            AVLNode<K,T>* lchild = node->left;
            int llh, lrh;
            lchild->childHeights(llh, lrh);

            if (llh < lrh) {
                rotateLeft(lchild);
            }
            node = rotateRight(node);
        }
        else if (lh+2 == rh) {
            // right child is higher

            AVLNode<K,T>* rchild = node->right;
            int rlh, rrh;
            rchild->childHeights(rlh, rrh);

            if (rlh > rrh) {
                rotateRight(rchild);
            }
            node = rotateLeft(node);
        }

        // whether we rotated or not, "node" is now the
        // root of the subtree we were checking

        // crawl up the tree one step
        node = node->parent;
    }
}

template <typename K, typename T>
AVLNode<K,T>* AVLMap<K,T>::rotateRight(AVLNode<K,T>* node) {
    AVLNode<K,T> *lchild = node->left;
    assert(left != NULL);

    // To track all of these changes, it is best to
    // draw your own picture with all relevant pointers
    // to see these operations change them properly.
    if (node->parent) {
        if (node == node->parent->left) {
            node->parent->left = lchild;
        }
        else {
            node->parent->right = lchild;
        }
    }
    else {
        root = lchild;
    }

    lchild->parent = node->parent;
    node->parent = lchild;
    if (lchild->right) {
        lchild->right->parent = node;
    }

    node->left = lchild->right;
    lchild->right = node;

    node->recalcHeight();
    lchild->recalcHeight();

    return lchild;
}

template <typename K, typename T>
AVLNode<K,T>* AVLMap<K,T>::rotateLeft(AVLNode<K,T>* node) {
    AVLNode<K,T> *rchild = node->right;
    assert(left != NULL);

    // To track all of these changes, it is best to
    // draw your own picture with all relevant pointers
    // to see these operations change them properly.

    if (node->parent) {
        if (node == node->parent->left) {
            node->parent->left = rchild;
        }
        else {
            node->parent->right = rchild;
        }
    }
    else {
        root = rchild;
    }

    rchild->parent = node->parent;
    node->parent = rchild;
    if (rchild->left) {
        rchild->left->parent = node;
    }

    node->right = rchild->left;
    rchild->left = node;

    node->recalcHeight();
    rchild->recalcHeight();

    return rchild;
}

void printTree(const AVLMap<string, int>& tree) {
  for (AVLIterator<string, int> iter = tree.begin(); iter != tree.end(); ++iter) {
    cout << " - " << iter.key() << ' ' << iter.item() << endl;
  }
  cout << endl;
}

int main() {
  AVLMap<string, int> tree;

  while (true) {
    char cmd;
    string name;
    int grade;

    cin >> cmd;
    if (cmd == 'S') {
        cout << tree.size() << endl;
    }
    else if (cmd == 'U') {
      cin >> name >> grade;
      tree[name] = grade;
    }
    else if (cmd == 'F') {
      cin >> name;
      if (tree.hasKey(name)) {
        cout << name << " found with grade " << tree[name] << endl;
      }
      else {
        cout << name << " not found" << endl;
      }
    }
    else if (cmd == 'R') {
      cin >> name;
      if (!tree.hasKey(name)) {
        cout << name << " not found" << endl;
      }
      tree.remove(name);
    }
    else if (cmd == 'P') {
      cout << "Printing" << endl;
      printTree(tree);
    }
    else if (cmd == 'Q') {
      cout << "stopping" << endl;
      return 0;
    }
    else {
      cout << "invalid command" << endl;
      cout << "Possible Commands:" << endl
      << "S - print the size of the map" << endl
      << "U <name> <grade> - update the grade for the name" << endl
      << "F <name> - check if the name is in the tree" << endl
      << "R <name> - remove the entry with the given name" << endl
      << "P - print all entries in the tree, ordered by key" << endl
      << "Q - stop" << endl;

      // eat up the rest of the line
      getline(cin, name);
    }
  }
}
