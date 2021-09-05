#include<iostream>

using namespace std;

struct node {
    int key;
    int val;
    int freq;

    node(): key(0), val(0), freq(0) {}

    bool operator<(const node &nd) {
        return this->freq < nd.freq;
    }
};

class heap {
    node **tree;
    size_t cap;
    size_t size;

   
    public:heap(size_t cap) {
        this->cap = cap;
        this->size = 0;
        tree = new node*[cap];
        for(int i=0; i<cap; ++i)
            tree[i] = nullptr;
    }

    private:void heapifyUp(int node) {
        if(node == 0)
            return;
        
        int p = (node-1)/2;
        if(*tree[node] < *tree[p]) {
            swap(tree[node], tree[p]);
            heapifyUp(p);
        }
    }

    private:void heapifyDown(int root) {
        if(root >= size || tree[root] == nullptr)
            return;
        
        int l = 2*root+1;
        int r = 2*root+2;
        int choice = 0;
        if(l<size && tree[l] != nullptr && r<size && tree[r] != nullptr) {
            if(*tree[l] < *tree[r] && *tree[l] < *tree[root])
                choice = 1;
            else if(*tree[r] < *tree[root])
                choice = 2;
        } else if(l<size && tree[l] != nullptr && *tree[l] < *tree[root]) {
            choice = 1;
        } else if(r < size && tree[r] != nullptr && *tree[r] < *tree[root]) {
            choice = 2;
        }

        if(choice == 1) {
            swap(tree[root], tree[l]);
            heapifyDown(l);
        } else if(choice == 2) {
            swap(tree[root], tree[r]);
            heapifyDown(r);
        }
    }

    public:void insert(node *nd) {
        if(size == cap)
            return;
        
        tree[size] = nd;
        heapifyUp(size++);
    }

    public:node* top() {
        if(size == 0)
            return nullptr;
        return tree[0];
    }

    public:void pop() {
        if(size == 0)
            return;
        
        swap(tree[0], tree[size-1]);
        tree[--size] = nullptr;
        heapifyDown(0);
    }

    public:~heap() {
        delete[] tree;
    }

    public:void print() {
        for(int i=0; i<size; ++i)
            cout<<tree[i]->freq<<" ";
        cout<<"\n";
    }
};

int main() {
    node n1, n2, n3, n4, n5, n6, n7;
    n1.freq = 10;
    n2.freq = 2;
    n3.freq = 3;
    n4.freq = 4;
    n5.freq = 34;
    n6.freq = 0;
    n7.freq = -1;

    heap h(8);
    h.insert(&n1);
    h.insert(&n2);
    h.insert(&n3);
    h.insert(&n4);

    h.print();
    h.pop();
    h.print();

    h.insert(&n5);
    h.insert(&n6);
    h.insert(&n7);
    h.pop();

    h.print();
}