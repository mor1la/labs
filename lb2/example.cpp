#include <iostream>
#include <vector>

using namespace std;

// Функция для построения префикс-функции (функции lps)
vector<int> computeLPS(const string &pattern) {
    int m = pattern.size();
    vector<int> lps(m, 0);
    int j = 0;
    
    for (int i = 1; i < m; ++i) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = lps[j - 1];
        }
        if (pattern[i] == pattern[j]) {
            lps[i] = j + 1;
            ++j;
        }
    }
    return lps;
}

// Функция КМП для поиска всех вхождений шаблона в текст
vector<int> KMP(const string &text, const string &pattern) {
    vector<int> result;
    vector<int> lps = computeLPS(pattern);
    int n = text.size(), m = pattern.size();
    int j = 0;
    
    for (int i = 0; i < n; ++i) {
        while (j > 0 && text[i] != pattern[j]) {
            j = lps[j - 1];
        }
        if (text[i] == pattern[j]) {
            ++j;
        }
        if (j == m) {
            result.push_back(i - m + 1);
            j = lps[j - 1];
        }
    }
    return result;
}

int main() {
    string P, T;
    cin >> P >> T;
    
    vector<int> occurrences = KMP(T, P);
    
    if (occurrences.empty()) {
        cout << "-1";
    } else {
        for (size_t i = 0; i < occurrences.size(); ++i) {
            if (i > 0) cout << ",";
            cout << occurrences[i];
        }
    }
    cout << endl;
    
    return 0;
}
