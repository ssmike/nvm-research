#include <libpmemobj.h>

#include <unistd.h>

#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <algorithm>

#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/transaction.hpp>

constexpr uint64_t playground_size = 2000000;

struct RootObject {
    pmem::obj::persistent_ptr<RootObject> next = nullptr;
    char data[playground_size];
};

template<typename F>
std::chrono::steady_clock::duration measure_(F&& f, std::function<void()> s, size_t repeats = 1) {
    std::chrono::steady_clock::duration res = std::chrono::steady_clock::duration::zero();
    for (size_t i = 0 ; i < repeats; ++i) {
        s();
        res += measure(f, 1);
    }
    return res / repeats;
}


template<typename F>
std::chrono::steady_clock::duration measure(F&& f, size_t repeats = 1) {
    auto pt = std::chrono::steady_clock::now();
    for (size_t i = 0; i < repeats; ++i) {
        f();
    }
    return (std::chrono::steady_clock::now() - pt) / repeats;
}

template<typename T>
void print_time(T time) {
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(time).count() << "ns" << std::endl;
}

void print_statistics(std::vector<std::chrono::steady_clock::duration>& times, std::string header) {
    std::cout << "stats for " << header << std::endl;
    std::chrono::steady_clock::duration sum = std::chrono::steady_clock::duration::zero();
    std::sort(times.begin(), times.end());
    for (auto time : times) {
        sum += time;
    }
    std::cout << "avg " << std::chrono::duration_cast<std::chrono::nanoseconds>(sum).count() << "ns" << std::endl;
    std::cout << "min " << std::chrono::duration_cast<std::chrono::nanoseconds>(times[0]).count() << "ns" << std::endl;
    std::cout << "max " << std::chrono::duration_cast<std::chrono::nanoseconds>(times.back()).count() << "ns" << std::endl;
    ssize_t q50 = std::min<ssize_t>(times.size() * 0.5, ssize_t(times.size()) - 1);
    std::cout << "q50 " << std::chrono::duration_cast<std::chrono::nanoseconds>(times[q50]).count() << "ns" << std::endl;
    ssize_t q90 = std::min<ssize_t>(times.size() * 0.9, ssize_t(times.size()) - 1);
    std::cout << "q90 " << std::chrono::duration_cast<std::chrono::nanoseconds>(times[q90]).count() << "ns" << std::endl;
}

int main() {
    auto pool = pmem::obj::pool<RootObject>::create("storage/testpool", "layout", 100 * 1024 * 1024);
    auto ptr = pool.root();

    for (size_t i = 0; i < 10; ++i) {
        pmem::obj::transaction::run(
            pool,
            [&] {
                ptr->next = pmem::obj::make_persistent<RootObject>();
                ptr = ptr->next;
            });
    }
    ptr = pool.root();

    size_t xorsum = 0;
    auto read_all = [&] {
        for (auto p = ptr; p; p = p->next) {
            for (size_t i = 0; i < playground_size; ++i) {
                xorsum += p->data[i];
            }
        }
    };

    //RootObject obj;
    //print_time(measure([&, cnt=0, ptr=ptr.get()] () mutable {
    //        for (size_t i = 0; i < playground_size; ++i) {
    //            ptr->data[i] = (i + cnt) % 256;
    //        }
    //    },
    //    1000));
    print_time(measure([&, cnt=0, ptr=ptr.get()] () mutable {
            for (size_t i = 0; i < playground_size; ++i) {
                ptr->data[i] = (i + cnt) % 256; 
            }
            pool.flush(ptr, sizeof(RootObject));
        },
        1000));
    print_time(measure([&, cnt=0, ptr=ptr.get()] () mutable {
            for (size_t i = 0; i < playground_size; ++i) {
                ptr->data[i] = (i + cnt) % 256; 
            }
            pool.flush(ptr, sizeof(RootObject));
            pool.drain();
        },
        1000));
    for (size_t line = 0; line < 200; ++line) {
        std::cout << "line size = " << line << std::endl;
        print_time(measure([&, cnt=0, ptr=ptr.get()] () mutable {
                for (size_t i = 0; i < line; ++i) {
                    ptr->data[i] = (i + cnt) % 256; 
                }
                pool.flush(ptr, line);
                pool.drain();
            },
            1000));
    }

    print_time(measure_([&, cnt=0, ptr=ptr.get()] () mutable {
            xorsum += ptr->data[0];
        },
        read_all,
        1000));
    

    std::cerr << xorsum;
    unlink("storage/testpool");
}
