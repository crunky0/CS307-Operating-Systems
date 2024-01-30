#include <iostream>
#include <list>
#include <pthread.h>

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct linkNode{
    int id;
    int size;
    int index; 
};

class HeapManager{
public:
    HeapManager() {
        linkNode head;
        list_.push_back(head);
    }

    int initHeap(int size){
        pthread_mutex_lock(&mutex);
        list_.front().id = -1;
        list_.front().size = size;
        list_.front().index = 0;
        cout << "Memory initialized" << endl;
        print();
        pthread_mutex_unlock(&mutex);
        return 1;
    }

    int myMalloc(int ID, int size){
        pthread_mutex_lock(&mutex); 
        auto it = list_.begin();
        int idx = 0;
        bool found = false;
        int return_idx = -1;
        auto list_size = list_.size();
        while(idx < list_size && !found){
            if(it->id == -1 && it->size > size){
                linkNode new_empty_node;
                new_empty_node.id = -1;
                new_empty_node.size = it->size - size;
                new_empty_node.index = it->index + size;
                it->id = ID;
                it->size = size;
                return_idx = it->index;
                found = true;
                cout << "Allocated for thread " << it->id << endl;
                list_.insert(++it, new_empty_node);
                print();
                 
            }
            else if(it->id == -1 && it->size == size){
                it->id = ID;
                return_idx = it->index;
                found = true;
                cout << "Allocated for thread " << it->id << endl;
                print();
            }
            ++it;
            idx++;
        }
        if(!found){
            cout << "Can not allocate, requested size "<< size << " for thread " << ID  << " is bigger than remaining size" << endl;
            print();
        }
        pthread_mutex_unlock(&mutex);
        return return_idx;
    }

    int myFree(int ID, int index){
        pthread_mutex_lock(&mutex); 
        auto it = list_.begin();
        int idx = 0;
        bool found = false;
        int return_val = 1;
        auto list_size = list_.size();
        while(idx < list_size && !found){
            if(it->id == ID && it->index == index){
                found = true;
                cout << "Freed for thread " << ID << endl;
                it->id = -1;
                ++it;
                if(it->id == -1){
                    --it;
                    --it;
                    if(it->id == -1){ // both left and right is free
                        int size_mid,size_next;
                        ++it;
                        size_mid = it->size;
                        it = list_.erase(it);
                        size_next = it->size;
                        it = list_.erase(it);
                        --it;
                        it->size += size_mid+size_next;
                    }
                    else{ //just right is free
                        ++it;
                        ++it;
                        int size_next;
                        size_next = it->size;
                        it = list_.erase(it);
                        --it;
                        it->size += size_next;
                    }
                }

                else{
                    --it;
                    --it;
                    if(it->id == -1){ // just left is free
                        int size_mid;
                        ++it;
                        size_mid = it->size;
                        it = list_.erase(it);
                        --it;
                        it->size += size_mid;
                    }

                    else{ // left and right occupied
                        ++it;
                    }
                }
            }
            idx++;
            if (!found) ++it;
        }
        if(!found){
            cout << "not found bro" << endl;
            return_val = -1;
        }
        print();
        pthread_mutex_unlock(&mutex);
        return return_val;

    }


    ~HeapManager() {
        cout << "Execution is done" << endl;
        print();
    }

private:
    list<linkNode> list_;
    
    void print(){
        int counter = 0;
        auto cizgisiz = list_.size() - 1;
        for(linkNode node: list_){
            if(counter == cizgisiz) cout << "[" << node.id << "]" << "[" << node.size << "]" << "[" << node.index << "]";
            else cout << "[" << node.id << "]" << "[" << node.size << "]" << "[" << node.index << "]" << "---";
            counter++;
        }
        cout << endl;
    }
};