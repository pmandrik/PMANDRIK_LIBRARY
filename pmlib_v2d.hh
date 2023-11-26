// P.~Mandrik, 2021, https://github.com/pmandrik

#ifndef PMLIB_V2D_HH
#define PMLIB_V2D_HH 1 

#include "pmlib_math_consts.hh"

#ifndef CERN_ROOT
  #include <string>
  #include <ostream>
#endif

namespace pm {
  
  template<typename T = float> class v3;
  // ======= v2 =====================================================================================================
  template<typename T = float>
  class v2 {
    public:
    v2(T xx = 0., T yy = 0., T zz = 0.) : x(xx),y(yy),z(zz) {};
    v2 Rotate(float angle){
      float c = cos(angle); 
      float s = sin(angle);
      return  v2( x * c - y * s, x * s + y * c);
    }
    inline v2 Rotated(float angle){return Rotate(angle * PI_180);}
    inline T L(){return sqrt(x*x + y*y);}
    inline T L2(){return x*x + y*y;}
    inline float Angle(){return (y > 0 ? acos(x / (L()+OSML) ) : PI - acos(x / (L()+OSML) ));}
    inline float Angled(){return IP_081 * (y > 0 ? acos(x / (L()+OSML) ) : 2*PI - acos(x / (L()+OSML) ));}
    inline v2 Set(const v2 & v){x = v.x; y = v.y; return *this;}

    v2 & operator *= (float value){x *= value; y *= value; return *this;}
    v2 & operator /= (float value){x /= value; y /= value; return *this;}
    v2 & operator += (const v2 & v){x += v.x; y += v.y; return *this;}
    v2 & operator += (const v3<T> & v);
    v2 & operator -= (const v2 & v){x -= v.x; y -= v.y; return *this;}
    v2 & operator *= (const v2 & v){x *= v.x; y *= v.y; return *this;}
    v2 & operator /= (const v2 & v){x /= v.x; y /= v.y; return *this;}
    v2 operator - () const {return v2(-x, -y);}

    T x, y, z;
  };

  template<typename T = float> v2<T> operator + (v2<T> va, v2<T> vb){return v2<T>(va.x + vb.x, va.y + vb.y);}
  template<typename T = float> v2<T> operator - (v2<T> va, v2<T> vb){return v2<T>(va.x - vb.x, va.y - vb.y);}
  template<typename T = float> v2<T> operator * (float value, v2<T> v){return v2<T>(v.x*value, v.y*value);}
  template<typename T = float> v2<T> operator * (v2<T> v, float value){return v2<T>(v.x*value, v.y*value);}
  template<typename T = float> v2<T> operator * (v2<T> va, v2<T> vb){return v2<T>(va.x * vb.x, va.y * vb.y);}
  template<typename T = float> v2<T> operator / (v2<T> v, float value){return v2<T>(v.x/value, v.y/value);}
  template<typename T = float> v2<T> operator / (v2<T> va, v2<T> vb){return v2<T>(va.x / vb.x, va.y / vb.y);}
  template<typename T = float> bool operator == (v2<T> va, v2<T> vb){return ((va.x == vb.x) and (va.y == vb.y));}
  template<typename T = float> bool operator != (v2<T> va, v2<T> vb){return ((va.x != vb.x) or (va.y != vb.y));}
  template<typename T = float> std::ostream & operator << (std::ostream & out, v2<T> v){return out << "v2(" << v.x << "," << v.y << ")";}

  using v2c = v2<char>;
  using v2i = v2<int>;
  using v2s = v2<size_t>;
  using v2f = v2<float>;
  using v2d = v2<double>;

  // ======= v3 =====================================================================================================
  template<typename T>
  class v3 {
    public:
    v3(T xx = 0., T yy = 0., T zz = 0.):x(xx),y(yy),z(zz){};
    v3(v2<T> v):x(v.x),y(v.y),z(v.z){};
    inline v3 Rotate(float angle, int axis){
      if(axis==0){
        v2 yz = v2(y, z).Rotate(angle);
        return v3(x, yz.x, yz.y);
      }
      if(axis==1){
        v2 xz = v2(x, z).Rotate(angle);
        return v3(xz.x, y, xz.y);
      }
      if(axis==2){
        v2 xy = v2(x, y).Rotate(angle);
        return v3(xy.x, xy.y, z);
      }
      return *this;
    }
    inline v3 Rotated(const float & angle, int axis){return Rotate(angle * PI_180, axis);}
    inline v3 Rotated(const float & angle_x, const float & angle_y, const float & angle_z){
      return Rotate(angle_x * PI_180, 0).Rotate(angle_y * PI_180, 1).Rotate(angle_z * PI_180, 2);
    }

    inline v3 PerspectiveScale(const v3 & start_pos, const float & end_plane_x, const float & end_plane_y){
      return v3(start_pos.x - (z - end_plane_x) / (start_pos.z - end_plane_x) * (start_pos.x - x), 
                start_pos.y - (z - end_plane_y) / (start_pos.z - end_plane_y) * (start_pos.y - y), 
                z);
    }

    inline float L(){return sqrt(x*x + y*y + z*z);}
    inline float L2(){return x*x + y*y + z*z;}

    T x, y, z;

    v3 & operator *= (float value){x *= value; y *= value; z *= value; return *this;}
    v3 & operator /= (float value){x /= value; y /= value; z /= value; return *this;}
    v3 & operator += (const v3 & v){x += v.x; y += v.y; z += v.z; return *this;}
    v3 & operator += (const v2<T> & v){x += v.x; y += v.y; return *this;}
    v3 & operator -= (const v3 & v){x -= v.x; y -= v.y; z -= v.z; return *this;}
    v3 & operator *= (const v3 & v){x *= v.x; y *= v.y; z *= v.z; return *this;}
    v3 & operator /= (const v3 & v){x /= v.x; y /= v.y; z /= v.z; return *this;}
    v3 operator - () const {return v3(-x, -y, -z);}
  };

  template<typename T = float> v3<T> operator + (v3<T> va, v3<T> vb){return v3(va.x + vb.x, va.y + vb.y, va.z + vb.z);}
  template<typename T = float> v3<T> operator - (v3<T> va, v3<T> vb){return v3(va.x - vb.x, va.y - vb.y, va.z - vb.z);}
  template<typename T = float> v3<T> operator * (float value, v3<T> v){return v3(v.x*value, v.y*value, v.z*value);}
  template<typename T = float> v3<T> operator * (v3<T> v, float value){return v3(v.x*value, v.y*value, v.z*value);}
  template<typename T = float> v3<T> operator * (v3<T> va, v3<T> vb){return v3(va.x * vb.x, va.y * vb.y, va.z * vb.z);}
  template<typename T = float> v3<T> operator / (v3<T> v, float value){return v3(v.x/value, v.y/value, v.z/value);}
  template<typename T = float> v3<T> operator / (v3<T> va, v3<T> vb){return v3(va.x / vb.x, va.y / vb.y, va.z / vb.z);}

  template<typename T = float> bool operator == (v3<T> va, v3<T> vb){return ((va.x == vb.x) and (va.y == vb.y) and (va.z == vb.z));}
  template<typename T = float> bool operator != (v3<T> va, v3<T> vb){return ((va.x != vb.x) or (va.y != vb.y) or (va.z != vb.z));}

  template<typename T = float> std::ostream & operator << (std::ostream & out, v3<T> v){return out << "v3(" << v.x << "," << v.y << "," << v.z << ")";};

  template<typename T = float> v3<T> operator + (const v3<T> & va, const v2<T> & vb){return v3(va.x + vb.x, va.y + vb.y);}
  template<typename T = float> v2<T> operator + (const v2<T> & va, const v3<T> & vb){return v2(va.x + vb.x, va.y + vb.y);}
  template<typename T> v2<T> & v2<T>::operator += (const v3<T> & v){x += v.x; y += v.y; return *this;}

  // ======= rgb ====================================================================
  class rgb {
    public:
    rgb(float xx = 0., float yy = 0., float zz = 0., float aa = 1.):r(xx),g(yy),b(zz),a(aa){};
    float r, g, b, a;

    rgb & operator *= (float value){r *= value; g *= value; b *= value; return *this;}
    rgb & operator /= (float value){r /= value; g /= value; b /= value; return *this;}
    rgb & operator += (const rgb & v){r += v.r; g += v.g; b += v.b; return *this;}
    rgb & operator -= (const rgb & v){r -= v.r; g -= v.g; b -= v.b; return *this;}
    rgb & operator *= (const rgb & v){r *= v.r; g *= v.g; b *= v.b; return *this;}
    rgb & operator /= (const rgb & v){r /= v.r; g /= v.g; b /= v.b; return *this;}
    rgb operator - () const {return rgb(-r, -g, -b);}

    rgb Norm_255i(){ return rgb(int(r*255), int(g*255), int(b*255), int(a*255) ); }
    rgb Norm_255f(){ return rgb(r*255, g*255, b*255, a*255 ); }
    rgb Norm_1f()  { return rgb(r/255., g/255., b/255., a/255. ); }

    bool Equal_i(const int & rr, const int & gg, const int & bb) const {
      return (int(r)==rr) and (int(g)==gg) and (int(b)==bb);
    }
  };

  rgb operator + (rgb va, rgb vb){return rgb(va.r + vb.r, va.g + vb.g, va.b + vb.b);}
  rgb operator - (rgb va, rgb vb){return rgb(va.r - vb.r, va.g - vb.g, va.b + vb.b);}
  rgb operator * (float value, rgb v){return rgb(v.r*value, v.g*value, v.b*value);}
  rgb operator * (rgb v, float value){return rgb(v.r*value, v.g*value, v.b*value);}
  rgb operator * (rgb va, rgb vb){return rgb(va.r * vb.r, va.g * vb.g, va.b * vb.b);}
  rgb operator / (rgb v, float value){return rgb(v.r/value, v.g/value, v.b/value);}
  rgb operator / (rgb va, rgb vb){return rgb(va.r / vb.r, va.g / vb.g, va.b / vb.b);}

  std::ostream & operator << (std::ostream & out, rgb v){return out << " rgb( " << v.r << ", " << v.g << ", " << v.b << ", " << v.a << " ) ";};
}

#endif




