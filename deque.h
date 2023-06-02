#include <iostream>
#include <iterator>
#include <type_traits>

template<typename Iterator>
class common_reverse_iterator {
    Iterator Iter;
public:
    common_reverse_iterator() = default;
    common_reverse_iterator(const common_reverse_iterator& other) = default;
    common_reverse_iterator(const Iterator& Iter): Iter(Iter) {}

    common_reverse_iterator& operator++() { 
        return --Iter; 
    }
    common_reverse_iterator operator++(int) { 
        return Iter--; 
    }
    common_reverse_iterator& operator--() { 
        return ++Iter; 
    }
    common_reverse_iterator operator--(int) { 
        return Iter--; 
    }

    common_reverse_iterator& operator+=(int num) { 
        return Iter -= num;
    }
    common_reverse_iterator& operator-=(int num) { 
        return Iter += num;
    }
    common_reverse_iterator operator+(int num) { 
        return Iter - num;
    }
    common_reverse_iterator operator-(int num) {
        return Iter + num;
    }

    common_reverse_iterator& operator*() {
        return *Iter;
    }
    common_reverse_iterator* operator->() {
        return Iter.operator->();
    }
};

template <typename T>
class Deque { 
public:
    template <bool IsConst>
    struct common_iterator {
    public:
        common_iterator();
        common_iterator(T** , size_t);
        std::conditional_t<IsConst, const T&, T&> operator*();
        std::conditional_t<IsConst, const T*, T*> operator->();
        common_iterator<IsConst>& operator++(int) &;
        common_iterator<IsConst> operator++() &;
        common_iterator<IsConst>& operator--(int) &;
        common_iterator<IsConst> operator--() &;
        common_iterator<IsConst>& operator+=(size_t);
        common_iterator<IsConst> operator+(size_t);
        common_iterator<IsConst>& operator-=(size_t);
        common_iterator<IsConst> operator-(size_t);
        size_t operator-(common_iterator&);
        common_iterator<IsConst>& operator=(common_iterator);
        bool operator>(common_iterator) const;
        bool operator<(common_iterator) const;
        bool operator==(common_iterator) const;
        bool operator!=(common_iterator) const;
        bool operator>=(common_iterator) const;
        bool operator<=(common_iterator) const;
        operator common_iterator<true>() {
            return common_iterator<true>(ptr_, index_);
        }
    private:
        T** ptr_;
        size_t index_;
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = common_reverse_iterator<iterator>;
    using const_reverse_iterator = common_reverse_iterator<const_iterator>;

    Deque();
    explicit Deque(size_t, const T& = T());
    Deque(const Deque<T>&);
    ~Deque();
    Deque<T>& operator=(const Deque<T>&);
    T& operator[](size_t) const;
    T& at(size_t) const;
    size_t size() const;
    void push_back(const T&);
    void push_front(const T&);
    void pop_back();
    void pop_front();
    bool empty() {
        return size_ == 0;
    }
   
    iterator begin();
    const_iterator begin() const; 
    iterator end();
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    
    reverse_iterator rend() const;
    reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    void insert(iterator, const T&);
    void erase(iterator);
private:
    static const size_t array_capacity_ = 128;
    size_t capacity_;
    size_t last_index_;
    size_t first_index_;
    size_t last_array_;
    size_t first_array_;
    size_t size_;
    T** container_;

    void check_size();
    void update_size();
    void clear();
};



template <typename T>
void Deque<T>::check_size() {
    if (first_array_ == 0 || last_array_ == capacity_ - 1) {
        capacity_ = (last_array_ - first_array_ + 1) * 3;
        update_size();
    }
}

template <typename T>
void Deque<T>::update_size() {
    size_t temporaire = capacity_ / 3;
    T** newcontainer = new T*[capacity_];
    for (size_t i = 0; i <= last_array_ - first_array_; ++i) {
        newcontainer[temporaire + i] = container_[first_array_ + i];
    }
    last_array_ = temporaire + last_array_ - first_array_;
    first_array_ = temporaire;
    delete [] container_;
    container_ = newcontainer;
}

template <typename T>
void Deque<T>::clear() {
    for (iterator it = begin(); it < end(); ++it) {
        it->~T();
    }
    for (size_t i = first_array_; i <= last_array_; ++i) {
        delete [] reinterpret_cast<uint8_t*>(container_[i]); 
    }
}



template <typename T>
Deque<T>::Deque() : capacity_(4), 
        last_index_(array_capacity_ - 1), 
        first_index_(0),
        last_array_(1), 
        first_array_(2), 
        size_(0), 
        container_(new T*[capacity_]) {}

template <typename T>
Deque<T>::Deque(size_t n, const T& value) : capacity_(4), 
        last_index_(array_capacity_ - 1), 
        first_index_(0), 
        last_array_(1), 
        first_array_(2), 
        size_(0), 
        container_(new T*[capacity_]) {

    for (size_t i = 0; i < n; ++i) {
        push_back(value);
    }
}

template <typename T>
Deque<T>::Deque(const Deque<T>& other) : capacity_(other.capacity_),
        last_index_(other.last_index_), first_index_(other.first_index_),
        last_array_(other.last_array_), first_array_(other.first_array_),
        size_(other.size_), container_(new T*[other.capacity_]) {
    size_t start_index, end_index;
    for (size_t i = first_array_; i <= last_array_; ++i) {
        container_[i] = reinterpret_cast<T*>(new uint8_t[array_capacity_ * sizeof(T)]);
        start_index = (i == first_array_ ? first_index_ : 0);
        end_index = (i == last_array_ ? last_index_ : array_capacity_ - 1);
        for (size_t j = start_index; j <= end_index; ++j) {
            container_[i][j] = other.container_[i][j];
        }
    }
}



template <typename T>
Deque<T>::~Deque() {
    clear();
    delete [] container_;
}



template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
    if (this != &other) {
        clear();
        Deque<T> temp = other;
        std::swap(capacity_, temp.capacity_);
        std::swap(last_index_, temp.last_index_);
        std::swap(first_index_, temp.first_index_);
        std::swap(last_array_, temp.last_array_);
        std::swap(first_array_, temp.first_array_);
        std::swap(size_, temp.size_);
        std::swap(container_, temp.container_);
    }
    return *this;
}



template <typename T>
T& Deque<T>::operator[](size_t index) const {
    return container_[first_array_ + (first_index_ + index) / array_capacity_][
        (first_index_ + index) % array_capacity_];
}

template <typename T>
T& Deque<T>::at(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("out_of_range");
    }
    return operator[](index);
}



template <typename T>
size_t Deque<T>::size() const {
    return size_;
}



template <typename T>
void Deque<T>::push_back(const T& value) {
    ++size_;
    check_size();
    if (last_index_ + 1 == array_capacity_) {
        container_[++last_array_] = reinterpret_cast<T*>(
                new uint8_t[array_capacity_ * sizeof(T)]);
        last_index_ = 0;
    } else {
        ++last_index_;
    }
    new(container_[last_array_]+last_index_) T(value);
}

template <typename T>
void Deque<T>::push_front(const T& value) {
    ++size_;
    check_size();
    if (first_index_ == 0) {
        container_[--first_array_] = reinterpret_cast<T*>(
                new uint8_t[array_capacity_ * sizeof(T)]);
        first_index_ = array_capacity_ - 1;
    } else {
        --first_index_;
    }
    new(container_[first_array_] + first_index_) T(value);
}

template <typename T>
void Deque<T>::pop_back() {
    if (empty()) {
        return;
    }
    --size_;
    (container_[last_array_]+last_index_)->~T();
    if (last_index_ == 0) {
        last_index_ = array_capacity_ - 1;
        delete [] reinterpret_cast<uint8_t*>(container_[last_array_--]);
    } else {
        --last_index_;
    }
}

template <typename T>
void Deque<T>::pop_front() {
    if (empty()) {
        return;
    }
    --size_;
    (container_[first_array_]+first_index_)->~T();
    if (first_index_ == array_capacity_ - 1) {
        first_index_ = 0;
        delete [] reinterpret_cast<uint8_t*>(container_[first_array_++]);
    } else {
        ++first_index_;
    }
}

///////////////////////////////// ITERATORS /////////////////////////////////

template <typename T>
template <bool IsConst>
Deque<T>::common_iterator<IsConst>::common_iterator() = default;

template <typename T>
template <bool IsConst>
Deque<T>::common_iterator<IsConst>::common_iterator(T** ptr, size_t index) : 
    ptr_(ptr), index_(index){}

template <typename T>
template <bool IsConst>
std::conditional_t<IsConst, const T&, T&> 
        Deque<T>::common_iterator<IsConst>::operator*() {
    return (*ptr_)[index_];
}

template <typename T>
template <bool IsConst>
std::conditional_t<IsConst, const T*, T*> 
        Deque<T>::common_iterator<IsConst>::operator->() {
    return *(ptr_)+index_;
}


template <typename T> 
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>& 
        Deque<T>::common_iterator<IsConst>::operator++(int) & {
    ++index_;
    if (index_ >= array_capacity_) {
        ++ptr_;
        index_ = 0;
    }

    return *this;
}

template <typename T> 
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst> 
        Deque<T>::common_iterator<IsConst>::operator++() & {
    Deque<T>::common_iterator<IsConst> it = *this;
    ++index_;
    if (index_ >= array_capacity_) {
        ++ptr_;
        index_ = 0;
    }
    return it;
}

template <typename T> 
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>& 
        Deque<T>::common_iterator<IsConst>::operator--(int) & {
    if (index_ == 0) {
        --ptr_;
        index_ = array_capacity_ - 1;
    } else {
        --index_;
    }
    return *this;
}

template <typename T> 
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst> 
        Deque<T>::common_iterator<IsConst>::operator--() & {
    Deque<T>::common_iterator<IsConst> it = *this;
    if (index_ == 0) {
        --ptr_;
        index_ = array_capacity_ - 1;
    } else {
        --index_;
    }
    return it;
}



template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>&
        Deque<T>::common_iterator<IsConst>::operator+=(size_t n) {
    ptr_ += (n + index_) / array_capacity_;
    index_ = (n + index_) % array_capacity_;
    return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>
        Deque<T>::common_iterator<IsConst>::operator+(size_t n) {
    common_iterator<IsConst> it = *this;
    it += n;
    return it;
}

template <typename T>                                                           
template <bool IsConst>                                                         
typename Deque<T>::template common_iterator<IsConst>&                           
        Deque<T>::common_iterator<IsConst>::operator-=(size_t n) {              
    if (n <= index_) {
        index_ -= n;
    } else {
        n -= index_;
        ptr_ -= (n + array_capacity_ - 1) / array_capacity_;
        index_ = array_capacity_ - 1 - (n + array_capacity_ - 1) % array_capacity_;
    }
    return *this;
}                                                                               
                                                                                
template <typename T>                                                           
template <bool IsConst>                                                         
typename Deque<T>::template common_iterator<IsConst>                           
        Deque<T>::common_iterator<IsConst>::operator-(size_t n) {               
    common_iterator<IsConst> it = *this;                                                        
    it -= n;                                                                    
    return it;
}

template <typename T>                                                           
template <bool IsConst>                                                         
size_t Deque<T>::common_iterator<IsConst>::operator-(                          
        typename Deque<T>::template common_iterator<IsConst>& other) {
    long long diff = index_;
    diff -= other.index_;    
    return (ptr_ - other.ptr_) * array_capacity_ + diff;                                                               
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>& 
        Deque<T>::common_iterator<IsConst>::operator=(
        typename Deque<T>::template common_iterator<IsConst> other) {
    if (*this != other) {
        index_ = other.index_;
        ptr_ = other.ptr_;
    }
    return *this;
}

template <typename T>                                                            
template <bool IsConst>                                                          
bool Deque<T>::common_iterator<IsConst>::operator>(                           
        typename Deque<T>::template common_iterator<IsConst> other) const {           
    if (ptr_ != other.ptr_) {
        return ptr_ > other.ptr_;
    }
    return index_ > other.index_;
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator<(
        typename Deque<T>::template common_iterator<IsConst> other) const {
    return other > *this;
}

template <typename T>                                                            
template <bool IsConst>                                                          
bool Deque<T>::common_iterator<IsConst>::operator==(                             
        typename Deque<T>::template common_iterator<IsConst> other) const {           
    if (ptr_ != other.ptr_) {                                                   
        return false;                                               
    }                                                                           
    return index_ == other.index_;                                                
}

template <typename T>                                                            
template <bool IsConst>                                                          
bool Deque<T>::common_iterator<IsConst>::operator!=(                             
        typename Deque<T>::template common_iterator<IsConst> other) const {           
    return !(*this == other);                                                
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator>=(
        typename Deque<T>::template common_iterator<IsConst> other) const {
    return (*this > other) | (*this == other);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator<=(
        typename Deque<T>::template common_iterator<IsConst> other) const {
    return (*this < other) | (*this == other);
}


template <typename T>
typename Deque<T>::iterator Deque<T>::begin() {
    return iterator(container_ + first_array_, first_index_);
}

template <typename T>                                                           
typename Deque<T>::const_iterator Deque<T>::begin() const {                                 
    return const_iterator(container_ + first_array_, first_index_);             
} 

template <typename T>
typename Deque<T>::iterator Deque<T>::end() {
    return iterator(container_ + last_array_, last_index_ + 1);
}

template <typename T>                                                           
typename Deque<T>::const_iterator Deque<T>::end() const {                      
    return const_iterator(container_ + last_array_, last_index_ + 1);     
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
    return const_iterator(container_ + first_array_, first_index_);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
    return const_iterator(container_ + last_array_, last_index_ + 1);
}


template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rbegin() const {
    return common_reverse_iterator<iterator>(--begin());
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rend() const {
    return common_reverse_iterator<iterator>(--end());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crbegin() const {
    return common_reverse_iterator<const_iterator>(cbegin());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crend() const {
    return common_reverse_iterator<const_iterator>(--cend());
}




template <typename T>
void Deque<T>::insert(typename Deque<T>::iterator iter, const T& element) {
    push_back(T());
    for (iterator i = end() - 1; i >= iter; --i) {
        *(i + 1) = *i;
    }
    *iter = element;
}

template <typename T>
void Deque<T>::erase(typename Deque<T>::iterator iter) {
    for (iterator i = iter; i < end(); ++i) {
        *i = *(i + 1);
    }
    pop_back();
}

