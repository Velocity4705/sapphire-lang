#include "algorithms.h"
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>

namespace sapphire {
namespace stdlib {
namespace Algorithms {

// Graph Algorithms

std::vector<int> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& graph, int start) {
    int n = graph.size();
    std::vector<int> dist(n, std::numeric_limits<int>::max());
    dist[start] = 0;
    
    using pii = std::pair<int, int>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (auto [v, w] : graph[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    
    return dist;
}

std::vector<int> bfs(const std::vector<std::vector<int>>& graph, int start) {
    int n = graph.size();
    std::vector<int> visited(n, 0);
    std::vector<int> order;
    std::queue<int> q;
    
    q.push(start);
    visited[start] = 1;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        
        for (int v : graph[u]) {
            if (!visited[v]) {
                visited[v] = 1;
                q.push(v);
            }
        }
    }
    
    return order;
}

std::vector<int> dfs(const std::vector<std::vector<int>>& graph, int start) {
    int n = graph.size();
    std::vector<int> visited(n, 0);
    std::vector<int> order;
    std::stack<int> s;
    
    s.push(start);
    
    while (!s.empty()) {
        int u = s.top();
        s.pop();
        
        if (visited[u]) continue;
        visited[u] = 1;
        order.push_back(u);
        
        for (int v : graph[u]) {
            if (!visited[v]) {
                s.push(v);
            }
        }
    }
    
    return order;
}

// Dynamic Programming

int64_t fibonacci(int n) {
    if (n <= 1) return n;
    
    int64_t a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int64_t c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int lcs_length(const std::string& s1, const std::string& s2) {
    int m = s1.length(), n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    
    return dp[m][n];
}

int knapsack(const std::vector<int>& weights, const std::vector<int>& values, int capacity) {
    int n = weights.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));
    
    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            if (weights[i-1] <= w) {
                dp[i][w] = std::max(dp[i-1][w], dp[i-1][w - weights[i-1]] + values[i-1]);
            } else {
                dp[i][w] = dp[i-1][w];
            }
        }
    }
    
    return dp[n][capacity];
}

// String Algorithms

int kmp_search(const std::string& text, const std::string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;
    
    std::vector<int> lps(m, 0);
    int len = 0, i = 1;
    
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else if (len != 0) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }
    
    i = 0;
    int j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) return i - j;
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    
    return -1;
}

std::vector<int> rabin_karp(const std::string& text, const std::string& pattern) {
    std::vector<int> matches;
    int n = text.length(), m = pattern.length();
    if (m > n) return matches;
    
    const int d = 256;
    const int q = 101;
    int h = 1;
    
    for (int i = 0; i < m - 1; i++) h = (h * d) % q;
    
    int p = 0, t = 0;
    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }
    
    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) matches.push_back(i);
        }
        
        if (i < n - m) {
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) t += q;
        }
    }
    
    return matches;
}

int edit_distance(const std::string& s1, const std::string& s2) {
    int m = s1.length(), n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }
    
    return dp[m][n];
}

} // namespace Algorithms
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    void sapphire_quick_sort_int(int* arr, int size) {
        std::vector<int> vec(arr, arr + size);
        sapphire::stdlib::Algorithms::quick_sort(vec, 0, size - 1);
        std::copy(vec.begin(), vec.end(), arr);
    }
    
    void sapphire_merge_sort_int(int* arr, int size) {
        std::vector<int> vec(arr, arr + size);
        sapphire::stdlib::Algorithms::merge_sort(vec, 0, size - 1);
        std::copy(vec.begin(), vec.end(), arr);
    }
    
    int sapphire_binary_search_int(const int* arr, int size, int target) {
        std::vector<int> vec(arr, arr + size);
        return sapphire::stdlib::Algorithms::binary_search(vec, target);
    }
    
    int64_t sapphire_fibonacci(int n) {
        return sapphire::stdlib::Algorithms::fibonacci(n);
    }
    
    int sapphire_edit_distance(const char* s1, const char* s2) {
        return sapphire::stdlib::Algorithms::edit_distance(s1, s2);
    }
}
