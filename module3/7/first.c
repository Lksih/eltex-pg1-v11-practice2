#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>
#include <unistd.h>

#define Q_A2B "/chat_a2b"
#define Q_B2A "/chat_b2a"
#define MSG_SIZE 256
#define TERM_PRIO 1
#define NORM_PRIO 0
#define MAX_MSG 10

int main()
{
    mqd_t send_q, recv_q;
    struct mq_attr attr;

    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = MSG_SIZE;

    send_q = mq_open(Q_A2B, O_WRONLY | O_CREAT, 0666, &attr);
    if (send_q == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    recv_q = mq_open(Q_B2A, O_RDONLY | O_CREAT, 0666, &attr);
    if (recv_q == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Чат A запущен. Введите 'exit' для выхода.\n");

    while (1)
    {
        char recieved[MSG_SIZE];
        char input[MSG_SIZE];
        unsigned int priority;

        printf("A: ");
        fflush(stdout);
        if (fgets(input, MSG_SIZE, stdin) == NULL)
        {
            if (mq_send(send_q, "exit", 4, TERM_PRIO) == -1)
            {
                perror("mq_send (exit)");
            }
            fprintf(stderr, "Ошибка чтения.\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            if (mq_send(send_q, "exit", 4, TERM_PRIO) == -1)
            {
                perror("mq_send (exit)");
            }
            printf("Завершение чата.\n");
            break;
        }

        if (mq_send(send_q, input, strlen(input) + 1, NORM_PRIO) == -1)
        {
            perror("mq_send");
            break;
        }

        ssize_t bytes = mq_receive(recv_q, recieved, MSG_SIZE, &priority);
        if (bytes == -1)
        {
            perror("mq_receive");
            break;
        }
        recieved[bytes] = '\0';

        if (priority == TERM_PRIO)
        {
            printf("B завершил чат. Выход.\n");
            break;
        }
        printf("B: %s\n", recieved);
    }

    mq_close(send_q);
    mq_close(recv_q);
    mq_unlink(Q_A2B);
    mq_unlink(Q_B2A);

    return 0;
}