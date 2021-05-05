#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>

#define PAGE_SIZE 4096
#define REQ_SIZE 256
#define GET_REDIRECTS 0x00
#define GET_CONTENT 0x01
#define GET_DISAMBIGUATION 0x02
#define GET_SEARCH 0x04
#define RESP_OK 0xFF

typedef struct resp_data {
    char * data;
    size_t length;
} response_data_t;

typedef struct reqdata {
    //Lets the underlying function know what API request to make
    int req_type;

    //Lets the user know what to do next
    int res_type;

    //Response makes it clear that this is a disambiguation
    char * next_choice; 

    bool is_lucky;

    int verbose;

} reqdata_t; 


size_t handle_resp (char *ptr, size_t size, size_t nmemb, void * userdata);
void make_request(char * wiki_page, reqdata_t * request_data);

#endif

//End of file

