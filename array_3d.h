#ifndef ARRAY_3D_H
#define ARRAY_3D_H

#include <boost/multi_array.hpp>

typedef boost::multi_array<double, 3> array_3d;
typedef array_3d::index index_3d;

typedef boost::multi_array<double, 2> array_2d;
typedef array_2d::index index_2d;

#endif // ARRAY_3D_H