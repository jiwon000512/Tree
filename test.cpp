#include <iostream>

using namespace std;

struct Node
{
    int key;
    int height;
    int bf;

    struct Node *right;
    struct Node *left;
};

Node *getAVLNode(int newKey)
{
    Node *newNode = new Node;
    newNode->key = newKey;
    newNode->height = 0;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->bf = 0;

    return newNode;
}

void pluss(Node **a)
{
    Node *b = getAVLNode(3);
    *a = b;
    delete *a;
}

int main()
{
    Node *a = getAVLNode(2);

    pluss(&a);

    cout << a->key << '\n';
}