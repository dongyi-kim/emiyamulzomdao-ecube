#include <stdio.h>
#include <curl/curl.h>

#define PHOTO_HOST "http://immense-garden-82747.herokuapp.com/api/photos"

int send_file_to_server(char path[]) {

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
                 CURLFORM_FILE, path,
                 CURLFORM_END);


    curl = curl_easy_init();
    /* initialize custom header list (stating that Expect: 100-continue is not
       wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, PHOTO_HOST);
//        if((argc == 2) && (!strcmp(argv[1], "noexpectheader")))
//            /* only disable 100-continue header if explicitly requested */
//            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
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
}

int main(int argc, char *argv[])
{
    char buffer[100];
    scanf("%s", buffer);
    send_file_to_server(buffer);

    return 0;
}