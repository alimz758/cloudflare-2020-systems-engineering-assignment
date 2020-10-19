#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <getopt.h>
#include <limits.h>
#include <vector>
#include "timer.cpp"

//print error messages
void error(const char *errorMessage) { 
    perror(errorMessage); 
    exit(1); 
}

//print out information on --help
void help() {
    printf("Usage: [--url=<path>] [--host=<host-name>] [--portno=<port-number>]\n");
    printf("\t[--help] [--profile=<number-of-requests>]\n\n");
    printf("\t host:\t Provide the hostname like `localhost`\n");
    printf("\t portno:\t Provide the port number like `80`\n");
    printf("\t url:\t Provide the path like `/links`\n");
    printf("\t profile:\t Provide a positive number, this would be the number of requests\n\n");
}

//Sending request with write() system call
int sendRequest(int sockfd, int bytes, int total, int sent, char *header) {
    do {
        bytes = write(sockfd, header+sent, total-sent);
        if (bytes < 0) {
            error("ERROR! Could not write the header to socket");
            return -1;
        }
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);

    return sent;
}

//receiving the resposne back with read() system call
int receivedResponse(int sockfd, char *response, int received, int total, int bytes) {
    do {
        bytes = read(sockfd, response+received, total-received);
        if (bytes < 0) {
            error("ERROR! Could not read the resposne");
            return -1;
        }
            
        if (bytes == 0)
            break;
        received += bytes;
    } while (received < total);

    return received;
}

double getTotalTime(std::vector< double > timeVector) {
    double sum = 0;
    for (auto& elem : timeVector) {
        sum += elem;
    }
    return sum;
}

double getMedian(std::vector< double > timeVector, int profile) {
    sort(timeVector.begin(), timeVector.end()); 
    if (profile % 2 != 0) 
        return (double)timeVector[profile / 2]; 
  
    return (double)(timeVector[(profile - 1) / 2] + timeVector[profile / 2]) / 2.0;
}

//tool result
void toolResult(char* host, int numberOfRequests, int smallestResponse, int largestResponse,
                int number_success, double slowest_time, double fastest_time, double total_time, double median) {
    printf("\n\n\t\t===== Results =====\n\n");
    printf("%d %s sent to %s in %f seconds\n", numberOfRequests, numberOfRequests > 1 ? "requests were": "request was", host, total_time);
    printf("The size in bytes of the smallest response: %d\n", smallestResponse);
    printf("The size in bytes of the largest response: %d\n", largestResponse);
    printf("The percentage of requests that succeeded: %d\n", (number_success/numberOfRequests)*100);
    printf("The fastest time: %f\n", fastest_time);
    printf("The slowest time: %f\n", slowest_time);
    printf("The mean of times: %f\n", total_time / numberOfRequests);
    printf("The median of times: %f\n", median);
}

int main(int argc,char *argv[])
{
   
    struct hostent *server;
    struct sockaddr_in serv_addr;
    Timer timer;
    int sockfd, sent, received, total, 
        header_size, portno, profile, c,
        number_success = 0,
        smallest_response = INT_MAX,
        largest_response = 0;
    double fastest_time = 5, 
        slowest_time, median, total_time;
    char *header, response[4096], *host, *path; // note the I allocated a fixed size, 4096B, for the resposne and not using malloc/realloc for more
                                                // dynamic memory allocation depending on the response size. Due to having school and being busy
                                                // with school projects did not have enough time to implement and test with dynamic allocation as need more time
                                                // I hope this implementation, with its flaw, would be fine for the purpose of this interview.s
    static struct option long_options[] = {
        {"host",   1,  0,  'h' },
        {"portno",  1,  0, 'p' },
        {"url",  1,  0, 'u' },
        {"help",  0,  0, 'k' },
        {"profile",  1,  0, 'l' },
    };

    while (1) {
        c = getopt_long(argc, argv, "h:p:u:l:k", long_options,NULL);
        if (c == -1)
            break;
        switch (c) {
            //hostname
            case 'h':
                host = optarg;
                break;
            //port number
            case 'p':
                portno = atoi(optarg);
                break;
            //profile
            case 'l':
                profile = atoi(optarg);
                if (profile <=0) {
                    error("Profile takes only positive numbers!\n");
                    exit(1);
                }
                break;
            //url
            case 'u':
                path = optarg;
                break;
            //help
            case 'k':
                help();
                exit(1);
            default:
                error("Unrecognized argument! Try again!\n");
                help();
                exit(1);
        }
    }

    // setting the header size
    header_size=0;
    header_size+=strlen("%s %s HTTP/1.0\r\nHost: %s\r\nConnection: keep-alive\r\n");        
    header_size+=strlen("GET");                        
    header_size+=strlen(path); 
    header_size+=strlen(host);                                               
    header_size+=strlen("\r\n");                       
    
    //allocate space for the header 
    header = (char *)malloc(header_size);
    sprintf(header,"%s %s HTTP/1.0\r\nHost: %s\r\nConnection: keep-alive\r\n", "GET", strlen(path)>0?path:"/", host); 
    strcat(header,"\r\n");                           
    
    // lookup the ip address based on the hostname
    server = gethostbyname(host);
    if (server == NULL) {
        error("ERROR! Such Host doesn't exist");
    }

    // set up server_addr
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        
    // send the request 
    total = strlen(header);
    int counter = profile;
    std::vector< double > timeVector;
    while (counter != 0) {
        //create the socket 
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            error("ERROR! Could not create the socket.");
            continue;
        } 
        //connect the socket
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
            error("ERROR! Could not connect to the socket");
            continue;
        }
        //send request
        sent = 0;
        timer.start();
        if ( (sent = sendRequest(sockfd, 0, total, sent, header)) == -1 ) {
            continue;
        }
        // receive the response 
        memset(response,0,sizeof(response));
        total = sizeof(response)-1;
        received = 0;
        if ((received = receivedResponse(sockfd, response, received, total, 0)) == -1) {
            continue;
        }

        if (received == total) {
            error("ERROR! Could not store the response");
            continue;
        }

        //store largest and smaller response size
        int sizeOfResponse =  strlen(response);
        if (sizeOfResponse > largest_response) {
            largest_response = sizeOfResponse;
        }
        if (sizeOfResponse < smallest_response) {
            smallest_response = sizeOfResponse;
        }

        timer.stop();
        double elapsedTime = timer.elapsed();
        if (counter == profile)
            slowest_time = elapsedTime;
        timeVector.push_back(elapsedTime);
        //store the time
        if (elapsedTime < fastest_time) {
            fastest_time = elapsedTime;
        }
        if (elapsedTime > slowest_time) {
            slowest_time = elapsedTime;
        }

        printf("\nThe response number %d:\n%s\n", profile - counter + 1,response);
        //free the socket to reconnect again
        close(sockfd);
        number_success++;
        counter--;
    }  
    //free the header buffer
    free(header);
    total_time = getTotalTime(timeVector);
    median = getMedian(timeVector, profile);
    toolResult(host, profile, smallest_response, largest_response, number_success, slowest_time, fastest_time, total_time, median);
    return 0;
}