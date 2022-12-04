#include <iostream>
#include <stack>
#include <vector>
#include <fstream>

using namespace std;

#define MAX_SIZE 5
#define MAX_ST_SIZE 10000

struct Node
{
    int n;
    int K[MAX_SIZE];
    Node *P[MAX_SIZE];
};

void inorderBT(Node *target, int m);

Node *getBTNode(int newKey)
{
    Node *newNode = new Node;
    newNode->n = 1;
    newNode->K[0] = newKey;
    return newNode;
}

bool searchPath(Node **T, int m, int key, Node *st[], int &sp)
{
    Node *x = new Node;
    x = *T;

    int i;
    do
    {
        i = 0;
        while (i < x->n && key > x->K[i])
            i++;

        st[sp++] = x;
        if (i < x->n && key == x->K[i])
            return true;
        x = (x->P[i]);
    } while (x != NULL);
    return false;
}

void insertKey(Node **T, int m, Node **x, Node *y, int newKey)
{
    int i = (*x)->n - 1;
    while (i >= 0 && newKey < (*x)->K[i])
    {
        (*x)->K[i + 1] = (*x)->K[i];
        (*x)->P[i + 2] = (*x)->P[i + 1];
        i--;
    }
    (*x)->K[i + 1] = newKey;
    (*x)->P[i + 2] = y;
    (*x)->n = (*x)->n + 1;
}

Node *splitNode(Node **T, int m, Node **x, Node *y, int &newKey)
{
    Node *tempNode = new Node;
    memcpy(tempNode, *x, sizeof(Node));
    insertKey(T, m, &tempNode, y, newKey);
    int centerKey = tempNode->K[tempNode->n / 2];

    (*x)->n = 0;
    int i = 0;
    while (tempNode->K[i] < centerKey)
    {
        (*x)->K[i] = tempNode->K[i];
        (*x)->P[i] = tempNode->P[i];
        i++;
        (*x)->n = (*x)->n + 1;
    }
    (*x)->P[i] = tempNode->P[i];
    Node *newNode = new Node;
    newNode->n = 0;
    i++;

    while (i < tempNode->n)
    {
        newNode->K[newNode->n] = tempNode->K[i];
        newNode->P[newNode->n] = tempNode->P[i];
        i++;
        newNode->n = newNode->n + 1;
    }
    newNode->P[newNode->n] = tempNode->P[i];
    newKey = centerKey;
    delete tempNode;
    return newNode;
}

void deleteKey(Node **T, int m, Node **x, int oldKey)
{
    int i = 0;
    while (oldKey > (*x)->K[i])
        i++;
    while (i < (*x)->n)
    {
        (*x)->K[i] = (*x)->K[i + 1];
        (*x)->P[i + 1] = (*x)->P[i + 2];
        i++;
    }
    (*x)->n--;
}

int bestSibling(Node **T, int m, Node *x, Node *y)
{
    int i = 0;
    while (y->P[i] != x)
        i++;

    if (i == 0)
        return i + 1;
    else if (i == y->n)
        return i - 1;
    else if (y->P[i]->n >= y->P[i + 1]->n)
        return i - 1;
    return i + 1;
}

void redistributeKeys(Node **T, int m, Node **x, Node **y, int bestSib)
{
    int i = 0;
    while ((*y)->P[i] != (*x))
        i++;
    
    Node *bestNode = (*y)->P[bestSib];
    if (bestSib < i)
    {
        int lastKey = bestNode->K[bestNode->n - 1];
        insertKey(T, m, x, NULL, (*y)->K[i - 1]);
        (*x)->P[1] = (*x)->P[0];
        (*x)->P[0] = bestNode->P[bestNode->n];
        bestNode->P[bestNode->n] = NULL;
        deleteKey(T, m, &bestNode, lastKey);
        (*y)->K[i - 1] = lastKey;
    }
    else
    {
        int firstKey = bestNode->K[0];
        insertKey(T, m, x, NULL, (*y)->K[i]);
        (*x)->P[(*x)->n] = bestNode->P[0];
        bestNode->P[0] = bestNode->P[1];
        deleteKey(T, m, &bestNode, firstKey);
        (*y)->K[i] = firstKey;
    }
}

void mergeNode(Node **T, int m, Node **x, Node **y, int bestSib)
{
    int i = 0;
    while ((*y)->P[i] != *x)
        i++;

    Node *bestNode = (*y)->P[bestSib];
    if (bestSib > i)
    {
        int tmp = i;
        i = bestSib;
        bestSib = tmp;

        Node *tmpNode = *x;
        (*x) = bestNode;
        bestNode = tmpNode;
    }

    bestNode->K[bestNode->n] = (*y)->K[i - 1];
    bestNode->n++;
    int j = 0;
    while (j < (*x)->n)
    {
        bestNode->K[bestNode->n] = (*x)->K[j];
        bestNode->P[bestNode->n] = (*x)->P[j];
        bestNode->n++;
        j++;
    }
    bestNode->P[bestNode->n] = (*x)->P[(*x)->n];
    deleteKey(T, m, y, (*y)->K[i - 1]);
}

bool insertBT(Node **T, int m, int newKey)
{
    if (*T == NULL)
    {
        *T = new Node;
        (*T)->K[0] = newKey;
        (*T)->n = 1;
        return true;
    }

    Node *st[MAX_ST_SIZE];
    int sp = 0;

    bool found = searchPath(T, m, newKey, st, sp);
    if (found)
    {
        return false;
    }

    bool finished = false;

    Node *x = st[--sp];
    Node *y = NULL;

    do
    {
        if (x->n < m - 1)
        {
            insertKey(T, m, &x, y, newKey);
            finished = true;
        }
        else
        {
            Node *newNode = splitNode(T, m, &x, y, newKey);
            y = newNode;
            if (sp > 0)
            {
                x = st[--sp];
            }
            else
            {
                *T = getBTNode(newKey);
                (*T)->P[0] = x;
                (*T)->P[1] = y;
                (*T)->n = 1;
                finished = true;
            }
        }
    } while (!finished);

    return true;
}

bool deleteBT(Node **T, int m, int oldKey)
{
    Node *st[MAX_ST_SIZE];
    int sp = 0;

    bool found = searchPath(T, m, oldKey, st, sp);
    if (!found)
    {
        return false;
    }

    Node *x = st[--sp];
    Node *y = NULL;
    bool isTerminalNode = true;
    for (int i = 0; i < x->n; i++)
    {
        if (x->P[i] != NULL)
        {
            isTerminalNode = false;
            break;
        }
    }
    if (!isTerminalNode)
    {
        Node *internalNode = x;
        int i;
        for (i = 0; i < x->n; i++)
        {
            if (x->K[i] == oldKey)
                break;
        }

        st[sp++] = x;
        searchPath(&(x->P[i + 1]), m, x->K[i], st, sp);

        x = st[--sp];
        int temp = internalNode->K[i];
        internalNode->K[i] = x->K[0];
        x->K[0] = temp;
    }

    bool finished = false;
    deleteKey(T, m, &x, oldKey);
    

    if (sp > 0)
        y = st[--sp];

    do
    {
        if (*T == x || x->n >= (m - 1) / 2)
        {
            finished = true;
        }
        else
        {
            int bestSib = bestSibling(T, m, x, y);
        
            if (y->P[bestSib]->n > (m - 1) / 2)
            {
                redistributeKeys(T, m, &x, &y, bestSib);

                finished = true;
            }
            else
            {
                mergeNode(T, m, &x, &y, bestSib);
                x = y;
                if (sp > 0)
                    y = st[--sp];
                else
                    finished = true;
            }
        }
    } while (!finished);

    if (y != NULL && y->n == 0)
        (*T) = y->P[0];

    return true;
}

void inorderBT(Node *target, int m)
{
    if (target == NULL)
        return;
    for (int i = 0; i < target->n; i++)
    {
        inorderBT(target->P[i], m);
        cout << target->K[i] << ' ';
    }
    inorderBT(target->P[target->n], m);
}
int main()
{
    Node *T = NULL;
    insertBT(&T, 3, 1);
    insertBT(&T, 3, 2);
    insertBT(&T, 3, 3);
    insertBT(&T, 3, 4);
    insertBT(&T, 3, 5);
    insertBT(&T, 3, 6);
    insertBT(&T, 3, 7);
    deleteBT(&T, 3, 2);
    deleteBT(&T, 3, 7);
    deleteBT(&T, 3, 3);
    deleteBT(&T, 3, 1);
    // inorderBT(T, 3);
    
    //단말노드가 아예 사라지는 상황 버그

    for (int i = 0; i < T->P[0]->n; i++)
        cout << "i: " << i << ' ' << " key: " << T->P[0]->K[i] << '\n';
    cout << " n:    " << T->P[0]->n << '\n';
    cout << '\n';
    for (int i = 0; i < T->n; i++)
        cout << "i: " << i << ' ' << " key: " << T->K[i] << '\n';
    cout << " n:    " << T->n << '\n';
    cout << '\n';
    for (int i = 0; i < T->P[1]->n; i++)
        cout << "i: " << i << ' ' << " key: " << T->P[1]->K[i] << '\n';
    cout << " n:    " << T->P[1]->n << '\n';
    cout << '\n';
    /*
    for (int i = 0; i < T->P[2]->n; i++)
        cout << "i: " << i << ' ' << " key: " << T->P[2]->K[i] << '\n';
    cout << " n:    " << T->P[2]->n << '\n';
    */
}