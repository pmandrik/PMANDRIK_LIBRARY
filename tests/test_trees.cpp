
#include "pmlib_other.hh"

using namespace std;
using namespace pm;

class TestData {
  public:
  string name;
  string GetKey(){
    return name;
  }
};

NaryNode<TestData,string> * make_node(string key){
  TestData * data = new TestData(); 
  data->name = key;
  NaryNode<TestData,string> * node = new NaryNode<TestData,string>();
  node->SetData( data );
  return node;
}

int main(){
   NaryTree tree;
   
   auto nA = make_node( "A" );
   auto nA1 = make_node( "A1" );
   auto nA2 = make_node( "A2" );
   auto nA3 = make_node( "A3" );
   
   nA->AddChild( nA1 );
   nA->AddChild( nA2 );
   nA->AddChild( nA3 );
   nA3->SetFather( nA );
   
   nA->Print();
   nA1->Print();
   nA2->Print();
   nA3->Print();
   
   printf("buy buy");
   
   return 0;
}




