#ifndef PMLIB_OTHER_HH
#define PMLIB_OTHER_HH 1

#ifdef NO_CERN_ROOT
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
  template<class T, class KEY>
  class NaryNode {
    private:
    NaryNode* father;
    size_t priority;
    std::map<KEY, NaryNode*> childs;
    T* data = nullptr;
    
    public:
    void SetData(T* data){ this->data = data; }
    T* GetData(T* data){ return data; }
    
    KEY GetKey(){
      if( data == nullptr ) throw std::logic_error("data nullptr");
      return data->GetKey();
    }
    
    /// one o zero fathers
    void SetFather( NaryNode* father ){ this->father = father; } 
    NaryNode* GetFather(){ return this->father; }
    
    /// many or zero chfatherildrens
    int AddChild( NaryNode* child ) { 
      KEY key = child->GetKey();
      if(childs.count( key )){
        return PM_ERROR_DUPLICATE;
      }
      childs[ key ] = child;
      return PM_SUCCESS; 
    }
    
    int RemoveChild( NaryNode* child ) {
      KEY key = child->GetKey();
      auto find = childs.find( key );
      if( find == childs.end() ) return PM_ERROR_MAP_FIND;
      childs.remove( find );
      return PM_SUCCESS;
    }
    
    NaryNode* GetChild( KEY key ) {
      auto find = childs.find( key );
      if( find == childs.end() ) return nullptr;
      return find->second;
    }

    /// order of childres with same fathers or no fathers
    void SetPriority( size_t priority ) { this->priority = priority; }
    size_t GetPriority() { return priority; }
    
    ///
    void Print(){
      printf("NaryNode(%p): father = %p, N childs = %d, priority = %d, data = %p\n", this, father, childs.size(), priority, data);
    }
  };
  
  class NaryTree {
    
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
