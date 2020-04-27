
///////////////////////////////////////////////////////////////////////////////
//  Author      :     P.S. Mandrik, IHEP
//  Date        :     16/04/20
//  Last Update :     16/04/20
//  Version     :     1.0
///////////////////////////////////////////////////////////////////////////// 

#ifndef pmlib_path_search
#define pmlib_path_search 1

// Astar implementation at the moment and related classes to use

#include <vector>
#include <map>

namespace pm {

  // structute to be used for path finding shold have following metods
  class GraphPoint {
    public:
    virtual int GetNumberOfConnections(void) const;
    virtual int GetConnectionAt(const int & index) const;
    virtual float RealDistance(const GraphPoint & other) const;
    virtual float EmpiricDistance(const GraphPoint & other) const;
  };

  struct AstarData {
    AstarData(AstarData * father_ = NULL, int id_ = -1, float dte = -1.f, float dts = -1.f){
      father           = father_;
      id               = id_;
      distance_to_end   = dte;
      distance_to_start = dts;
    }

    AstarData * father;
    int id, distance_to_end, distance_to_start;
  };

  template<typename GraphType> 
  void Astar(const std::vector<GraphType> & graph_points, const int & start_index, const int & end_index, std::vector<int> & answer_path ){
    if(start_index == end_index){
      answer_path.push_back( start_index );
      return;
    }
    if( start_index >= (int)graph_points.size() or end_index >= (int)graph_points.size()){
      printf("pm::Astar(): wrong initial indexes s=%d e=%d for graph with size=%d\n", start_index, end_index, (int)graph_points.size());
      return;
    }
    
    const GraphType & point_end = graph_points[end_index];
    
    std::map<int, AstarData*> open_list; 
    std::map<int, AstarData*> close_list; 
    AstarData * it_end;
    open_list[ start_index ] = new AstarData( NULL, -1, point_end.EmpiricDistance( graph_points[ start_index ] ), 0 );
    
    while( true ){
      if( not open_list.size() ) goto exit_mark;
      std::map<int, AstarData*>::iterator it_min = open_list.begin();
      float minimal_path = it_min->second->distance_to_end + it_min->second->distance_to_start;
      for(std::map<int, AstarData*>::iterator it = open_list.begin(); it != open_list.end(); ++it){
        if( it->second->distance_to_end + it->second->distance_to_start >= minimal_path ) continue;
        minimal_path = it->second->distance_to_end + it->second->distance_to_start;
        it_min = it;
      }
      
      int checked_id = it_min->first;
      AstarData * checked_point = it_min->second;
      close_list[ it_min->first ] = checked_point;
      open_list.erase( it_min );
      
      const GraphType & graph_point = graph_points.at( checked_id );
      for( unsigned int i = 0; i < graph_point.GetNumberOfConnections(); i++ ){
        const int & id = graph_point.GetConnectionAt(i);
        std::map<int, AstarData*>::iterator it_closed = close_list.find(id);
        if( it_closed != close_list.end() ) continue;
        std::map<int, AstarData*>::iterator it_prev = open_list.find(id);
        float distance_to_start = checked_point->distance_to_start + graph_point.RealDistance( graph_points.at( id ) );
        if( it_prev != open_list.end() ){
          if( it_prev->second->distance_to_start > distance_to_start ){
              it_prev->second->father = checked_point;
              it_prev->second->distance_to_start = distance_to_start;
            }
        } else{
          open_list[ id ] = new AstarData( checked_point, id, point_end.EmpiricDistance(graph_points.at(id)), distance_to_start );
        }
        
        if(id == end_index) goto find_end_mark;
      }
    }
    
    find_end_mark : 
    it_end = open_list.find( end_index )->second;
    while( it_end->father != NULL ){
      answer_path.push_back( it_end->id );
      it_end = it_end->father;
      if( answer_path.size() > 9999 ) break;
    }
    
    exit_mark :
    for(std::map<int, AstarData*>::iterator it = open_list.begin(); it != open_list.end(); it++)
      delete it->second;
    for(std::map<int, AstarData*>::iterator it = close_list.begin(); it != close_list.end(); it++)
      delete it->second;
  }
  
}

#endif
