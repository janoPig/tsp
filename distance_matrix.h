#pragma once
#include <list>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace tsp_heur
{
    using path = std::list<uint32_t>;
    class distance_matrix
    {
        struct point
        {
            double x, y;
        };

        std::vector<int32_t> data{};
        std::vector<int32_t *> matrix{};

    public:
        distance_matrix() = default;
        ~distance_matrix() = default;

        inline auto size() const noexcept { return matrix.size(); }
        inline const auto &dm() const noexcept { return matrix; }

        void load_points(const std::vector<point> &points)
        {
            data.resize(points.size() * points.size());
            matrix.resize(points.size());
            for (int32_t i = 0; i < points.size(); i++)
            {
                matrix[i] = &data[points.size() * i];
            }

            // Eulidean distance rounded to the nearest whole number (the TSPLIB EUC_2D-norm).
            auto TSPLIB_EUC_2D = [](const point &a, const point &b) noexcept
            {
                return std::lround(std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
            };

            for (int i = 0; i < points.size(); i++)
            {
                for (int j = 0; j < points.size(); j++)
                {
                    matrix[i][j] = TSPLIB_EUC_2D(points[i], points[j]);
                }
            }
        }

        bool load_tsv(const std::string &path)
        {
            std::vector<point> p;
            std::ifstream is(path);

            if (!is)
                return false;

            auto remove_colon = [](std::string &str)
            { str.erase(std::remove(str.begin(), str.end(), ':'), str.end()); };

            std::string line, name;
            int32_t dim = -1;
            while (std::getline(is, line))
            {
                remove_colon(line);
                std::istringstream iss(line);
                if (iss >> name)
                {
                    if (name == "NODE_COORD_SECTION")
                        break;
                    if (name == "DIMENSION" && iss >> dim)
                        p.reserve(dim);
                }
            }

            for (auto i = 0; i < dim && std::getline(is, line); i++)
            {
                uint32_t j;
                double x, y;
                std::istringstream iss(line);
                if (!(iss >> j >> x >> y))
                    break;
                if (j != i + 1)
                    return false;
                p.push_back({x, y});
            }

            if (p.size() != dim || dim <= 0)
                return false;

            load_points(p);
            return true;
        }

        auto evaluate_path(const path &p) const
        {
            const auto first = p.begin();
            int64_t val = matrix[*std::prev(p.end())][*first];
            auto prev = first;
            for (auto it = std::next(first); it != p.end(); prev = it++)
                val += matrix[*prev][*it];

            return val;
        }
    };
}
