#ifndef AGARIO_TYPES_HPP
#define AGARIO_TYPES_HPP

//#include "THST/QuadTree.h"

namespace Agario {

  typedef int position;
  typedef float length;
  typedef unsigned int mass;
  typedef unsigned int score;

  typedef unsigned short pid;
  typedef unsigned long tick;

//  template <typename T> struct Box2 {
//    T min[2];
//    T max[2];
//
//    explicit operator spatial::BoundingBox<int, 2>() const {
//      return spatial::BoundingBox<int, 2>(min, max);
//    }
//  };

}


#endif //AGARIO_TYPES_HPP
