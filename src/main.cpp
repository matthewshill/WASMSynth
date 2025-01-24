#include <iostream>
#include <emscripten/bind.h>

using namespace emscripten;

int main(){
    return 0;
}

std::string hello(){
    return "hello";
}

EMSCRIPTEN_BINDINGS(my_module){
    function("hello", &hello);
}