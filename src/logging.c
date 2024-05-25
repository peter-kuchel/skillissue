#include "logging.h"


int log_to_file_stream(FILE* log_file, char* msg){
    size_t msg_len = strlen(msg);

    size_t bytes_written = fwrite(msg, sizeof(char), msg_len, log_file);

    if (bytes_written < msg_len){
        int ferror_status = ferror(log_file); 
        perror("Error writing to log file - returning ferror and errno set:");
        return ferror_status; 
    }

    return 0; 
}

int log_to_file(char* log_file, char* msg){
    FILE* f = fopen(log_file, "a");

    if (f == NULL){ 
        perror("Error with opening the file - errno set:");
        return -1;
    }

    int res = log_to_file_stream(f, msg);

    fclose(f);
    return res; 
}

int setup_logger(Logger* logger, char* log_file){
    FILE* f = fopen(log_file, "a");
    if (f == NULL){
        printf("[Error]: Something went wrong opening log file");
        exit(1); 
    }

    logger->fstream = f; 
    logger->fname = log_file;

    return 0;  
}
int teardown_logger(Logger* logger){
    fclose(logger->fstream);

    return 0; 
}