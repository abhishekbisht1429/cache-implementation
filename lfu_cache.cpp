#include<iostream>
#include<unordered_map>
#include<chrono>

using namespace std;

struct node {
    int key;
    int val;
    int freq;
    int pos;
    long long ts;

    node(): key(0), val(0), freq(0), pos(-1) {}

    node(int key, int val, int freq):
        key(key), val(val), freq(freq), pos(-1) {}

    bool operator<(const node &nd) {
        return (this->freq < nd.freq)?true:(this->ts < nd.ts); 
    }
};

long long get_time_stamp() {
    return chrono::duration_cast<chrono::nanoseconds>(
                chrono::system_clock::now().time_since_epoch()).count();
}
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

    void swap_nodes(node *&n1, node *&n2) {
        swap(n1, n2);
        swap(n1->pos, n2->pos);
    }

    private:void heapifyUp(int node) {
        if(node == 0)
            return;
        
        int p = (node-1)/2;
        if(*tree[node] < *tree[p]) {
            swap_nodes(tree[node], tree[p]);
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
            swap_nodes(tree[root], tree[l]);
            heapifyDown(l);
        } else if(choice == 2) {
            swap_nodes(tree[root], tree[r]);
            heapifyDown(r);
        }
    }

    public:void insert(node *nd) {
        if(size == cap)
            return;
        
        tree[size] = nd;
        tree[size]->pos = size;
        tree[size]->ts = get_time_stamp();
        heapifyUp(size++);
    }

    public:node* top() {
        if(size == 0)
            return nullptr;
        return tree[0];
    }

    public:node* pop() {
        if(size == 0)
            return nullptr;
        
        swap_nodes(tree[0], tree[size-1]);
        node *temp = tree[size-1];
        tree[--size] = nullptr;
        heapifyDown(0);
        
        return temp;
    }

    public:~heap() {
        delete[] tree;
    }

    public:void update(node *nd, int val) {
        nd->val = val;
        refer(nd);
    }

    public:void refer(node *nd) {
        nd->freq++;
        nd->ts = get_time_stamp();
        heapifyDown(nd->pos);
        heapifyUp(nd->pos);
    }

    public:void print() {
        for(int i=0; i<size; ++i)
            cout<<tree[i]->key<<" ";
        cout<<"\n";
    }
};

class lfu_cache {
    unordered_map<int, node*> cache;
    heap *min_heap;

    int cap;
    int size;

    public:lfu_cache(int cap): cap(cap), size(0) {
        min_heap = new heap(cap);
    }

    public:~lfu_cache() {
        delete min_heap;
    }

    public:void set(int key, int val) {
        if(cache.find(key) != cache.end()) {
            min_heap->update(cache[key], val);
        } else {
            if(size == cap) {
                node *temp = min_heap->pop();
                cache.erase(temp->key);
                size--;
                delete temp;
            }
            cache[key] = new node(key, val, 1);
            min_heap->insert(cache[key]);
            size++;
        }
    }

    public:int get(int key) {
        if(cache.find(key) == cache.end())
            return -1;
        min_heap->refer(cache[key]);
        return cache[key]->val;
    }

    public:void display() {
        for(auto &p : cache) {
            cout<<p.second->val<<" ";
        }
        cout<<"\n";
    }
};

int main() {
    lfu_cache cache(3);
    int arr[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

    for(int i=0; i<sizeof(arr)/sizeof(int); ++i) {
        cache.set(arr[i], arr[i]*1000);
        cout<<arr[i]<<" ";
        cache.display();
    }
    // node n1, n2, n3, n4, n5, n6, n7;
    // n1.freq = 10;
    // n2.freq = 2;
    // n3.freq = 3;
    // n4.freq = 4;
    // n5.freq = 34;
    // n6.freq = 0;
    // n7.freq = -1;

    // heap h(6);
    // h.insert(&n1);
    // h.insert(&n2);
    // h.insert(&n3);
    // h.insert(&n4);

    // h.print();
    // h.pop();
    // h.print();

    // h.insert(&n5);
    // h.insert(&n6);
    // h.pop();
    // h.insert(&n7);

    // h.print();
    
    // h.refer(&n7);
    // h.print();
}