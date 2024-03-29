#ifndef PMLIB_OTHER_HH
#define PMLIB_OTHER_HH 1

#ifndef CERN_ROOT
  #include <vector>
  #include <map>
  #include <string>
  #include <algorithm>
  
  #include "pmlib_msg.hh"
#endif

namespace pm {

  //=================================== Timer ====================================================================
  class Timer{
    public:
    Timer(){};
    Timer(int max_time) : mtime(max_time) { dtime = 1./float(mtime); Reset(); }
    inline void Set(int time){itime = time; ftime = itime * dtime;}
    inline void End(){ Set(mtime); }
    inline void Reset(){ Set(0); }
    inline void ResetBack(){ Set(mtime-1); }

    bool Tick(int val=1){ 
      ntime = itime+val;
      if(ntime > mtime){ Reset(); return false; }
      if(ntime < 0){ ResetBack(); return false; }
      itime = ntime; ftime += dtime*val;
      return true; 
    }

    bool TickOneShoot(int val=1){
      ntime = itime+val;
      if(ntime > mtime){ return false; }
      if(ntime < 0){ return false; }
      itime = ntime; ftime += dtime*val;
      return true; 
    }

    int mtime, itime, ntime;
    float dtime, ftime;
  };

  //=================================== STRING MANIPULATION =============================================================
  static void ltrim(std::string &s) {
    if(not s.size()) return;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
  }

  static void rtrim(std::string &s) {
    if(not s.size()) return;
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
  }

  std::string strip(std::string s) {
    if(not s.size()) return s;
    ltrim(s);
    rtrim(s);
    return s;
  }

  void split_string(std::string str, std::vector<std::string> & answer, const std::string & sep = " "){
    size_t sep_size = sep.size();
    for(size_t i = 0; i + sep_size < str.size(); i++){
      if( sep == str.substr(i, sep_size) ){
        answer.push_back( str.substr(0, i) ); 
        str = str.substr(i + sep_size, str.size() - i - sep_size);
        i = 0;
      }
    }
    answer.push_back(str);
  }

  void split_string_strip(std::string str, std::vector<std::string> & answer, const std::string & sep = " "){
    size_t sep_size = sep.size();
    for(size_t i = 0; i + sep_size < str.size(); i++){
      if( sep == str.substr(i, sep_size) ){
        answer.push_back( str.substr(0, i) ); 
        str = str.substr(i + sep_size, str.size() - i - sep_size);
        i = 0;
      }
    }
    answer.push_back( strip(str) );
  }
  
  void replace_all(std::string & str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
  }
  
  void replace_all_map(std::string & path, std::map<std::string,std::string> dictionary){
    if(not path.size()) return;
    for(auto it = dictionary.begin(); it != dictionary.end(); ++it){
      replace_all( path, it->first, it->second );
    }
  }

  void parce_string_function(std::string str, std::string & fname, std::vector<std::string> & fargs, const std::string & bracket="(", const std::string & fsep=","){
    fname = str;
    fargs.clear();
    auto found_s = str.find( bracket );
    auto found_e = str.find( bracket );
    if (found_s==std::string::npos or found_e==std::string::npos) return;
    if (found_s > found_e) return;
    fname = str.substr(0, found_s);
    str   = str.substr(found_s+1, found_e-1);
    split_string_strip(str, fargs, fsep);
  }

  std::string add_leading_zeros(const size_t & index, const size_t & N_symbols){
    std::string frame_index_str = std::to_string( index );
    if( N_symbols < frame_index_str.size() ) frame_index_str = frame_index_str.substr(0, index);
    return std::string(N_symbols - frame_index_str.length(), '0') + frame_index_str;
  }
  
  std::string normalize_lenght(std::string s, const size_t & lenght, const std::string & symbol){ // add 'symbol'
    while(s.size() < lenght) s = symbol + s;
    return s;
  }

  bool bool_from_string(const std::string & val, const bool & def_answer){
    if(val=="false" or val=="False" or val=="False" or val=="NULL" or val=="null" or val=="nullptr" or val=="0") return false;
    // if(val=="true" or val=="True") return true;
    if( val.size() ) return true;
    return def_answer;
  }

  std::string upper_string(std::string str){
	  for(int i=0; str[i]!='\0'; i++){
		  if (str[i] >= 'a' && str[i] <= 'z')
			  str[i] = str[i] - 32; 
	  }
	  return str;
  }
  
  std::string wrap_into_width(const std::string & data, const int box_w, const int letter_w){
    std::string answer;
    std::vector<std::string> data_lines;
    // std::cout << data << std::endl;
    split_string(data, data_lines, "\n");
    for(size_t j = 0; j < data_lines.size(); ++j){
      std::vector<std::string> data_v;
      split_string(data_lines[j], data_v);
     
      size_t max_letters = box_w / letter_w;
      size_t cur_letters = 0;
      for(size_t i = 0; i < data_v.size(); ++i){
        std::string str = data_v[i];
        
        // std::cout << i << " <" << str << ">" << std::endl;
        
        while(str.size() > max_letters){
          answer += str.substr(0, max_letters-1);
          answer += "-";
          answer += "\n";
          str = str.substr(max_letters-1, str.size()-max_letters);
        }
        
        if( cur_letters + str.size() > max_letters ){ // start new line
          answer += "\n";
          cur_letters = 0;
        }
        
        answer += str;
        cur_letters += str.size();
        
        if( cur_letters + 1 < max_letters ) {
          answer += " ";
          cur_letters += 1;
        }
      }
      
      if(j+1 < data_lines.size()){
        if( answer[answer.size()-1] != '\n') answer += "\n";
      }
    }
    
    return answer;
  }
  
  //=================================== MAP =========================================================
  template <typename K, typename V> V map_get(const  std::map <K,V> & m, const K & key, const V & defval ) {
    typename std::map<K,V>::const_iterator it = m.find( key );
    if ( it == m.end() ) return defval;
    return it->second;
  }
  
  //=================================== TREE =========================================================
  template<class DATA, class LABEL>
  class NaryNode {
    private:
    NaryNode<DATA,LABEL>* father = nullptr;
    int priority;
    std::map<LABEL, NaryNode*> childs;
    DATA* data = nullptr;
    LABEL label;
    
    public:
    void SetData(DATA* data){ this->data = data; }
    DATA* GetData() { return data; }
    
    /// unic label
    LABEL GetLabel(){ return label; }
    void SetLabel(LABEL & label){ this->label = label; }
    
    /// one o zero fathers
    void SetFather( NaryNode* father ){ this->father = father; } 
    NaryNode* GetFather(){ return this->father; }
    
    /// many or zero chfatherildrens
    int AddChild( NaryNode* child ) { 
      LABEL label = child->GetLabel();
      if(childs.count( label )){
        return PM_ERROR_DUPLICATE;
      }
      childs[ label ] = child;
      return PM_SUCCESS; 
    }
    
    int RemoveChild( NaryNode* child ) {
      LABEL label = child->GetLabel();
      auto find = childs.find( label );
      if( find == childs.end() ) return PM_ERROR_MAP_FIND;
      childs.erase( find );
      return PM_SUCCESS;
    }
    
    NaryNode* GetChild( LABEL label ) {
      auto find = childs.find( label );
      if( find == childs.end() ) return nullptr;
      return find->second;
    }
    
    std::vector<LABEL> GetChildLabels() {
      std::vector<LABEL> answer;
      for(auto const& item: childs)
        answer.push_back(item.first);
      return answer;
    }
    
    std::vector<NaryNode*> GetChilds() {
      std::vector<NaryNode*> answer;
      for(auto const& item: childs){
        answer.push_back(item.second);
      }
      std::sort(answer.begin(), answer.end(), 
        [](const NaryNode<DATA,LABEL> *a, const NaryNode<DATA,LABEL> *b){ return a->priority < b->priority; }
      );
      return answer;
    }

    /// order of childres with same fathers or no fathers
    void SetPriority( int priority ) { this->priority = priority; }
    int GetPriority() const { return priority; }
    
    ///
    void Print() const {
      printf("NaryNode(%p): father = %p, N childs = %ld, priority = %d, data = %p\n", this, father, childs.size(), priority, data);
    }
  };
  
  template<class DATA, class LABEL>
  class NaryTree {
    private:
    std::map<LABEL, NaryNode<DATA,LABEL>* > nodes;
    int removeStrategy = 0;
    
    public:
    
    void SetRemoveStrategy(int val){ removeStrategy = val; }
    
    NaryNode<DATA,LABEL>* GetNode(LABEL label){
      auto find = nodes.find( label );
      if( find == nodes.end() ) return nullptr;
      return find->second;
    }
    
    int AddNode(LABEL label){
      if( nodes.count( label )  ){
        return PM_ERROR_DUPLICATE;
      }
      NaryNode<DATA,LABEL> * node = new NaryNode<DATA,LABEL>();
      node->SetLabel( label );
      nodes[ label ] = node;
      return PM_SUCCESS;
    }
    
    int RemoveNode(LABEL label){
      // remove from nodes
      auto find = nodes.find( label );
      if( find == nodes.end() ) return PM_ERROR_MAP_FIND;
      nodes.erase( find );
      
      // remove from childs & fathers
      auto node = find->second;
      auto father = node->GetFather();
      if( father ){
        father->RemoveChild( node );
      }
      auto childs = node->GetChilds();
      for(auto child : childs){
        if( removeStrategy == 1 && father ){
          child->SetFather( father );
          father->AddChild( child );
        }
        else 
          child->SetFather( nullptr );
      }
      
      delete node;
      return PM_SUCCESS;
    }
    
    int SetNodeFather(LABEL label, LABEL flabel){
      if( label == flabel ) return PM_ERROR;
      auto node = GetNode( label );
      if( node == nullptr ) return PM_ERROR_MAP_FIND;
      auto fnode = GetNode( flabel );
      if( fnode == nullptr ) return PM_ERROR_MAP_FIND;
      
      auto oldFather = node->GetFather();
      if( oldFather != nullptr ){
        int err = oldFather->RemoveChild( node );
        if( err ) return err;
      }
      
      node->SetFather( fnode );
      fnode->AddChild( node );
      return PM_SUCCESS;
    }
    
    int RemoveNodeFather(LABEL label){
      auto node = GetNode( label );
      if( node == nullptr ) return PM_ERROR_MAP_FIND;
      auto fnode = node->GetFather();
      if( fnode == nullptr ) return PM_SUCCESS;
      int err = fnode->RemoveChild( node );
      if( err ) return err;
      node->SetFather( nullptr );
      return PM_SUCCESS;
    }
    
    int SetNodeData(LABEL label, DATA * data){
      auto node = GetNode( label );
      if( node == nullptr ) return PM_ERROR_MAP_FIND;
      node->SetData( data );
      return PM_SUCCESS;
    }
    
    int SetNodePriority(LABEL label, int priority){
      auto node = GetNode( label );
      if( node == nullptr ) return PM_ERROR_MAP_FIND;
      node->SetPriority( priority );
      return PM_SUCCESS;
    }
    
    std::vector<NaryNode<DATA,LABEL>*> GetHeads(){
      std::vector<NaryNode<DATA,LABEL>*> heads;
      
      for(auto const& x : nodes){
        auto node = x.second;
        if( node->GetFather() != nullptr ) continue;
        heads.push_back( node );
      }
      
      std::sort(heads.begin(), heads.end(), 
        [](const NaryNode<DATA,LABEL> *a, const NaryNode<DATA,LABEL> *b){ 
          return a->GetPriority() < b->GetPriority(); 
          });
          
      return heads;
    }
    
    void GetNodeListRec( NaryNode<DATA,LABEL>* node, std::vector<NaryNode<DATA,LABEL>*> *answer ){
      auto childs = node->GetChilds();
      for(auto child : childs){
        GetNodeListRec(child, answer);
      }
      answer->push_back( node );
    }
    
    std::vector<NaryNode<DATA,LABEL>*> GetNodeList(){
      std::vector<NaryNode<DATA,LABEL>*> answer;
      std::vector<NaryNode<DATA,LABEL>*> heads = GetHeads();
      for(auto head : heads){
        GetNodeListRec(head, &answer);
      }
      return answer;
    }
    
    std::vector<DATA*> GetDataList(){
      std::vector<DATA*> answer;
      auto nodes = GetNodeList();
      for(auto node : nodes){
        answer.push_back( node->GetData() );
      }
      return answer;
    }
  };
  
  //=================================== OS, FOLDERS MANIPULATION =========================================================
#ifndef NO_CERN_ROOT
  void get_all_files_in_folder(std::string input_folder, std::string pattern, std::vector<std::string> & answer){
    regex reg_incl( pattern );
    TSystemDirectory dir(input_folder.c_str(), input_folder.c_str()); 
    TList *files = dir.GetListOfFiles();
    if (not files) return;
    
    TSystemFile *file; 
    TString fname; 
    TIter next(files); 
    while ((file=(TSystemFile*)next())) { 
      fname = file->GetName(); 
      string name = fname.Data();
      if( !regex_match(name, reg_incl) ) continue;
      answer.push_back( name );
    }
  }
#endif
}
  
#endif
