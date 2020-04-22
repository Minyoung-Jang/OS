#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 512

char buffer[BUF_SIZE];
char sentence[BUF_SIZE];
int buffer_size = 0;
int buffer_pos = 0;

int ReadTextLine(int fd, char str[], int max_len)
{
    int j = 0;
    int ret = 0;

    // if current position is 0, reset buffer size and pos
    if(lseek(fd, 0, SEEK_CUR) == 0) buffer_pos = buffer_size = 0;
    while(j < max_len - 1){
        if(buffer_pos == buffer_size){
            buffer[0] = 0;
            buffer_size = read(fd, buffer, BUF_SIZE);
            buffer_pos = 0;
        }

        if(buffer_size == 0){
            if(j == 0) ret = EOF;
            break;
        }

        while(j < max_len - 2 && buffer_pos < buffer_size){
            str[j++] = buffer[buffer_pos++];
            if(str[j - 1] == '\0' || str[j - 1] == 10){
                j--; // to remove CR
                max_len = j; // to terminate outer loop
                break; // break inner loop
            }
        }
    }
    str[j] = 0;
    return ret;
}

void choose_line(int line_number, int fd, char str[], int max_len){
    for(int i = 0 ; i < line_number; i++){
        ReadTextLine(fd, str, max_len);
    }
}
 
int main(){
    int fd;
    char buf[BUF_SIZE];
    int num_cpu = 0, mem_total = 0;
    float loadavg[3] = {};
    char model[BUF_SIZE]="";

    //CPU cores
    fd = open("/proc/cpuinfo", O_RDONLY);
    choose_line(13, fd, buf, BUF_SIZE);
    sscanf(buf, "%*s %*s    : %d", &num_cpu);
    printf("# of processor cores = %d\n", num_cpu);
    close(fd);

    //model name
    fd = open("/proc/cpuinfo", O_RDONLY);
    choose_line(5, fd, buf, BUF_SIZE);
    strcpy(model, buf+15);
    printf("CPU model = %s\n", model);
    close(fd);

    //memtotal
    fd = open("/proc/meminfo", O_RDONLY);
   	ReadTextLine(fd, buf, BUF_SIZE);
    sscanf(buf, "%*s\t%d", &mem_total);
    printf("MemTotal = %d\n", mem_total);
    close(fd);

    //loadavg
    fd = open("/proc/loadavg", O_RDONLY);
    ReadTextLine(fd, buf, BUF_SIZE);
    sscanf(buf, "%f %f %f", &loadavg[0], &loadavg[1], &loadavg[2]);
    printf("loadavg1 = %f, loadavg5 = %f, loadavg15 = %f\n", loadavg[0], loadavg[1], loadavg[2]);
    close(fd);

    return 0;
} 

