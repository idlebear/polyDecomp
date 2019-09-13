#ifndef _COMMON_H
#define _COMMON_H

#include <vector>
#include <utility>
#include <cmath>
#include <ctime>
#include <cstdlib>

namespace Polygon_Decomposition {

   typedef double Scalar;

    bool eq(const Scalar &a, Scalar const &b);

    Scalar min(const Scalar &a, const Scalar &b);

    int wrap(const int &a, const int &b);

    // Scalar srand(const Scalar &min, const Scalar &max);

    template<class T>
    T &at(std::vector<T> v, int i) {
        return v[wrap(i, v.size())];
    };

}
#endif