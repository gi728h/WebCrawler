#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <stdbool.h>
#include <regex.h>
#include <unistd.h>

struct CURLResponse
{
    char *html;
    size_t size;
};

struct QNode
{
    char *url;
    struct QNode *next;
};

struct Queue
{
    struct QNode *front, *rear;
};

struct Queue *createQueue();

void enQueue(struct Queue *q, int k);

void deQueue(struct Queue *q);

void printQueue(struct Queue *queue);

bool relativelink(char *url)
{
    regex_t regex;
    int value;
    // Comparing pattern "https://" with
    value = regcomp(&regex, "https://", 0);
    value = regexec(&regex, url, 0, NULL, 0);
    if (value == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static size_t WriteHTMLCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct CURLResponse *mem = (struct CURLResponse *)userp;
    char *ptr = realloc(mem->html, mem->size + realsize + 1);

    if (!ptr)
    {
        printf("Not enough memory available (realloc returned NULL)\n");
        return 0;
    }

    mem->html = ptr;
    memcpy(&(mem->html[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->html[mem->size] = 0;

    return realsize;
}

struct CURLResponse GetRequest(CURL *curl_handle, const char *url)
{
    CURLcode res;
    struct CURLResponse response;

    // initialize the response
    response.html = malloc(1);
    response.size = 0;

    // specify URL to GET
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    // send all data returned by the server to WriteHTMLCallback
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteHTMLCallback);
    // pass "response" to the callback function
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response);
    // set a User-Agent header
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36");
    // perform the GET request
    res = curl_easy_perform(curl_handle);

    // check for HTTP errors
    if (res != CURLE_OK)
    {
        fprintf(stderr, "GET request failed: %s\n", curl_easy_strerror(res));
    }

    return response;
}

// ...
// struct CURLResponse GetRequest(CURL *curl_handle, const char *url) ...

int extract_links(struct QNode *q)
{
    // initialize curl globally
    curl_global_init(CURL_GLOBAL_ALL);

    // initialize a CURL instance
    CURL *curl_handle = curl_easy_init();

    // retrieve the HTML document of the target page
    struct CURLResponse response = GetRequest(curl_handle, "https://nodejs.org/docs/latest/api/");
    // print the HTML content
    // printf("%s\n", response.html);

    // scraping logic...
    FILE *fptr;
    // Open a file in writing mode
    fptr = fopen("new.html", "w");

    // Write some text to the file
    fprintf(fptr, response.html);

    // Close the file
    fclose(fptr);

    // Create Xml Object from HTML
    htmlDocPtr doc = htmlReadMemory(response.html, (unsigned long)response.size, NULL, NULL, HTML_PARSE_NOERROR);

    // Scrape AnchorTag elements using XML
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    xmlXPathObjectPtr productHTMLElements = xmlXPathEvalExpression((xmlChar *)"//a", context);

    // Save href LInks to a file
    for (int i = 0; i < productHTMLElements->nodesetval->nodeNr; ++i)
    {
        xmlNodePtr urlHTMLElement = productHTMLElements->nodesetval->nodeTab[i];
        char *url = (char *)xmlGetProp(urlHTMLElement, (xmlChar *)"href");
        // strncat(url, '\0', 1);
        // printf("%d\n",sizeof(url));
        // int l = 0;
        // while(*(url + i) != '\0'){
        //     printf("%c",*(url+i));
        //     i++;
        // }
        // sleep(10);
        // Enqueue url in explore queue
        FILE *fptr;
        // Open a file in writing mode
        fptr = fopen("links.txt", "a+");
        // Write some text to the fileS
        if (relativelink(url))
        {
            fprintf(fptr, "%s\n", url);
            enQueue(q, url);
        }
        else
        {
            fprintf(fptr, "https://nodejs.org/docs/latest/api/%s\n", url);
            enQueue(q, url);
        }
        free(url);
        // Close the file
        fclose(fptr);
    }
    free(response.html);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    // cleanup the curl instance
    curl_easy_cleanup(curl_handle);
    // cleanup the curl resources
    curl_global_cleanup();

    return 0;
}

int main()
{
    struct Queue *q = createQueue();
    extract_links(q);
    struct QNode *temp = q->front;
    int i = 0;
    while (temp)
    {
        i++;

        temp = temp->next;
    }
    printQueue(q);
    printf("%d\n", i);
    return 0;
}