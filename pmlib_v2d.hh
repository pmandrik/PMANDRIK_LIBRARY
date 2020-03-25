///////////////////////////////////////////////////////////////////////////////
//  Author      :     P.S. Mandrik, IHEP
//  Date        :     08/03/20
//  Last Update :     08/03/20
//  Version     :     1.0
///////////////////////////////////////////////////////////////////////////// 

#ifndef PMLIB_V2D_HH
#define PMLIB_V2D_HH 1 

#include "pmlib_const.hh"

namespace pm {
  
  // ======= v2 =====================================================================================================
  class v2 {
    public:
    v2(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v2 Rotate(float angle){
      float c = cos(angle); 
      float s = sin(angle);
      return  v2( x * c - y * s, x * s + y * c);
    }
    inline v2 Rotated(float angle){return Rotate(angle * PI_180);}
    inline float L(){return sqrt(x*x + y*y);}
    inline float L2(){return x*x + y*y;}
    inline float Angle(){return (y > 0 ? acos(x / (L()+OSML) ) : PI - acos(x / (L()+OSML) ));}
    inline float Angled(){return IP_081 * (y > 0 ? acos(x / (L()+OSML) ) : 2*PI - acos(x / (L()+OSML) ));}

    v2 & operator *= (float value){x *= value; y *= value; return *this;}
    v2 & operator /= (float value){x /= value; y /= value; return *this;}
    v2 & operator += (const v2 & v){x += v.x; y += v.y; return *this;}
    v2 & operator -= (const v2 & v){x -= v.x; y -= v.y; return *this;}
    v2 & operator *= (const v2 & v){x *= v.x; y *= v.y; return *this;}
    v2 & operator /= (const v2 & v){x /= v.x; y /= v.y; return *this;}
    v2 operator - () const {return v2(-x, -y);}

    float x, y, z;
  };

  v2 operator + (v2 va, v2 vb){return v2(va.x + vb.x, va.y + vb.y);}

  v2 operator - (v2 va, v2 vb){return v2(va.x - vb.x, va.y - vb.y);}

  v2 operator * (float value, v2 v){return v2(v.x*value, v.y*value);}
  v2 operator * (v2 v, float value){return v2(v.x*value, v.y*value);}
  v2 operator * (v2 va, v2 vb){return v2(va.x * vb.x, va.y * vb.y);}

  v2 operator / (v2 v, float value){return v2(v.x/value, v.y/value);}
  v2 operator / (v2 va, v2 vb){return v2(va.x / vb.x, va.y / vb.y);}

  bool operator == (v2 va, v2 vb){return ((va.x == vb.x) and (va.y == vb.y));}
  bool operator != (v2 va, v2 vb){return ((va.x != vb.x) or (va.y != vb.y));}

  ostream & operator << (ostream & out, v2 v){return out << "v2(" << v.x << "," << v.y << ")";};

  // ======= v3 =====================================================================================================

};

#endif




