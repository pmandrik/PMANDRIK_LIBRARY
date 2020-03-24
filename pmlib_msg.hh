///////////////////////////////////////////////////////////////////////////////
//  Author      :     P.S. Mandrik, IHEP
//  Date        :     01/03/20
//  Last Update :     01/03/20
//  Version     :     1.0
///////////////////////////////////////////////////////////////////////////// 

#ifndef PMLIB_MSG_HH
#define PMLIB_MSG_HH 1

#include<iostream> 
#include<string>
#include<vector>

namespace pm {
  // ======= PART I. msg variardic templates ====================================================================
  // ======= msg ====================================================================
  void msg(){ std::cout << "\n" << std::endl; };
  template<typename T> void msg(T t) { std::cout << t << std::endl; }
  template<typename T, typename... Args> void msg(T t, Args... args){
    std::cout << t << " ";
    msg(args...);
  }

  void msg_nll(){ std::cout << std::endl; };
  template<typename T> void msg_nll(T t) { std::cout << t << " "; }
  template<typename T, typename... Args> void msg_nll(T t, Args... args){
    std::cout << t << " ";
    msg_nll(args...);
  }

  // ======= msg_sep ====================================================================
  template<typename T> void msg_sep(const std::string & separator, const std::string & last_symbol, T t){ std::cout << t << last_symbol; };
  template<typename T, typename... Args> void msg_sep(const std::string & separator, const std::string & last_symbol, T t, Args... args){
    std::cout << t << separator;
    msg_sep(separator, last_symbol, args...);
  }

  // ======= msg_err ====================================================================
  void msg_err(){ std::cerr << "\n" << std::endl; };
  template<typename T> void msg_err(const T & t) { std::cerr << t << std::endl; }
  template<typename T, typename... Args> void msg_err(const T & t, Args... args){
    std::cerr << t << " ";
    msg_err(args...);
  }

  void msg_err_nll(){ std::cerr << "\n" << std::endl; };
  template<typename T> void msg_err_nll(T t) { std::cerr << t << std::endl; }
  template<typename T, typename... Args> void msg_err_nll(T t, Args... args){
    std::cerr << t << " ";
    msg_err_nll(args...);
  }

  // ======= PART II. msg variardic templates ====================================================================
  // ======= error macros  ==================================================================== 
  enum verbose{
    SILENCE=0,
    ERROR,
    WARNING,
    INFO,
    DEBUG=4,
    VERBOSE
  };

  #define MSG_ERROR(...) if(verbose_lvl >= pmlib::verbose::ERROR)   pmlib::msg_err(__VA_ARGS__)
  #define MSG_WARNING(...) if(verbose_lvl >= pmlib::verbose::WARNING) pmlib::msg_err(__VA_ARGS__)
  #define MSG_INFO(...) if(verbose_lvl >= pmlib::verbose::INFO)    pmlib::msg(__VA_ARGS__)
  #define MSG_DEBUG(...) if(verbose_lvl >= pmlib::verbose::DEBUG)   pmlib::msg(__VA_ARGS__)
  #define MSG_VERBOSE(...) if(verbose_lvl >= pmlib::verbose::VERBOSE) pmlib::msg(__VA_ARGS__)

  #define MSG_ERROR_NLL(...) if(verbose_lvl >= pmlib::verbose::ERROR)   pmlib::msg_err_nll(__VA_ARGS__)
  #define MSG_WARNING_NLL(...) if(verbose_lvl >= pmlib::verbose::WARNING) pmlib::msg_err_nll(__VA_ARGS__)
  #define MSG_INFO_NLL(...) if(verbose_lvl >= pmlib::verbose::INFO)    pmlib::msg_nll(__VA_ARGS__)
  #define MSG_DEBUG_NLL(...) if(verbose_lvl >= pmlib::verbose::DEBUG)   pmlib::msg_nll(__VA_ARGS__)
  #define MSG_VERBOSE_NLL(...) if(verbose_lvl >= pmlib::verbose::VERBOSE) pmlib::msg_nll(__VA_ARGS__)

  // basic class for macross usage
  unsigned short DEFAULT_VERBOSE_LEVEL = verbose::INFO;
  class PmMsg {
    PmMsg () { verbose_lvl = DEFAULT_VERBOSE_LEVEL; };
    int verbose_lvl;
  };

  // ======= PART III. output formatting ====================================================================
  // ======= vector print helpers ====================================================================
  template<typename T> void print_simple_vector(const std::vector <T> & v, std::string separator = "\n"){
    int size = v.size();
    for(int i = 0; i < size; ++i) std::cout << v[i] << separator;
    std::cout << std::endl;
  }

  template<typename T> void print_simple_vector(const std::vector <T> * v, std::string separator = "\n"){
    int size = v->size();
    for(int i = 0; i < size; ++i) std::cout << v->at(i) << separator;
    std::cout << std::endl;
  }

  template<typename T> void print_matrix(const std::vector<T> & matrix){
    int size = sqrt(matrix.size());
    std::cout << "[";
    for(int i = 0; i < size; i++){
      for(int j = 0; j < size; j++)
        std::cout << matrix[i * size + j] << " ";
      if(i != size-1) std::cout << " " << std::endl;
      else std::cout << "]" << std::endl;
    }
  }

  // ======= msg progress  ==================================================================== 
  void msg_progress(double progress, int barWidth = 50){
    if(progress > 1.) progress = 1.;
    if(progress < 0.) progress = 0.;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos) std::cout << "=";
      else if (i == pos) std::cout << ">";
      else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
  }

  // ======= tables  ==================================================================== 
  void print_table_item(const std::string & item, int max_available_chars, const std::string & separator, const std::string & reduction_symbol){
    std::cout << separator;
    max_available_chars -= separator.size();
    if( item.size() > max_available_chars ){
      max_available_chars -= reduction_symbol.size();
      std::cout << item.substr(0, std::max(max_available_chars, 1) ) << reduction_symbol;
      return;
    }
    int total_spaces = max_available_chars - item.size();
    std::cout << std::string(total_spaces/2 + total_spaces%2, ' ');
    std::cout << item;
    std::cout << std::string(total_spaces/2, ' ');
  }

  void print_as_table(const std::vector< std::vector<std::string> > & items, const int & max_total_width = 150, const int & graphic_mode = 0){
    if(not items.size()) return;

    std::string separator = "|";
    std::string reduction_symbol = "$";
    if(graphic_mode == 1){}; // TODO

    int Nx = items.size();
    int Ny = items.at(0).size();

    vector<int> column_widthes = vector<int>(Nx, 0);
    for(int y = 0; y < Ny; y++){
      const std::vector<std::string> & line = items[y];
      for(int x = 0; x < Nx; x++)
        column_widthes[x] = std::max((int)line[x].size(), column_widthes[x]);
    }

    int total_width = 0;
    for(int x = 0; x < Nx; x++){
      column_widthes[x] += 3; // add separators and spaces
      total_width += column_widthes[x];
    }

    if(total_width > max_total_width){
      vector<int> column_widthes_delta = column_widthes;
      int sum_resized_column_widthes = 0;
      int max_resize_index = 0;
      for(int x = 0; x < Nx; x++){
        column_widthes[x] = int( float(column_widthes[x] * max_total_width) / total_width );
        column_widthes_delta[x] -= column_widthes[x];
        sum_resized_column_widthes += column_widthes[x];
      }

      int extra_space = max(0, max_total_width - sum_resized_column_widthes);
      while( extra_space ){
        std::vector<int>::iterator max_element_it = std::max_element(column_widthes_delta.begin(), column_widthes_delta.end());
        column_widthes[ std::distance(column_widthes_delta.begin(), max_element_it) ] += 1;
        column_widthes_delta[ std::distance(column_widthes_delta.begin(), max_element_it) ] -= 1;
        extra_space -- ;
      }
      total_width = max_total_width; 
    }

    // print first line
    const std::vector<std::string> & line_0 = items[0];
    for(int x = 0; x < Nx; x++)
      print_table_item(line_0[x], std::max(column_widthes[x], 2), separator, reduction_symbol);

    // print separator
    std::cout << std::endl << std::string(total_width, '-') << std::endl;

    // print other parts of the table
    for(int y = 0; y < Ny; y++){
      const std::vector<std::string> & line = items[y];
      for(int x = 0; x < Nx; x++)
        print_table_item(line[x], std::max(column_widthes[x], 2), separator, reduction_symbol);
      std::cout << std::endl;
    }
  }
};

#endif

















 
