// P.S. Mandrik https://github.com/pmandrik/

#ifndef PMLIB_DRAWING_HH
#define PMLIB_DRAWING_HH 1

#ifndef CERN_ROOT
  #include <vector>
  #include <map>
  #include <string>
  #include <algorithm>
  #include <stack>
  
  #include "pmlib_msg.hh"
  #include "pmlib_v2d.hh"
#endif

namespace pm {

  namespace drawing{
    const unsigned int   MAX_QUAD_NUMBER      = 100000;
    const unsigned int   MAX_QUAD_NUMBER10    = MAX_QUAD_NUMBER/10;
    const unsigned int   QUAD_VERTEXES_NUMBER = 4;
    const unsigned int   VERTEX_ARRAY_SIZE    = 5;
    const unsigned int   QUAD_ARRAY_SIZE      = QUAD_VERTEXES_NUMBER * VERTEX_ARRAY_SIZE;
    const float PERSPECTIVE_EDGE     = -1000000.;
    const float HIDE_EDGE            = PERSPECTIVE_EDGE/2;
    
    #ifdef USE_OPENGL
      const unsigned int   VERTEX_BITE_SIZE     = VERTEX_ARRAY_SIZE*sizeof(GL_FLOAT);
    #endif
    
    const int TARGET_SCREEN_ID = 0;
    
    int fps = 60;
    int WW = 800; int WH = 600;
    int WW2 = WW / 2; int WH2 = WH / 2;
    v2i WV  = v2i(WW,  WH); v2i WV2 = v2i(WW2, WH2);   
    void set_windows_size(int ww, int wh){
      WW = ww;
      WH = wh;
      WW2 = WW / 2;
      WH2 = WH / 2;
      WV  = v2i(WW,  WH );
    }
  };
  
  // ======= Drawing-related tools ====================================================================
  struct CameraTarget{
    CameraTarget() : pos(0), time(0), zoom(0), normal(0), max_time(1)  {}
    CameraTarget(v2 p, int t, float z, v2 n) : pos(p), time(t), zoom(z), normal(n), max_time(t)  {}

    void Tick( float mval ){
      float val = mval * (time / (float) max_time);
      pos *= val;
      zoom *= val;
      normal *= val;
      time = max(0, time-1);
    }

    v2 pos;
    int time;
    int max_time;
    float zoom;
    v2 normal;
  };
  
  class Camera {
    // TODO rewrite this
    public:
      inline void Resize( const int & r){ zoom = max(0.000000001, zoom - 0.1*r*zoom); }
      inline void ResetZoom(){ zoom = 1.; }
      void Reset(){
        to = v2(0, 0, 0);
        from = v2(0, 0, 1);
        normal = v2(0, 1, 0);
        zoom = 1;
        buffer = drawing::z_buffer;
        pause = true;
        smooth = false;
        angleZ = 0.;
      }

      void SetTarget(v2 target){
        to.Set(target);
        from.Set(target);
      }

      void SetTarget(v2 target, float z, v2 norm){
        SetTarget(target);
        zoom = z;
        normal = norm;
      }

      void Move(v2 shift){
        to += shift;
        from += shift;
      }
      virtual void SetScreenTarget(){}
      virtual void SetDraweTarget(){}
      void PushPoint(v2 pos, int time, float zoom, v2 normal = v2(0,1)){
        targets.push_back( CameraTarget(pos, time, zoom, normal) );
      }

      v2 AbsToScreen(int x, int y, bool flip_y = true){ return -to + v2(x - drawing::WW2, (-y + drawing::WH2) * (-1 + 2 * flip_y) ) * zoom; }
      v2 AbsToScreen(v2 pos,       bool flip_y = true){ return -to + v2(pos.x - drawing::WW2, (-pos.y + drawing::WH2) * (-1 + 2 * flip_y) ) * zoom; }

      float GetPhi(){ return normal.Angle(); }
      bool Finished(){ return not targets.size(); }
      void Print(){ msg(to, zoom, normal); }
      
      virtual void SetScreenTarget(){}
      virtual void SetDraweTarget(){}
      virtual void ReTick(){}
      virtual void LoadFBDefault(){}
      virtual void LoadDefault(){}
      virtual void Tick(){}

      v2 to, from, normal; 
      CameraTarget inertia;
      float zoom, buffer, angleZ;
      vector<CameraTarget> targets;
      bool pause, smooth;
  };

  // ======= Texture-related classes ====================================================================
  /// Texture is a wrapper around image, registered at the GPU Device space side
  struct TexTile {
    /// TexTile store the info about position & size of part of image in Texture normalized to 1. space
    /// and original position & size of 
    TexTile() {}
    TexTile(v2f p, v2f s) : pos(p), size(s) {
		  tsize = v2f(1,1);
      tpos = v2f();      
    }
    TexTile(v2f p, v2f s, v2f nsize) : pos(p), size(s) {
		  tsize = size / nsize;
      tpos = pos / nsize;
    }

    v2f pos, size; // size in image size dimension, such as size in pixels
    v2f tpos, tsize; // size in space normalized to texture size
    void Print(){ msg(__PFN__, pos, size, tpos, tsize); };
  };

  class QuadData {
    /// store data we need to know to draw quad with texure
    public:
      QuadData(){}
      QuadData(v2f pos_, v2f size_, v2f flip_, float angle_, TexTile * ttile_){
        Set(pos_, size_, flip_, angle_, ttile_);
      }

      void Set(v2f pos_, v2f size_, v2f flip_, float angle_, TexTile * ttile_){
        pos = pos_;
        size = size_;
        flip = flip_;
        angle = angle_;
        ttile = ttile_;
      }

      float angle;
      v2f pos, size, flip;
      TexTile * ttile;      
  };
  
  class Texture {
    /// Texture is wraped around Image and know how to draw it.
    public:
    Texture(size_t width, size_t height, size_t format, size_t type, const void * data){
      size = v2f(width, height);
      w = width;
      h = height;
      this->type = type;
      this->format = format;
      pixels = data;
    };
    
    virtual ~Texture() = default;
    virtual void Bind()  { }
    virtual void Unbind(){ }

    virtual void DrawFast(v2f pos, v2f size, TexTile *ttile, float z_pos=0){}
    virtual void Draw(v2f pos, v2f size, v2f tpos=v2f(0., 0.), v2f tsize=v2f(1., 1.), float z_pos=0,  const float & angle=0, bool flip_x=false, bool flip_y=false){}
    
    void Draw(v2f pos, v2f size, TexTile * ttile, float z_pos=0, const float & angle=0, bool flip_x=false, bool flip_y=false){
      Draw(pos, size, ttile->tpos, ttile->tsize, z_pos, angle, flip_x, flip_y);
    }

    void Draw( QuadData & dqd ){
      Draw( dqd.pos, dqd.size, dqd.ttile, dqd.pos.z, dqd.angle, dqd.flip.x, dqd.flip.y );
    }
    
    v2f size;
    size_t w,h, format, type;
    const void * pixels;
  };
  
  class TextureAtlas {
    private:
    std::map<std::string, TexTile> atlas;
    Texture * texture;
    
    public:
    TextureAtlas(Texture * texture_){
      texture = texture_;
    }
    
    TexTile * Get(const std::string & key){
      auto ptr = atlas.find(key);
      if(ptr == atlas.end()) return nullptr;
      return &(ptr->second);
    }

    void Add(const std::string & key, const v2f & p, const v2f & s){ 
      if(Get(key)) return;
      atlas[ key ] = TexTile(p, s, texture->size);
    }
  };

  // ======= classes to store data in arrays suitable for Device ====================================================================
  template<typename T>
  class QuadDataArray {
    protected:
    T * data;
    Texture* texture;
    public:
    void SetTexture(Texture* text){ texture = text; }
    const T * GetData() const { return data; }
    virtual void Draw(){}
    virtual void Clean(){}
    virtual void Remove( unsigned int id ){}
    virtual unsigned int AddDummy(){ return 0; }
    virtual unsigned int Add(v2f & pos, v2f & size, float & angle, v2f & flip, TexTile *ttile){ return 0; };
    virtual void Change( unsigned int id, v2f & pos, v2f & size, float & angle, v2f & flip, TexTile *ttile ){};
    virtual void ChangeSafe( unsigned int id, v2f & pos, v2f & size, float & angle, v2f & flip, TexTile *ttile ){};
  };
  
  template<typename T>
  class QuadDataArrayDef : public QuadDataArray<T> { 
    public :
      QuadDataArrayDef(unsigned int max_size = drawing::MAX_QUAD_NUMBER) : QuadDataArray<T>() {
        max_quads_number = std::min(max_size, drawing::MAX_QUAD_NUMBER);
        this->data = new T[max_size*drawing::QUAD_ARRAY_SIZE + 1];
        Clean();
      }
      
      ~QuadDataArrayDef(){
        delete [] this->data;
      }

      unsigned int FindFreePosition(){
        if( free_positions.size() ) {
          unsigned int answer = free_positions.top();
          free_positions.pop();
          return answer;
        }
        for(unsigned int i = quads_number; i < max_quads_number; i++){
          if(this->data[i*drawing::QUAD_ARRAY_SIZE+2] <= drawing::PERSPECTIVE_EDGE){ quads_number = std::min(i+1, drawing::MAX_QUAD_NUMBER); return i; }
        }
        for(unsigned int i = 0; i < quads_number; i++){
          if(this->data[i*drawing::QUAD_ARRAY_SIZE+2] <= drawing::PERSPECTIVE_EDGE){ quads_number = std::min(i+1, drawing::MAX_QUAD_NUMBER); return i; }
        }
        // MSG_WARNING("QuadDataArrayDef.FindFreePosition(): warning, cant find free position");
        return (quads_number < max_quads_number ? quads_number : 0);
      }

      unsigned int AddDummy(){
        /// reserve ID without updating of the vertex array data
        unsigned int id = FindFreePosition() * drawing::QUAD_ARRAY_SIZE;
        return id;
      }

      unsigned int Add(v2f pos, v2f size, v2f tpos, v2f tsize){
        int id = FindFreePosition() * drawing::QUAD_ARRAY_SIZE;
        Change(id, pos, size, tpos, tsize, 0, false);
        return id;
      }

      void Remove(const int & id){
        this->data[id+2]  = drawing::PERSPECTIVE_EDGE;
        this->data[id+7]  = drawing::PERSPECTIVE_EDGE;
        this->data[id+12] = drawing::PERSPECTIVE_EDGE;
        this->data[id+17] = drawing::PERSPECTIVE_EDGE;
        if( free_positions.size() < drawing::MAX_QUAD_NUMBER10 ) free_positions.push( id );
      }

      virtual void Clean(){
        for(int id = 0, id_max = max_quads_number*drawing::QUAD_ARRAY_SIZE; id < id_max; id+=drawing::QUAD_ARRAY_SIZE){
          this->data[id+2]  = drawing::PERSPECTIVE_EDGE;
          this->data[id+7]  = drawing::PERSPECTIVE_EDGE;
          this->data[id+12] = drawing::PERSPECTIVE_EDGE;
          this->data[id+17] = drawing::PERSPECTIVE_EDGE;
        }
      }

      void Move(const int & id, const v2f & shift){
        for(int i = id; i < id+drawing::QUAD_ARRAY_SIZE; i+=drawing::VERTEX_ARRAY_SIZE){
          this->data    public :[i+0] += shift.x;
		      this->data[i+1] += shift.y;
        }
      }

      void ChangeFast(const int & id, const v2f & pos, const v2f & size){
        this->data[id+0]  = pos.x - size.x;
		    this->data[id+1]  = pos.y + size.y;

		    this->data[id+5]  = pos.x + size.x;
		    this->data[id+6]  = pos.y + size.y;

		    this->data[id+10] = pos.x + size.x;
		    this->data[id+11] = pos.y - size.y;

		    this->data[id+15] = pos.x - size.x;
		    this->data[id+16] = pos.y - size.y;
      }

      void Change(const int & id, const v2f & pos, v2f size, const v2f & tpos, const v2f & tsize, const int & angle, const v2f & flip){
        if(flip.x) size.x *= -1;
        if(flip.y) size.y *= -1;
        
        v2f perp = v2f(-size.x, size.y);
        if(angle){
          size = size.Rotated(angle);
          perp = perp.Rotated(angle);
          // perp = v2f(-size.x, size.y);
        }

        this->data[id+0]  = pos.x - size.x; this->data[id+3]  = tpos.x;
		    this->data[id+1]  = pos.y + size.y; this->data[id+4]  = tpos.y;
        this->data[id+2]  = pos.z;

		    this->data[id+5]  = pos.x - perp.x; this->data[id+8]  = tpos.x + tsize.x;
		    this->data[id+6]  = pos.y + perp.y; this->data[id+9]  = tpos.y;
        this->data[id+7]  = pos.z;

		    this->data[id+10] = pos.x + size.x; this->data[id+13] = tpos.x + tsize.x;
		    this->data[id+11] = pos.y - size.y; this->data[id+14] = tpos.y + tsize.y;
        this->data[id+12] = pos.z;

		    this->data[id+15] = pos.x + perp.x; this->data[id+18] = tpos.x;
		    this->data[id+16] = pos.y - perp.y; this->data[id+19] = tpos.y + tsize.y;
        this->data[id+17] = pos.z;
      }

      void ChangeSafe(const int & id, const v2f & pos, const v2f & size, const v2f & tpos, const v2f & tsize, const int & angle, const v2f & flip){
        if(id < 0 or id >= drawing::MAX_QUAD_NUMBER) { /*MSG_WARNING(__PFN__, "can't update data, incorrect index/id", id);*/ return; }
        Change(id, pos, size, tpos, tsize, angle, flip);
      }

      void Print(){
        msg(__PFN__);
        for(int i = 0; i < max_quads_number; i++){
          if(this->data[i*drawing::QUAD_ARRAY_SIZE+2] > drawing::PERSPECTIVE_EDGE){
            int id = i*drawing::QUAD_ARRAY_SIZE;
            msg( i ); 
            for(int v = 0; v < 20; v+=5)
              msg( "[(", this->data[id+0+v],this->data[id+1+v],this->data[id+2+v], "),(", this->data[id+3+v], this->data[id+4+v], ")]" );
          }
        }
      }

      int max_quads_number, quads_number = 0;
      std::stack<int> free_positions;
  };
  
  // ======= classes to Draw  ====================================================================
  class Drawer {
    /// Following workflow we have: loop sync, loop drawings where bind target first,
    /// then pass data to the Device to actual draw, later unbind target if needed 
      int target_id;
    public :
      //! Drawable Object need to sync with Drawer 
      //!   -> Drawer store Drawable data and provide interface for Drawable Object
      //!   -> use interface of internal data holder
      virtual void UpdateDrawer( unsigned int draw_id ){}
      virtual void UpdateDrawerSafe( unsigned int draw_id ){}
      virtual void UpdateDrawerFast( unsigned int draw_id ){}
      virtual void Hide(unsigned int draw_id){}
      //! return Drawer Id 
      virtual unsigned int RegisterDrawableObject(){ return 0; }
    
      //! Target for drawer is binded by central DrawSystem 
      //!   -> Drawer store Target reference
      int SetTarget(int target_id){ this->target_id = target_id; } 
      int GetTarget(){ return this->target_id; }
      
      //! Drawer get Drawable data and pass to the Drawing Low level implementations to pass to the Device
      virtual int Draw(){ return PM_SUCCESS; }
      
      // list of Update functions
  };
  
  // Drawable Objects
  class DrawableObject {
      std::shared_ptr<Drawer> drawer;
      unsigned int draw_id;
    
    public:
      void SetDrawerImp(std::shared_ptr<Drawer> drawer){
        /// Define what kind of drawer implementation will be used
        this->drawer = drawer;
        draw_id = drawer->RegisterDrawableObject();
      }
  };
  
  class DrawableQuad : public DrawableObjects {
      /// DrawableQuad is defined by pos (x,y,z), size (w,h), angle. We also have boolean to flip/mirror it over x, y axis.
      /// Thus, DrawableQuad is actually a 2D Quad.
      /// Finally, what we actually draw is Texture (OpenGL) defined by TexTile object (relative coordinates in texture).
      /// Drawer - class where we actually draw this.
      /// Drawer child classes where we implements how we draw this.
      
      public:
        DrawableQuad(v2 pos_, v2 size_, v2 flip_, float angle_, TexTile * ttile_){
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
  
  // DrawSystem
  class DrawSystem {
    public:
    
      std::vector< std::shared_ptr<Drawer> > drawers;
      std::vector<int> drawers_order;
      std::vector<int> targets;
      int target_id;
      std::shared_ptr<Camera> camera;
      
      //! change active target to drawing
      void ChangeTarget(int target_id_new){
        if( target_id == target_id_new ) return;
        auto target_old = targets[target_id];
        target_old->Unbind();
        
        if( target_id_new == drawing::TARGET_SCREEN_ID ) {
          // screen
          camera->SetScreenTarget();
          return;
        }
        auto target_new = targets[target_id_new];
        target_new->Bind();
        camera->SetTarget( target_new );
        
        target_id = target_id_new;
      }
      
      void Draw(){
        //! 
        for(int i = 0, i_max = drawers_order.size(); i < i_max; ++i){
          int index = drawers_order[i];
          std::shared_ptr<Drawer> drawer = drawers[index];
          
          int target_id_new = drawer->GetTarget();
          // change target
          ChangeTarget( target_id_new );
          
          drawer->Draw();
        }
      }
  };
  
  /// DrawSystem iterates over [Drawer] in right order and call Draw() function
  /// DrawSystem get targets from Drawer and Bind() it first
  /// DrawSystem blit final Drawers to the Screen
  /// 
};

#endif







