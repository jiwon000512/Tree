#include <fstream>
#include <iostream>
#include <string>

using namespace std;

#define MaxSize 10000

struct Node
{
    int key;
    int height;
    int bf;

    struct Node *right;
    struct Node *left;
};

void inorderBST(Node *target);
bool insertAVL(Node **T, int newKey);
bool insertBST(Node **T, int newKey, Node *p, Node *q, Node *st[], int &sp);
bool deleteAVL(Node **T, int deleteKey);
Node *deleteBST(Node **T, int deleteKey, Node **p, Node **q, Node *st[], int &sp);
int noNodes(Node *T);
Node *maxNode(Node *T, Node *st[], int &sp);
Node *minNode(Node *T, Node *st[], int &sp);
void checkBalance(Node **T, int newKey, string &rotationType, Node **p, Node **q, Node *st[], int &sp);
void rotateTree(Node **T, string rotationType, Node **p, Node **q);
void resetHB(Node **target, int stopHeight);

int main()
{
    string line;
    Node *T = NULL;
    ifstream in;
    in.open("AVL-input.txt");

    if (in.is_open())
    {
        while (getline(in, line))
        {
            pair<string, int> p;
            p.first = line.substr(0, 1);
            p.second = stoi(line.substr(2, line.size()));
            
            if (p.first == "i")
            {
                if (!insertAVL(&T, p.second))
                {
                    cout << p.first << ' ' << p.second << " : The key already exists" << '\n';
                }
            }        
            else if (p.first == "d")
            {
                if (!deleteAVL(&T, p.second))
                {
                    cout << p.first << ' ' << p.second << " : The key does not exist" << '\n';
                }
            }
            inorderBST(T);
            cout << '\n';
        }

    }
}

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

void inorderBST(Node *target)
{
    if (target == NULL)
    {
        return;
    }

    inorderBST(target->left);
    cout << '(' << target->key << ", " << target->bf << ") ";
    inorderBST(target->right);
}

int setHeight(Node *target)
{
    if (target->right == NULL && target->left == NULL)
    {
        return 0;
    }
    int right = (target->right == NULL ? 0 : target->right->height);
    int left = (target->left == NULL ? 0 : target->left->height);

    return 1 + (right > left ? right : left);
}

int setBF(Node *target)
{
    int right = (target->right == NULL ? 0 : target->right->height + 1);
    int left = (target->left == NULL ? 0 : target->left->height + 1);
    return left-right;
}

bool insertAVL(Node **T, int newKey)
{
    Node *p = *T;
    Node *q = NULL;
    Node *st[MaxSize];
    int sp = 0;

    // newKey의 삽입 위치를 검색하여 삽입 실행함
    bool found = insertBST(T, newKey, p, q, st, sp);
    if (found)
    {
        return false;
    }

    // newKey부터 루트까지 BF다시 계산
    string rotationType;
    checkBalance(T, newKey, rotationType, &p, &q, st, sp);

    //불균형 수정
    if (rotationType.compare("NO") != 0)
    {
        rotateTree(T, rotationType, &p, &q);
    }

    cout << rotationType << ' ';

    return true;
}

bool insertBST(Node **T, int newKey, Node *p, Node *q, Node *st[], int &sp)
{
    //노드 삽입 위치 탐색
    while (p != NULL)
    {
        //이미 해당 키값이 존재
        if (newKey == p->key)
        {
            return true;
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
    Node *y = getAVLNode(newKey);
    //노드 q의 자식으로 새로운 노드 삽입
    if (*T == NULL)
    {
        *T = y;
        return false;
    }
    else if (newKey < q->key)
    {
        q->left = y;
    }
    else
    {
        q->right = y;
    }

    return false;
}

bool deleteAVL(Node **T, int deleteKey)
{
    Node *p = *T;
    Node *q = NULL;
    Node *st[MaxSize];
    int sp = 0;

    q = deleteBST(T, deleteKey, &p, &q, st, sp);
    if(q == NULL)
    {
        return false;
    }
    //키가 삭제된 후 BF다시 계산
    string rotationType;
    checkBalance(T, deleteKey, rotationType, &p, &q, st, sp);

    //불균형 수정
    if (rotationType.compare("NO") != 0)
    {
        rotateTree(T, rotationType, &p, &q);
    }

    cout << rotationType << ' ';

    return true;
}

Node *deleteBST(Node **T, int deleteKey, Node **p, Node **q, Node *st[], int &sp)
{

    //삭제할 노드의 위치 검색
    while (*p != NULL && deleteKey != (*p)->key)
    {
        
        *q = *p;
        st[sp++] = *q;

        if (deleteKey < (*p)->key)
        {
            *p = (*p)->left;
        }
        else
        {
            (*p) = (*p)->right;
        }
    }

    //삭제할 원소가 없음
    if (*p == NULL)
    {
        return NULL;
    }

    // degree : 2
    if ((*p)->left != NULL && (*p)->right != NULL)
    {
        st[sp++] = (*p);
        Node *tempNode = (*p);

        if ((*p)->left->height < (*p)->right->height)
        {
            *p = minNode((*p)->right, st, sp);
        }
        else if ((*p)->left->height > (*p)->right->height)
        {
            *p = maxNode((*p)->left, st, sp);
        }
        else
        {
            if (noNodes((*p)->left) > noNodes((*p)->right))
            {
                *p = maxNode((*p)->left, st, sp);
            }
            else
            {
                *p = minNode((*p)->right, st, sp);
            }
        }

        tempNode->key = (*p)->key;
        (*q) = st[sp - 1];
    }

    // degree : 0 or 1
    if ((*p)->left == NULL && (*p)->right == NULL)
    {
        if (*q == NULL)
        {
            T = NULL;
        }
        else if ((*q)->left == *p)
        {
            (*q)->left = NULL;
        }
        else
        {
            (*q)->right = NULL;
        }
    }
    else
    {
        if ((*p)->left != NULL)
        {
            if (*q == NULL)
            {
                *T = (*T)->left;
            }
            else if ((*q)->left == *p)
            {
                (*q)->left = (*p)->left;
            }
            else
            {
                (*q)->right = (*p)->left;
            }
        }
        else
        {
            if (*q == NULL)
            {
                *T = (*T)->right;
            }
            else if ((*q)->left == *p)
            {
                (*q)->left = (*p)->right;
            }
            else
            {
                (*q)->right = (*p)->right;
            }
        }
    }

    return *p;
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

void checkBalance(Node **T, int newKey, string &rotationType, Node **p, Node **q, Node *st[], int &sp)
{
    Node *x = NULL;
    Node *f = NULL;
    while (sp > 0)
    {
        *q = st[--sp];
        (*q)->height = setHeight(*q);
        (*q)->bf = setBF(*q);

        if (1 < (*q)->bf || (*q)->bf < -1)
        {
            if (x == NULL)
            {
                x = (*q);
                if (sp <= 0)
                {
                    f = NULL;
                }
                else
                {
                    f = st[sp - 1];
                }
            }
        }
    }

    if (x == NULL)
    {
        rotationType = "NO";
        *p = NULL;
        *q = NULL;
        return;
    }

    if (1 < x->bf)
    {
        if (x->left->bf < 0)
        {
            rotationType = "LR";
        }
        else
        {
            rotationType = "LL";
        }
    }
    else
    {
        if (x->right->bf > 0)
        {
            rotationType = "RL";
        }
        else
        {
            rotationType = "RR";
        }
    }

    *p = x;
    *q = f;
}

void rotateTree(Node **T, string rotationType, Node **p, Node **q)
{
    Node *a = *p;
    Node *b;

    if (rotationType.compare("LL") == 0)
    {
        b = (*p)->left;
        a->left = b->right;
        b->right = a;
        a->bf = 0;
        b->bf = 0;

        a->height = setHeight(a);
        b->height = setHeight(b);
    }
    else if (rotationType.compare("LR") == 0)
    {
        b = (*p)->left;
        Node *c = b->right;
        b->right = c->left;
        a->left = c->right;
        c->right = a;
        c->left = b;
        switch (c->bf)
        {
        case 0:
            b->bf = 0;
            a->bf = 0; // LR(a)
            break;
        case 1:
            a->bf = -1;
            b->bf = 0; // LR(b)
            break;
        case -1:
            b->bf = 1;
            a->bf = 0; // LR(c)
            break;
        }
        a->height = setHeight(a);
        b->height = setHeight(b);
        c->height = setHeight(c);

        c->bf = 0;
        b = c;
    }
    else if (rotationType.compare("RR") == 0)
    {
        b = (*p)->right;
        a->right = b->left;
        b->left = a;
        a->bf = 0;
        b->bf = 0;

        a->height = setHeight(a);
        b->height = setHeight(b);
    }
    else
    {
        b = (*p)->right;
        Node *c = b->left;
        b->left = c->right;
        a->right = c->left;
        c->left = a;
        c->right = b;
        switch (c->bf)
        {
        case 0:
            b->bf = 0;
            a->bf = 0;
            break;
        case 1:
            a->bf = 0;
            b->bf = -1;
            break;
        case -1:
            a->bf = 1;
            b->bf = 0;
            break;
        }

        a->height = setHeight(a);
        b->height = setHeight(b);
        c->height = setHeight(c);

        c->bf = 0;
        b = c;
    }
    if (*q == NULL)
    {
        *T = b;
    }
    else if (a == (*q)->left)
    {
        resetHB(T, (*q)->height);
        (*q)->left = b;
        (*q)->height = setHeight(*q);
        (*q)->bf = setBF(*q);
    }
    else if (a == (*q)->right)
    {
        resetHB(T, (*q)->height);
        (*q)->right = b;
        (*q)->height = setHeight(*q);
        (*q)->bf = setBF(*q);
    }
}

void resetHB(Node **target, int stopHeight)
{
    if((*target)->height < stopHeight)
    {
        return;
    }
    Node *left = (*target)->left;
    Node *right = (*target)->right;
    resetHB(&left, stopHeight);
    resetHB(&right, stopHeight);
    (*target)->height = setHeight(*target);
    (*target)->bf = setBF(*target);
}
