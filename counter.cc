#include "/home/jpeng/mywork/header.h"

class Counter {
private:
    uint32_t curr = 0;
    array<bool, 1024> num_arr;
    int pos = -1, end = 0;

    mutex mutex_get;
    mutex mutex_put;

public:
    // 弱一致性
    uint32_t Min() {
        auto end__ = end;
        while (pos != end__ && num_arr[pos] == false) ++pos;
        return (pos == end__ ? 0 : pos); 
    }

    uint32_t Get() {
        lock_guard<mutex> lck(mutex_get);

        auto data = curr;
        ++curr;
        num_arr[data] = true;
        end = data;
        return data;
    }

    void Put(uint32_t number) {
        lock_guard<mutex> lck(mutex_put);

        num_arr[number] = false;
    }
};

class Result {
private:
    mutex mutex_res;
    map<int, int> res_map;

public:
    void In(uint32_t num) {
        lock_guard<mutex> lck(mutex_res);
        res_map[num]++;
    }

    void Confirm() {
        for (auto elem : res_map) {
            if (elem.second > 1) {
                cout << "result wrong!" << endl;
                return ;
            }
        }

        cout << "result correct!" << endl;
    }
};

void RandomSleep() {
    static default_random_engine e;
    // 随机休眠0.001s-0.1s
    static uniform_int_distribution<int> u(1000, 100000);
    usleep(u(e));
}

Counter counter;
Result result;

unordered_map<int, int> hash_map;

void ThreadFunc(int num) {
    auto ret = counter.Get();
    result.In(ret);
    // cout << "Thread " + to_string(num) + " get : " + to_string(num) << endl; 
    RandomSleep();
    counter.Put(ret);
}

int main(int argc, char** argv) {
    vector<thread> thread_arr;
    for (int i = 0; i < 30; ++i) {
        RandomSleep();
        for (int j = 0; j < 30; ++j) {
            thread_arr.push_back(thread(ThreadFunc, i * 30 + j));
        }

        cout << "Main thread get min : " + to_string(counter.Min()) << endl;
    }


    for (int i = 0; i < thread_arr.size(); ++i)
        thread_arr[i].join();
    cout << "main thread over" << endl;

    result.Confirm();

    return 0;
}
