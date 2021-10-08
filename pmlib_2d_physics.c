// P.~Mandrik, 2021, https://github.com/pmandrik/GerdaEngine

#ifndef pmphysics
#define pmphysics 1

// g++ -DBUILD_WITH_PYTHON=1 -Wfatal-errors -Wall -Wno-long-long -I/usr/include/python2.7 -lpython2.7  -Wextra -O -ansi -pedantic -shared -o pmphysics.so -fPIC pmlib_2d_physics.c

#ifdef BUILD_WITH_PYTHON
  #include <Python.h>
#endif

#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <chrono>

#include "pmlib_math.hh" // pm::PI_180 = 3.141 / 180.
#include "pmlib_v2d.hh" // pm::v2

// #define DEBUG_CODE 1 
#ifdef DEBUG_CODE 
#include <iostream>
#define DPRINT(x) (x)
#else 
#define DPRINT(x) do{}while(0)
#endif

  // class to interact with tiled map data =====================
  class pmTiledMap{
    public:
    pmTiledMap(){}
    pmTiledMap( const int & map_size_x, const int & map_size_y, const int & map_size_layers, const int & tile_size, int * map_data ){
      Init(map_size_x, map_size_y, map_size_layers, tile_size, map_data);
    }

    void Init(const int & map_size_x, const int & map_size_y, const int & map_size_layers, const int & tile_size, int * map_data){
      msize_x = map_size_x;
      msize_y = map_size_y;
      msize_layers = map_size_layers;
      msize_layers_x_msize_y = msize_y * msize_layers;
      tsize = tile_size;
      tsize_vec2 = pm::v2(tsize, tsize)*0.5;
      mdata = map_data;
    }
    
    int default_tile_type = 0;
    int msize_x, msize_y, msize_layers, msize_layers_x_msize_y;
    int tsize, bullet_grid_size;
    int * mdata;
    pm::v2 tsize_vec2;

    // get map values ============================>
    int ipx_, ipy_;
    int GetMapIndexSafe( const float & px, const float & py ) {
      ipx_ = int(px) / tsize; ipy_ = int(py) / tsize;
      if(ipx_ < 0 or ipx_ > msize_x or ipy_ < 0 or ipy_ > msize_y){
        printf("pmPhysic.GetMapIndexSafe(): invalid request %f %f for map %i %i %i \n", px, py, msize_x, msize_y, tsize);
        return 0;
      }
      return msize_layers_x_msize_y*ipx_ + msize_layers*ipy_;
    }

    int GetTypeI( const int & ipx, const int & ipy, const int & layer) { return mdata[ msize_layers_x_msize_y*ipx + msize_layers*ipy + layer ]; }
    int GetTypeSafeI( const int & ipx, const int & ipy, const int & layer) {
      if(ipx < 0 or ipx > msize_x or ipy < 0 or ipy > msize_y or layer < 0 or layer > msize_layers) return default_tile_type;
      return mdata[ msize_layers_x_msize_y*ipx + msize_layers*ipy + layer ];
    }

    int GetType( const float & px, const float & py, const int & layer)  { return GetTypeI(int(px) / tsize, int(py) / tsize, layer); }
    int GetTypeSafe( const float & px, const float & py, const int & layer) { return GetTypeSafeI(int(px) / tsize, int(py) / tsize, layer); }
    bool IsSolid(const float & px, const float & py){ return (GetTypeSafeI(int(px) / tsize, int(py) / tsize, 0) == 0); }
    bool IsSolid(const pm::v2 & pos){ return (GetTypeSafeI(int(pos.x) / tsize, int(pos.y) / tsize, 0) == 0); }
    
    float STP = 0.001;
    bool IsSolidU(const float & px, const float & py){ return (GetTypeSafeI(int(px) / tsize, int(py+STP) / tsize, 0) == 0); }
    bool IsSolidD(const float & px, const float & py){ return (GetTypeSafeI(int(px) / tsize, int(py-STP) / tsize, 0) == 0); }
    bool IsSolidL(const float & px, const float & py){ return (GetTypeSafeI(int(px-STP) / tsize, int(py) / tsize, 0) == 0); }
    bool IsSolidR(const float & px, const float & py){ return (GetTypeSafeI(int(px+STP) / tsize, int(py) / tsize, 0) == 0); }
    bool IsSolidU(const pm::v2 & pos){ return IsSolidU(pos); }
    bool IsSolidD(const pm::v2 & pos){ return IsSolidD(pos); }
    bool IsSolidL(const pm::v2 & pos){ return IsSolidL(pos); }
    bool IsSolidR(const pm::v2 & pos){ return IsSolidR(pos); }

    pm::v2 GetTileCenter(const float & px, const float & py){
      return pm::v2( int( px / tsize ), int( py / tsize ) ) * tsize + tsize_vec2;
    }

    // set map values ============================>
    void SetMapTypeSafe(const int & x, const int & y, const float * map_data){
      if(x < 0 or y < 0 or x > msize_x or y > msize_y) return;
      std::memcpy(mdata + msize_layers_x_msize_y*x + msize_layers*y, map_data, msize_layers * sizeof(int));
    }

    void Set(const int & x, const int & y, const int & layer, const int & value){
      /// unsafe
      mdata[msize_layers_x_msize_y*x + msize_layers*y + layer] = value;
    }

    void Fill(const int & layer, const int & value){
      for(int y = 0; y < msize_y; y++)
        for(int x = 0; x < msize_x; x++)
          mdata[msize_layers_x_msize_y*x + msize_layers*y + layer] = value;
    }

    void Fill(const int & sx, const int & sy, const int & ex, const int & ey, const int & layer, const int & value){
      /// unsafe
      for(int y = sy; y < ey; y++)
        for(int x = sx; x < ex; x++)
          mdata[msize_layers_x_msize_y*x + msize_layers*y + layer] = value;
    }
    
    // other ============================>
    bool CheckRayTracer(float pos_x, float pos_y, float targ_pos_x, float targ_pos_y){
      int room_id      = GetTypeSafe(pos_x, pos_y, 1);
      if( room_id != GetTypeSafe(targ_pos_x, targ_pos_y, 1) ) return false;
      float dx = targ_pos_x - pos_x;
      float dy = targ_pos_y - pos_y;
      float L = sqrt(dx*dx+dy*dy);
      float step_dist = 0.9*tsize;
      dx = step_dist / L * dx;
      dy = step_dist / L * dy;
      int max_steps = L / step_dist;
      if( max_steps < 1 ) return true; 
      for(int i = 0; i < max_steps; i++){
        pos_x += dx;
        pos_y += dy;
        if( room_id != GetTypeSafe(pos_x, pos_y, 1) ) return false;
        if( not GetTypeSafe(pos_x, pos_y, 0) ) return false;
      }
      return true;
    }

    void PrintMap(const int & layer) const {
      for(int y = 0; y < msize_y; y++){
        for(int x = 0; x < msize_x; x++)
          printf("%d", mdata[msize_layers_x_msize_y*x + msize_layers*y + layer] );
        printf("\n");
      }
    }
  };

  // class to calculate objects, bullets and map interations =========
  struct pmObject{
    pmObject(){}
    pmObject(const pm::v2 & pos_ , const pm::v2 & speed_, const pm::v2 & size_, const float & angle_){
      pos = pos_; 
      speed = speed_;
      size = size_;
      angle = angle_;
    }
    pm::v2 pos, speed, size;
    float angle;
    int dir    = 0;
    float mass = 1;
    int id = -1;
    bool interact_with_solid = false;
    bool in_solid            = false;
    int * tiles;
  };

  struct pmParticle{
    pmParticle(){}
    pmParticle(const pm::v2 & pos_, const pm::v2 & speed_){
      pos = pos_; 
      speed = speed_;
    }
    pm::v2 pos, speed;
    int id;
    int * tiles;
  };
  
  class pmPhysicContainer{
    /// handle unordered vectors of pmObject* & pmParticle*
    public:
    int msize_layers;
    std::vector<pmObject*>   objects;
    std::vector<pmParticle*> particles;

    template<typename T> void PushContainer(T * obj, vector<T*> & container){
      obj->id = container.size();
      container.push_back( obj );
    }
    
    template<typename T> bool PopContainer(const int & index, vector<T*> & container){
      int N = container.size();
      if( not N ) return false;
      if( index < 0 or index >= N ) return false;
      if( N == 1 ){
        container.pop_back();
        return true;
      }
      T* last = container.back();
      last->id = index;
      container[index] = last;
      container.pop_back();
      return true;
    }
    
    template<typename T> bool PopContainer(T * obj, vector<T*> & container){ return PopContainer(obj->id, container); }
    
    void Push(pmObject* obj){    PushContainer(obj, objects); }
    void Push(pmParticle* obj){  PushContainer(obj, particles); }
    void Pop(pmObject* obj){     PopContainer(obj, objects); }
    void Pop(pmParticle* obj){   PopContainer(obj, particles); }
    void PopObject(const int & index){     PopContainer(index, objects); }
    void PopParticle(const int & index){   PopContainer(index, particles); }
    
    pmObject* Create(){
      pmObject* obj = new pmObject();
      obj->tiles = new int[ msize_layers ];
      Push(obj);
      return obj;
    }
    
    template<typename T> void Delete(T * obj){
      delete obj->tiles;
      Pop(obj);
    }
    
  };

  class pmPhysic;
  typedef std::function< void( pmPhysic*, const int &, float &, float & ) > GetObjectPos_function;
  
  class pmPhysic{
    public:
    pmPhysic( bool use_objects = false ){
      printf("pmphysics.pmPhysic() ... ");
      tune_collision_speed = false;
      check_solid_tiles_of_body_points = false;
      check_internal_points = false;
      if(use_objects){
        get_object_pos = &pmPhysic::GetObjectPos_object;
      } else {
        get_object_pos = &pmPhysic::GetObjectPos_array;
      }
    }
    
    void SetMap( pmTiledMap * map ){
      printf("pmphysics.ChangeMap() ... ");
      game_map         = map;
      mdata            = map->mdata;
      bullet_grid_size = map->tsize*5;
      msize_layers     = map->msize_layers;
      tsize            = game_map->tsize;
      body_points_presision = map->tsize*0.5;
      printf("ok\n");
    }
    
    float * object_positions_;
    void GetObjectPos_array(const int & id, float & x, float & y){
      x = object_positions_[id];
      y = object_positions_[id+1];
    }
    
    vector<pmObject*> * objects_;
    void GetObjectPos_object(const int & id, float & x, float & y){
      x = (*objects_)[id]->pos.x;
      y = (*objects_)[id]->pos.y;
    }
    
    GetObjectPos_function get_object_pos;
    
    pmTiledMap * game_map;
    int msize_layers;
    int * mdata;
    int tsize;
    float body_points_presision;
    bool tune_collision_speed, check_solid_tiles_of_body_points, check_internal_points;

    bool CheckLinesIntersection(const pm::v2 & s1, const pm::v2 & e1, const pm::v2 & s2, const pm::v2 & e2){
      float dx1 = e1.x - s1.x;
      float dy1 = e1.y - s1.y;
      float dx2 = e2.x - s2.x;
      float dy2 = e2.y - s2.y;
      float scalar = dy1 * dx2  - dx1 * dy2;
      if( scalar < 0.001 ) return false;
      float alpha = ((s1.x - s2.x) * dy1 - (s1.y - s2.y) * dx1)/scalar;
      float betta = ((s1.x - s2.x) * dy2 - (s1.y - s2.y) * dx2)/scalar;
      if( alpha < 0 or alpha > 1 ) return false;
      if( betta < 0 or betta > 1 ) return false;
      return true;
    }

    bool CheckIfPointInRect(const pm::v2 & c, const pm::v2 & sv1, const pm::v2 & sv2, pm::v2 point){
      // check 4 points for rect solving equation v2(point_x - pos_x, point_y - pos_y) = a * (sv1) + b * (sv2), |a| < 1, |b| < 1 , |a| + |b| < 1
      // |y * x1 - x * y1| < | y2 * x1 - x2 * y1 |
      // |y * x2 - x * y2| < | y2 * x1 - x2 * y1 |
      point.x -= c.x;
      point.y -= c.y;
      if(std::abs(point.y * sv1.x - point.x * sv1.y) + std::abs(point.y * sv2.x - point.x * sv2.y) > std::abs( sv2.y * sv1.x - sv2.x * sv1.y ) ) return false;
      //printf("%f %f \n", c.x, c.y);
      //printf("%f %f %f %f %f %f \n", point.y, sv1.x, sv1.y, point.x, sv2.x, sv2.y);
      //printf("%f %f \n", point.y * sv1.x - point.x * sv1.y, point.y * sv2.x - point.x * sv2.y);
      return true;
    }

    pm::v2 edges_1_[5], edges_2_[5];
    bool CheckRectCollisions(const pm::v2 & c1, const pm::v2 & sv11, const pm::v2 & sv12, const pm::v2 & c2, const pm::v2 & sv21, const pm::v2 & sv22){
      // check simple distance condition
      if(abs(c1.x - c2.x) > max(abs(sv11.x), abs(sv12.x)) + max(abs(sv21.x), abs(sv22.x))) return false;
      if(abs(c1.y - c2.y) > max(abs(sv11.y), abs(sv12.y)) + max(abs(sv21.y), abs(sv22.y))) return false;

      edges_1_[0] = c1 + sv11 ;
      edges_1_[1] = c1 + sv12 ; 
      edges_1_[2] = c1 - sv11 ;
      edges_1_[3] = c1 - sv12 ;
      edges_1_[4] = c1 + sv11 ;
      edges_2_[0] = c2 + sv21 ;
      edges_2_[1] = c2 + sv22 ; 
      edges_2_[2] = c2 - sv21 ;
      edges_2_[3] = c2 - sv22 ;
      edges_2_[4] = c2 + sv21 ;

      // check in addition if vertexes are in rect
      for(int i = 0, i_max = 4; i < i_max; i++){
        if( CheckIfPointInRect(c1, sv11, sv12, edges_2_[i]) ) return true;
        if( CheckIfPointInRect(c2, sv21, sv22, edges_1_[i]) ) return true;
      }

      // check collisions using edges intersections
      for(int i = 0, i_max = 4; i < i_max; i++){
        pm::v2 & s1 = edges_1_[i];
        pm::v2 & e1 = edges_1_[i+1];
        for(int j = 0, j_max = 4; j < j_max; j++)
          if( CheckLinesIntersection( s1 , e1 , edges_2_[j], edges_2_[j+1] ) ) return true;
      }

      return false;
    }

    bool CheckSolidCollisionSimple(const float & pos_x, const float & pos_y, float & speed_x, float & speed_y){
      npos_x_ = pos_x + speed_x;
      npos_y_ = pos_y + speed_y;
      // pm::msg("check simple", npos_x_, npos_y_);
      if( not game_map->GetTypeSafe(npos_x_, pos_y, 0) )  { speed_x = 0; npos_x_ = pos_x; } //pm::msg("x"); }
      if( not game_map->GetTypeSafe(pos_x, npos_y_, 0) )  { speed_y = 0; npos_y_ = pos_y; } //pm::msg("y"); }
      if( not game_map->GetTypeSafe(npos_x_, npos_y_, 0) ){ speed_x = 0; speed_y = 0;     return true; } //pm::msg("xy"); }
      return false;
    }

    bool CheckSolidCollision(const float & pos_x, const float & pos_y, float & speed_x, float & speed_y){
      if(tune_collision_speed){
        // check if already stick in solid
        if( not game_map->GetTypeSafe(pos_x, pos_y, 0) ){
          return CheckSolidCollisionSimple(pos_x, pos_y, speed_x, speed_y);
        }
        // check if speed too small
        float L = sqrt(pow(speed_x,2)+pow(speed_y,2));
        if(L < body_points_presision) {
          return CheckSolidCollisionSimple(pos_x, pos_y, speed_x, speed_y);
        }
        // ray tracer 
        float dx = body_points_presision / L * speed_x;
        float dy = body_points_presision / L * speed_y;
        float end_pos_x = pos_x + speed_x;
        float end_pos_y = pos_y + speed_y;
        float prev_pos_x = pos_x;
        float prev_pos_y = pos_y;
        int flag_x = 1;
        int flag_y = 1;
        bool flag_end = false;
        int i_max = 0;
        if(abs(speed_x) > abs(speed_y))   i_max = speed_x / dx+1;
        else                    i_max = speed_y / dy+1;
        for(int i = 0; i < i_max; ++i){
          npos_x_ = prev_pos_x + dx * flag_x;
          npos_y_ = prev_pos_y + dy * flag_y;
          // pm::msg("iter", i, "/", i_max, npos_x_, npos_y_, speed_x, speed_y);
          if( (speed_x > 0 and npos_x_ > end_pos_x) or (speed_x < 0 and npos_x_ < end_pos_x) ) npos_x_ = end_pos_x;
          if( (speed_y > 0 and npos_y_ > end_pos_y) or (speed_y < 0 and npos_y_ < end_pos_y) ) npos_y_ = end_pos_y;
          
          if( flag_x ) if( not game_map->GetTypeSafe(npos_x_, prev_pos_y, 0) ) { 
            flag_x = false; 
            npos_x_ = int(npos_x_) / tsize * tsize + tsize * (speed_x < 0) + 0.1 - 0.2 * (speed_x > 0);
          }
          if( flag_y ) if( not game_map->GetTypeSafe(prev_pos_x, npos_y_, 0) ) { 
            flag_y = false; 
            // pm::msg("tune", npos_y_, int(npos_y_) / tsize * tsize + tsize * (speed_y < 0));
            npos_y_ = int(npos_y_) / tsize * tsize + tsize * (speed_y < 0) + 0.1 - 0.2 * (speed_y > 0);
          }
          if( not game_map->GetTypeSafe(npos_x_, npos_y_, 0) ) { 
            flag_x = false; 
            flag_y = false; 
            flag_end = true; 
            // npos_x_ = int(npos_x_) / tsize * tsize + tsize * (speed_x < 0);
            // npos_y_ = int(npos_y_) / tsize * tsize + tsize * (speed_y < 0);
          }
          prev_pos_x = npos_x_;
          prev_pos_y = npos_y_;
        }
        if(flag_x and flag_y) return false;
        // no collision at previous step, so put speed up to the edge of solid tile
        // pm::msg("y:", prev_pos_y, npos_y_, speed_y, dy);
        speed_x = prev_pos_x - pos_x ;
        speed_y = prev_pos_y - pos_y ;
        return false;
      } else return CheckSolidCollisionSimple(pos_x, pos_y, speed_x, speed_y);
    }

    bool DUMMY_BOOL = false;
    std::pair<int,int> bkey_;
    pm::v2 *sv1_, *sv2_, *nsv1_, *nsv2_;
    int index_, type_;
    std::map< std::pair<int,int>, std::vector<int> > obj_grid;
    float pos_x_, pos_y_, npos_x_, npos_y_, speed_x_, speed_y_, L_, point_x_, point_y_;
    float size_x_, size_y_, ccos_, ssin_;
    float size_start_x_, size_end_x_, size_start_y_, size_end_y_;
    void Tick( vector<pmObject*> & objects, vector<pmParticle*> & particles, std::vector< std::pair<int, int> > & interacted_object_x_object_pairs, std::vector< std::pair<int, int> > & interacted_bullet_x_object_pairs ){
      pm::msg("Tick");
std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      // no bool * object_x_solids, int * object_tiles, int * bullet_tiles,  
      // - check if object is going to interact with solid tiles
      objects_= & objects;
      obj_grid.clear();
      int N_objects = objects.size();
      int N_particles = particles.size();
      pm::v2 * size_vectors = new pm::v2 [2 * N_objects];
      for(int i_2=0; i_2 < N_objects; ++i_2){
        pmObject* obj = objects[ i_2 ];
        ProcessObject(i_2, obj->pos.x, obj->pos.y, obj->speed.x, obj->speed.y, obj->size.x, obj->size.y, obj->angle, size_vectors + 2*i_2,  obj->interact_with_solid, obj->in_solid, obj->tiles );
      }
std::chrono::steady_clock::time_point end;
end = std::chrono::steady_clock::now();
std::cout << "Time difference 1= " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

      // - check if object is going to interact with another object
      for(int i = 0; i < N_objects; ++i){
        pmObject* obj = objects[ i ];

        sv1_ = & size_vectors[ 2*i   ];
        sv2_ = & size_vectors[ 2*i+1 ];

        for(int j = i+1; j < N_objects; ++j){
          pmObject* obj2 = objects[ j ];
          npos_x_ = obj->pos.x;
          npos_y_ = obj->pos.y;
          nsv1_ = & size_vectors[ 2*j   ];
          nsv2_ = & size_vectors[ 2*j+1 ];

          if( not CheckRectCollisions( obj->pos, *sv1_, *sv2_, obj2->pos, *nsv1_, *nsv2_) ) continue;
          //if( abs(pos_x - npos_x) > size_x + nsize_x or abs(pos_y - npos_y) > size_y + nsize_y ) continue;
          // => return ids of objects
          interacted_object_x_object_pairs.push_back( std::make_pair<int, int>(int(i), int(j)) );
        }
      }
end = std::chrono::steady_clock::now();
std::cout << "Time difference 2= " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

      // - check bullet is about interation with solid tiles
      // - check if object is going to interact with bullets
      for(int i = 0, i_2 = 0, i_max = 2*N_particles; i < i_max; i+=2){
        pmParticle* par = particles[i];
        ProcessParticle(i_2, par->pos.x, par->pos.y, par->tiles, obj_grid, size_vectors, interacted_bullet_x_object_pairs);        
        i_2++;
      }
end = std::chrono::steady_clock::now();
std::cout << "Time difference 3= " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;


      // - check if objects is going to interact with lines ... (such as laser, lighting traps etc)
      // TODO

      delete [] size_vectors;
    }
    
    void Tick(const int & N_objects, float * object_positions, float * object_speeds, int * object_tiles, float * object_sizes_xy, float * object_angles, bool * object_x_solids,
              const int & N_bullets, float * bullet_positions, int * bullet_tiles, 
              std::vector< std::pair<int, int> > & interacted_object_x_object_pairs, std::vector< std::pair<int, int> > & interacted_bullet_x_object_pairs){
      // every Tick step we will care about:
      // - check if objects is going to interact with solid tiles
      //   => return the maximum available speed
      //   => return list of types of the tiles we interact with
      // - check if object is going to interact with another object
      //   => return ids of objects
      // - check bullets is about interation with solid tiles
      //   => return such information too
      // - check if object is going to interact with bullets
      //   => return list of [id_bullet, (id_objects, ... , )]
      object_positions_ = object_positions;

      // - check if object is going to interact with solid tiles
      obj_grid.clear();
      pm::v2 * size_vectors = new pm::v2 [2 * N_objects];
      for(int i = 0, i_1 = 1, i_2=0, i_max = 2*N_objects; i < i_max; i+=2){
        ProcessObject(i_2, object_positions[ i ], object_positions[ i_1 ], object_speeds[ i ], object_speeds[ i_1 ], object_sizes_xy[ i ], object_sizes_xy[ i_1 ], object_angles[i_2], size_vectors + i, object_x_solids[i_2], DUMMY_BOOL, object_tiles + msize_layers*i_2 );
        i_1+=2;
        i_2++;
      }

      // - check if object is going to interact with another object
      for(int i = 0, i_max = 2*N_objects; i < i_max; i+=2){
        pos_x_ = object_positions[ i   ];
        pos_y_ = object_positions[ i+1 ];

        sv1_ = & size_vectors[ i   ];
        sv2_ = & size_vectors[ i+1 ];

        for(int j = i+2; j < i_max; j+=2){
          npos_x_ = object_positions[ j   ];
          npos_y_ = object_positions[ j+1 ];
          nsv1_ = & size_vectors[ j   ];
          nsv2_ = & size_vectors[ j+1 ];

          if( not CheckRectCollisions( pm::v2(pos_x_, pos_y_), *sv1_, *sv2_, pm::v2(npos_x_, npos_y_), *nsv1_, *nsv2_) ) continue;
          //if( abs(pos_x - npos_x) > size_x + nsize_x or abs(pos_y - npos_y) > size_y + nsize_y ) continue;
          // => return ids of objects
          interacted_object_x_object_pairs.push_back( std::make_pair<int, int>(i/2, j/2) );
        }
      }

      // - check bullet is about interation with solid tiles
      // - check if object is going to interact with bullets
      for(int i = 0, i_2 = 0, i_max = 2*N_bullets; i < i_max; i+=2){
        ProcessParticle(i_2, bullet_positions[ i ], bullet_positions[ i+1 ], bullet_tiles+msize_layers*i_2, obj_grid, size_vectors, interacted_bullet_x_object_pairs);        
        i_2++;
      }

      // - check if objects is going to interact with lines ... (such as laser, lighting traps etc)
      // TODO

      delete [] size_vectors;
    }


    void ProcessObject(const int & id, float & pos_x, float & pos_y, float & speed_x, float & speed_y, const float & size_x, const float & size_y, const float & angle, pm::v2 * size_vectors, bool & object_x_solid, bool & object_in_solid, int * object_tiles ){        
      ccos_ = cos( angle * pm::PI_180 );
      ssin_ = sin( angle * pm::PI_180 );

      speed_x_ = speed_x;
      speed_y_ = speed_y;

      size_vectors[ 0 ].x =   size_x * ccos_ + size_y * ssin_;
      size_vectors[ 0 ].y = - size_x * ssin_ + size_y * ccos_;
      size_vectors[ 1 ].x =   size_x * ccos_ - size_y * ssin_;
      size_vectors[ 1 ].y = - size_x * ssin_ - size_y * ccos_;

      sv1_ = & size_vectors[ 0 ];
      sv2_ = & size_vectors[ 1 ];
      object_in_solid = false;

      // by default map is filled with 0 - solid tiles
      if(check_solid_tiles_of_body_points){
        // check grid points in the 
        size_start_x_ = std::min(std::min(size_vectors[ 0 ].x, size_vectors[ 1 ].x), std::min(-size_vectors[ 0 ].x, -size_vectors[ 1 ].x));
        size_end_x_   = std::max(std::max(size_vectors[ 0 ].x, size_vectors[ 1 ].x), std::max(-size_vectors[ 0 ].x, -size_vectors[ 1 ].x));
        size_start_y_ = std::min(std::min(size_vectors[ 0 ].y, size_vectors[ 1 ].y), std::min(-size_vectors[ 0 ].y, -size_vectors[ 1 ].y));
        size_end_y_   = std::max(std::max(size_vectors[ 0 ].y, size_vectors[ 1 ].y), std::max(-size_vectors[ 0 ].y, -size_vectors[ 1 ].y));

        // explicitly check end edges
        for(float point_npos_x = size_start_x_; point_npos_x <= size_end_x_; point_npos_x += body_points_presision ){
          if(object_in_solid) goto in_solid;
          object_in_solid = CheckSolidCollision( pos_x + point_npos_x, pos_y + size_start_y_, speed_x, speed_y );
          object_in_solid = CheckSolidCollision( pos_x + point_npos_x, pos_y + size_end_y_, speed_x, speed_y );
        }
        size_start_y_ += body_points_presision;
        size_end_y_   -= body_points_presision;
        for(float point_npos_y = size_start_y_; point_npos_y <= size_end_y_; point_npos_y += body_points_presision ){
          if(object_in_solid) goto in_solid;
          object_in_solid = CheckSolidCollision( pos_x + size_start_x_, pos_y + point_npos_y, speed_x, speed_y );
          object_in_solid = CheckSolidCollision( pos_x + size_end_x_,   pos_y + point_npos_y, speed_x, speed_y );
        }
        if(check_internal_points and object_in_solid){
          size_start_x_ += body_points_presision;
          size_end_x_   -= body_points_presision;
          for(float point_npos_x = size_start_x_; point_npos_x <= size_end_x_; point_npos_x += body_points_presision )
            for(float point_npos_y = size_start_y_; point_npos_y <= size_end_y_; point_npos_y += body_points_presision ){
              if(object_in_solid) goto in_solid;
              object_in_solid = CheckSolidCollision( pos_x + point_npos_x, pos_y + point_npos_y, speed_x, speed_y );
            }
        }
      } else {
        // check just center
        object_in_solid = CheckSolidCollision( pos_x, pos_y, speed_x, speed_y );
      }

      in_solid:
      pos_x = pos_x + speed_x;
      pos_y = pos_y + speed_y;

      if(std::abs(speed_x_ - speed_x) > 0.00001f or std::abs(speed_y_ - speed_y) > 0.00001f) 
        object_x_solid = true; 
      else object_x_solid = false; 

      // => return the maximum available speed
      // create a grid also
      //obj_grid[std::make_pair<int, int>(int(npos_x - sv1->x)/bullet_grid_size, int(npos_y - sv1->y)/bullet_grid_size)].push_back( i );
      //obj_grid[std::make_pair<int, int>(int(npos_x + sv1->x)/bullet_grid_size, int(npos_y + sv1->y)/bullet_grid_size)].push_back( i );
      //obj_grid[std::make_pair<int, int>(int(npos_x - sv2->x)/bullet_grid_size, int(npos_y - sv2->y)/bullet_grid_size)].push_back( i );
      //obj_grid[std::make_pair<int, int>(int(npos_x + sv2->x)/bullet_grid_size, int(npos_y + sv2->y)/bullet_grid_size)].push_back( i );
      //obj_grid[std::make_pair<int, int>(int(npos_x)/bullet_grid_size, int(npos_y)/bullet_grid_size)].push_back( i );

      L_ = sqrt(size_x * size_x + size_y*size_y);
      for(int sx = 0; sx < L_; sx += bullet_grid_size)
        for(int sy = 0; sy < L_; sy += bullet_grid_size){
          obj_grid[std::make_pair<int, int>(int(sx + pos_x)/bullet_grid_size, int(sy + pos_y)/bullet_grid_size)].push_back( id );
          if(sx) obj_grid[std::make_pair<int, int>(int(pos_x - sx)/bullet_grid_size, int(sy + pos_y)/bullet_grid_size)].push_back( id );
          if(sy) obj_grid[std::make_pair<int, int>(int(sx + pos_x)/bullet_grid_size, int(pos_y - sy)/bullet_grid_size)].push_back( id );
          if(sx and sy) obj_grid[std::make_pair<int, int>(int(pos_x - sx)/bullet_grid_size, int(pos_y - sy)/bullet_grid_size)].push_back( id );
        }
      obj_grid[std::make_pair<int, int>(int(pos_x - L_)/bullet_grid_size, int(pos_y - L_)/bullet_grid_size)].push_back( id );
      obj_grid[std::make_pair<int, int>(int(pos_x + L_)/bullet_grid_size, int(pos_y + L_)/bullet_grid_size)].push_back( id );
      obj_grid[std::make_pair<int, int>(int(pos_x - L_)/bullet_grid_size, int(pos_y - L_)/bullet_grid_size)].push_back( id );
      obj_grid[std::make_pair<int, int>(int(pos_x + L_)/bullet_grid_size, int(pos_y + L_)/bullet_grid_size)].push_back( id );

      // => return list of types of the tiles we interact with
      index_ = game_map->GetMapIndexSafe(pos_x, pos_y);
      for(int type = 0; type < msize_layers; type++)
        object_tiles[ type ] = mdata[index_ + type];
    }

    void ProcessParticle(const int & id, const float & pos_x, const float & pos_y, int * bullet_tiles, const std::map< std::pair<int,int>, std::vector<int> > & obj_grid, pm::v2 * size_vectors, std::vector< std::pair<int, int> > & interacted_bullet_x_object_pairs){
        //   => return list of types of the tiles we interact with
        index_ = game_map->GetMapIndexSafe(pos_x, pos_y);
        for(int type = 0; type < msize_layers; type++)
          bullet_tiles[ type ] = mdata[index_ + type]; // array is shifted before
         
        // hardcode solution for testing purpose
        /*
        printf("process bullet %d %f %f \n", i_2, pos_x, pos_y);
        for(int j = 0, j_2=-1, j_max = 2*N_objects; j < j_max; j+=2){
          j_2++;
          if( pow(pos_x - object_positions[ j   ], 2) + pow(pos_y - object_positions[ j+1 ], 2) > pow(object_r_sizes[j_2], 2) ) continue; 
          // interacted_bullet_x_object_pairs.push_back( std::make_pair<int, int>(i_2, j_2) );
          printf("I. add %d %d pairs ... \n", i_2, j_2);
        }
        */
        
        bkey_ = std::make_pair<int, int>(int(pos_x)/bullet_grid_size, int(pos_y)/bullet_grid_size);
        if( obj_grid.find(bkey_) != obj_grid.end() ){
          const std::vector<int> & ids = obj_grid.find(bkey_)->second;
          for(unsigned int id_index = 0; id_index != ids.size(); id_index++){
            int obj_id = ids[ id_index ];

            // check 4 points for rect solving equation v2(point_x - pos_x, point_y - pos_y) = a * (sv1) + b * (sv2), |a| < 1, |b| < 1, |a| + |b| < 1
            // |y * x1 - x * y1| < | y2 * x1 - x2 * y1 |
            // |y * x2 - x * y2| < | y2 * x1 - x2 * y1 |
            sv1_ = & size_vectors[ obj_id   ];
            sv2_ = & size_vectors[ obj_id+1 ];
            // printf("sv1, sv2 %f %f %f %f \n", sv1->x, sv1->y, sv2->x, sv2->y);
            this->get_object_pos(this, obj_id, pos_x_, pos_y_);
            
            point_x_ = pos_x - pos_x_; // object_positions[obj_id]  ;
            point_y_ = pos_y - pos_y_; // object_positions[obj_id+1];
            // printf("point_x, point_y %f %f\n", point_x, point_y);
            // printf("A, B %f %f\n", abs(point_y * sv1->x - point_x * sv1->y) / abs( sv2->y * sv1->x - sv2->x * sv1->y ), abs(point_y * sv2->x - point_x * sv2->y) / abs( sv2->y * sv1->x - sv2->x * sv1->y ));
            if(abs(point_y_ * sv1_->x - point_x_ * sv1_->y) + abs(point_y_ * sv2_->x - point_x_ * sv2_->y) > abs( sv2_->y * sv1_->x - sv2_->x * sv1_->y )) continue;

            // if( abs(pos_x - object_positions[obj_id]) > object_sizes_xy[obj_id] or abs(pos_y - object_positions[obj_id+1]) > object_sizes_xy[obj_id+1] ) continue; // bullet is a point
            // => return ids of objects
            interacted_bullet_x_object_pairs.push_back( std::make_pair<int, int>(int(id), int(obj_id/2)) );
          }
        }
    }

#ifdef BUILD_WITH_PYTHON
    PyObject* FillPyListFromPairVector(const std::vector< std::pair<int, int> > & vec){
      PyObject* answer = PyList_New( 2*vec.size() );
      for(int i = 0, imax = vec.size(), list_index=0; i<imax; ++i){
        PyList_SetItem( answer, list_index,   PyInt_FromLong( vec[i].first  ) );
        PyList_SetItem( answer, list_index+1, PyInt_FromLong( vec[i].second ) );
        list_index+=2;
      }
      return answer;
    }
#endif

    int bullet_grid_size;
  };

#ifdef BUILD_WITH_PYTHON
  extern "C" {
    void * pmTiledMap_new( int map_size_x, int map_size_y, int map_size_layers, int tile_size, int * map_data ) {
      DPRINT( std::cout << "pmTiledMap_new()" << std::endl );
      pmTiledMap * answer = new pmTiledMap( map_size_x, map_size_y, map_size_layers, tile_size, map_data );
      DPRINT( std::cout << " ... " << answer->tsize << std::endl );
      return answer;
    }
    
    void pmTiledMap_PrintMap(void * game_map, int layer){
      DPRINT( std::cout << "pmPhysic_PrintMap()" << std::endl );
      pmTiledMap * game_map_ = static_cast<pmTiledMap*>(game_map);
      game_map_->PrintMap(layer);
    }
    
    void * pmPhysic_new( void * game_map ) {
      DPRINT( std::cout << "pmPhysic_new()" << std::endl );
      pmTiledMap * game_map_ = static_cast<pmTiledMap*>(game_map);
      pmPhysic * answer = new pmPhysic( game_map_ );
      DPRINT( std::cout << " ... " << answer->tsize << std::endl );
      return answer;
    }

    void pmPhysic_del(void * physic){
      DPRINT( std::cout << "pmPhysic_del()" << std::endl );
      pmPhysic* physic_ = static_cast<pmPhysic*>(physic);
      delete physic_;
    }

    PyObject* pmPhysic_Tick(void * physic, int N_objects, float * object_positions, float * object_speeds, int * object_tiles, float * object_sizes_xy, float * object_angles, bool * object_x_solids, int N_bullets, float * bullet_positions, int * bullet_tiles) {
      DPRINT( std::cout << "pmPhysic_PyTick()" << std::endl );
      pmPhysic* physic_ = static_cast<pmPhysic*>(physic);

      std::vector< std::pair<int, int> > interacted_object_x_object_pairs;
      std::vector< std::pair<int, int> > interacted_bullet_x_object_pairs;
      physic_->Tick( N_objects, object_positions, object_speeds, object_tiles, object_sizes_xy, object_angles, object_x_solids, N_bullets, bullet_positions, bullet_tiles, interacted_object_x_object_pairs, interacted_bullet_x_object_pairs);

      PyGILState_STATE gstate = PyGILState_Ensure();
      PyObject* answer = PyList_New(2);
      PyObject* interacted_object_x_object_pairs_PyList = physic_->FillPyListFromPairVector( interacted_object_x_object_pairs );
      PyObject* interacted_bullet_x_object_pairs_PyList = physic_->FillPyListFromPairVector( interacted_bullet_x_object_pairs );

      PyList_SetItem( answer, 0, interacted_object_x_object_pairs_PyList);
      PyList_SetItem( answer, 1, interacted_bullet_x_object_pairs_PyList);
      PyGILState_Release(gstate);
      return answer;
    }
  }

  extern "C" int check_pmphysic(){ return 0; }
#endif
#endif


 
