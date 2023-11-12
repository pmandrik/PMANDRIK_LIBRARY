// P.~Mandrik, 2021, https://github.com/pmandrik

#ifndef PMLIB_MATH_HH
#define PMLIB_MATH_HH 1 

#include "pmlib_math_consts.hh"
#include "pmlib_v2d.hh"

namespace pm {
  
  // ======= DIFFERENT ALGOS & FUNCTIONS ====================================================================
  template <class T> int check_sign(T a){ if(a>0.){return 1;} if(a<0.){return -1;} return 0; }
  template <class T> bool check_in(T a, T min, T max){ return (a<min or a>max) ? false : true;}
  template <class T> T wrap_around(T a, T min, T max){ 
    if( a < min ) return max-1; 
    if( a >= max ) return min; 
    return a;
  }
  
  bool is_point_in_box(const v2 & point, const v2 & box_center, const v2 & box_size){ // box size non zero plz
    // dV.x = x_t - x_0 = A * x_1 + B * x_2
    // dV.y = y_t - y_0 = A * y_1 + B * y_2
    // A = dV.x/x_1 - B*x_2/x_1 = dV.y/y_1 - B*y_2/y_1
    // (x_1*y_2-y_1*x_2)*B = x_1*dV.y - y_1*dV.x
    v2 dV    = point - box_center;
    v2 sizer = v2(box_size.x, 0); // x_1
    v2 perpr = v2(0, box_size.y); // x_2

    float B = (sizer.x * dV.y - sizer.y * dV.x) / (sizer.x * perpr.y - sizer.y * perpr.x);
    if(abs(B) > 1) return false;
    float A;
    if( abs(sizer.x) > abs(sizer.y) ) A = (dV.x - B * perpr.x)/sizer.x;
    else                              A = (dV.y - B * perpr.y)/sizer.y;
    if(abs(A) > 1) return false;
    return true;
  }
  
  bool is_point_in_box(const v2 & point, const v2 & box_center, const v2 & box_size, const float & angle){
    // dV.x = x_t - x_0 = A * x_1 + B * x_2
    // dV.y = y_t - y_0 = A * y_1 + B * y_2
    // A = dV.x/x_1 - B*x_2/x_1 = dV.y/y_1 - B*y_2/y_1
    // (x_1*y_2-y_1*x_2)*B = x_1*dV.y - y_1*dV.x
    v2 dV    = point - box_center;
    v2 sizer = v2(box_size.x, 0).Rotated(angle); // x_1
    v2 perpr = v2(0, box_size.y).Rotated(angle); // x_2

    float B = (sizer.x * dV.y - sizer.y * dV.x) / (sizer.x * perpr.y - sizer.y * perpr.x);
    if(abs(B) > 1) return false;
    float A;
    if( abs(sizer.x) > abs(sizer.y) ) A = (dV.x - B * perpr.x)/sizer.x;
    else                              A = (dV.y - B * perpr.y)/sizer.y;
    if(abs(A) > 1) return false;
    return true;
  }
  
  //------------------------------------------------------------------ GEOMETRICAL ABSRACTS -----------------------------------------------------------------
  struct Rect {
    /// rectangular box
    Rect(float x_min_, float y_min_, float x_max_, float y_max_, float angle_ = 0){
      x_min = x_min_; x_max = x_max_; y_min = y_min_; y_max = y_max_;
    }
    
    v2 GetCenter() const { return v2(x_max*0.5 + x_min*0.5, y_max*0.5 + y_min*0.5); }
    v2 GetSize() const { return v2(x_max-x_min,y_max-y_min); }
    bool Contain(const v2 & pos) const { return is_point_in_box(pos, GetCenter(), GetSize(), angle); }
    float x_min, x_max, y_min, y_max;
    float angle;
  };
  
  //------------------------------------------------------------------ ANGLE MANIPULATION ------------------------------------------------------------------
  void angle_norm(float & angle){
    while(angle < 0) angle += 360.f;
    while(angle > 360.f) angle -= 360.f;
  }

  float angle_disstance(const float & a1, const float & a2){
    float d1 = max(a1, a2) - min(a1, a2);
    return min(d1, 360.f - d1);
  }

  float angle_interpolate(float angle, float target_angle, const float & frac){
    angle_norm(angle);
    angle_norm(target_angle);
    float d1 = max(angle, target_angle) - min(angle, target_angle);
    float d2 = 360.f - d1;
    if(d1 < d2) angle = angle + frac * (target_angle - angle);
    else        angle = angle - frac * d2 * check_sign(target_angle - angle);
    angle_norm(angle);
    return angle;
  }



};

#endif
