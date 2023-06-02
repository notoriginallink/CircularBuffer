#pragma once
#include <iostream>

template<typename T, typename Allocator = std::allocator<T>>
class CCircularBuffer{
private:
    size_t capacity_;
    size_t size_;
    T* start_;
    uint32_t readptr_;
    uint32_t writeptr_;
    Allocator alloc_;
public:
    using value_type = T;
    using const_value_type = const T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    CCircularBuffer() : capacity_(0), start_(nullptr), readptr_(0), writeptr_(0), size_(0) {}

    explicit CCircularBuffer(size_t size) : capacity_(size) {
        start_ = alloc_.allocate(size);
        readptr_ = 0;
        writeptr_ = 0;
        size_ = 0;
    }

    CCircularBuffer(size_t size, T sample) : capacity_(size) {
        start_ = alloc_.allocate(size);
        readptr_ = 0;
        writeptr_ = 0;
        size_ = size;
        for (; writeptr_ != size; start_[writeptr_] = sample, writeptr_++);
        writeptr_ = 0;
    }

    explicit CCircularBuffer(const CCircularBuffer<T>& other) {
        readptr_ = other.readptr_;
        writeptr_ = other.writeptr_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        start_ = alloc_.allocate(other.capacity_);
        for (size_type i = 0; i < other.size_; ++i) {
            start_[i] = other.start_[i];
        }
    }

    ~CCircularBuffer() {
        alloc_.deallocate(start_, capacity_);
    }


    CCircularBuffer& operator=(const CCircularBuffer<value_type>& other) {
        this->clear();
        if (capacity_ < other.capacity_) {
            alloc_.deallocate(start_, capacity_);
            start_ = alloc_.allocate(other.capacity_);
            capacity_ = other.capacity_;
            size_ = other.size_;
            for (size_type i = 0; i < other.size_; ++i)
                start_[i] = other.start_[i];
            readptr_ = other.readptr_;
            writeptr_ = other.writeptr_;
        } else {
            for (size_type i = 0; i < other.size_; ++i)
                this->push_back(other.start_[i]);
        }

        return *this;
    }

    template<typename Tp, bool ReverseIterator = false>
    class Iterator {
    public:
        using value_type = Tp;
        using reference = Tp&;
        using pointer = Tp*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        Iterator(pointer ptr, pointer start, size_type size, bool cycle) {
            ptr_ = ptr;
            start_ = start;
            size_ = size;
            cycle_ = cycle;
        }

        ~Iterator() = default;

        reference operator*() { return *ptr_; }

        Iterator& operator=(const Iterator<Tp, ReverseIterator>& it) {
            ptr_ = it.ptr_;
            start_ = it.start_;
            size_ = it.size_;
            cycle_ = it.cycle_;

            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            if (ReverseIterator) {
                --(*this);
            } else
                ++(*this);

            return temp;
        }

        Iterator& operator++() {
            if (ReverseIterator) {
                if (ptr_ == start_) {
                    ptr_ = start_ + size_ - 1;
                    cycle_ = !cycle_;
                } else
                    --(this->ptr_);
            } else {
                if (ptr_ - start_ == size_ - 1) {
                    ptr_ = start_;
                    cycle_ = !cycle_;
                } else
                    ++(this->ptr_);
            }

            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            if (ReverseIterator)
                ++(*this);
            else
                --(*this);

            return temp;
        }

        Iterator& operator--() {
            if (ReverseIterator) {
                if (ptr_ - start_ == size_ - 1) {
                    ptr_ = start_;
                    cycle_ = !cycle_;
                } else
                    ++(this->ptr_);
            } else {
                if (ptr_ == start_) {
                    ptr_ = start_ + size_ - 1;
                    cycle_ = !cycle_;
                } else
                    --(this->ptr_);
            }

            return *this;
        }

        Iterator& operator+=(const difference_type n) {
            if (ReverseIterator) {
                *this -= -1 * (n);
            } else {
                bool sign = n < 0;
                difference_type shift;
                if (sign)
                    shift = -1 * ((-1 * n) % size_);
                else
                    shift = n % size_;
                if (!sign) {
                    if (shift > start_ + size_ - 1 - ptr_)
                        cycle_ = !cycle_;
                    shift = (ptr_ - start_ + shift) % size_;
                    ptr_ = start_ + shift;
                } else {
                    if (shift + ptr_ < start_)
                        cycle_ = !cycle_;
                    shift = (ptr_ - start_ + (size_ + shift)) % size_;
                    ptr_ = start_ + shift;
                }
            }

            return *this;
        }

        Iterator operator+(const difference_type& n) {
            Iterator temp = *this;

            return temp += n;
        }

        friend Iterator operator+(const difference_type n, Iterator& it) {
            Iterator temp = it;
            if (ReverseIterator)
                return temp -= n;
            else
                return temp += n;
        }

        Iterator operator-=(const difference_type n) {
            if (ReverseIterator) {
                bool sign = n < 0;
                difference_type shift;
                if (sign)
                    shift = -1 * ((-1 * n) % size_);
                else
                    shift = n % size_;
                if (!sign) {
                    if (shift > start_ + size_ - 1 - ptr_)
                        cycle_ = !cycle_;
                    shift = (ptr_ - start_ + shift) % size_;
                    ptr_ = start_ + shift;
                } else {
                    if (shift + ptr_ < start_)
                        cycle_ = !cycle_;
                    shift = (ptr_ - start_ + (size_ + shift)) % size_;
                    ptr_ = start_ + shift;
                }
            } else {
                *this += -1 * (n);
            }

            return *this;
        }

        Iterator operator-(const difference_type n) {
            Iterator temp = *this;

            return (temp -= n);
        }

        difference_type operator-(const Iterator& it) {
            if (cycle_ == it.cycle_)
                return ptr_ - it.ptr_;
            else
                return ptr_ - it.ptr_ + (cycle_ ? size_ : -1 * size_);
        }

        reference operator[](const difference_type n) {
            return *(*this + n);
        }

        operator Iterator<const T, ReverseIterator>() const {
            return Iterator<const T, ReverseIterator>(ptr_, start_, size_, cycle_);
        }

        bool operator==(const Iterator& other) const{ return this->ptr_ == other.ptr_ && this->cycle_ == other.cycle_; }
        bool operator!=(const Iterator& other) { return !(*this == other); }
        bool operator<(Iterator& other) const { return (other - *this) > 0; }
        bool operator>(const Iterator& other) {return (other < *this); }
        bool operator>=(const Iterator& other) { return !(*this < other); }
        bool operator<=(const Iterator& other) { return !(*this > other); }
        pointer operator->() { return ptr_; }
    private:
        pointer ptr_;
        pointer start_;
        size_type size_;
        // cycle_ - is supporting flag for correct iterator comparisons
        bool cycle_;
    };

    using iterator = Iterator<value_type>;
    using const_iterator = Iterator<const_value_type>;
    using reverse_iterator = Iterator<value_type, true>;
    using const_reverse_iterator = Iterator<const_value_type, true>;

    CCircularBuffer(iterator& first, iterator& last) {
        capacity_ = std::distance(first, last);
        size_ = capacity_;
        start_ = alloc_.allocate(capacity_);
        for (size_type i = 0; i < size_; ++i)
            start_[i] = *first++;
        readptr_ = 0;
        writeptr_ = 0;
    }

    CCircularBuffer(const std::initializer_list<T>& il) {
        start_ = alloc_.allocate(il.size());
        size_ = il.size();
        capacity_ = il.size();
        readptr_ = 0;
        writeptr_ = 0;
        typename std::initializer_list<T>::iterator it = il.begin();
        for (size_type i = 0; i < size_; ++i)
            start_[i] = it[i];
    }

    CCircularBuffer<T>& operator=(const std::initializer_list<T>& il) {
        if (il.size() <= capacity_) {
            typename CCircularBuffer<T>::iterator it1 = this->begin();
            typename std::initializer_list<T>::iterator it2 = il.begin();
            for (; it2 != il.end(); ++it1, ++it2)
                *it1 = *it2;
            writeptr_ = il.size() % size_;
            size_ = il.size();
        } else {
            CCircularBuffer<T> temp(il);
            this->swap(temp);
        }

        return *this;
    }

    iterator begin() { return iterator(&start_[readptr_], start_, capacity_, false); }

    iterator end() {
        if (size_ == capacity_){
            iterator temp(&start_[readptr_], start_, capacity_, true);
            return temp;
        } else {
            if (writeptr_ < readptr_) {
                iterator temp(&start_[writeptr_], start_, capacity_, true);
                return temp;
            }
            iterator temp(&start_[writeptr_], start_, capacity_, false);
            return temp;
        }
    }

    const_iterator cbegin() const { return const_iterator(&start_[readptr_], start_, capacity_, false); }

    const_iterator cend() const {
        if (size_ == capacity_){
            const_iterator temp(&start_[readptr_], start_, capacity_, true);
            return temp;
        } else {
            if (writeptr_ < readptr_) {
                const_iterator temp(&start_[writeptr_], start_, capacity_, true);
                return temp;
            }
            const_iterator temp(&start_[writeptr_], start_, capacity_, false);
            return temp;
        }
    }

    reverse_iterator rbegin() {
        if (size_ == capacity_) {
            reverse_iterator temp(&start_[readptr_], start_, capacity_, true);
            ++temp;
            return temp;
        } else {
            if (writeptr_ < readptr_) {
                reverse_iterator temp(&start_[writeptr_], start_, capacity_, true);
                ++temp;
                return temp;
            }
            reverse_iterator temp(&start_[writeptr_], start_, capacity_, false);
            ++temp;
            return temp;
        }
    }

    reverse_iterator rend() {
        reverse_iterator temp(&start_[readptr_], start_, capacity_, false);
        ++temp;
        return temp;
    }

    const_reverse_iterator crbegin() const {
        if (size_ == capacity_) {
            const_reverse_iterator temp(&start_[readptr_], start_, capacity_, true);
            ++temp;
            return temp;
        } else {
            if (writeptr_ < readptr_) {
                const_reverse_iterator temp(&start_[writeptr_], start_, capacity_, true);
                ++temp;
                return temp;
            }
            const_reverse_iterator temp(&start_[writeptr_], start_, capacity_, false);
            ++temp;
            return temp;
        }
    }

    const_reverse_iterator crend() const {
        const_reverse_iterator temp(&start_[readptr_], start_, capacity_, false);
        ++temp;
        return temp;
    }

    void push_back(const value_type& value) {
        *(start_ + writeptr_) = value;
        writeptr_ = (writeptr_ + 1) % capacity_;
        if (size_ == capacity_)
            readptr_ = (readptr_ + 1 ) % capacity_;
        else
            size_++;
    }

    void push_front(const value_type& value) {

        if (readptr_ == 0)
            readptr_ = capacity_ - 1;
        else
            readptr_--;
        *(start_ + readptr_) = value;
        if (size_ == capacity_)
            writeptr_ = (writeptr_ == 0 ? capacity_ - 1 : writeptr_ - 1);
        else
            size_++;

    }

    void pop_front() {
        if (!empty()) {
            readptr_++;
            if (readptr_ == size_)
                readptr_ = 0;
            size_--;
        }
    }

    void pop_back() {
        if (!empty()) {
            if (writeptr_ == 0)
                writeptr_ = capacity_ - 1;
            else
                writeptr_--;
            size_--;
        }
    }

    reference front() { return *this->begin(); }

    reference back() {
        iterator temp = this->end();
        --temp;

        return *temp;
    }

    void clear() {
        writeptr_ = 0;
        readptr_ = 0;
        size_ = 0;
        for (int i = 0; i < capacity_; ++i)
            alloc_.destroy(start_ + i);
    }

    iterator insert(const_iterator p, const value_type& value) {
        if (p == this->cbegin()) {
            iterator temp = this->begin() + (p - this->cbegin());

            return temp;
        }
        if (full()) {
            iterator temp = this->begin() + (p - this->cbegin());
            --temp;
            *temp = value;

            return temp;
        } else {
            iterator temp = this->end();
            for (; p != temp; --temp)
                *temp = *(temp - 1);
            *temp = value;
            size_++;
            writeptr_ = (writeptr_ + 1) % capacity_;

            return temp;

        }

    }

    iterator insert(const_iterator p, const size_type& n, const value_type& value) {
        size_type ins = std::min<size_t>(n, (p - this->cbegin()) + this->space_left());
        size_type overwrite = std::min<int32_t>(p - this->cbegin(), std::max<int32_t>(0, n - this->space_left()));
        if (this->full()) {
            iterator temp = this->begin() + (p - this->cbegin());
            for (size_type i = 1; i <= overwrite; ++i) {
                *(temp - i) = value;
            }

            return temp;
        } else {
            iterator temp = this->begin() + (p - this->cbegin());
            size_t shift = this->end() - temp;
            for (size_t i = shift; i > 0; --i)
                *(temp + i - 1 + (ins - overwrite)) = *(temp + i - 1);
            for (size_t i = 0; i < (ins - overwrite); ++i)
                *(temp + i) = value;
            writeptr_ = (writeptr_ + ins - overwrite) % capacity_;
            size_ = size_ + ins - overwrite;
            if (overwrite != 0) {
                for (size_t i = 1; i <= overwrite; ++i)
                    *(temp - i) = value;
            }

            return temp - overwrite;
        }
    }

    template<typename InputIterator>
    iterator insert(const_iterator p, InputIterator first, InputIterator last) {
        size_t len = std::distance(first, last);
        size_t ins = std::min<size_t>(len,
                                      p - this->cbegin() + this->space_left());
        size_t overwrite = std::min<int32_t>(p - this->cbegin(),
                                             std::max<int32_t>(0, static_cast<int32_t>(len - this->space_left())));
        if (full()) {
            iterator temp = this->begin() + (p - this->cbegin());
            for (size_t i = 0; i < len - ins; ++i, ++first);
            for (size_t i = 0; i < overwrite; ++i)
                *(temp - ins + i) = *first++;

            return temp;
        } else {
            iterator temp = this->begin() + (p - this->cbegin());
            size_t shift = this->end() - temp;
            for (size_t i = shift; i > 0; --i)
                *(temp + i - 1 + (ins - overwrite)) = *(temp + i - 1);
            writeptr_ = (writeptr_ + ins - overwrite) % capacity_;
            size_ = size_ + ins - overwrite;
            temp -= overwrite;
            for (size_t i = 0; i < len - ins; ++i, ++first);
            for (size_t i = 0; i < ins; ++i)
                *(temp + i) = *first++;

            return temp;
        }
    }

    iterator insert(const_iterator p, std::initializer_list<T> il) {
        return this->insert(p, il.begin(), il.end());
    }

    iterator erase(const_iterator p) {
        if (p == this->cend())
            return this->end();

        if (p == this->cbegin()) {
            readptr_++;
            size_--;
            return this->begin();
        }

        iterator temp = this->begin() + (p - this->cbegin());
        for (; temp != this->end(); ++temp)
            *temp = *(temp + 1);
        size_--;
        if (writeptr_ == 0)
            writeptr_ = size_;
        else
            writeptr_--;

        return this->begin() + (p - this->cbegin());
    }

    iterator erase(const_iterator q1, const_iterator q2) {
        if (q1 == q2)
            return this->begin();
        if (q1 == this->cbegin()) {
            readptr_ = q2 - this->cbegin();
            size_ -= std::distance(q1, q2);

            return this->begin();
        }
        if (q2 == this->cend()) {
            size_ -= std::distance(q1, q2);
            writeptr_ = size_;

            return this->end() - 1;
        }
        size_t rem = std::distance(q1, q2);
        iterator temp_q1 = this->begin() + (q1 - this->cbegin());
        iterator temp_q2 = this->begin() + (q2 - this->cbegin());
        for (; temp_q2 != this->end(); ++temp_q2)
            *temp_q1++ = *temp_q2;
        size_ -= rem;
        writeptr_ = size_;

        return this->end() - 1;
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        this->clear();
        if (std::distance(first, last) > capacity_)
            this->resize(std::distance(first, last));
        for (; first != last; ++first)
            this->push_back(*first);
    }

    void assign(std::initializer_list<T> il) {
        this->assign(il.begin(), il.end());
    }

    void assign(const size_t& n, const value_type& value) {
        this->clear();
        if (n > capacity_)
            this->resize(n);
        for (size_t i = 0; i < n; ++i)
            this->push_back(value);
    }

    template<typename... Args>
    iterator emplace(const_iterator p, Args&&... args) {
        this->insert(p, value_type(std::forward<Args>(args)...));
    }

    template<typename... Args>
    iterator emplace_back(Args&&... args) {
        this->push_back(value_type(std::forward<Args>(args)...));
    }

    reference operator[](size_type index) {
        return *(this->begin() + index);
    }

    reference at(size_type index) {
        return *(this->begin() + index);
    }

    bool operator==(const CCircularBuffer<T>& other) const {
        if (this->size_ != other.size_)
            return false;
        if (std::equal(this->cbegin(), this->cend(), other.cbegin(), other.cend()))
            return true;

        return false;
    }

    bool operator!=(const CCircularBuffer<T>& other) const { return !(*this == other); }

    void swap(CCircularBuffer<T, Allocator>& other) {
        std::swap(start_, other.start_);
        std::swap(readptr_, other.readptr_);
        std::swap(writeptr_, other.writeptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    size_type size() const { return size_; }

    size_type capacity() const { return capacity_; }

    size_type max_size() const { return alloc_.max_size(); }

    bool full() const {return size_ == capacity_; }

    bool empty() { return size_ == 0; }

    size_type space_left() const { return capacity_ - size_; }

    void resize(const size_type newSize) {
        if (newSize == capacity_)
            return;
        else if (newSize < capacity_) {
            iterator first = this->begin();
            iterator last = this->begin() + newSize - 1;
            CCircularBuffer<T> temp(first, last);
            this->swap(temp);
        } else {
            CCircularBuffer temp(newSize);
            for (iterator it = this->begin(); it != this->end(); ++it)
                temp.push_back(*it);
            this->swap(temp);
        }
    }

};

template<typename T>
void swap(CCircularBuffer<T>& a, CCircularBuffer<T>& b) { a.swap(b); }

