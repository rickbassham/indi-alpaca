#include "jsonRequest.h"

#include <curl/curl.h>
#include <libindi/indidevapi.h>

static
void dump(const char *text,
          FILE *stream, unsigned char *ptr, size_t size)
{
    size_t i;
    size_t c;
    unsigned int width=0x10;

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
            text, (long)size, (long)size);

    for(i=0; i<size; i+= width) {
        fprintf(stream, "%4.4lx: ", (long)i);

        /* show hex to the left */
        for(c = 0; c < width; c++) {
            if(i+c < size)
                fprintf(stream, "%02x ", ptr[i+c]);
            else
                fputs("   ", stream);
        }

        /* show data on the right */
        for(c = 0; (c < width) && (i+c < size); c++) {
            char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream); /* newline */
    }
}

static
int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp)
{
    const char *text;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
    case CURLINFO_TEXT:
        fputs("== Info: ", stderr);
        fwrite(data, size, 1, stderr);
    default: /* in case a new one is introduced to shock us */
        return 0;

    case CURLINFO_HEADER_OUT:
        text = "=> Send header";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> Send data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> Send SSL data";
        break;
    case CURLINFO_HEADER_IN:
        text = "<= Recv header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= Recv data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= Recv SSL data";
        break;
    }

    dump(text, stderr, (unsigned char *)data, size);
    return 0;
}

struct response_t
{
    char *response;
    size_t size;
};

static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
{
    size_t realsize        = size * nmemb;
    struct response_t *mem = (struct response_t *)userp;

    char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL)
        return 0; /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

nlohmann::json get_json(const char *url)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        struct response_t chunk = { .response = nullptr, .size = 0 };

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLcode::CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (http_code == 200)
                return nlohmann::json::parse(chunk.response);
        }
    }

    return nlohmann::json(nullptr);
}

nlohmann::json put_json(const char* url, const std::map<std::string, std::string> &body)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        struct response_t chunk = { .response = nullptr, .size = 0 };

        struct curl_slist *headers=NULL; // init to NULL is important
        curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if (body.size() > 0)
        {
            char post_data[1024];
            memset(post_data, 0, 1024);

            std::map<std::string, std::string>::const_iterator it;
            for (it = body.begin(); it != body.end(); it++)
            {
                strcat(post_data, it->first.c_str());
                strcat(post_data, "=");
                strcat(post_data, it->second.c_str());
                strcat(post_data, "&");
            }

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        }

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLcode::CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (http_code == 200)
                return nlohmann::json::parse(chunk.response);
        }
    }

    return nlohmann::json(nullptr);
}
