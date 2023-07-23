//
//  Color.h
//  ClusterBotzGPU
//
//  Created by Scott Schafer on 5/22/17.
//  Copyright © 2017 Scott Schafer. All rights reserved.
//

#ifndef Color_h
#define Color_h

class Color {
public:
  float r, g, b, a;
  
  Color() {}
  Color(float r, float g, float b, float a = 1): r(r), g(b), b(b), a(a) {}
  
};

const Color TRANSPARENT = Color(0,0,0,0);

#endif /* Color_h */
