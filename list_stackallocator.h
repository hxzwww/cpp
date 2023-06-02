#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <type_traits>



template <size_t N>
struct StackStorage {
public:
    StackStorage() : pointer(memory) {}
    uint8_t* move_ptr(size_t n, size_t align) {
        size_t address = reinterpret_cast<uintptr_t>(pointer);
        size_t shift = (align - address % align) % align;
        pointer = reinterpret_cast<uint8_t*>(address + shift);
        uint8_t* tmp = pointer;
        pointer += n;
        return tmp;
    }
private: 
    uint8_t memory[N];
    uint8_t* pointer;
};



template <typename T, size_t N>
struct StackAllocator {
public:
    using value_type = T;
    
    explicit StackAllocator() = default;

    template <typename Y>
    StackAllocator(const StackAllocator<Y, N>& alloc) : 
        stack_storage(alloc.get_stack_storage()) {}

    explicit StackAllocator(StackStorage<N>& storage) : stack_storage(&storage) {}


    ~StackAllocator() = default;

    bool operator!=(const StackAllocator& other) {                              
        return stack_storage != other.stack_storage;                            
    }

    template <typename Y>
    StackAllocator& operator=(StackAllocator<Y, N>& alloc) {
        stack_storage = alloc.stack_storage;
        return *this;
    }
    
    StackStorage<N>* get_stack_storage() const {
        return stack_storage;
    }

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    }; 

    T* allocate(size_t n) {
        return reinterpret_cast<T*>(
                stack_storage->move_ptr(n * sizeof(T), alignof(T)));
    }

    void deallocate(T*, size_t) {}
    
private:
    StackStorage<N>* stack_storage;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, typename Alloc = std::allocator<T>>
class List {
private:    
    struct Node;
    using RealAlloc = typename Alloc::template rebind<Node>::other;
    using AllocTraits = std::allocator_traits<RealAlloc>;
public:
    List() : alloc(RealAlloc()), size_(0) {
        make_fake_node_();
    }
    
    List(size_t n, const T& val, Alloc all) : alloc(all), size_(0) {
        fill_(n, val);
    } 
    
    explicit List(size_t);

    explicit List(size_t n, const T& val) : size_(0) {
        fill_(n, val);
    }
    
    List(size_t, Alloc);
    
    explicit List(Alloc all) : alloc(AllocTraits::
            select_on_container_copy_construction(all)), 
            size_(0) { 
                make_fake_node_();
            }   
    
    List(const List<T, Alloc>& list) : alloc(AllocTraits::
        select_on_container_copy_construction(list.alloc)), size_(list.size_) { 
        memcpy_(list);
    }
    
    List& operator=(const List<T, Alloc>&);

    ~List();
    
    RealAlloc& get_allocator() { 
        return alloc;
    }
    
    size_t size() const {
        return size_;
    }

    template <bool IsConst>
        struct common_iterator {
        public:
            using value_type = std::conditional_t<IsConst, const T, T>;
            using reference = std::conditional_t<IsConst, const T&, T&>;
            using pointer = std::conditional_t<IsConst, const T*, T*>;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = long long;

            common_iterator() = default;
            explicit common_iterator(Node* node) : ptr_(node) {}

            common_iterator<IsConst>& operator=(const common_iterator& iter) {
                if (*this != iter){
                    ptr_ = iter.ptr_;
                }
                return *this;
            }

            std::conditional_t<IsConst, const T&, T&> operator*() { 
                return ptr_->value;
            }

            std::conditional_t<IsConst, const T*, T*> operator->() {
                return &(ptr_->value);
            }

            common_iterator<IsConst>& operator++() & {
                ptr_ = ptr_->to;
                return *this;
            }

            common_iterator<IsConst> operator++(int) & {
                common_iterator<IsConst> iter = *this;
                ptr_ = ptr_->to;
                return iter;
            }

            common_iterator<IsConst>& operator--() & {
                ptr_ = ptr_->from;
                return *this;
            }

            common_iterator<IsConst> operator--(int) & {
                common_iterator<IsConst> iter = *this;
                ptr_ = ptr_->from;
                return iter;
            }

            bool operator==(const common_iterator& iter) const {
                return ptr_ == iter.ptr_;
            }

            bool operator!=(const common_iterator& iter) const {
                return ptr_ != iter.ptr_;
            }

            operator common_iterator<true>() {
                return common_iterator<true>(ptr_);
            }

            Node* ptr() { 
                return ptr_; 
            }
        
        private:
            Node* ptr_;
        };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { 
        return iterator(fake_node->to);
    }

    const_iterator begin() const {
        return const_iterator(fake_node->to);
    }

    iterator end() {
        return iterator(fake_node);
    }

    const_iterator end() const {
        return const_iterator(fake_node); 
    }

    const_iterator cbegin() { 
        return const_iterator(fake_node->to);
    }

    const_iterator cend() {
        return const_iterator(fake_node);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin()); 
    }

    const_reverse_iterator crbegin() {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() {
        return const_reverse_iterator(cbegin());
    }
   
    void insert(const_iterator, const T&);
    void erase(const_iterator);

    void push_back(const T&);
    void push_front(const T&);
    void pop_back();
    void pop_front();
private:
    RealAlloc alloc;
    size_t size_;
    Node* fake_node;
   
    void fill_(size_t, const T&);
    void make_fake_node_();    
    void memcpy_(const List<T, Alloc>&);
};

template <typename T, typename Alloc>
struct List<T, Alloc>::Node {                                                                
    T value;                                                                 
    Node* to;                                                                
    Node* from;                                                              
                                                                             
    Node() = default;                                                                
    explicit Node(const T& val) : value(val), to(nullptr), from(nullptr) {}
    Node& operator*() {
        return *this;
    }    
};

template <typename T, typename Alloc>
void List<T, Alloc>::fill_(size_t n, const T& val) {                                        
    make_fake_node_();                                                      
    for (size_t i = 0; i < n; ++i) {                                        
        push_back(val);                                                     
    }                                                                       
}

template <typename T, typename Alloc>
void List<T, Alloc>::make_fake_node_() {                                                    
    fake_node = alloc.allocate(1);                                          
    fake_node->from = fake_node;                                            
    fake_node->to = fake_node;                                              
}

template <typename T, typename Alloc>                                           
void List<T, Alloc>::memcpy_(const List<T, Alloc>& list) {                      
    size_ = 0;                                                                  
    make_fake_node_();                                                          
    Node* tmp = fake_node;
    Node* node = list.fake_node;
    try {                                                                       
        for (auto i = list.begin(); i != list.end(); ++i) {                     
            try {
                tmp->to = alloc.allocate(1);                                        
                AllocTraits::construct(alloc, tmp->to, *i);
            }
            catch (...) {
                throw;
            }            
            tmp->to->from = tmp;                                                
            tmp = tmp->to;                                                      
            ++size_;                                                            
        }                                                                       
        tmp->to = fake_node;                                                    
        fake_node->from = tmp;                                                  
    }                                                                           
    catch (...) {                                                               
        tmp = fake_node->to;                                                    
        for (size_t i = 0; i < size_; ++i) {                                    
            node = tmp->to;                                                     
            AllocTraits::deallocate(alloc, tmp, 1);                             
            tmp = node;                                                         
        }                                                                       
        size_ = 0;                                                              
        fake_node->from = fake_node;                                            
        fake_node->to = fake_node;                                              
    }                                                                           
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t n, Alloc all) : alloc(all), size_(n) {                           
    make_fake_node_();                                                       
    Node* tmp = fake_node;                                                   
    for (size_t i = 0; i < n; ++i) {                                         
        tmp->to = alloc.allocate(1);                                         
        AllocTraits::construct(alloc, tmp->to);                              
        tmp->to->from = tmp;                                                 
        tmp = tmp->to;                                                       
    }                                                                        
    fake_node->from = tmp;                                                   
    tmp->to = fake_node;                                                     
}

template <typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& list) {                               
    for (auto i = begin(); i != end(); ++i) {                               
        (*i).~T();                                                          
    }                                                                       
    if (AllocTraits::propagate_on_container_copy_assignment::value) {       
        alloc = list.alloc;                                                 
    }                                                                       
    memcpy_(list);                                                          
    return *this;                                                           
} 

template <typename T, typename Alloc>
List<T, Alloc>::~List() {
    Node* tmp;
    Node* cur = fake_node->to;
    for (size_t i = 0; i < size_; ++i) {
        tmp = cur->to;
        cur->value.~T();
        AllocTraits::deallocate(alloc, cur, 1);
        cur = tmp;
    }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t n) : size_(n) {
    make_fake_node_();
    Node* tmp = fake_node;
    Node* node; 
    for (size_t i = 0; i < n; ++i) {
        try {
            node = alloc.allocate(1);
            AllocTraits::construct(alloc, node);
        }
        catch (...) {
            tmp = fake_node->to;
            for (size_t j = 0; j < i; ++j) {
                node = tmp->to;
                AllocTraits::deallocate(alloc, tmp, 1);
                tmp = node;
            }
            fake_node->from = fake_node;
            fake_node->to = fake_node;
            size_ = 0;
            return;
        }
        node->from = tmp;
        tmp->to = node;
        tmp = node;
    }
    tmp->to = fake_node;
    fake_node->from = tmp;
}

template <typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator iterat, const T& val) {
    ++size_;
    auto iter = iterat.ptr();
    Node* new_element = AllocTraits::allocate(alloc, 1);
    new_element->value = val;
    new_element->from = iter->from;
    new_element->to = iter;
    iter->from->to = new_element;
    iter->from = new_element;
}

template <typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator iter) {
    --size_;
    Node* deleting = iter.ptr();
    deleting->from->to = deleting->to;
    deleting->to->from = deleting->from;
    AllocTraits::deallocate(alloc, deleting, 1);
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& val) {
    ++size_;
    Node* new_element = AllocTraits::allocate(alloc, 1);
    fake_node->from->to = new_element;
    new_element->value = val;
    new_element->from = fake_node->from;
    new_element->to = fake_node;
    fake_node->from = new_element;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& val) {
    ++size_;
    Node* new_element = AllocTraits::allocate(alloc, 1);
    fake_node->to->from = new_element;
    new_element->value = val;
    new_element->to = fake_node->to;
    new_element->from = fake_node;
    fake_node->to = new_element;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
    --size_;
    fake_node->from->from->to = fake_node;
    fake_node->from->value.~T();
    fake_node->from = fake_node->from->from;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
    --size_;
    fake_node->to->to->from = fake_node;
    fake_node->to->value.~T();
    fake_node->to = fake_node->to->to;
}

