#pragma once

// containers
#include <vector>
#include <list>
#include <map>


#include <fstream> // for saving files and making plots
#include <chrono> // for timer
#include <cstdlib> // rand

using namespace std::chrono;

// Structure with X, Y values for plots
struct result {
    std::vector<double> time{0};
    std::vector<int> size{0};

    //creates *.dat file, that could bu used
    // for making plot in gnuplot
    void write(std::string filename) {
        std::ofstream out("./dat/" + filename + ".dat");
        for (int i = 0; i < time.size(); ++i) {
            out << size[i] << ' ' <<  time[i] << '\n';
        }
        out.close();
    }
};


// Class that implements tests on allocator with different 
// containers type
template <typename Allocator>
class Benchmark {
public:
    result test_vector(Allocator& allocator, size_t count_of_tests = 100, size_t size_step = 1000) {
        return test<std::vector<char, Allocator>>(allocator, count_of_tests, size_step, [](std::vector<char, Allocator>& c) {
            c.push_back('a');
        });
    }

    result test_list(Allocator& allocator, size_t count_of_tests = 100, size_t size_step = 1000) {
        return test<std::list<char, Allocator>>(allocator, count_of_tests, size_step, [](std::list<char, Allocator>& c) {
            c.push_back('a');
        });
    }

    result test_map(Allocator& allocator, size_t count_of_tests = 100, size_t size_step = 1000) {
        return test<std::map<int, char, Allocator>>(allocator, count_of_tests, size_step, [](std::map<int, char, Allocator>& c) {
            c[std::rand() % (INT32_MAX * 2) - INT32_MAX] = 'a';
        });
    }

private:
    template<typename Container, typename Function>
    result test (Allocator& allocator, size_t count_of_tests, size_t size_step, Function add_to_container) {
        auto start = high_resolution_clock::now();
        result res;
        Container c(allocator);
        size_t current_size = 1;
        for (size_t i = 0; i < count_of_tests; ++i) {
            for (size_t j = 0; j < size_step; ++j) {
               add_to_container(c);
            }
            auto diff = duration_cast<nanoseconds>(high_resolution_clock::now() - start);
            res.time.push_back((double)(diff).count() / 1'000'000);
            res.size.push_back(c.size());
        }
        return res;
    }
};

// Creates pipe on gnuplot -persist and describes
// type of the plot
void make_graph(std::vector<std::string> files, std::string filename) {
    FILE *gp = popen("/usr/local/bin/gnuplot -persist","w");
    if (gp == NULL)
    {
        printf("Error opening pipe to GNU plot.\n");
        exit(0);
    }

    std::string embeded = "plot ";
    for (auto& el : files) {
        embeded += "\"./dat/" + el + "\" using 1:2 with lines title \"" + el.substr(0, el.find_last_of('.')) + "\", ";
    }
    embeded.erase(embeded.end());
    fprintf(gp, "set terminal png size 1280, 720\n");
    fprintf(gp, "set output \"img/%s.png\"\n", filename.c_str());
    fprintf(gp, "set grid xtics ytics\n");
    fprintf(gp, "set ylabel \"Time, ms\"\n");
    fprintf(gp, "set xlabel \"Written elements\"\n");
    fprintf(gp, "%s", embeded.c_str());
    pclose(gp);
}