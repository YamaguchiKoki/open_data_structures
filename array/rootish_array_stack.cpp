#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>

template<typename T>
class RootishArrayStack {
private:
    std::vector<T*> blocks;  // ブロックへのポインタの配列
    int n;                   // 現在の要素数

public:
    RootishArrayStack() : n(0) {}

    ~RootishArrayStack() {
        clear();
    }

    T& get(int i) {
        if (i < 0 || i > n) {
            throw std::out_of_range("index out of range");
        }
        int b = i2b(i);
        int j = i - b * (b + 1) / 2; // 所属ブロック内でのインデックス
        return blocks[b][j];
    }

    void set(int i, const T& x) {
        get(i) = x;
    }

    void add(int i, const T& x) {
        if (i < 0 || i > n) throw std::out_of_range("add index out of range");

        int r = blocks.size();
        if (r * (r + 1) / 2 <= n) grow();

        n++;
        for (int j = n - 1; j > i; j--)
            set(j, get(j - 1));
        set(i, x);
    }

    T remove(int i) {
        if (i < 0 || i >= n) throw std::out_of_range("add index out of range");

        T x = get(i);
        for (int j = i; j < n - 1; j++)
            set(j, get(j + 1));
        n--;

        int r = blocks.size();
        if (r > 2 && (r - 2) * (r - 1) / 2 >= n) shrink();
        return x;
    }

    int size() const {
        return n;
    }

    bool empty() const {
        return n == 0;
    }

    void clear() {
        for (auto* block : blocks) {
            delete[] block;
        }
        blocks.clear();
        n = 0;
    }

    void printStructure() const {
        std::cout << "要素数: " << n << ", ブロック数: " << blocks.size() << std::endl;
        for (size_t i = 0; i < blocks.size(); i++) {
            std::cout << "ブロック" << i << " (容量" << (i + 1) << "): ";
            for (int j = 0; j < (int)(i + 1) && i * (i + 1) / 2 + j < n; j++) {
                std::cout << blocks[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    int i2b(int i) const {
        // ブロックb には 0+1+2+...+b = (b+1)(b+2)/2 個の要素まで入る
        // iは、(b+1)(b+2)/2 > i を満たす最小のbにあるので二次不等式を解いて丸め込めばいい -> O(1)
        return (int)std::floor((-1.0 + std::sqrt(1.0 + 8.0 * i)) / 2.0);
    }

    void grow() {
        // ガッツリstdのメソッド使ってます
        blocks.push_back(new T[blocks.size() + 1]);
    }

    void shrink() {
        int r = blocks.size();
        while (r > 2 && n <= (r - 2) * (r - 1) / 2) {
            delete[] blocks.back();
            blocks.pop_back();
            r--;
        }
    }
};

int main() {
    try {
        std::cout << "=== Rootish Array Stack テスト ===" << std::endl;

        RootishArrayStack<int> stack;

        // 1. 基本的な追加テスト
        std::cout << "\n1. 基本追加テスト" << std::endl;
        for (int i = 0; i < 10; i++) {
            stack.add(i, i * 10);
            std::cout << "add(" << i << ", " << (i * 10) << ") size=" << stack.size() << std::endl;
        }

        std::cout << "\n構造:" << std::endl;
        stack.printStructure();

        // 2. 中間挿入テスト
        std::cout << "\n2. 中間挿入テスト" << std::endl;
        stack.add(5, 999);
        std::cout << "add(5, 999) 後:" << std::endl;
        stack.printStructure();

        // 3. 要素アクセステスト
        std::cout << "\n3. 要素アクセステスト" << std::endl;
        for (int i = 0; i < stack.size(); i++) {
            std::cout << "get(" << i << ") = " << stack.get(i) << std::endl;
        }

        // 4. 要素変更テスト
        std::cout << "\n4. 要素変更テスト" << std::endl;
        stack.set(3, 777);
        std::cout << "set(3, 777) 後: get(3) = " << stack.get(3) << std::endl;

        // 5. 削除テスト
        std::cout << "\n5. 削除テスト" << std::endl;
        int removed = stack.remove(5);
        std::cout << "remove(5) = " << removed << std::endl;
        std::cout << "削除後の構造:" << std::endl;
        stack.printStructure();

        // 6. 複数削除でshrinkテスト
        std::cout << "\n6. shrinkテスト（複数削除）" << std::endl;
        for (int i = 0; i < 5; i++) {
            if (!stack.empty()) {
                int val = stack.remove(stack.size() - 1);  // 末尾削除
                std::cout << "末尾削除: " << val << ", size=" << stack.size() << std::endl;
            }
        }
        stack.printStructure();

        // 7. 大量データテスト
        std::cout << "\n7. 大量データテスト" << std::endl;
        RootishArrayStack<int> bigStack;

        // 100個追加
        for (int i = 0; i < 100; i++) {
            bigStack.add(i, i);
        }
        std::cout << "100個追加後: size=" << bigStack.size() << std::endl;

        // ランダム削除
        for (int i = 0; i < 50; i++) {
            if (!bigStack.empty()) {
                bigStack.remove(bigStack.size() / 2);  // 中央削除
            }
        }
        std::cout << "50個削除後: size=" << bigStack.size() << std::endl;

        // 8. エラーハンドリングテスト
        std::cout << "\n8. エラーハンドリングテスト" << std::endl;
        try {
            stack.get(-1);
        } catch (const std::exception& e) {
            std::cout << "get(-1) エラー: " << e.what() << std::endl;
        }

        try {
            stack.get(stack.size());
        } catch (const std::exception& e) {
            std::cout << "get(size) エラー: " << e.what() << std::endl;
        }

        try {
            stack.add(-1, 123);
        } catch (const std::exception& e) {
            std::cout << "add(-1, 123) エラー: " << e.what() << std::endl;
        }

        try {
            stack.remove(stack.size());
        } catch (const std::exception& e) {
            std::cout << "remove(size) エラー: " << e.what() << std::endl;
        }

        // 9. パフォーマンステスト
        std::cout << "\n9. パフォーマンステスト" << std::endl;
        RootishArrayStack<int> perfStack;

        std::cout << "1000個の要素を追加中..." << std::endl;
        for (int i = 0; i < 1000; i++) {
            perfStack.add(i, i);
        }
        std::cout << "完了: size=" << perfStack.size() << std::endl;

        std::cout << "500個の要素を削除中..." << std::endl;
        for (int i = 0; i < 500; i++) {
            perfStack.remove(0);  // 先頭削除（最も重い操作）
        }
        std::cout << "完了: size=" << perfStack.size() << std::endl;

        std::cout << "\n=== すべてのテスト完了 ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "エラー: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

