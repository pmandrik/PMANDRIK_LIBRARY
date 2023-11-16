#ifndef PMLIB_DRAWING_HH
#define PMLIB_DRAWING_HH 1

#ifndef CERN_ROOT
  #include <vector>
  #include <map>
  #include <string>
  #include <algorithm>
  
  #include "pmlib_msg.hh"
  #include "pmlib_v2.hh"
#endif

namespace pm {

  //=================================== 
  struct TexTile {
    /// TexTile store the info about position & size of part of image in Texture normalized to 1. space
    /// and original position & size of 
    TexTile() {}
    TexTile(v2 p, v2 s) : pos(p), size(s) {
		  tsize = v2(1,1);
      tpos = v2();      
    }
    TexTile(v2 p, v2 s, v2 asize) : pos(p), size(s) {
		  tsize = size / asize;
      tpos = pos / asize;
    }

    v2 pos, size, tpos, tsize;
    void Print(){ msg(__PFN__, pos, size, tpos, tsize); };
  };

  class QuadData {
    /// store data we need to know to draw quad with texure
    public:
      QuadData(){}
      QuadData(v2 pos_, v2 size_, v2 flip_, float angle_, TexTile * ttile_){
        Set(pos_, size_, flip_, angle_, ttile_);
      }

      void Set(v2 pos_, v2 size_, v2 flip_, float angle_, TexTile * ttile_){
        pos = pos_;
        size = size_;
        flip = flip_;
        angle = angle_;
        ttile = ttile_;
      }

      float angle;
      v2 pos, size, flip;
      TexTile * ttile;      
  };
  
  // ======= Texture interface class ====================================================================
  class TextureAtlas {
    private:
    map<string, TexTile> data;
    
    public:
    TexTile Get(const string & key){ // TODO maybe Texture Atlas should be separated from Texture ...
      auto ptr = atlas.find(key);
      if(ptr == atlas.end()) return nullptr;
      return &(ptr->second);
    }

    void Add(const string & key, const v2 & p, const v2 & s){ 
      if(GetTexTile(key)) return;
      atlas[ key ] = TexTile(p, s, size);
    }

    string GenerateAtlasGridName(int x, int y) const { // TODO set interface for this
      return "X" + to_string(x) + "Y" + to_string(y);
    };
    
    string GenerateAtlasName(string name, int x, int y) const {
      return name + "_" + to_string(x) + "_" + to_string(y);
    };
  };
  
  // ======= Texture interface class ====================================================================
  template<typename IMAGE>
  class Texture : public BaseClass {
    /// Texture is wraped around Image and know how to draw it.
    /// 
    public:
    Texture():image(nullptr){}
    Texture(std::shared_ptr<IMAGE> img){};
    
    virtual void Bind()  { }
    virtual void Unbind(){ }

    virtual void DrawFast(v2 pos, v2 size, TexTile *ttile, float z_pos=0){}
    virtual void Draw(v2 pos=sys::WV2, v2 size=sys::WV2, v2 tpos=v2(0., 0.), v2 tsize=v2(1., 1.), float z_pos=0,  const float & angle=0, bool flip_x=false, bool flip_y=false){}
    
    void Draw(v2 pos, v2 size, TexTile * ttile, float z_pos=0, const float & angle=0, bool flip_x=false, bool flip_y=false){
      Draw(pos, size, ttile->tpos, ttile->tsize, z_pos, angle, flip_x, flip_y);
    }

    void Draw( QuadData & dqd ){
      Draw( dqd.pos, dqd.size, dqd.ttile, dqd.pos.z, dqd.angle, dqd.flip.x, dqd.flip.y );
    }
    
    // v2 size;
    // double w,h;
    std::shared_ptr<IMAGE> image;
  };

};

#endif







