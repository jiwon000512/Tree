#include <fstream>
#include <iostream>
#include <stack>
#include <string>

using namespace std;

#define MaxSize 100000

struct Node
{
    int key;
    int height;

    struct Node *right;
    struct Node *left;
};

bool insertBST(Node *&T, int newKey);
bool deleteBST(Node *&T, int deleteKey);
Node *getBstNode(int newKey);
void height(Node *st[], int sp);
int noNodes(Node *T);
Node *maxNode(Node *T, Node *st[], int &sp);
Node *minNode(Node *T, Node *st[], int &sp);
void inorderBST(Node *target);

int main()
{
    string line;
    Node *T = NULL;
    ifstream in;
    in.open("BST-input.txt");

    if (in.is_open())
    {
        while (getline(in, line))
        {
            pair<string, int> p;
            p.first = line.substr(0, 1);
            p.second = stoi(line.substr(2, line.size()));

            if (p.first == "i")
            {
                if (!insertBST(T, p.second))
                {
                    cout << p.first << ' ' << p.second << " : The key already exists" << '\n';
                }
            }
            else if (p.first == "d")
            {
                if (!deleteBST(T, p.second))
                {
                    cout << p.first << ' ' << p.second << " : The key does not exist" << '\n';
                }
            }
            inorderBST(T);
            cout << '\n';
        }
    }
}

bool insertBST(Node *&T, int newKey)
{
    Node *p = T;
    Node *q = NULL;
    Node *st[MaxSize];
    int sp = 0;

    //노드 삽입 위치 탐색
    while (p != NULL)
    {
        //이미 해당 키값이 존재
        if (newKey == p->key)
        {
            return false;
        }

        q = p;
        st[sp++] = q;

        if (newKey > p->key)
        {
            p = p->right;
        }
        else
        {
            p = p->left;
        }
    }

    //새로운 노드 생성
    Node *newNode = getBstNode(newKey);

    //노드 q의 자식으로 새로운 노드 삽입
    if (T == NULL)
    {
        T = newNode;
        return true;
    }
    else if (newKey < q->key)
    {
        q->left = newNode;
    }
    else
    {
        q->right = newNode;
    }

    //각 노드의 위치 재배열
    height(st, sp);

    return true;
}

bool deleteBST(Node *&T, int deleteKey)
{
    Node *p = T;
    Node *q = NULL;
    Node *st[MaxSize];
    int sp = 0;

    //삭제할 노드의 위치 검색
    while (p != NULL && deleteKey != p->key)
    {
        q = p;
        st[sp++] = q;

        if (deleteKey < p->key)
        {
            p = p->left;
        }
        else
        {
            p = p->right;
        }
    }

    //삭제할 원소가 없음
    if (p == NULL)
    {
        return false;
    }

    // degree : 2
    if (p->left != NULL && p->right != NULL)
    {
        st[sp++] = p;
        Node *tempNode = p;

        if (p->left->height < p->right->height)
        {
            p = minNode(p->right, st, sp);
        }
        else if (p->left->height > p->right->height)
        {
            p = maxNode(p->left, st, sp);
        }
        else
        {
            if (noNodes(p->left) >= noNodes(p->right))
            {
                p = maxNode(p->left, st, sp);
            }
            else
            {
                p = minNode(p->right, st, sp);
            }
        }

        tempNode->key = p->key;
        q = st[sp - 1];
    }

    // degree : 0 or 1
    if (p->left == NULL && p->right == NULL)
    {
        if (q == NULL)
        {
            T = NULL;
        }
        else if (q->left == p)
        {
            q->left = NULL;
        }
        else
        {
            q->right = NULL;
        }
    }
    else
    {
        if (p->left != NULL)
        {
            if (q == NULL)
            {
                T = T->left;
            }
            else if (q->left == p)
            {
                q->left = p->left;
            }
            else
            {
                q->right = p->left;
            }
        }
        else
        {
            if (q == NULL)
            {
                T = T->right;
            }
            else if (q->left == p)
            {
                q->left = p->right;
            }
            else
            {
                q->right = p->right;
            }
        }
    }

    delete p;

    //높이 재정렬
    height(st, sp);

    return true;
}

Node *getBstNode(int newKey)
{
    Node *newNode = new Node;
    newNode->key = newKey;
    newNode->height = 0;
    newNode->right = NULL;
    newNode->left = NULL;

    return newNode;
}

void height(Node *st[], int sp)
{
    Node *q = new Node;
    while (sp > 0)
    {
        q = st[--sp];
        if (q->left == NULL && q->right == NULL)
        {
            q->height = 0;
            continue;
        }
        int right = (q->right == NULL ? 0 : q->right->height);
        int left = (q->left == NULL ? 0 : q->left->height);

        q->height = 1 + (right > left ? right : left);
    }
}

int noNodes(Node *T)
{
    if (T == NULL)
    {
        return 0;
    }
    return noNodes(T->left) + noNodes(T->right) + 1;
}

Node *maxNode(Node *T, Node *st[], int &sp)
{
    Node *max = T;
    while (max->right != NULL)
    {
        st[sp++] = max;
        max = max->right;
    }

    return max;
}

Node *minNode(Node *T, Node *st[], int &sp)
{
    Node *min = T;
    while (min->left != NULL)
    {
        st[sp++] = min;
        min = min->left;
    }

    return min;
}

void inorderBST(Node *target)
{
    if (target == NULL)
    {
        return;
    }

    inorderBST(target->left);
    cout << target->key << ' ';
    inorderBST(target->right);
}