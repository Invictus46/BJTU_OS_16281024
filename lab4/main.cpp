//
//  main.cpp
//  PageReplacement
//
//  Created by 张宗灏 on 2019/5/26.
//  Copyright © 2019 张宗灏. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#define R 32    //物理内存块数
#define V 64   //虚拟内存块数

using namespace std;

typedef struct LNode{
    int data;
    int flag;
    int modify;
}LNode;

typedef struct PNode{
    int data;
    int flag;
    int modify;
    PNode* next;
}PNode;

typedef struct Link{
    int num;
    PNode* next;
}Link;

typedef struct node{
    int num;
    node* next;
} Node, *pNode;

typedef struct queue{
    int n;
    pNode front;
    pNode rear;
} Queue, *pQueue;

void Generate();
void InitMemo();
bool IsInMemo (int n);
void Optimal (int n);
void MainOptimal();
void LRU (int n);
void MainLRU();
bool IsInNodes (int n);
void UpdatedClock (int n);
void MainClock();
void InitQueue (pQueue q);
void QPush (pQueue q, int num);
void QPop (pQueue q);
void QDestroy (pQueue q);
bool FindQueue (pQueue q, int num);
void FIFO (pQueue q, int num);
void MainFIFO();
void AddLink (int data, int type);
void FreeIdle();
void FreeMod();
void PBA (int n);
void MainPBA();
bool IsInPNodes (int n);

int Block = 3;
int Access[32]; //访问序列
int* Memo;
int Lost = 0;
int Index = 0;
LNode* Nodes;
int Size = 3;//分配的内存
int p;//工作集起始位置
int table[32];//物理内存
PNode* Pnodes;
Link Idle;
Link Modified;

int main (){
    Generate();
    MainOptimal();
    MainLRU();
    MainClock();
    MainFIFO();
    MainPBA();
    
    cout << "*******************************************************************" << endl;
    cout << "访问序列：" << endl;
    for (int i = 0; i < 32; i++){
        cout << Access[i] << ", ";
    }
    return 0;
}

void Generate(){
    srand ( (unsigned) time (NULL)); //用时间产生随机数
    int p = rand() % 64;
    int m = 8, e = 8;
    int i, j;
    double t;
    t = rand() % 10 / 10.0;
    
    for (i = 0; i < 4; i++){
        for (j = i * m; j < (i + 1) *m; j++)
            Access[j] = (p + rand() % e) % 64;
        
        double r = (rand() % 10) / 10.0;
        
        if (r < t)
            p = rand() % 64;
        else
            p = (p + 1) % 64;
    }
}

void InitMemo(){
    Memo = (int*) malloc (Block * sizeof (int));
    int i = 0;
    
    for (; i < Block; i++)
        Memo[i] = -1;
    
    return;
}

void MainOptimal(){
    InitMemo();
    int i = 0;
    cout << "******************************************************************" << endl;
    cout << "最佳置换算法：" << endl;
    Lost = 0;
    for (; i < 32; i++){
        Optimal (i);
        cout << Memo[0] << " " << Memo[1] << " " << Memo[2] << endl;
    }
    
    cout << "最佳置换算法缺页率：" << Lost / 32.0 << " " << Lost << endl;
    Lost = 0;
    free (Memo);
    Index = 0;
}

bool  IsInMemo (int n){
    for (int i = 0; i < Block; i++){
        if (Access[n] == Memo[i]){
            return true;
        }
    }
    
    return false;
}

void Optimal (int n){
    int i = 0, j = 0;
    
    if (IsInMemo (n))
        cout << "页面已被调入" << endl;
    else
        if (Index == Block){
            Lost++;
            int max = 0, Pos, Tag;
            for (i = 0; i < Block; i++){
                Tag = -1;
                for (j = n + 1; j < 32; j++){
                    if (Access[j] == Memo[i]){
                        Tag = j;
                        break;
                    }
                }
                if (Tag == -1){
                    max = 32;
                    Pos = i;
                    break;
                }
                else{
                    if (max < Tag){
                        max = Tag;
                        Pos = i;
                    }
                }
            }
            Memo[Pos] = Access[n];
        }
        else{
            Memo[Index] = Access[n];
            Index++;
        }
}

void LRU (int n){
    int i, j;

    if (IsInMemo (n))
        cout << "已经装入内存" << endl;
    else
        if (Index == Block){
            int max = n, Pos = -1, Tag;
            for (i = 0; i < Block; i++){
                for (j = n - 1; j >= 0; j--){
                    if (Access[j] == Memo[i]){
                        Tag = j;
                        break;
                    }
                }
                if (Tag < max){
                    max = Tag;
                    Pos = i;
                    if (max == 0)
                        break;
                }
            }
            Memo[Pos] = Access[n];
            Lost++;
        }
        else{
            Memo[Index] = Access[n];
            Index++;
        }
}

void MainLRU(){
    int i;
    InitMemo();
    cout << "*******************************************************************" << endl;
    cout << "最近最久未使用算法" << endl;
    Lost = 0;
    for (i = 0; i < 32; i++){
        LRU (i);
        cout << Memo[0] << " " << Memo[1] << " " << Memo[2] << endl;
    }
    
    cout << "最近最久未使用缺页率： " << Lost / 32.0 << " " << Lost << endl;
    Lost = 0;
    Index = 0;
    free (Memo);
}

bool IsInNodes (int n){
    for (int i = 0; i < Block; i++){
        if (Nodes[i].data == Access[n])
            return true;
    }
    
    return false;
}

void UpdatedClock (int n){
    if (IsInNodes (n))
        cout << "已经装入内存" << endl;
    else
        if (Index == Block){
            Lost++;
            int i = 0, Tag = -1;
            while (true){
                if ( (i / Block) % 2 == 0){
                    if (Nodes[i % Block].flag == 0 && Nodes[i % Block].modify == 0){
                        Tag = i % Block;
                        break;
                    }
                }
                if ( (i / Block) % 2 == 1){
                    if (Nodes[i % Block].flag == 0 && Nodes[i % Block].modify == 1){
                        Tag = i % Block;
                        break;
                    }
                    else
                        Nodes[i % Block].flag = 0;
                }
                i++;
            }
            Nodes[Tag].data = Access[n];
            Nodes[Tag].flag = 1;
            
            if (rand() % 10 < 4)
                Nodes[Tag].modify = 1;
            else
                Nodes[Tag].modify = 0;
        }
        else{
            Nodes[Index].data = Access[n];
            Nodes[Index].flag = 1;
            
            if (rand() % 10 < 4)
                Nodes[Index].modify = 1;
            else
                Nodes[Index].modify = 0;
            Index++;
        }
}

void MainClock(){
    int i = 0, j = 0;
    cout << "*******************************************************************" << endl;
    cout << "改进型Clock置换算法" << endl;
    Nodes = (LNode*) malloc (Block * sizeof (LNode));
    Lost = 0;
    for (i = 0; i < Block; i++){
        Nodes[i].data = -1;
        Nodes[i].flag = -1;
        Nodes[i].modify = -1;
    }
    for (i = 0; i < 32; i++){
        UpdatedClock (i);
        
        for (j = 0; j < Block; j++){
            cout << Nodes[j].data << " ";
        }
        
        cout << endl;
    }
    cout << "改进型Clock置换算法缺页率： " << Lost / 32.0 << " " << Lost << endl;
    Lost = 0;
    Index = 0;
}

void MainFIFO(){
    Queue q;
    pNode p;
    InitQueue (&q);
    int i = 0;
    cout << "*******************************************************************" << endl;
    cout << "先进先出置换算法" << endl;
    Lost = 0;
    for (; i < 32; i++){
        FIFO (&q, Access[i]);
        p = q.front->next;
        
        while (p){
            cout << p->num << " ";
            p = p->next;
        }
        cout << endl;
    }
    
    cout << "先进先出算法缺页率：" << Lost/32.0 << "  " << Lost << endl;
    QDestroy (&q);
}

void InitQueue (pQueue q){
    q->rear = (pNode) malloc (sizeof (Node));
    
    if (q->rear == NULL)
        cout << "failed" << endl;
    else{
        q->front = q->rear;
        q->rear->next = NULL;
        q->front->next = NULL;
        q->n = 0;
    }
}

void QPush (pQueue q, int num){
    pNode p = (pNode) malloc (sizeof (Node));
    
    if (p == NULL)
        cout << "failed" << endl;
    else{
        p->next = NULL;
        p->num = num;
        
        if (q->front == q->rear){
            q->front->next = p;
            q->rear = p;
        }
        else{
            q->rear->next = p;
            q->rear = p;
        }
        q->n++;
    }
}

void QPop (pQueue q){
    pNode p;
    
    if (q->front != q->rear){
        p = q->front->next;
        q->front->next = p->next;
        
        if (p == q->rear)
            q->front = q->rear;
        q->n--;
        free (p);
    }
}

void QDestroy (pQueue q){
    while (q->front != q->rear)
        QPop (q);
}


bool FindQueue (pQueue q, int num){
    pNode p;
    
    if (q->front != q->rear){
        p = q->front->next;
        
        while (p){
            if (p->num == num)
                return true;
            else
                p = p->next;
        }
    }
    return false;
}

void FIFO (pQueue q, int num){
    if (FindQueue (q, num))
        cout << "已装入内存" << endl;
    else{
        if (q->n == Size){
            QPop (q);
            QPush (q, num);
            Lost++;
        }
        else
            QPush (q, num);
    }
}

void MainPBA(){
    int i = 0, j = 0;
    cout << "*******************************************************************" << endl;
    cout << "页面缓冲置换算法(PBA)" << endl;
    Idle.num = 0;
    Idle.next = NULL;
    Modified.num = 0;
    Modified.next = NULL;
    Pnodes = (PNode*) malloc (Size * sizeof (PNode));
    Memo[0] = -1;Memo[1] = -1;Memo[2] = -1;
    Lost = 0;
    for (i = 0; i < Size; i++){
        Pnodes[i].data = -1;
        Pnodes[i].flag = 0;
        Pnodes[i].modify = 0;
        Pnodes[i].next = NULL;
    }
    for (i = 0; i < 32; i++){
        PBA (i);
        
        for (j = 0; j < Size; j++){
            cout << Pnodes[j].data << " ";
        }
        cout << endl;
    }
    cout << "页面缓冲置换算法（PBA）缺页率：" << Lost / 32.0 << "   " << Lost << endl;
}

bool IsInPNodes (int n){
    int i;
    
    for (i = 0; i < 3; i++){
        if (Pnodes[i].data == Access[n])
            return true;
    }
    return false;
}

PNode* isinLinks (int n){
    PNode*p, *q;
    p = Idle.next;
    q = NULL;
    
    while (p){
        if (p->data == Access[n]){
            if (q != NULL){
                q->next = p->next;
                p->next = NULL;
                Idle.num--;
                break;
            }
            else
                Idle.next = NULL;
        }
        q = p;
        p = p->next;
    }
    if (p == NULL){
        p = Modified.next;
        while (p != NULL){
            if (p->data == Access[n]){
                if (p == Modified.next)
                    Modified.next = p->next;
                else{
                    q->next = p->next;
                    p->next = NULL;
                    Modified.num--;
                }
                if (Modified.num == 0)
                    Modified.next = NULL;
                break;
            }
            q = p;
            p = p->next;
        }
    }
    return p;
}


void PBA (int n){
    if (IsInPNodes (n))
        cout << "已装入内存" << endl;
    else
        if (Index == Size){
            PNode *p;
            if ( (p = isinLinks (n)) != NULL){
                Pnodes = (PNode*) realloc (Pnodes, (Size + 1) * sizeof (PNode));
                Pnodes[Size] .data = p->data;
                Pnodes[Size].flag = p->flag;
                Pnodes[Size].modify = p->modify;
                Pnodes[Size].next = p->next;
                free (p);
                Size++;
                Index++;
            }
            else{
                Lost++;
                
                if (Pnodes[n % 3].modify == 1)
                    AddLink (Pnodes[n % 3].data, 1);
                else
                    AddLink (Pnodes[n % 3].data, 0);
                
                Pnodes[n % 3].data = Access[n];
                Pnodes[n % 3].flag = 1;
                Pnodes[n % 3].next = NULL;
                
                if (rand() % 10 < 4)
                    Pnodes[n % 3].modify = 0;
                else
                    Pnodes[n % 3].modify = 1;
            }
        }
        else{
            Pnodes[Index].data = Access[n];
            Pnodes[Index].flag = 1;
            Pnodes[Index].next = NULL;
            
            if (rand() % 10 < 4)
                Pnodes[Index].modify = 1;
            else
                Pnodes[Index].modify = 0;
            Index++;
        }
}

void AddLink (int data, int type){
    PNode* p;
    PNode* q;
    q = (PNode*) malloc (sizeof (PNode));
    q->data = data;
    q->flag = 1;
    
    if (type == 1){
        q->modify = 1;
        p = Modified.next;
    }
    else{
        q->modify = 0;
        p = Idle.next;
    }
    q->next = NULL;
    if (p == NULL){
        if (type == 0)
            Idle.next = q;
        
        else{
            Modified.next = q;
        }
    }
    else{
        while (p){
            if (p->next == NULL){
                p->next = q;
                break;
            }
            else
                p = p->next;
        }
    }
    
    if (type == 0){
        Idle.num += 1;
        
        if (Idle.num == 10)
            FreeIdle();
    }
    else{
        Modified.num += 1;
        
        if (Modified.num == 10)
            FreeMod();
    }
}

void FreeIdle (){
    PNode* p;
    p = Idle.next;
    
    while (p){
        Idle.next = p->next;
        free (p);
        p = Idle.next;
    }
    
    Idle.num = 0;
}

void FreeMod(){
    PNode* p;
    p = Modified.next;
    
    while (p){
        Modified.next = p->next;
        free (p);
        p = Modified.next;
    }
    
    Modified.num = 0;
}
