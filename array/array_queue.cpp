#include <iostream>
#include <stdexcept>

template<typename T>
class ArrayQueue {
    private:
        T * data;
        int capacity;
        int front; // 次削除
        int rear; //  次追加
        int count;
    public:
        explicit ArrayQueue(int size) : capacity(size + 1), front(0), rear(0), count(0) {
            data = new T[capacity];
        }

        ~ArrayQueue() {
            delete[] data;
        }

        void enqueue(const T& item) {
            if (isFull()) {
                throw std::overflow_error("キューが満杯です");
            }
            data[rear] = item;
            rear = (rear + 1)%capacity; // ポインタのみ移動させて循環させる。これによりシフトが発生せずO(1)保証
            count++;

            std::cout << "追加: " << item << " (rear: " << rear << ", count: " << count << ")" << std::endl;
        }

        T dequeue() {
            if (isEmpty()) {
                throw std::underflow_error("キューが空です");
            }
            T item = data[front];
            front = (front + 1)%capacity;
            count--;

            std::cout << "削除: " << item << " (front: " << front << ", count: " << count << ")" << std::endl;
            return item;
        }

        T peek() const {
            if (isEmpty()) {
                throw std::underflow_error("キューが空です");
            }
            return data[front];
        }

          // 空かどうか
        bool isEmpty() const {
            return count == 0;
        }

        // 満杯かどうか
        bool isFull() const {
            return count == capacity - 1;
        }
};

int main() {
    try {
        ArrayQueue<int> queue(5);
        std::cout << "=== 循環配列FIFOキューのデモ ===" << std::endl;
        queue.enqueue(10);
        queue.enqueue(20);
        queue.enqueue(30);

        std::cout << "peek: " << queue.peek() << std::endl;
        queue.dequeue();
        queue.dequeue();

    } catch (const std::exception& e) {
        std::cout << "エラー: " << e.what() << std::endl;
    }

    return 0;
}


