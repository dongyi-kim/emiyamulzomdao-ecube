#include <iostream>

using namespace std;

/**
 * http://www.cplusplus.com/forum/unices/45878/
 */
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up)
{ //callback must have this declaration
    //buf is a pointer to the data that curl has for us
    //size*nmemb is the size of the buffer

    string str = "";

    for (int c = 0; c<size*nmemb; c++)
    {
        str += buf[c];
    }
    (*((string*)up)) = str;
    return size*nmemb; //tell curl how many bytes we handled
}

