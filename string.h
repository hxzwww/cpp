#include <iostream>
#include <cstring>

class String {

private:

    size_t capacity;
    size_t cur_size;
    char* string;

    void recalculate_capacity() {
        if (cur_size >= capacity - 2) {
            update_capacity();
        }
    }

    void update_capacity() {
        char* string_copy = new char [cur_size * 2];
        memcpy(string_copy, string, cur_size);
        capacity = cur_size * 2;
        delete [] string;
        string = string_copy;
    }

public:

    String() : capacity(4), cur_size(0), string(new char [capacity]) {}

    String(size_t n, char c) {
        capacity = n * 2;
        cur_size = n;
        string = new char [capacity];
        memset(string, c, n);
    }

    String(const char* cstyle_string) {
        cur_size = strlen(cstyle_string);
        capacity = cur_size * 2;
        string = new char [capacity];
        memcpy(string, cstyle_string, cur_size);
    }

    String(char sym) {
        capacity = 4;
        cur_size = 1;
        string = new char [capacity];
        string[0] = sym;
    }

    String(const String& str) {
        cur_size = str.cur_size;
        capacity = cur_size * 2;
        string = new char [capacity];
        memcpy(string, str.string, cur_size);
    }

    ~String() {
        delete [] string;
    }

    size_t length() const {
        return cur_size;
    }

    void push_back(char symbol) {
        ++cur_size;
        recalculate_capacity();
        string[cur_size - 1] = symbol;
    }

    void pop_back() {
        --cur_size;
    }

    char& front() {
        return string[0];
    }

    const char& front() const {
        return string[0];
    }

    char& back() {
        return string[cur_size - 1];
    }

    const char& back() const {
        return string[cur_size - 1];
    }

    const char& operator[](size_t index) const {
        return string[index];
    }

    char& operator[](size_t index) {
        return string[index];
    }

    String& operator+=(const String& str) {
        size_t start_size = cur_size;
        cur_size += str.cur_size;
        recalculate_capacity();
        for (size_t i = start_size; i < cur_size; ++i) {
            string[i] = str[i - start_size];
        }
        return *this;
    }

    String& operator+=(const char other) {
        push_back(other);
        return *this;
    }

    String& operator=(const String& other) {
        if (this == &other) {
            return *this;
        }
        cur_size = other.cur_size;
        recalculate_capacity();
        memcpy(string, other.string, cur_size);
        return *this;
    }

    size_t find(const String& required_string) const {
        size_t index;
        for (size_t i = 0; i <= (cur_size - required_string.cur_size); ++i) {
            if (string[i] == required_string[0]) {
                index = i;
                for (size_t k = i; k < i + required_string.cur_size; ++k) {
                    if (string[k] != required_string[k - i]) {
                        break;
                    }
                    if (k + 1 == i + required_string.cur_size) {
                        return index;
                    }
                }
            }
        }
        return cur_size;
    }

    size_t rfind(const String& required_string) const {
        for (size_t i = cur_size - 1; 1 >= required_string.cur_size - 1; --i) {
            if (string[i] == required_string[required_string.cur_size-1]) {
                for (size_t k = i; k > i - required_string.cur_size; --k) {
                    if (string[k] != required_string[required_string.cur_size - 1 + k - i]) {
                        break;
                    }
                    if (k - 1 == i - required_string.cur_size) {
                        return k;
                    }
                }
            }
        }
        return cur_size;
    }

    bool empty() {
        return !cur_size;
    }

    void clear() {
        capacity = 4;
        cur_size = 0;
        delete [] string;
        string = new char [capacity];
    }

    String substr(size_t start, size_t count) const {
        String new_string;
        for (size_t i = start; i < start + count; ++i) {
            new_string.push_back(string[i]);
        }
        return new_string;
    }
};

bool operator==(const String& lhs, const String& rhs) {
    if (lhs.length() != rhs.length()) {
        return false;
    }
    for (size_t i = 0; i < lhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

String operator+(const String& lhs, const String& rhs) {
    String copy(lhs);
    copy += rhs;
    return copy;
}

String operator+(const String& lhs, char rhs) {
    String copy(lhs);
    copy += rhs;
    return copy;
}

String operator+(char lhs, const String& rhs) {
    String copy_lhs(1, lhs);
    copy_lhs += rhs;
    return copy_lhs;
}

std::istream& operator>>(std::istream& in, String& str) {
    str.clear();
    char sym;
    while (in.get(sym)) {
        if (sym == '\n' || sym == ' ') {
            break;
        }
        str.push_back(sym);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        out << str[i];
    }
    return out;
}

