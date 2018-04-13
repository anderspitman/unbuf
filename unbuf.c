#define _XOPEN_SOURCE 600 // posix_openpt
#include <stdio.h>
#include <stdlib.h> // exit, posix_openpt, grantpt, unlockpt
#include <fcntl.h> // O_RDWR
#include <errno.h> // errno
#include <unistd.h> // fork
#include <stdbool.h> // bool

#define BUF_SIZE_BYTES 2

void error(char *message) {
    fprintf(stderr, "ERROR(%d): %s\n", errno, message);
    exit(1);
}

void checkReturn(int returnCode, char *message) {
    if (returnCode < 0) {
        error(message);
    }
}

void checkPointer(void *ptr, char *message) {
    if (ptr == 0) {
        error(message);
    }
}

bool processIsChild(pid_t pid) {
    return pid == 0;
}

void runChildProcess(int ptySlaveFd, char **command, int commandLen) {

    // set child stdin, stdout, and stderr to use the pty
    close(0);
    close(1);
    close(2);
    dup(ptySlaveFd);
    dup(ptySlaveFd);
    dup(ptySlaveFd);

    int execResult = execvp(command[0], command);
    checkReturn(execResult, "exec");
}

void runParentProcess(int ptyMasterFd) {

    char buf[BUF_SIZE_BYTES];

    ssize_t readResult = -1;
    size_t numBytesRead = 0;

    while (1) {
        readResult = read(ptyMasterFd, buf, sizeof(buf) - 1);
        // TODO: might want to try to keep alive rather than assuming the
        // child has exited
        if (readResult < 0) {
            break;
        }

        numBytesRead = readResult;

        if (numBytesRead > 0) {

            buf[numBytesRead] = '\0';
            printf("%s", buf);
        }
    }
}

int main(int argc, char **argv) {

    int returnCode = -1;

    int ptyMasterFd = posix_openpt(O_RDWR);
    checkReturn(ptyMasterFd, "posix_openpt");

    returnCode = grantpt(ptyMasterFd);
    checkReturn(returnCode, "grantpt");

    returnCode = unlockpt(ptyMasterFd);
    checkReturn(returnCode, "unlockpt");

    char *ptyFilePath = ptsname(ptyMasterFd);
    checkPointer(ptyFilePath, "pty file path");

    int ptySlaveFd = open(ptyFilePath, O_RDWR);
    checkReturn(ptySlaveFd, "open slave pty");

    pid_t processId = fork();

    if (processIsChild(processId)) {
        close(ptyMasterFd);

        char **command = &argv[1];
        const int commandLen = argc - 1;
        runChildProcess(ptySlaveFd, command, commandLen);
    }
    else {
        close(ptySlaveFd);
        runParentProcess(ptyMasterFd);
    }

    return 0;
}
