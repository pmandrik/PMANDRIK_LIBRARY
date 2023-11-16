
#include "pmlib_other.hh"

using namespace std;
using namespace pm;

NaryNode<string,string> * make_node(string data){
  NaryNode<string,string> * node = new NaryNode<string,string>();
  node->SetData( &data );
  node->SetLabel( data );
  return node;
}

int main(){
   
   {
     printf("T1=============\n");
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
   }
   {
      printf("T2=============\n");
      NaryTree<string,string> *tree = new NaryTree<string,string>();
      tree->AddNode( "A" );
      tree->AddNode( "A1" );
      tree->AddNode( "A2" );
      tree->AddNode( "A3" );

      tree->SetNodeFather( "A1", "A" );
      tree->SetNodeFather( "A2", "A1" );
      tree->SetNodeFather( "A3", "A1" );
      
      tree->SetNodePriority( "A2", 100 );
      tree->SetNodePriority( "A3", 10 );

      tree->GetNode("A")->Print();
      tree->GetNode("A1")->Print();
      tree->GetNode("A2")->Print();
      tree->GetNode("A3")->Print();
      
      auto nodeListX = tree->GetNodeList();
      for(auto node : nodeListX){
        printf("%p %s, ", node, node->GetLabel().c_str());
      }
      printf("\n");
      tree->SetRemoveStrategy(1);
      tree->RemoveNode( "A1" );
      printf("tree->RemoveNode( A1 );\n");
      tree->GetNode("A")->Print();
      // tree->GetNode("A1")->Print();
      tree->GetNode("A2")->Print();
      tree->GetNode("A3")->Print();

      auto nodeList = tree->GetNodeList();
      for(auto node : nodeList){
        printf("%p %s, ", node, node->GetLabel().c_str());
      }
      printf("\n");
   }
   
   printf("buy buy");
   
   return 0;
}




