#pragma once
#include "distance_matrix.h"
#include <random>
#include <tuple>
#include <chrono>
#include <numeric>

namespace tsp_heur
{
    class hc_solver
    {
        struct undo
        {
            size_t mut_pos;
            path::iterator new_pos;
        };

    public:
        hc_solver(const distance_matrix &distanceMatrix, uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count())
            : dm(distanceMatrix), re(seed)
        {
        }

        auto transform(const path &input)
        {
            path tmp;
            best_insert(input, tmp);
            best_reinsert(input, tmp);
            return tmp;
        }

        std::tuple<path, size_t> search(const distance_matrix &dm, size_t maxIt, int64_t stopVal = -1)
        {
            auto input = random_path();
            auto bestLength = std::numeric_limits<int64_t>::max();
            size_t it = 0;
            do
            {
                it++;
                const auto u = do_mutation(input);
                path tmp = transform(input);
                const auto length = dm.evaluate_path(tmp);

                if (length > bestLength)
                    undo_mutation(input, u);
                if (length < bestLength)
                {
                    bestLength = length;
                    if (stopVal > 0 && length == stopVal)
                        break;
                }
            } while (it < maxIt);
            return {transform(input), it};
        }

    private:
        inline auto dist(uint32_t from, uint32_t to) const noexcept
        {
            return dm.dm()[from][to];
        }

        const auto find_best_position(const path &p, uint32_t index) const
        {
            auto bestPos = p.end();
            auto bestVal = dist(*std::prev(p.end()), index) + dist(index, *p.begin()) - dist(*std::prev(p.end()), *p.begin());
            for (auto prev = p.begin(), next = std::next(prev); next != p.end(); prev = next++)
            {
                const auto val = dist(*prev, index) + dist(index, *next) - dist(*prev, *next);
                if (val < bestVal)
                {
                    bestVal = val;
                    bestPos = next;
                }
            }
            return bestPos;
        }

        void best_insert(const path &input, path &result) const
        {
            auto it = input.begin();
            result.push_back(*it++);
            result.push_back(*it++);
            while (it != input.end())
            {
                const auto x = *it++;
                result.insert(find_best_position(result, x), x);
            }
        }

        void best_reinsert(const path &input, path &result) const
        {
            for (auto x : input)
            {
                result.remove(x);
                result.insert(find_best_position(result, x), x);
            }
        }

        undo do_mutation(path &p)
        {
            const auto mutIdx = re() % p.size();
            auto mutPos = std::next(p.begin(), mutIdx);
            auto x = *mutPos;
            p.erase(mutPos);
            return {mutIdx, p.insert(std::next(p.begin(), re() % (p.size() - 1)), x)};
        }

        void undo_mutation(path &p, const undo &u) const
        {
            const auto x = *u.new_pos;
            p.erase(u.new_pos);
            p.insert(std::next(p.begin(), u.mut_pos), x);
        }

        path random_path()
        {
            std::vector<uint32_t> tmp(dm.size());
            std::iota(tmp.begin(), tmp.end(), 0);
            std::shuffle(tmp.begin(), tmp.end(), re);

            return {tmp.begin(), tmp.end()};
        }

    private:
        const distance_matrix &dm;
        std::default_random_engine re;
    };

}
