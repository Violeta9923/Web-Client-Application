#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

// pentru a implementa functiile din acest fisier, am utilizat
// laboratorul 10

// functia aceasta este primeste ca parametru fie cookie-ul rezultat in
// urma autentificarii si care ne este necesar accesarii bibliotecii,
// fie tokenul care ne este necesar stergerii unei carti
//implementeaza cererea de tip POST sau DELETE
char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, int cont)
{
    // in functie de valoarea variabilei auxiliare cont, stim daca 
    //utilizam GET sau DELETE;
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if(cont == 0){
        if (query_params != NULL) {
            sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
        } else {
            sprintf(line, "GET %s HTTP/1.1", url);
        }
    }
    if(cont == 1){
        if (query_params != NULL) {
            sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
        } else {
            sprintf(line, "DELETE %s HTTP/1.1", url);
        }
    }
    // formatul mesajului in functie de header-ul necesar
    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        if(cont == 0)
            sprintf(line, "Cookie: ");
        if(cont == 1)
            sprintf(line, "Authorization: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, "%s; ");
        }
        strcat(line, cookies[cookies_count - 1]);
        compute_message(message, line);
    }
    compute_message(message, ""); 
    return message;
}

// o functie pe care am implementat-o pornind de la compute_get_request
// diferenta este ca aceasta va avea intotdeauna header-ul Authorization
char *compute_get_token(char *host, char *url, char *query_params,
                            char **token, int count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: ");
        for (int i = 0; i < count - 1; i++) {
            strcat(line, token[i]);
            strcat(line, "%s; ");
        }
        strcat(line, token[count - 1]);
        compute_message(message, line);
    }
    compute_message(message, "");
    return message;
}

// implementeaza cererea de tip POST
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (body_data != NULL) {
        for (int i = 0; i < body_data_fields_count - 1; i++) {
            strcat(body_data_buffer, body_data[i]);
            strcat(body_data_buffer, "&");
        }
        strcat(body_data_buffer, body_data[body_data_fields_count - 1]);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, "%s; ");
        }
        strcat(line, cookies[cookies_count - 1]);
        compute_message(message, line);
    }
    compute_message(message, "");
    if (body_data != NULL) {
        compute_message(message, body_data_buffer);
    }
    free(line);
    return message;
}

// functie implementata pornind de la compute_post_request
// este utilizata de comanda add_book si are header-ul Authorization
char *compute_post_book(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (body_data != NULL) {
        for (int i = 0; i < body_data_fields_count - 1; i++) {
            strcat(body_data_buffer, body_data[i]);
            strcat(body_data_buffer, "&");
        }
        strcat(body_data_buffer, body_data[body_data_fields_count - 1]);
    }

    // formatul mesajului cu header-ul necesar
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: ");
        for (int i = 0; i < cookies_count - 1; i++) {
            strcat(line, cookies[i]);
            strcat(line, "%s; ");
        }
        strcat(line, cookies[cookies_count - 1]);
        compute_message(message, line);
    }
    compute_message(message, "");
    if (body_data != NULL) {
        compute_message(message, body_data_buffer);
    }
    free(line);
    return message;
}
