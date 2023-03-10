#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count, int contor);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count);
char *compute_get_token(char *host, char *url, char *query_params,
                            char **token, int count);
char *compute_post_book(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **book, int book_count);

#endif
