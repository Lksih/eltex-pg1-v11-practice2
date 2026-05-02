#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#define MSG_KEY 12345
#define MSG_SIZE 256

typedef struct {
    long mtype;
    char mtext[MSG_SIZE];
} message;

#endif