#include <iostream>
#include "asembler.h"
#include "checks.h"

int main(int argc, char* argv[]){
  Asembler* asembler;
  try{
    Checks::checksArgsLength(argc, argv);
    if(argc == 3){
      asembler = new Asembler(argv[2], "random.o");
    }else{
      asembler = new Asembler(argv[3], argv[2]);
    }
    asembler->start();
  }
  catch(invalid_argument& e){
    cout << e.what() << endl;
    asembler = new Asembler(argv[1], "random.o");
  }
}
