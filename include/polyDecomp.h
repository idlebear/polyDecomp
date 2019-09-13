//
// Created by bjgilhul on 13/09/19.
//

#ifndef POLYDECOMP_POLYDECOMP_H
#define POLYDECOMP_POLYDECOMP_H

#include <vector>

#include "Point.h"
#include "common.h"

namespace Polygon_Decomposition {

    typedef std::vector<Point> Polygon;

    bool isReflex(const Polygon &p, const int &i);
    void makeCCW(Polygon &poly);
    void decomposePoly(Polygon poly, std::vector<Polygon>& decomposition );

}


#endif //POLYDECOMP_POLYDECOMP_H
