/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <iterator>
#include <cstddef>
#include <iostream>

template <typename T>
struct Node {
    // 这两个指针会造成什么问题？请修复
    std::unique_ptr<Node> next;
    Node* prev;
    // 如果能改成 unique_ptr 就更好了!

    T value;

    // 这个构造函数有什么可以改进的？
    explicit Node(int val): next{nullptr}, prev{nullptr}, value{val} {}

    void erase() {
        if (next)
            next->prev = prev;
        if (prev)
            prev->next = std::move(next);
    }

    ~Node() {
        printf("~Node()\n");   // 应输出多少次？为什么少了？
    }
};

template<typename T>
struct List {

    class iterator {
    private:
        Node<T>* m_ptr;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using diffrent_type     = std::ptrdiff_t;
        using value_type        = Node<T>;
        using pointer           = value_type*;
        using reference         = value_type&; 

        iterator(pointer ptr): m_ptr(ptr) {}
        reference operator*() { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        iterator& operator++(){
            m_ptr = m_ptr -> next.get();
            return *this;
        }
        
        iterator operator++(int){
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator == (const iterator& a, const iterator& b){
            return a.m_ptr == b.m_ptr;
        }
        friend bool operator != (const iterator& a, const iterator& b){
            return a.m_ptr != b.m_ptr;
        }
    };
    std::unique_ptr<Node<T> > head;
    Node<T>* back{nullptr};
    List() = default;

    List(List const &other) {
        printf("List 被拷贝！\n");
        // 请实现拷贝构造函数为 **深拷贝**
        for(auto it = other.begin(); it != other.end(); it++){
            push_back(it->value);
        }
    }

    List(List &&) = default;
    List &operator=(List &&) = default;

    Node<T> *front() const {
        return head.get();
    }

    T pop_front() {
        if(begin() == end()){
            throw std::out_of_range("pop_front()");
        }
        T ret = head->value;
        if(head.get() == back) 
            back = nullptr;
        head = std::move(head->next);
        return ret;
    }

    void push_front(const T& value) {
        auto node = std::make_unique<Node<T>>(value);
        if (head)
            head->prev = node.get();
        else
            back = node.get();
        node -> next = std::move(head);
        head = std::move(node);
    }

    Node<T> *at(size_t index) const {
        auto curr = front();
        for (size_t i = 0; i < index; i++) {
            curr = curr->next.get();
        }
        return curr;
    }

    void push_back(const T& value){
        auto node = std::make_unique<Node<T>>(value);
        if(back != nullptr){
            node -> prev = back;
            back -> next = std::move(node);
            back = back->next.get();
        }
        else{
            head = std::move(node);
            back = head.get();
        }
    }
    iterator begin() const {return iterator(head.get());}
    iterator end() const {return iterator(nullptr);}
};

void print(const List<int>& lst) {  // 有什么值得改进的？
    printf("[");
    for (auto &v: lst) {
        printf(" %d", v.value);
    }
    printf(" ]\n"); 
}

int main() {
    List<int> a;

    a.push_front(7);
    a.push_front(5);
    a.push_front(8);
    a.push_front(2);
    a.push_front(9);
    a.push_front(4);
    a.push_front(1);

    print(a);   // [ 1 4 9 2 8 5 7 ]

    a.at(2)->erase();

    print(a);   // [ 1 4 2 8 5 7 ]

    List<int> b = a;

    a.at(3)->erase();

    print(a);   // [ 1 4 2 5 7 ]
    print(b);   // [ 1 4 2 8 5 7 ]

    b = {};
    a = {};

    return 0;
}
