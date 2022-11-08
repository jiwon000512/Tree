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

bool insertAVL(Node **T, int newKey);
bool insertBST(Node **T, int newKey, Node *p, Node *q, Node *st[], int &sp);
void checkBalance(Node **T, int newKey, string &rotationType, Node **p, Node **q, Node *st[], int &sp);
void rotateTree(Node **T, string rotationType, Node **p, Node **q);
void inorderBST(Node *target);

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
                inorderBST(T);
                cout << '\n';
            }
            /*
            else if (p.first == "d")
            {
                if (!deleteBST(T, p.second))
                {
                    cout << p.first << ' ' << p.second << " : The key does not exist" << '\n';
                }
            }
            inorderBST(T);
            cout << '\n';
            */
        }

    }

    /*
    Node *T = NULL;

    insertAVL(&T, 8);
    insertAVL(&T, 9);
    insertAVL(&T, 10);
    insertAVL(&T, 2);
    insertAVL(&T, 1);
    insertAVL(&T, 5);
    insertAVL(&T, 3);
    insertAVL(&T, 6);
    insertAVL(&T, 4);
    inorderBST(T);
    */
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
                    // cout << newKey << ' ' << "NULL" << '\n';
                    f = NULL;
                }
                else
                {
                    f = st[sp - 1];
                    // cout <<newKey << ' ' << f->key << '\n';
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
            a->bf = 1;
            b->bf = 0;
            break;
        case -1:
            a->bf = -1;
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
        (*q)->left = b;
        (*q)->height = setHeight(*q);
        (*q)->bf = setBF(*q);
    }
    else if (a == (*q)->right)
    {
        (*q)->right = b;
        (*q)->height = setHeight(*q);
        (*q)->bf = setBF(*q);
    }
}
