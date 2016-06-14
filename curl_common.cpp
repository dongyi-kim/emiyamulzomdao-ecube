#include <iostream>
#include <curl/curl.h>
#include "common.h"

using namespace std;

const string POST_HOST = "http://immense-garden-82747.herokuapp.com/api/data/";
const string PHOTO_HOST = "http://immense-garden-82747.herokuapp.com/api/photos/";
const string HOST = "http://immense-garden-82747.herokuapp.com";
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

int sendConfigToServer(Data data, string id)
{
    
    cout<<data.illumination<<" "<<data.temperature<<" "<<data.humidity<<" "<<data.soil_humidity<<endl;

    return 200;
}



int sendDataToServer(Data d, string id) {
    char query[200];
    CURL *curl;
    CURLcode res;
    int http_code;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
           string host = POST_HOST+id;
           curl_easy_setopt(curl, CURLOPT_URL, host.c_str());
        /* Now specify the POST data */
           sprintf(query, "illumination=%d&humidity=%d&temperature=%d&soil_humidity=%d", d.illumination, d.humidity, d.temperature, d.soil_humidity);
           curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);

         /* Perform the request, res will get the return code */
           curl_easy_perform(curl);

           curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

        /* Check for errors */

           curl_easy_cleanup(curl);
       }
       curl_global_cleanup();

       return http_code;
   }

int sendEventToServer(string msg, string id) {
    char query[200];
    CURL *curl;
    CURLcode res;
    int http_code;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
       string host = HOST+"/api/sns";
       curl_easy_setopt(curl, CURLOPT_URL, host.c_str());
    /* Now specify the POST data */
       sprintf(query, "message=[%s]%s", id.c_str(), msg.c_str());
       curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);

     /* Perform the request, res will get the return code */
       curl_easy_perform(curl);

       curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

    /* Check for errors */

       curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 200;
}
int sendFileToServer(string path, string id) {

    int http_code;
    CURL *curl;
    CURLcode res;

    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    static const char buf[] = "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);

    /* Fill in the file upload field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "sendfile",
                 CURLFORM_FILE, path.c_str(),
                 CURLFORM_END);


    curl = curl_easy_init();
    /* initialize custom header list (stating that Expect: 100-continue is not
       wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
        /* what URL that receives this POST */
        string host = PHOTO_HOST;
        curl_easy_setopt(curl, CURLOPT_URL, host.c_str());
//        if((argc == 2) && (!strcmp(argv[1], "noexpectheader")))
//            /* only disable 100-continue header if explicitly requested */
//            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

        /* then cleanup the formpost chain */
        curl_formfree(formpost);
        /* free slist */
        curl_slist_free_all (headerlist);
    }
    return http_code;
}

int authorize(string id, string* result) {
   CURL *curl;
   CURLcode res;
   int http_code;

   char buffer[100];

   curl_global_init(CURL_GLOBAL_ALL);

   curl = curl_easy_init();

   if(curl) {

       const string USER_GET = HOST+"/api/users/"+id;

       curl_easy_setopt(curl, CURLOPT_URL, USER_GET.c_str());
       curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
       curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)result);
       res = curl_easy_perform(curl);

       if( res != CURLE_OK ) {
           return -1;
       }

       curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
       curl_easy_cleanup(curl);
   }

   curl_global_cleanup();
    return http_code;
}