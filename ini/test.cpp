#include "INIReader.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(void)
{

    INIReader reader("./fig.ini");

    // string str = reader.GetString("TEST", "DBPATHSERVER", "");

    // // 根据 , 进行分割
    // vector<string> buf;
    // istringstream ss(str);
    // string token;

    // while(getline(ss,token,','))
    // {
    //     buf.push_back(token);
    // }

    cout << "-----------------------00---------------------" << endl;
    vector<string> buf;
    reader.GetStrArr("TEST", "DBPATHSERVER", buf);
    for(auto it = buf.begin(); it != buf.end(); ++it)
    {
        cout << *it << endl;
    }
    cout << "---------------------------------------------" << endl;

    cout << "------------------------11--------------------" << endl;
    vector<int64_t> buf2;
    reader.GetInt64Arr("TEST", "INTARR", buf2);
    for(auto it = buf2.begin(); it != buf2.end(); ++it)
    {
        cout << *it << endl;
    }
    cout << "----------------------------------------------" << endl;

    cout << "------------------------12--------------------" << endl;
    double dou;
    dou = reader.GetDou("TEST", "DOUBLE", 0.0);
    cout << dou << endl;
    cout << "----------------------------------------------" << endl;

    cout << "------------------------13--------------------" << endl;
    vector<double> buf3;
    dou = reader.GetDouArr("TEST", "DOUARR", buf3);
    for(auto it = buf3.begin(); it != buf3.end(); ++it)
    {
        cout << *it << endl;
    }
    cout << "----------------------------------------------" << endl;

    return 0;
}