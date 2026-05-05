#include "message.h"

int msqid;
int client_type;
int running = 1;

void *receiver_thread(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Использование: %s <номер_клиента>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    client_type = atoi(argv[1]);

    msqid = msgget(MSG_KEY, 0);
    if (msqid == -1)
    {
        perror("msgget (клиент)");
        fprintf(stderr, "Убедитесь, что сервер запущен и очередь создана.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, receiver_thread, NULL) != 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    printf("Клиент %d запущен. Введите 'shutdown' для выхода.\n", client_type);

    while (1)
    {
        char input[MSG_SIZE];
        message msg;
        fgets(input, MSG_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "shutdown") == 0)
        {
            msg.mtype = 10;
            snprintf(msg.mtext, MSG_SIZE, "%d:shutdown", client_type);
            if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1)
            {
                perror("msgsnd shutdown");
            }
            printf("Отключение...\n");
            break;
        }

        msg.mtype = 10;
        snprintf(msg.mtext, MSG_SIZE, "%d:%s", client_type, input);
        if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) == -1)
        {
            perror("msgsnd");
            break;
        }
    }

    running = 0;
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    printf("\nКлиент %d отключён.\n", client_type);
    return 0;
}

void *receiver_thread(void *arg)
{
    message msg;
    while (running)
    {
        if (msgrcv(msqid, &msg, MSG_SIZE, client_type, 0) == -1)
        {
            if (errno == EIDRM)
            {
                fprintf(stderr, "Очередь удалена сервером. Завершение.\n");
                break;
            }
            perror("msgrcv");
            continue;
        }
        printf("%s\n", msg.mtext);
        fflush(stdout);
    }
    return NULL;
}