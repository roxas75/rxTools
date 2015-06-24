/*
    padxorer by xerpi
    compile with:
        gcc -O3 padxorer.c -o padxorer
    usage:
        padxorer <input file 1> <input file 2>

    the output will be <input file 1>.out
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

#define BUF_SIZE (8*1024*1024)

void print_usage();

int main(int argc, char *argv[])
{
    int ret_val = EXIT_SUCCESS;
    
    if (argc < 3) {
        print_usage();
        ret_val = EXIT_FAILURE;
        goto exit_fail_1;
    }
    
    FILE *fd_in1, *fd_in2, *fd_out;
    if (!(fd_in1 = fopen(argv[1], "rb"))) {
        printf("Error opening input file 1\n");
        ret_val = EXIT_FAILURE;
        goto exit_fail_1;
    }
    if (!(fd_in2 = fopen(argv[2], "rb"))) {
        printf("Error opening input file 2\n");
        ret_val = EXIT_FAILURE;
        goto exit_fail_2;
    }
    
    fseek(fd_in1, 0, SEEK_END);
    unsigned int in_size1 = ftell(fd_in1);
    fseek(fd_in1, 0, SEEK_SET);
    
    fseek(fd_in2, 0, SEEK_END);
    unsigned int in_size2 = ftell(fd_in2);
    fseek(fd_in2, 0, SEEK_SET);
    
    unsigned int min_size = (in_size1 < in_size2) ? in_size1 : in_size2;

    char *out_name = malloc(strlen(argv[1]) + strlen(".out") + 1);
    sprintf(out_name, "%s.out", argv[1]);
    
    if (!(fd_out = fopen(out_name, "wb+"))) {
        printf("Cannot create output file\n");
        ret_val = EXIT_FAILURE;
        goto exit_fail_3;
    }
    
    unsigned char *data_buf1 = (unsigned char *)malloc(BUF_SIZE);
    unsigned char *data_buf2 = (unsigned char *)malloc(BUF_SIZE);

    #define BAR_LEN 50
    unsigned int step_bytes = min_size/100;
    unsigned int temp_bytes = 0;
    size_t bytes_read1, bytes_read2, total_read = 0, min_read;
    while ((total_read < min_size) && (bytes_read1 = fread(data_buf1, 1, BUF_SIZE, fd_in1)) &&
                                      (bytes_read2 = fread(data_buf2, 1, BUF_SIZE, fd_in2))) {
        min_read = (bytes_read1 < bytes_read2) ? bytes_read1 : bytes_read2;
        total_read += min_read;
        temp_bytes += min_read;
        size_t i;
        for (i = 0; i < min_read; i++) {
            data_buf1[i] ^= data_buf2[i];
        }
        fwrite(data_buf1, 1, min_read, fd_out);
        
        if ((temp_bytes >= step_bytes) || (total_read == min_size)) {
            temp_bytes = 0;
            unsigned int percent = (unsigned int)(total_read*(100.0/min_size));
            printf("%3i%% [", percent);
            int j;
            int bar_size = (BAR_LEN*percent)/100;
            for (j = 0; j < BAR_LEN; j++) {
                if (j < bar_size) printf("=");
                else if (j == bar_size) printf(">");
                else printf(" ");
            }
            printf("]\r");
            fflush(stdout);
        }
    }
    fflush(fd_out);
    fclose(fd_out);
    free(data_buf1);
    free(data_buf2);
    printf("\nFinished!\n");
exit_fail_3:
    free(out_name);
    fclose(fd_in2);
exit_fail_2:
    fclose(fd_in1);
exit_fail_1:
    return ret_val;
}

void print_usage()
{
    printf("padxorer by xerpi\n"
           "usage: padxorer <input file 1> <input file 2>\n"
           "the output will be <input file 1>.out\n");
}