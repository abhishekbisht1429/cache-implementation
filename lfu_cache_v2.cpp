#include<iostream>
#include<unordered_map>

using namespace std;

struct node {
    node *prev;
    node *next;
    node(): prev(nullptr), next(nullptr) {}
    virtual ~node() {}
};

class list {
    node *_front;
    node *_back;
    int _size;

    public:list(): _front(nullptr), _back(nullptr), _size(0) {}

    public:void push_back(node *nd) {
        if(nd == nullptr)
            return;
        _size++;
        if(empty()) {
            _back = nd;
            _front = nd;
            return;
        }

        insertAfter(_back, nd);
    }

    public:void push_front(node *nd) {
        if(nd == nullptr)
            return;
        _size++;
        if(empty()) {
            _back = nd;
            _front = nd;
            return;
        }

        insertBefore(_front, nd);
    }

    public:node *detach(node *nd) {
        if(nd == nullptr)
            return nullptr;
        
        node *prev = nd->prev;
        node *next = nd->next;

        if(prev!=nullptr) prev->next = next;
        else _front = next;
        if(next!=nullptr) next->prev = prev;
        else _back = prev;

        nd->next = nullptr;
        nd->prev = nullptr;
        --_size;

        return nd;
    }

    public:void insertAfter(node *nd, node *new_node) {
        if(nd == nullptr || new_node == nullptr)
            return;
        ++_size;
        new_node->next = nd->next;
        new_node->prev = nd;
        if(nd->next != nullptr) nd->next->prev = new_node;
        nd->next = new_node;

        if(new_node->next == nullptr)
            _back = new_node;
    }

    public:void insertBefore(node *nd, node *new_node) {
        if(nd == nullptr || new_node == nullptr)
            return;
        ++_size;
        new_node->next = nd;
        new_node->prev = nd->prev;
        if(nd->prev != nullptr) nd->prev->next = new_node;
        nd->prev = new_node;

        if(new_node->prev == nullptr)
            _front = new_node;
    }

    public:node *pop_front() {
        return detach(_front);
    }

    public:node *pop_back() {
        return detach(_back);
    }

    // public:void delete_node(node *nd) {
    //     if(nd == nullptr)
    //         return;
        
    //     detach(nd);
    //     delete nd;
    // }

    public:bool empty() {
        return _back == nullptr;
    }

    public:int size() {
        return _size;
    }

    public:node *front() {
        return _front;
    }

    public:node *back() {
        return _back;
    }

    friend void print_li(list *);
};

struct freq_node : public node {
    int freq;
    list *data_list;

    freq_node(int freq): freq(freq), data_list(nullptr) {
        data_list = new list();
    }

    ~freq_node() {
        delete data_list;
    }
};

struct data_node : public node {
    int key;
    int val;
    freq_node *fnode;

    data_node(int key, int val): key(key), val(val), fnode(nullptr) {}
    data_node() {}
};

void print_li(list *li) {
    data_node *temp = dynamic_cast<data_node*>(li->front());
    while(temp!=nullptr) {
        cout<<temp->val<<" ";
        temp = dynamic_cast<data_node*>(temp->next);
    }
    cout<<"\n";
}


class freq_list : private list {

    public:void delete_fnode(freq_node *fnode) {
        delete detach(fnode);
    }

    public:void increment_freq(data_node *dnode) {
        if(dnode == nullptr)
            return;

        freq_node *fnode = dnode->fnode;
        if(fnode == nullptr) {
            cout<<"invalid state: dnode whoes freq is to be incremented \
                    does not have pointer to fnode \n";
            return;
        }

        /* check if next frequency node exists and create one if it doesn't */
        freq_node *fnode_next = dynamic_cast<freq_node*>(fnode->next);
        if(fnode_next == nullptr || fnode_next->freq != fnode->freq+1) {
            fnode_next = new freq_node(fnode->freq+1);
            insertAfter(fnode, fnode_next);
        }

        /* detach the data node */
        fnode->data_list->detach(dnode);

        /* delete freq node if its list is empty */
        if(fnode->data_list->empty())
            delete_fnode(fnode);
        
        /* update the fnode */
        dnode->fnode = fnode_next;
        /* attach to the next freq node */
        fnode_next->data_list->push_front(dnode);

    }

    public:void insert_dnode(data_node *dnode) {
        if(dnode == nullptr) {
            cout<<"cannot insert nullptr in fnode's data list\n";
            return;
        }
        freq_node *front = dynamic_cast<freq_node*>(this->front());
        /* create a freq node with freq 1 if it does not exist */
        if(front == nullptr || front->freq > 1) {
            front = new freq_node(1);
            this->push_front(front);
        }
        dnode->fnode = front;
        front->data_list->push_front(dnode);
    }

    public:data_node *detach_lfu() {
        if(this->empty())
            return nullptr;
        
        freq_node *lfnode = dynamic_cast<freq_node*>(this->front());
        data_node *temp = dynamic_cast<data_node*>(lfnode->data_list->pop_back());

        /* set the fnode pointer to null */
        temp->fnode = nullptr;

        /* delete fnode if its data list is empty */
        if(lfnode->data_list->empty())
            delete_fnode(lfnode);
        
        return temp;
    }

    public:void print() {
        freq_node *temp = dynamic_cast<freq_node*>(front());
        while(temp!=nullptr) {
            cout<<temp->freq<<" : ";
            print_li(temp->data_list);
            temp = dynamic_cast<freq_node*>(temp->next);
        }
    }
};

class lfu_cache {
    unordered_map<int, data_node*> cache;
    freq_list f_list;
    int _cap;
    int _size;

    public:lfu_cache(int cap) {
        this->_cap = cap;
    }

    public:void set(int key, int val) {
        if(cache.find(key) != cache.end()) {
            cache[key]->val = val;
            f_list.increment_freq(cache[key]);
        } else {
            if(_size == _cap) {
                data_node *temp = f_list.detach_lfu();
                cache.erase(temp->key);
                --_size;
                delete temp;
            }
            cache[key] = new data_node(key, val);
            f_list.insert_dnode(cache[key]);
            ++_size;
        }
    }

    public:int get(int key) {
        if(cache.find(key) == cache.end())
            return -1;
        f_list.increment_freq(cache[key]);
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
    // list li;
    // data_node arr[10];
    // for(int i=0; i<10; ++i)
    //     arr[i] = data_node(i, 1000*i);
    // li.push_back(&arr[0]);
    // li.push_back(&arr[1]);
    // li.push_front(&arr[2]);
    // li.insertAfter(&arr[0], &arr[5]);
    // li.insertAfter(&arr[1], &arr[7]);
    // li.insertBefore(&arr[2], &arr[9]);
    // li.insertBefore(&arr[0], &arr[8]);
    // li.pop_back();
    // li.pop_front();
    // li.detach(&arr[0]);
    // print_li(&li);

    // freq_list fli;
    // fli.insert_dnode(&arr[0]);
    // fli.insert_dnode(&arr[1]);
    // fli.insert_dnode(&arr[2]);
    // fli.insert_dnode(&arr[3]);
    // fli.increment_freq(&arr[0]);
    // fli.increment_freq(&arr[0]);
    // fli.increment_freq(&arr[2]);
    // fli.increment_freq(&arr[3]);
    // fli.print();
    // cout<<"\n";
    
    // fli.detach_lfu();
    // fli.print();
    // cout<<"\n";

    // fli.detach_lfu();
    // fli.print();
    // cout<<"\n";

    // fli.detach_lfu();
    // fli.print();
    // cout<<"\n";

    lfu_cache cache(3);
    int arr[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};

    for(int i=0; i<sizeof(arr)/sizeof(int); ++i) {
        cache.set(arr[i], arr[i]*1000);
        cout<<arr[i]<<" ";
        cache.display();
    }
}