#include "logging.h"


int log_to_file(Logger* logger, char* msg){

    if (!LOG_TOGGLE) return 0; 

    FILE* log_file = logger->fstream; 
    size_t msg_len = strlen(msg);

    size_t bytes_written = fwrite(msg, sizeof(char), msg_len, log_file);

    if (bytes_written < msg_len){
        int ferror_status = ferror(log_file); 
        perror("Error writing to log file - returning ferror and errno set:");
        return ferror_status; 
    }

    int flush_res = fflush(log_file);
    if (flush_res != 0){

        return -1;
    }

    return 0; 
}

int setup_logger(Logger* logger, char* log_file){
    FILE* f = fopen(log_file, "w");
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