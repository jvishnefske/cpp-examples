#include <array>
#include <string_view>
#include <iostream>
#include <cstring>
#include <algorithm>

template<size_t N>
class SmallString {
private:
    std::array<char, N + 1> data_{};  // +1 for null terminator
    size_t size_ = 0;
    
public:
    constexpr SmallString() = default;
    
    explicit constexpr SmallString(const char* str) {
        const char* end = str;
        while (*end && size_ < N) {
            data_.at(size_) = *end;
            ++size_;
            ++end;
        }
        data_.at(size_) = '\0';
    }
    
    explicit constexpr SmallString(std::string_view sv) {
        size_t copy_size = std::min(sv.size(), N);
        for (size_t i = 0; i < copy_size; ++i) {
            data_.at(i) = sv.at(i);
        }
        size_ = copy_size;
        data_.at(size_) = '\0';
    }
    
    constexpr size_t size() const { return size_; }
    constexpr size_t capacity() const { return N; }
    constexpr bool empty() const { return size_ == 0; }
    constexpr bool full() const { return size_ == N; }
    
    constexpr const char* c_str() const { return data_.data(); }
    constexpr const char* data() const { return data_.data(); }
    
    constexpr char& operator[](size_t index) { return data_.at(index); }
    constexpr const char& operator[](size_t index) const { return data_.at(index); }
    
    constexpr auto begin() { return data_.begin(); }
    constexpr auto end() { return data_.begin() + size_; }
    constexpr auto begin() const { return data_.begin(); }
    constexpr auto end() const { return data_.begin() + size_; }
    
    // Convert to string_view
    explicit constexpr operator std::string_view() const {
        return std::string_view(data_.data(), size_);
    }
    
    // Append operations
    constexpr SmallString& append(char c) {
        if (size_ < N) {
            data_.at(size_) = c;
            ++size_;
            data_.at(size_) = '\0';
        }
        return *this;
    }
    
    constexpr SmallString& append(std::string_view sv) {
        size_t remaining = N - size_;
        size_t copy_size = std::min(sv.size(), remaining);
        
        for (size_t i = 0; i < copy_size; ++i) {
            data_.at(size_ + i) = sv.at(i);
        }
        size_ += copy_size;
        data_.at(size_) = '\0';
        return *this;
    }
    
    // Comparison operators
    constexpr bool operator==(const SmallString& other) const {
        if (size_ != other.size_) {
            return false;
        }
        for (size_t i = 0; i < size_; ++i) {
            if (data_.at(i) != other.data_.at(i)) {
                return false;
            }
        }
        return true;
    }
    
    constexpr bool operator==(std::string_view sv) const {
        if (size_ != sv.size()) {
            return false;
        }
        for (size_t i = 0; i < size_; ++i) {
            if (data_.at(i) != sv.at(i)) {
                return false;
            }
        }
        return true;
    }
    
    constexpr bool operator!=(const SmallString& other) const {
        return !(*this == other);
    }
    
    constexpr bool operator!=(std::string_view sv) const {
        return !(*this == sv);
    }
    
    // Clear the string
    constexpr void clear() {
        size_ = 0;
        data_[0] = '\0';
    }
    
    // Find character
    constexpr size_t find(char c) const {
        for (size_t i = 0; i < size_; ++i) {
            if (data_.at(i) == c) {
                return i;
            }
        }
        return size_t(-1); // npos equivalent
    }
};

// Helper function disabled due to C-array restrictions
// Use direct construction instead

// Stream output operator
template<size_t Size>
std::ostream& operator<<(std::ostream& os, const SmallString<Size>& str) {
    os << static_cast<std::string_view>(str);
    return os;
}

void test_small_string() {
    std::cout << "=== Testing SmallString ===\n";
    
    // Basic construction
    constexpr size_t STRING_SIZE = 20;
    SmallString<STRING_SIZE> str1("Hello");
    SmallString<STRING_SIZE> str2("World");
    
    std::cout << "str1: " << str1 << " (size: " << str1.size() << ")\n";
    std::cout << "str2: " << str2 << " (size: " << str2.size() << ")\n";
    
    // Append operations
    str1.append(' ').append(str2);
    std::cout << "After append: " << str1 << "\n";
    
    // Comparison
    SmallString<STRING_SIZE> str3("Hello World");
    std::cout << "str1 == str3: " << (str1 == str3) << "\n";
    std::cout << "str1 == \"Hello World\": " << (str1 == std::string_view("Hello World")) << "\n";
    
    // Find operation
    size_t space_pos = str1.find(' ');
    std::cout << "Position of space: " << space_pos << "\n";
    
    // Capacity checks
    std::cout << "Capacity: " << str1.capacity() << ", Full: " << str1.full() << "\n";
}

void test_constexpr_small_string() {
    std::cout << "\n=== Testing Constexpr SmallString ===\n";
    
    constexpr size_t STRING_SIZE = 20;
    
    // Compile-time string creation
    constexpr size_t COMPILE_SIZE = 10;
    constexpr auto compile_time_str = SmallString<COMPILE_SIZE>("Constexpr");
    std::cout << "Compile-time string: " << compile_time_str << "\n";
    std::cout << "Size: " << compile_time_str.size() << "\n";
    
    // Using direct construction
    constexpr size_t AUTO_SIZE = 8;
    constexpr SmallString<AUTO_SIZE> auto_sized("AutoSize");
    std::cout << "Auto-sized string: " << auto_sized << "\n";
    std::cout << "Capacity: " << auto_sized.capacity() << "\n";
    
    // Compile-time operations
    constexpr SmallString<STRING_SIZE> ct_str("Test");
    static_assert(ct_str.size() == 4);
    static_assert(!ct_str.empty());
    static_assert(ct_str[0] == 'T');
    std::cout << "Compile-time assertions passed!\n";
}

int main() {
    test_small_string();
    test_constexpr_small_string();
    return 0;
}