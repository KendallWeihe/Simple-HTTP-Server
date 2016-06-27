#include <string.h> //for strcmp, strcpy, strcat
#include <stdlib.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */

#define MAXSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

void HandleTCPClient(int clntSocket, char *html_path)
{
    char request_data[MAXSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, request_data, MAXSIZE, 0)) < 0)
        DieWithError("recv() failed");

    char *type = malloc(sizeof(char) * MAXSIZE); //hold request type
    char *get = "GET"; //
    char *http_version = malloc(sizeof(char) * MAXSIZE);
    char *file_type_string = malloc(sizeof(char) * MAXSIZE);
    // char *file_name = malloc(sizeof(char) * MAXSIZE);
    char *file_name_string = malloc(sizeof(char) * MAXSIZE);
    char *file_size_string = malloc(sizeof(char) * MAXSIZE);
    char *response = malloc(sizeof(char) * 4096);
    int header_count = 0, splice_count = 0, splice_index = 0, index = 0, flag = 0, file_size = 0;
    int i, j, k;
    FILE *read_file;

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
        for (i = 0; i < MAXSIZE; i++){ //loop over received data

          //splice off request type
          if (request_data[i] == ' ' && splice_count == 0){
            splice_count++;
            for (j = 0; j < i; j++){
              type[j] = request_data[j];
            }
            splice_index = i+1;
            for (j = 0; j < i; j++){
              if (type[j] != get[j]){
                flag = 1; //case where the type was not GET
              }
            }
            if (flag ==1){
              strcpy(response, "405 Method not allowed\r\n");
              send(clntSocket, response, strlen(response), 0);
              break;
            }
          }

          //splice off file name
          else if (request_data[i] == ' ' && splice_count == 1){
            for (j = splice_index; j < i; j++){
              file_name_string[index] = request_data[j];
              index++;
            }
            splice_count++;
            splice_index = i+1;
          }

          //splice off HTTP version
          else if (request_data[i] == 'H' && request_data[i+1] == 'T'  && request_data[i+2] == 'T'  && request_data[i+3] == 'P' && splice_count == 2){
            index = 0;
            for (j = i; j < i+8; j++){
              http_version[index] = request_data[j];
              index++;
            }
            splice_count++;

            if (file_name_string[strlen(file_name_string)-1] == '/') //case where last char = / so append index.html
              strcat(file_name_string, "index.html");
            strcat(html_path, file_name_string);
            if (read_file = fopen(html_path, "r")){ //file found

              strcat(response, http_version); //send back with the same HTTP version
              strcat(response, " 200 OK\r\n"); //concatentate response to response buffer
              send(clntSocket, response, strlen(response), 0); //send header

              //find and send Content-length header
              fseek(read_file, 0L, SEEK_END); //seek the end of the file
              file_size = ftell(read_file) - 1; //assign to integer
              sprintf(file_size_string, "%d", file_size); //redirect to char buffer
              fseek(read_file, 0L, SEEK_SET); //place fd back at beginning of file

              memset(response, 0, sizeof(response)); //flush the response buffer
              strcat(response, "Content-Length: "); strcat(response, file_size_string); strcat(response, "\r\n"); //concatentate
              send(clntSocket, response, strlen(response), 0); //send the header

              //send connection: close header
              memset(response, 0, sizeof(response)); //flush the response buffer
              strcat(response, "Connection: close\r\n");
              send(clntSocket, response, strlen(response), 0); //send the header

              //splice off content type
              index = 0;
              for (j = 0; j < strlen(file_name_string); j++){
                if (file_name_string[j] == '.'){
                  for (k = j+1; k < strlen(file_name_string); k++){
                    file_type_string[index] = file_name_string[k];
                    index++;
                  }
                }
              }

              //find and send Content-type header
              memset(response, 0, sizeof(response));
              strcat(response, "Content-Type: ");
              if (!strcmp(file_type_string, "txt"))
                strcat(response, "text/plain; charset=us-ascii\r\n\r\n");
              else if (!strcmp(file_type_string, "html"))
                strcat(response, "text/html; charset=iso-8859-1\r\n\r\n");
              else if (!strcmp(file_type_string, "jpg"))
                strcat(response, "image/jpeg; charset=UTF-8\r\n\r\n");
              else if (!strcmp(file_type_string, "gif"))
                strcat(response, "image/gif; charset=UTF-8\r\n\r\n");
              else{
                memset(response, 0, sizeof(response));
                strcat(response, "415 Unsupported media type\r\n");
              }
              send(clntSocket, response, strlen(response), 0);

              //begin transmitting file data back to client
              char *data = malloc(sizeof(char) * MAXSIZE);
              while (fread(data, 1, MAXSIZE, (FILE*)read_file) > 0){
                send(clntSocket, data, MAXSIZE, 0);
              }
            }

            else { //file not found
                strcpy(response, "404 File not found\r\n");
                send(clntSocket, response, strlen(response), 0);
            }
          }

        }


        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, request_data, MAXSIZE, 0)) < 0)
            DieWithError("recv() failed");
    }

    close(clntSocket);    /* Close client socket */
}
