#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

std::mutex mtx ,mtx2, mtx3;
std::condition_variable cv , cv2, cv3;

int k;
int A;
long long result = 1;

bool ready = false;

void Thread2Function(vector<double>& a) {
    unique_lock<std::mutex> lock(mtx3);
    cv3.wait(lock, [&] { return ready; });

    vector<double> smaller;
    vector<double> greater;

    for (double num : a) {
        if (num < A) {
            smaller.push_back(num);
        }
        else {
            greater.push_back(num);
        }
    }

    a.clear();
    a.insert(a.end(), greater.begin(), greater.end());
    a.insert(a.end(), smaller.begin(), smaller.end());

    cout << " a : ";
    for (int i = 0; i < a.size(); i++) {
        cout << a[i] << " ";
    }

    cout << endl;

    ready = false;
    cv.notify_one();
}

void Thread3Function(vector<double>& a) {

    mtx.lock();

    {
        unique_lock<std::mutex> lock(mtx2);
        cv2.wait(lock, [&] { return !ready; });
    }

    /*unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&] { return ready; });*/

    for (size_t i = k - 1; i < a.size(); i++) {
        result *= a[i];
    }

    mtx.unlock();

   /* ready = false;
    cv.notify_one();*/
}

int main() {
    k = 0;
    A = 0;
    std::vector<double> a;

    setlocale(LC_ALL, "RUSSIAN");

    cout << " Введите размер массива: ";
    int n;
    cin >> n;

    cout << " Введите элементы : ";
    for (int i = 0; i < n; ++i) {
        double element;
        cin >> element;
        a.push_back(element);
    }

    cout << " Размер массива : " << n;

    cout << endl;

    cout << " Массив : ";
    for (int i = 0; i < n; ++i) {
        cout << a[i] << " ";
    }

    cout << endl;

    std::thread thread2(Thread2Function, std::ref(a));

    cout << " Введите значение A : ";
    cin >> A;
    cout << " Введите значение k : ";
    cin >> k;
    cout << endl;

    {
        lock_guard<std::mutex> lock(mtx3);
        ready = true;
        cv3.notify_all();
    }

    {
        unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return !ready; });
    }

    std::thread thread3(Thread3Function, std::ref(a));

    cout << " Массив итог : ";
    for (int i = 0; i < n; ++i) {
        cout << a[i] << " ";
    }

    cout << endl;

    {
        unique_lock<std::mutex> lock(mtx);
        ready = true;
        cv2.notify_one();
    }

    mtx.lock();

    cout << "Итоговое произведение: " << result << std::endl;

    mtx.unlock();


    thread2.join();
    thread3.join();

    return 0;
}
