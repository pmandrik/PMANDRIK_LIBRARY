#ifndef PMLIB_OTHER_HH
#define PMLIB_OTHER_HH 1

namespace pm {

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
  
  void replace_all_map(string & path, map<string,string> dictionary){
    if(not path.size()) return;
    for(auto it = dictionary.begin(); it != dictionary.end(); ++it){
      replace_all( path, it->first, it->second );
    }
  }

  void parce_string_function(string str, string & fname, vector<string> & fargs, const string & bracket="(", const string & fsep=","){
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
