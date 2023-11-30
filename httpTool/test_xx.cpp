#include "jsonxx.h"
#include <string>
#include <iostream>

// g++ jsonxx.cc test_xx.cpp -I./ -otest_xx -std=c++11

int main(void){

    using namespace std;
    using namespace jsonxx;

    string teststr(
        "{"
        "  \"foo\" : 1,"
        "  \"bar\" : false,"
        "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60,},"
        "  \"data\": [\"abcd\", 42],"
        "}"
    );

    Object obj;
    obj.parse(teststr);
    int foo = obj.get<Number>("foo");
    bool bar = obj.get<Boolean>("bar");

    printf("foo:%d\n", foo);
    printf("bar:%d\n", bar);
    
    Object person;
    person = obj.get<Object>("person");
    string name = person.get<String>("name");
    int age = person.get<Number>("age");

    printf("name:%s\n", name.c_str());
    printf("age:%d\n", age);

    Array data = obj.get<Array>("data");
    printf("%s\n", data.get<String>(0).c_str());
    printf("%d\n", (int)data.get<Number>(1));

    //------------------------------------------------//
    Array a;
    a.append(123);
    a.append("hello world");
    a.append(3.1415);
    a.append(Object("key", "value"));

    Object o;
    o << "key" << a;

    cout << o.json() << endl;

    return 0;
}