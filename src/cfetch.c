#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int main() {
    CURL *curl;
    CURLcode res;
    FILE *file;

    char *url = "https://getsamplefiles.com/download/zip/sample-1.zip";
    char *output_filename = "sample.zip";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        file = fopen(output_filename, "wb");
        if(!file) {
            fprintf(stderr, "Error opening file for writing.\n");
            return 1;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Download successful\n");
        }
        fclose(file);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}