#pragma once
#include <limits>
#include <set>
#include <SDL2/SDL.h>

namespace rs
{
struct Interval
{
    Uint32 left;
    Uint32 right;  // not inclusive
};

bool operator<(const Interval& lhs, const Interval& rhs)
{
    return lhs.left < rhs.left && lhs.right < rhs.right;
}

class IdPool
{
public:
    IdPool()
    {
        _free.insert({1, std::numeric_limits<Uint32>::max()});
    }

    Uint32 getId()
    {
        Interval first = *(_free.begin());
        Uint32   id    = first.left;
        _free.erase(_free.begin());
        if (first.left + 1 < first.right)
            _free.insert({first.left + 1, first.right});
        return id < std::numeric_limits<Uint32>::max() ? id : 0;
    }
    void freeId(Uint32 id)
    {
        if (id == 0 || id == std::numeric_limits<Uint32>::max())
            return;

        Interval interval{id, id + 1};
        auto     it = _free.find(interval);
        if (it != _free.end())
            return;

        it = _free.upper_bound(interval);
        if (it == _free.end())
            return;
        else
        {
            Interval freeInterval = *(it);

            assert(freeInterval.left >= interval.right);

            if (interval.right < freeInterval.left)
            {
                _free.insert(interval);
                return;
            }

            if (interval.right == freeInterval.left)
            {
                _free.erase(it);
                _free.insert({interval.left, freeInterval.right});
            }
        }
    }

private:
    std::set<Interval> _free;
};
}
