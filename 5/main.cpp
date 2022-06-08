#include <iostream>
#include <vector>
#include "allocator/allocator.h"
#include "benchmark/benchmark.h"

int main() {
    // init
    size_t* chunks_v = new size_t[]{128, 1024, 4096, 32'768, 131'072};
    size_t* size_v = new size_t[]{100, 100 ,100, 50, 10};
    Allocator<char> av_custom(chunks_v, size_v, 5);

    size_t* chunks_l = new size_t[]{24};
    size_t* size_l = new size_t[]{100000};
    Allocator<char> al_custom(chunks_l, size_l, 1);

    size_t* chunks_m = new size_t[]{1};
    size_t* size_m = new size_t[]{10'000'000};
    Allocator<char> am_custom(chunks_l, size_l, 1);

    std::allocator<char> a_standart;

    Benchmark<Allocator<char>> b_custom;
    Benchmark<std::allocator<char>> b_standart;

    // vector test
    b_custom.test_vector(av_custom).write("custom_v");
    b_standart.test_vector(a_standart).write("standart_v");

    // list test
    b_custom.test_list(al_custom).write("custom_l");
    b_standart.test_list(a_standart).write("standart_l");

    // map test
    b_custom.test_list(am_custom).write("custom_m");
    b_standart.test_list(a_standart).write("standart_m");

    // plot
    make_graph(std::vector<std::string>{"custom_v.dat", "standart_v.dat"}, "vector_test");
    make_graph(std::vector<std::string>{"custom_l.dat", "standart_l.dat"}, "list_test");
    make_graph(std::vector<std::string>{"custom_m.dat", "standart_m.dat"}, "map_test");
    return 0;
}