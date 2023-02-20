#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.c"
#include "parson.h"

#define LEN 500

// functie utilizata pentru a crea un mesaj cu structura json
// pentru register si login
char* serialization_parson_user(char username[LEN], char password[LEN]) {
	char* my_message = NULL;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    my_message = json_serialize_to_string_pretty(root_value);
    json_value_free(root_value);
    return my_message;
}

// functie utilizata pentru a crea un mesaj cu structura json
// pentru add_book ce primeste ca parametrii campurile necesare comenzii
char* serialization_parson_add_book(char title[LEN], char author[LEN],
	char genre[LEN], char publisher[LEN], int page_count){
		char* my_message = NULL;
		JSON_Value *root_value = json_value_init_object();
	    JSON_Object *root_object = json_value_get_object(root_value);
	    json_object_set_string(root_object, "title", title);
	    json_object_set_string(root_object, "author", author);
	    json_object_set_string(root_object, "genre", genre);
	    json_object_set_string(root_object, "publisher", publisher);
	    json_object_set_number(root_object, "page_count", page_count);
	    my_message = json_serialize_to_string_pretty(root_value);
	    json_value_free(root_value);
		return my_message;
}

// functie ce intoarce cookie-ul ca string
char* find_cookies_from_response(char* response){
	char* cookies;
	char* find_cookies;
	char* point = NULL;
	char* new_point = malloc(sizeof(char) * LEN);
	cookies = malloc(sizeof(char) * LEN);
	find_cookies = "connect.sid";
	//cauta pozitia primului cuvant in response
	point = strstr(response, find_cookies);
	if(point != NULL){
		strcpy(new_point, point);
		// am utilizat un string diferit pentru a nu-l afecta pe cel initial
		char* aux_strtok = malloc(sizeof(char) * LEN);
		// cautam primul caracter care nu face parte din cookie
		aux_strtok = strtok(new_point, ";");
		int length = strlen(aux_strtok);
		// copiem pana la ;
		if (aux_strtok != NULL) 
			strncpy(cookies, aux_strtok, length);		
	}
	return cookies;
}

// functie ce intoarce token-ul
char* find_token_from_response(char* response){
	char* token = malloc(sizeof(char) * LEN);
	char* find_token;
	char* point;
	char* new_point = malloc(sizeof(char) * LEN);
	char* str = "Bearer ";
	find_token = "token";
	// cautam aparitia cuvantului token
	point = strstr(response, find_token);
	point += 8;
	// pastram in point inceputul token-ului
	strncpy(token, str, 7);	
	if(point != NULL){
		strcpy(new_point, point);		
		char* aux_strtok = malloc(sizeof(char) * LEN);
		// cautam primul caracter care nu face parte din token
		aux_strtok = strtok(new_point, "}");
		int length = strlen(aux_strtok);
		if (aux_strtok != NULL)
			// copiem token-ul dupa stringul str 
			strncpy(token + 7, aux_strtok, length - 1);
	}
	return token;
}

int main(int argc, char *argv[])
{

	int sockfd;
	char *message, *response, *type, *cookies, *token;
	char received_command[LEN], username[LEN], password[LEN];
	char title[LEN], author[LEN], genre[LEN], publisher[LEN];
	int page_count;
	type = malloc(sizeof(char) * LEN);
	cookies = NULL;
	token = NULL;
	int exit = 0;
	do{
		printf("Command: ");
		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
		scanf("%s", received_command);
		// verificam comanda primita
		if(!strcmp(received_command, "register")){
			
			// register afiseaza si citeste parametrii
			printf("Username: ");
			scanf("%s", username);
			printf("Password: ");
			scanf("%s", password);			
			char* to_send[1];
			// construim stringul cu structura json
			to_send[0] = serialization_parson_user(username, password);
			// construim mesajul pe care il trimitem serverului
			message = compute_post_request("3.8.116.10", "/api/v1/tema/auth/register", "application/json", to_send, 1, NULL, 0);
			printf("%s", message);
			printf("\n");
			// trimit serverului si afisez raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("%s", response);
			printf("\n");
		} else if(!strcmp(received_command, "login")){
			// asemanator register
			printf("Username: ");
			scanf("%s", username);
			printf("Password: ");
			scanf("%s", password);
			// contruim stringul folosind structra json
			char* to_send[1];
			to_send[0] = serialization_parson_user(username, password);
			message = compute_post_request("3.8.116.10", "/api/v1/tema/auth/login",
				"application/json", to_send, 1, NULL, 0);
			printf("%s", message);
			printf("\n");
			// trimit pe server si afisez raspunsul
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("%s", response);
			printf("\n");		
			cookies = find_cookies_from_response(response);			
		} else if(!strcmp(received_command, "enter_library")){
			// punem conditia ca cookie sa contina o valoare, adica
			// sa se fi folosit anterior comanda login
			if(cookies != NULL){
				char* send_cookies[1];
				token = malloc(sizeof(char) * LEN);
				send_cookies[0] = cookies;
				// construim mesajul
				message = compute_get_request("3.8.116.10", "/api/v1/tema/library/access",
					NULL, send_cookies, 1, 0);
				// trimit pe server si afisez raspunsul
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				token = find_token_from_response(response);
				printf("%s", response);
				printf("\n");				
			} else {
				printf("Login didn't worked!\n");
			} 
			
			
		} else if(!strcmp(received_command, "get_books")){
			// pun conditia ca in token sa existe o valoare diferita de null, 
			// adica sa se fi utilizat anterior enter_library
			if(token != NULL){
				char* send_token[1];
				send_token[0] = token;
				message = compute_get_token("3.8.116.10", "/api/v1/tema/library/books",
					NULL, send_token, 1);
				// trimit pe server si afisez raspunsul
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				printf("%s", response);
				printf("\n");
			} else {
				printf("Token not found!\n");
				
			} 
			
			
		} else if(!strcmp(received_command, "get_book")){
			// pun conditia ca in token sa existe o valoare diferita de null, 
			// adica sa se fi utilizat anterior enter_library
			if(token != NULL ){
			int id_int;
			char* id = malloc(sizeof(char) * LEN);
			printf("id=");
			scanf("%s", id);
			id_int = atoi(id);
			// adaug idul la finalul tipului
			strcpy(type, "/api/v1/tema/library/books/");
			strcat(type, id);
			printf("%s\n", type);
			char* s_token[1];
			s_token[0] = token;
			// trimit pe server si afisez
			message = compute_get_token("3.8.116.10", type, NULL, s_token, 1);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("%s", response);
			printf("\n");

			} else {
				printf("Token not found!\n");
				
			}

		} else if(!strcmp(received_command, "add_book")){
			// pun conditia ca in token sa existe o valoare diferita de null, 
			// adica sa se fi utilizat anterior enter_library
			if(token != NULL){
				printf("title=");
				scanf("%s", title);
				printf("author=");
				scanf("%s", author);
				printf("genre=");
				scanf("%s", genre);
				printf("publisher=");
				scanf("%s", publisher);
				printf("page_count=");
				scanf("%d", &page_count);
				if(title != NULL && author != NULL && genre != NULL && publisher != NULL &&
					page_count > 0){
					char* to_add_book[1];
					// construiesc un string cu ajutorul structurii json in care
					// am adaugat toate campurile necesare
					to_add_book[0] = serialization_parson_add_book(title, author,
						genre, publisher, page_count);
					char* send_tok[1];
					send_tok[0] = token;
					message = compute_post_book("3.8.116.10", 
						"/api/v1/tema/library/books", "application/json", 
						to_add_book, 1, send_tok, 1);
					// trimit si afisez
					send_to_server(sockfd, message);
					response = receive_from_server(sockfd);
					printf("%s", response);
					printf("\n");
				}
			}
			printf("Token not found");

		} else if(!strcmp(received_command, "delete_book")){
			if(token != NULL ){
			// pun conditia ca in token sa existe o valoare diferita de null, 
			// adica sa se fi utilizat anterior enter_library
			int id_int;
			char* id = malloc(sizeof(char) * LEN);
			printf("id=");
			scanf("%s", id);
			id_int = atoi(id);
			strcpy(type, "/api/v1/tema/library/books/");
			// adaug idul la final 
			strcat(type, id);
			printf("%s\n", type);
			char* t_token[1];
			t_token[0] = token;
			message = compute_get_request("3.8.116.10", type,
					NULL, t_token, 1, 1);
			// trimit pe server si afisez
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			printf("%s", response);
			printf("\n");
		}
		printf("Not deleted");
	} else if(!strcmp(received_command, "logout")){
		if(cookies != NULL ){
			// daca cookie are o valoare diferita de null, inseamna ca
			// a fost utilizata anterior comanda login
			token = NULL;
			cookies = NULL;
			serialization_parson_user("","");
		} else{
			printf("First login!");
		}
	} else if(!strcmp(received_command, "exit")){
			// in cazul exit ies din do while
			int exit = 1;
			break;
		}
	} while(exit != 1);

	close(sockfd);
	free(type);
	free(token);

	return 0;
}