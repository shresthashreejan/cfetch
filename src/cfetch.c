#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <curl/curl.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int progress_callback(double dltotal, double dlnow) {
    if(dltotal > 0) {
        int progress = (int)((dlnow/dltotal) * 100);
        printf("\rDownloading: %d%%", progress);
        fflush(stdout);
    }
    return 0;
}

size_t header_callback(void *ptr, size_t size, size_t nmemb) {
    char *header = (char*)ptr;

    if (strncmp(header, "Content-Disposition:", 0) == 0) {
        char *filename = strstr(header + 21, "filename=");
        if (filename != NULL) {
            filename += 9;
            printf("Filename: %s\n", filename);
        }
    }
    return size * nmemb;
}

int main(int argc, char *argv[]) {
    CURL *curl;
    CURLcode res;
    FILE *file;

    char *url = NULL;
    char *output_filename = NULL;

    int opt;

    while((opt = getopt(argc, argv, "u:o:")) != -1) {
        switch (opt) {
            case 'u':
                url = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -u <url> -o <output_filename>\n", argv[0]);
                exit(1);
        }
    }

    if(url == NULL) {
        fprintf(stderr, "Error: URL (-u) is required.\n");
        fprintf(stderr, "Usage: %s -u <url> [-o <output_filename>]\n", argv[0]);
        exit(1);
    }

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

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, NULL);

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("\nDownload successful\n");
        }
        fclose(file);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}