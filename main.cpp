#include "hc_solver.h"
#include <iostream>
#include <string>
using namespace std::chrono;

struct instance
{
    std::string name;
    int64_t length;
};

// Some examples from:
// https://www.math.uwaterloo.ca/tsp/world/countries.html
// https://www.math.uwaterloo.ca/tsp/vlsi/index.html
// http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/index.html
std::vector<instance> instances = {
    {"wi29", 27603},
    {"dj38", 6656},
    {"berlin52", 7542},
    {"rat99", 1211},
    {"kroD100", 21294},
    {"rd100", 7910},
    {"eil101", 629},
    {"lin105", 14379},
    {"xqf131", 564},
    {"pr136", 96772},
    {"qa194", 9352},
    {"xqg237", 1019},
    {"bcl380", 1621},
    //{"uy734", 79114}
};

int main()
{
    for (const auto &inst : instances)
    {
        tsp_heur::distance_matrix dm{};
        if (!dm.load_tsv("./instances/" + inst.name + ".tsp"))
        {
            std::cerr << "Error loading tsp file " << inst.name << std::endl;
            continue;
        }
        tsp_heur::hc_solver solver(dm, 42);
        const auto start = high_resolution_clock::now();
        const auto [tour, it] = solver.search(dm, 100000, inst.length);
        const auto length = dm.evaluate_path(tour);
        const auto stop = high_resolution_clock::now();

        const auto duration = 0.000001 * duration_cast<microseconds>(stop - start).count();
        const auto err = 100.0 * (((double)length / (double)inst.length) - 1.0);
        std::cout << inst.name << " it: " << it << " val: " << length << " err: " << err << "% time: " << duration << std::endl;
    }
}
