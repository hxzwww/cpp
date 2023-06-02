#include <iostream>
#include <vector>
#include <string>

class BigInteger {

private:
    bool is_negative = false;
    int BASE = 1e9;
    std::vector<int> number;
    size_t size = 0;
    void check_zeros();
    void same_signs_sum(const BigInteger& other);
    void different_signs_sum(const BigInteger& other);
    bool comp_by_abs_value(const BigInteger& other);
    void substraction(const BigInteger& other, size_t left, size_t right);
    bool cmp(const BigInteger& other, size_t left, size_t right);
    long long bin_search(const BigInteger& other, size_t i, size_t index);
    void delete_minus_before_zero();

public:
    BigInteger();
    BigInteger(int64_t num);
    explicit operator bool() const;
    std::string toString() const;
    size_t number_size();
    int number_i(size_t i);
    bool number_negative();
    BigInteger operator=(const BigInteger& other);
    BigInteger& operator+=(const BigInteger& other);
    BigInteger& operator+=(int other);
    BigInteger operator++(int);
    BigInteger& operator++();
    BigInteger& operator-=(const BigInteger& other);
    BigInteger& operator-=(int other);
    BigInteger operator--(int);
    BigInteger& operator--();
    BigInteger operator-();
    BigInteger& operator*=(const BigInteger& other);
    BigInteger& operator*=(int other);
    BigInteger& operator/=(const BigInteger& other);
    BigInteger& operator/=(int other);
    BigInteger& operator%=(const BigInteger& other);
    BigInteger& operator%=(int other);
    friend std::istream& operator>>(std::istream& in, BigInteger& number);
};

void BigInteger::delete_minus_before_zero() {
    if (number.back() == 0) is_negative = false;
}

BigInteger::BigInteger() = default;

BigInteger::BigInteger(int64_t input_num) {
    size = 2;
    is_negative = input_num < 0;
    int64_t num = input_num;
    if (is_negative) num *= -1;
    number.push_back(num % BASE);
    number.push_back(num / BASE);
    check_zeros();
}

BigInteger::operator bool() const {
    return number.back();
}

size_t BigInteger::number_size() {
    return size;
}

int BigInteger::number_i(size_t i) {
    return number[i];
}

bool BigInteger::number_negative() {
    return is_negative;
}

std::string BigInteger::toString() const  {
    std::string new_str;
    if (is_negative && (number.back() != 0)) {
        new_str += "-";
    }
    for (size_t i = 0; i < size; ++i) {
        std::string num = std::to_string(number[size - 1 -i]);
        if (i != 0) {
            for (size_t j = 0; j < 9 - num.size(); ++j) {
                new_str += '0';
            }
        }
        new_str += num;
    }
    return new_str;
}

void BigInteger::check_zeros() {
    while (number[size - 1] <= 0 && size > 1) {
        number.pop_back();
        --size;
    }
}

void BigInteger::same_signs_sum(const BigInteger& other) {
    if (number.back() == 0) {
        *this = other;
        is_negative = false;
        return;
    }
    size_t len = std::min(size, other.size);
    int buffer = 0;
    for (size_t i = 0; i < len; ++i) {
        number[i] = (number[i] + other.number[i] + buffer);
        buffer = number[i] / BASE;
        number[i] %= BASE;
    }
    if (size < other.size) {
        for (size_t i = size; i < other.size; ++i) {
            ++size;
            number.push_back(0);
            number[i] = (other.number[i] + buffer);
            buffer = number[i] / BASE;
            number[i] %= BASE;
        }
    }
    else {
        if (size > other.size) {
            for (size_t i = other.size; i < size; ++i) {
                number[i] = (number[i] + buffer);
                buffer = number[i] / BASE;
                number[i] %= BASE;
            }
        }
    }
    check_zeros();
    delete_minus_before_zero();
}

void BigInteger::different_signs_sum(const BigInteger& other) {
    int sign_of_sum = (comp_by_abs_value(other) ? 1 : -1);
    size_t len = std::min(size, other.size);
    for (size_t i = 0; i < len; ++i) {
        number[i] = (number[i] - other.number[i]) * sign_of_sum;
        if (number[i] < 0) {
            number[i] += BASE;
            if (i + 1 < size) {
                number[i + 1] -= sign_of_sum;
            }
            else {
                number.push_back(sign_of_sum);
            }
        }
    }
    if (size < other.size) {
        for (size_t i = size; i < other.size; ++i) {
            number.push_back(other.number[i]);
            if (number[i] < 0) {
                number[i] += BASE;
                number.push_back(1);
            }
            ++size;
        }
    } else {
        if (size > other.size) {
            for (size_t i = other.size; i < size; ++i) {
                if (number[i] < 0) {
                    number[i] += BASE;
                    if (i + 1 < size) {
                        number[i + 1] -= 1;
                    }
                }
            }
        }
    }
    if (sign_of_sum == -1) {
        is_negative = !is_negative;
    }
    check_zeros();
    delete_minus_before_zero();
}

bool BigInteger::comp_by_abs_value(const BigInteger& other) {
    if (size != other.size) {
        return size > other.size;
    }
    else {
        for (size_t i = 1; i <= size; ++i) {
            if (number[size - i] > other.number[size - i]) {
                return true;
            }
            if (number[size - i] < other.number[size - i]) {
                return false;
            }
        }
    }
    return false;
}

bool operator==(BigInteger& lhs, BigInteger& rhs) {
    if ((lhs.number_size() != rhs.number_size()) || (lhs.number_negative() != rhs.number_negative())) {
        return false;
    }
    for (size_t i = 0; i < lhs.number_size(); ++i) {
        if (lhs.number_i(lhs.number_size() - i - 1) != rhs.number_i(lhs.number_size() - i - 1)) {
            return false;
        }
    }
    return true;
}

bool operator!=(BigInteger& lhs, BigInteger& rhs) {
    return !(lhs == rhs);
}

bool operator>(BigInteger& lhs, BigInteger& rhs) {
    if (lhs.number_negative() && rhs.number_negative()) {
        if (lhs.number_size() != rhs.number_size()) {
            return lhs.number_size() < rhs.number_size();
        }
        for (size_t i = 1; i <= lhs.number_size(); ++i) {
            if (lhs.number_i(lhs.number_size() - i) != rhs.number_i(lhs.number_size() - i)) {
                return lhs.number_i(lhs.number_size() - i) < rhs.number_i(lhs.number_size() - i);
            }
        }
    }
    if (!lhs.number_negative() && !rhs.number_negative()) {
        if (lhs.number_size() != rhs.number_size()) {
            return lhs.number_size() > rhs.number_size();
        }
        for (size_t i = 1; i <= lhs.number_size(); ++i) {
            if (lhs.number_i(lhs.number_size() - i) != rhs.number_i(lhs.number_size() - i)) {
                return lhs.number_i(lhs.number_size() - i) > rhs.number_i(lhs.number_size() - i);
            }
        }
    }
    if (lhs.number_negative() && !rhs.number_negative()) {
        return false;
    }
    return false;
}

bool operator<(BigInteger& lhs, BigInteger& rhs) {
    return rhs > lhs;
}

bool operator>=(BigInteger& lhs, BigInteger& rhs) {
    return lhs > rhs || lhs == rhs;
}

bool operator<=(BigInteger& lhs, BigInteger& rhs) {
    return rhs > lhs || lhs == rhs;
}

BigInteger BigInteger::operator=(const BigInteger& other) {
    if (this == &other) {
        return *this;
    }
    size = other.size;
    is_negative = other.is_negative;
    number.clear();
    for (size_t i = 0; i < size; ++i) {
        number.push_back(other.number[i]);
    }
    delete_minus_before_zero();
    return *this;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
    if ((is_negative && other.is_negative) || (!is_negative && !other.is_negative)) {
        same_signs_sum(other);
        return *this;
    }
    else {
        different_signs_sum(other);
        return *this;
    }
}

BigInteger& BigInteger::operator+=(int other) {
    BigInteger copy(other);
    *this += copy;
    return *this;
}

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy += rhs;
    return copy;
}

BigInteger operator+(const BigInteger& lhs, int rhs) {
    BigInteger copy(rhs);
    return lhs + copy;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy -= rhs;
    return copy;
}

BigInteger operator-(const BigInteger& lhs, int rhs) {
    BigInteger copy(rhs);
    return lhs - copy;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    ++(*this);
    return copy;
}

BigInteger& BigInteger::operator++() {
    *this += 1;
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
    if ((is_negative && !other.is_negative) || (!is_negative && other.is_negative)) {
        same_signs_sum(other);
        return *this;        
    }
    else {
        different_signs_sum(other);
        return *this;
    }
}

BigInteger& BigInteger::operator-=(int other) {
    BigInteger copy(other);
    *this -= copy;
    return *this;
}

BigInteger BigInteger::operator--(int) {
    *this -= 1;
    return *this;
}

BigInteger& BigInteger::operator--(){
    *this -= 1;
    return *this;
}

BigInteger BigInteger::operator-() {
    if (number.back() == 0) {
        return *this;
    }
    BigInteger copy = *this;
    copy.is_negative = !copy.is_negative;
    return copy;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
    std::vector<int> rezult;
    rezult.resize(size + other.size, 0);
    long long multiply = 0;
    for (size_t i = 0; i < other.size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            multiply += long(number[j]) * long(other.number[i]);
            rezult[i + j] += int(multiply % BASE);
            multiply /= BASE;
            multiply += rezult[i + j] / BASE;
            rezult[i + j] %= BASE;
        }
        if (multiply) {
            rezult[i + size] = int(multiply);
            multiply = 0;
        }
    }
    if (other.is_negative) {
        is_negative = !is_negative;
    }
    size = rezult.size();
    number = rezult;
    check_zeros();
    delete_minus_before_zero();
    return *this;
}

BigInteger& BigInteger::operator*=(int other) {
    BigInteger copy(other);
    *this *= copy;
    return *this;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy *= rhs;
    return copy;
}

BigInteger operator*(const BigInteger& lhs, int rhs) {
    BigInteger copy = lhs;
    copy *= rhs;
    return copy;
}

void BigInteger::substraction(const BigInteger& other, size_t left, size_t right) {
    for (size_t i = left + 1; i <= right + 1; ++i) {
        number[i - 1] = number[i - 1] - other.number[i - left - 1];
        if (number[i - 1] < 0) {
            number[i - 1] += BASE;
            --number[i];
        }
    }
}

bool BigInteger::cmp(const BigInteger& other, size_t left, size_t right) {
    for (size_t i = right + 1; i >= left + 1; --i) {
        if (number[i - 1] != 0) {
            right = i - 1;
            break;
        }
    }
    if ((right-left+1) != other.size) {
        return (right-left+1) > other.size;
    }
    for (size_t i = right + 1; i >= left + 1; --i) {
        if (number[i - 1] != other.number[other.size - 2 - (right - i)]) {
            return number[i - 1] > other.number[other.size - 2 - (right - i)];
        }
    }
    return true;
}

long long BigInteger::bin_search(const BigInteger& other, size_t i, size_t index) {
    int left = 0, right = 1e9 - 1;
    long long multiplier;
    bool bool1, bool2;
    while (right >= left) {
        multiplier = (right + left) / 2;
        bool1 = cmp(other * multiplier, i-1, index-1);
        bool2 = cmp(other * (multiplier+1), i-1, index-1);
        if (bool1 && !bool2) {
            break;
        }
        if (!bool1) {
            right = multiplier - 1;
        }
        if (bool2) {
            left = multiplier + 1;
        }
    }
    return multiplier;
}


BigInteger& BigInteger::operator/=(const BigInteger& other) {
    std::vector<int> rezult; 
    size_t i = size, index = size;
    long long multiplier;
    while (i >= 1 && index >= other.size) {
        if (!cmp(other, i-1, index-1)) {
            rezult.push_back(0);
            --i;
        }
        else {
            multiplier = bin_search(other, i, index);
            substraction(other * multiplier, i-1, index-1);
            rezult.push_back(multiplier);
            while (number[index-1] == 0 && index > 1) --index;
            while (i > index + 1) {
                rezult.push_back(0);
                --i;
            }
            --i;
        }
    }
    if (rezult.size() == 0) {
        rezult.push_back(0);
    }
    number = rezult;
    size = rezult.size();
    if (other.is_negative) {
        is_negative = !is_negative;
    }
    for (size_t i = 0; i < size/2; ++i) {
        std::swap(number[i], number[size - i - 1]);
    }
    check_zeros();
    delete_minus_before_zero();
    return *this;
}

BigInteger& BigInteger::operator/=(int other) {
    BigInteger copy(other);
    *this /= copy;
    return *this;
}

BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy /= rhs;
    return copy;
}

BigInteger operator/(const BigInteger& lhs, int rhs) {
    BigInteger copy = lhs;
    copy /= rhs;
    return copy;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
    *this = *this - ((*this / other) * other);
    delete_minus_before_zero();
    return *this;
}

BigInteger& BigInteger::operator%=(int other) {
    *this = *this - ((*this / other) * other);
    delete_minus_before_zero();
    return *this;
}

BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs) {
    BigInteger copy = lhs;
    copy %= rhs;
    return copy;
}

BigInteger operator%(const BigInteger& lhs, int rhs) {
    BigInteger copy = lhs;
    copy %= rhs;
    return copy;
}

std::istream& operator>>(std::istream& in, BigInteger& big_num) {
    big_num.number.clear();
    big_num.size = 0;
    std::string str;
    in >> str;
    size_t s_len = str.length();
    if (s_len == 0) {
        return in;
    }
    else {
        if (str[0] == '-') {
            str = str.substr(1);
            --s_len;
            big_num.is_negative = true;
        }
        else {
            big_num.is_negative = false;
        }
        for (size_t i = 9; i <= s_len; i += 9) {
            big_num.number.push_back(stoi(str.substr(s_len - i, 9)));
            ++big_num.size;
        }
        if (s_len % 9) {
            big_num.number.push_back(stoi(str.substr(0, s_len % 9)));
            ++big_num.size;
        }
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& big_num) {
    std::string str = big_num.toString();
    out << str;
    return out;
}


class Rational {

private:
    BigInteger numerator;
    BigInteger denominator;
    bool is_negative;
    void reduction();

public:
    Rational();
    Rational(const BigInteger& input);
    Rational(int input);
    explicit operator double() const;
    std::string asDecimal(size_t precision) const;
    std::string toString();
    BigInteger& numerator_value();
    BigInteger& denominator_value();
    bool number_negative();
    Rational& operator=(const Rational& other);
    Rational& operator=(int other);
    Rational& operator+=(const Rational& other);
    Rational& operator+=(int other);
    Rational& operator-=(const Rational& other);
    Rational& operator-=(int other);
    Rational& operator*=(const Rational& other);
    Rational& operator*=(int other);
    Rational& operator/=(const Rational& other);
    Rational& operator/=(int other);
    Rational operator-() const;
};

Rational::Rational() = default;
    
Rational::Rational(const BigInteger& input) : numerator(input), denominator(1),
     is_negative(numerator.number_negative()) {}

Rational::Rational(int input) : numerator(input), denominator(1),
     is_negative(input < 0) {}

Rational::operator double() const{
    return stod(asDecimal(16));
    // Ğ² Ğ¸Ğ½Ñ ĞµÑ Ğ½ĞµÑ Ğµ Ğ½Ğ°Ğ¿Ğ¸Ñ Ğ°Ğ½Ğ¾, Ñ Ñ Ğ¾ Ñ Ğ¾Ñ Ğ½Ğ¾Ñ Ñ Ñ  Ñ  double Ğ¾Ğ±Ñ Ñ Ğ½Ğ¾ 16 Ğ·Ğ½Ğ°ĞºĞ¾Ğ²
}

std::string Rational::asDecimal(size_t precision) const{
    std::string ans = "";
    if (is_negative) ans = '-';
    ans += (numerator / denominator).toString();
    BigInteger remainder = numerator % denominator;
    if (precision) {
        std::string decimal;
        decimal += '.';
        for (size_t i = 0; i < precision; i += 9) {
            remainder *= 1000000000;
            std::string str = (remainder / denominator).toString();
            for (size_t i = 0; i < 9 - str.size(); ++i) {
                decimal += '0';
            }
            decimal += str;
            remainder %= denominator;
        }
        ans += decimal.substr(0, precision + 1);
    }
    return ans;
}

std::string Rational::toString() {
    std::string str_n = numerator.toString();
    std::string str_d = denominator.toString();
    std::string answer = "";
    if (denominator.number_negative()) {
        str_d = str_d.substr(1);
    }
    if (numerator.number_negative()) {
        str_n = str_n.substr(1);
    }
    if (is_negative && str_n != "0") {
        answer += '-';
    }
    answer += str_n;
    if (str_d != "1") {
        answer += ('/' + str_d);
    }
    return answer;
}

BigInteger& Rational::numerator_value() {
    return numerator;
}

BigInteger& Rational::denominator_value() {
    return denominator;
}

bool Rational::number_negative() {
    return is_negative;
}

void Rational::reduction() {
    BigInteger a = numerator;
    BigInteger b = denominator;
    if (b.toString() == "1") return;
    bool an = a.number_negative();
    bool bn = b.number_negative();
    if (bn) b = -b;
    if (an) a = -a;
    while (b) {
        a = a%b;
        std::swap(a, b);
    }
    if (an) a = -a;
    numerator /= a;
    denominator /= a;
}

bool operator==(Rational& lhs, Rational& rhs) {
    return lhs.numerator_value() == rhs.numerator_value() 
        && lhs.denominator_value() == rhs.denominator_value();
}

bool operator!=(Rational& lhs, Rational& rhs) {
    return lhs.toString() != rhs.toString();
}

bool operator!=(Rational lhs, int rhs) {
    Rational copy_r(rhs);
    return lhs != copy_r;
}

bool operator>(Rational& lhs, Rational& rhs) {
    int cnt = 0;
    if (lhs.numerator_value().number_negative()) ++cnt; 
    if (lhs.denominator_value().number_negative()) ++cnt;
    if (rhs.numerator_value().number_negative()) ++cnt;
    if (rhs.denominator_value().number_negative()) ++cnt;
    if (cnt%2) {
        return !(lhs.numerator_value() * rhs.denominator_value() -
            rhs.numerator_value() * lhs.denominator_value()).number_negative();
    }
    return (lhs.numerator_value() * rhs.denominator_value() - 
        rhs.numerator_value() * lhs.denominator_value()).number_negative();
}

bool operator<(Rational& lhs, Rational& rhs) {
    return rhs > lhs;
}

bool operator>=(Rational lhs, int rhs) {
    Rational copy_r(rhs);
    return (lhs > copy_r) || (lhs == copy_r);
}

Rational& Rational::operator=(const Rational& other) {
    numerator = other.numerator;
    denominator = other.denominator;
    is_negative = other.is_negative;
    reduction();
    return *this;
}

Rational& Rational::operator=(int other) {
    numerator = BigInteger(other);
    denominator = BigInteger(1);
    is_negative = other < 0;
    return *this;
}

Rational& Rational::operator+=(const Rational& other) {
    numerator = numerator*other.denominator + other.numerator*denominator;
    denominator *= other.denominator;
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational& Rational::operator+=(int other) {
    numerator += other;
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational& Rational::operator-=(const Rational& other) {
    numerator = numerator*other.denominator - other.numerator*denominator;
    denominator *= other.denominator;
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational& Rational::operator-=(int other) {
    numerator -= other;
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational& Rational::operator*=(const Rational& other) {
    numerator *= other.numerator;
    denominator *= other.denominator;
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational& Rational::operator*=(int other) {
    numerator *= other;
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational& Rational::operator/=(const Rational& other) {
    is_negative = (is_negative != other.is_negative);
    numerator *= other.denominator;
    denominator *= other.numerator;
    reduction();
    return *this;
}

Rational& Rational::operator/=(int other) {
    denominator = BigInteger(other);
    reduction();
    is_negative = numerator.number_negative() != denominator.number_negative();
    return *this;
}

Rational Rational::operator-() const{
    Rational copy = *this;
    copy.is_negative = !copy.is_negative;
    return copy;
}

Rational operator+(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy += rhs;
    return copy;   
}

Rational operator+(const Rational& lhs, int rhs) {
    Rational copy = lhs;
    copy += rhs;
    return copy;
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy -= rhs;
    return copy;
}

Rational operator-(const Rational& lhs, int rhs) {
    Rational copy = lhs;
    copy -= rhs;
    return copy;
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy *= rhs;
    return copy;
}

Rational operator*(const Rational& lhs, int rhs) {
    Rational copy = lhs;
    copy *= rhs;
    return copy;
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
    Rational copy = lhs;
    copy /= rhs;
    return copy;
}

Rational operator/(const Rational& lhs, int rhs) {
    Rational copy = lhs;
    copy /= rhs;
    return copy;
}

