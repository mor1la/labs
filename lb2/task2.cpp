#include <iostream>
#include <vector>

std::vector<int> computeLPS(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int i = 1, j = 0;
    
    while (i < m) {
        if (pattern[i] == pattern[j]) {
            lps[i++] = ++j;
        } else if (j > 0) {
            j = lps[j - 1];
        } else {
            lps[i++] = 0;
        }
    }
    return lps;
}

int KMP(const std::string& text, const std::string& pattern) {
    std::vector<int> lps = computeLPS(pattern);
    int n = text.size(), m = pattern.size();
    int i = 0, j = 0;

    while (i < n) {
        if (text[i] == pattern[j]) {
            i++, j++;
            if (j == m) return i - m;
        } else if (j > 0) {
            j = lps[j - 1];
        } else {
            i++;
        }
    }
    return -1;
}

int check(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size() || str1.empty()) return -1;
    std::string str = str2 + str2;
    return KMP(str, str1);
}

int main() {
    std::string str1, str2;
    std::cin >> str1 >> str2;
    std::cout << check(str1, str2) << '\n';
}
