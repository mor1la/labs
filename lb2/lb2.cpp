#include <iostream>
#include <vector>

std::vector<int> computeLPS(const std::string &pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int i = 1, j = 0;
    
    while (i < m) {
        if (pattern[i] == pattern[j]) {
            j++;
            lps[i] = j;
            i++;
        } else {
            if (j > 0) {
                j = lps[j - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

std::vector<int> KMP(const std::string &text, const std::string &pattern) {
    std::vector<int> result;
    std::vector<int> lps = computeLPS(pattern);
    int n = text.size(), m = pattern.size();
    int i = 0, j = 0;

    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
            if (j == m) {
                result.push_back(i - m);
                j = lps[j - 1];
            }
        } else {
            if (j > 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return result;
}

int main() {
    std::string pattern, text;
    std::cin >> pattern >> text;
    std::vector<int> result = KMP(text, pattern);

    if (result.empty()) {
        std::cout << -1;
    } else {
        for (size_t i = 0; i < result.size(); i++) {
            if (i > 0) std::cout << ',';
            std::cout << result[i];
        }
    }
    std::cout << '\n';
}
