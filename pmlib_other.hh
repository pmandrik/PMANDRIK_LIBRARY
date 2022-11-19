#ifndef PMLIB_OTHER_HH
#define PMLIB_OTHER_HH 1

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
      if(itime >= mtime){ Reset(); return true; }
      if(itime < 0){ ResetBack(); return true; }
      itime += val; ftime += dtime*val;
      return false; 
    }

    bool TickIf(int val=1){
      if(itime >= mtime){ return false; }
      if(itime < 0){ return false; }
      itime += val; ftime += dtime*val;
      return true; 
    }

    int mtime, itime;
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
    int sep_size = sep.size();
    for(int i = 0; i + sep_size < str.size(); i++){
      if( sep == str.substr(i, sep_size) ){
        answer.push_back( str.substr(0, i) ); 
        str = str.substr(i + sep_size, str.size() - i - sep_size);
        i = 0;
      }
    }
    answer.push_back(str);
  }

  void split_string_strip(std::string str, std::vector<std::string> & answer, const std::string & sep = " "){
    int sep_size = sep.size();
    for(int i = 0; i + sep_size < str.size(); i++){
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

  std::string add_leading_zeros(const int & index, const int & N_symbols){
    std::string frame_index_str = std::to_string( index );
    if( N_symbols < frame_index_str.size() ) frame_index_str = frame_index_str.substr(0, index);
    return std::string(N_symbols - frame_index_str.length(), '0') + frame_index_str;
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
  
  //=================================== MAP =========================================================
  template <typename K, typename V> V map_get(const  std::map <K,V> & m, const K & key, const V & defval ) {
    typename std::map<K,V>::const_iterator it = m.find( key );
    if ( it == m.end() ) return defval;
    return it->second;
  }
  
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
