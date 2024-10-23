#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

size_t strLen(const char *str) {
    return strlen(str);
}

size_t wordsCount(const char *str) {
    int wordsCnt = 1;
    int length = strlen(str);
    int leftWSP = 0;
    int rightWSP = length - 1;

    for (int i = 0; str[i] == ' '; ++i) {
        leftWSP = i;
    }
    for (int j = rightWSP; str[j] == ' '; --j) {
        rightWSP = j;
    }

    if (!leftWSP && !rightWSP) {
        for (int i = 0; i < length; ++i) {
            if (str[i] == ' ') {
                ++wordsCnt;
            }
        }

        return wordsCnt;
    }

    for (int i = leftWSP; i < rightWSP; ++i) {
        if (str[i] == ' ') {
            ++wordsCnt;
        }
    }

    return wordsCnt;
}

int main() {
    char *shMem = (char*)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    fgets(shMem, 4096, stdin);    
    shMem[strlen(shMem) - 1] = '\0';

    int length = strlen(shMem); 
    int status1;
    int status2;

    pid_t ch1 = fork();

    if (ch1 == 0) {
        sleep(1);
        ((int*)shMem)[length + 1] = strLen(shMem);

        exit(EXIT_SUCCESS);
    } else if (ch1 > 0) {
        pid_t ch2 = fork();

        if (ch2 == 0) {
            ((int*)shMem)[length + 5] = wordsCount(shMem);
            exit(EXIT_SUCCESS);
        } else if (ch2 > 0) {
            wait(&status1);
            wait(&status2);
            if (WIFEXITED(status1)) {
                printf("Symbols count is %d \n", ((int*)shMem)[length + 1]);
            } else if (WIFSIGNALED(status1)) {
                printf("<--Child 1 Process termination detected--> status <%d> \n", WTERMSIG(status1));
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(status2)) {
                printf("Words count is %d \n", ((int*)shMem)[length + 5]);
            } else if (WIFSIGNALED(status2)) {
                printf("<--Child 2 Process termination detected--> status <%d> \n", WTERMSIG(status2));
                exit(EXIT_FAILURE);
            }
        }
    }
    
    return 0;
}
