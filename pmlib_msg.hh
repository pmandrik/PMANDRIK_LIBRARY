 

#ifndef PMLIB_MSG_HH
#define PMLIB_MSG_HH 1

#include<iostream> 
#include<string>
#include<vector>

namespace pmlib{

  // ======= print helpers ====================================================================
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

};

#endif

 
