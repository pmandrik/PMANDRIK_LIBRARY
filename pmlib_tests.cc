
#include "pmlib_msg.hh"

void pmlib_tests(){


  // test print_as_table
  std::vector< std::vector<std::string> > items;
  int size_x = 10;
  int size_y = 10; 

  for(int y = 0; y < size_y; y++){
    std::vector<std::string> line;
    for(int x = 0; x < size_x; x++){
      std::string item = std::string(x+1, 'A');
      line.push_back( item );
    }
    items.push_back( line );
  }

  for(int  max_total_width = 0;  max_total_width < 150;  max_total_width += 10){
    cout << endl << "table" << endl;
    pm::print_as_table(items, max_total_width);
  }

}
