#include<iostream>
#include<unordered_map>

using namespace std;

struct node {
    int key;
    int val;
    node *prev;
    node *next;

    node(int key, int val): key(key), val(val), prev(nullptr), next(nullptr) {}
};

class list {
    node *front;
    node *rear;
    int count;

    public:list(): front(nullptr), rear(nullptr), count(0) {}

    public:void push_back(node *nd) {
        if(nd == nullptr)
            return;
        count++;
        if(rear == nullptr) {
            rear = nd;
            front = nd;
            return;
        }

        nd->prev = rear;
        rear->next = nd;
        rear = nd;
    }

    public:node *detach(node *nd) {
        if(nd == nullptr)
            return nullptr;
        
        node *prev = nd->prev;
        node *next = nd->next;

        if(prev!=nullptr) prev->next = next;
        else front = next;
        if(next!=nullptr) next->prev = prev;
        else rear = prev;

        nd->next = nullptr;
        nd->prev = nullptr;
        --count;

        return nd;
    }

    public:node *pop_front() {
        return detach(front);
    }

    public:int get_count() {
        return count;
    }

    public:node *get_front() {
        return front;
    }

    public:void print() {
        node *temp = front;
        while(temp!=nullptr) {
            cout<<temp->val<<" ";
            temp = temp->next;
        }
        cout<<"\n";
    }
};

class lru_cache {
    unordered_map<int, node*> cache;
    list li;
    int cap;

    public:lru_cache(int cap) {
        this->cap = cap;
    }

    public:int get(int key) {
        if(cache.find(key) == cache.end())
            return -1;
        
        int val = cache[key]->val;

        /* detach the node and push to the rear */
        li.detach(cache[key]);
        li.push_back(cache[key]);

        return val;
    }

    public:void set(int key, int val) {
        if(cache.find(key) != cache.end()) {
            /* update value */
            cache[key]->val = val;

            /* Detach the node and push to rear */
            li.detach(cache[key]);
            li.push_back(cache[key]);
            return;
        }
        if(li.get_count() == cap) {
            /* 
                if full remove node from front
                erase the corresponding key from cache
                delete the allocated memory
            */
            node *temp = li.pop_front();
            cache.erase(temp->key);
            delete[] temp;
        }

        cache[key] = new node(key, val);
        
        /* push new node to rear */
        li.push_back(cache[key]);
    }

    public:void display() {
        node *temp = li.get_front();
        while(temp!=nullptr) {
            cout<<temp->val<<" ";
            temp = temp->next;
        }
        cout<<"\n";
    }
};

int main() {
    lru_cache cache(3);

    int arr[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

    for(int i=0; i<sizeof(arr)/sizeof(int); ++i) {
        cache.set(arr[i], arr[i]*1000);
        cout<<arr[i]<<" ";
        cache.display();
    }
}