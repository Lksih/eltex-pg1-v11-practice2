#include "message.h"

#define MAX_CLIENTS 100

int msqid;
int clients[MAX_CLIENTS];
unsigned int client_count = 0;

void cleanup(int signum);
void add_client(int type);
void remove_client(int type);
void broadcast(int sender_type, const char *msg_text);

int main()
{
    msqid = msgget(MSG_KEY, IPC_CREAT | IPC_EXCL | 0666);
    if (msqid == -1)
    {
        perror("msgget (сервер)");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, cleanup);
    printf("Групповой чат сервер запущен. Для завершения нажмите Ctrl+C\n");

    while (1)
    {
        message rcv_msg;
        int sender_type;

        if (msgrcv(msqid, &rcv_msg, MSG_SIZE, 10, 0) == -1)
        {
            perror("msgrcv");
            continue;
        }

        char *colon = strchr(rcv_msg.mtext, ':');
        if (!colon)
        {
            fprintf(stderr, "Неправильный формат сообщения: %s\n", rcv_msg.mtext);
            continue;
        }
        *colon = '\0';
        sender_type = atoi(rcv_msg.mtext);

        add_client(sender_type);

        if (strcmp(colon + 1, "shutdown") == 0)
        {
            remove_client(sender_type);
            continue;
        }

        broadcast(sender_type, colon + 1);
    }

    return 0;
}

void cleanup(int signum)
{
    msgctl(msqid, IPC_RMID, NULL);
    printf("\nСервер завершён, очередь удалена.\n");
    exit(EXIT_SUCCESS);
}

void add_client(int type)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] == type)
        {
            return;
        }
    }
    if (client_count < MAX_CLIENTS)
    {
        clients[client_count] = type;
        client_count++;

        char con_msg[MSG_SIZE];
        snprintf(con_msg, MSG_SIZE, "Клиент %d подключился.", type);
        printf("%s\n", con_msg);
        broadcast(type, con_msg);
    }
}

void remove_client(int type)
{
    for (unsigned int i = 0; i < client_count; i++)
    {
        if (clients[i] == type)
        {
            for (unsigned int j = i, lim = client_count - 1; j < lim; j++)
            {
                clients[j] = clients[j + 1];
            }
            client_count--;

            char discon_msg[MSG_SIZE];
            snprintf(discon_msg, MSG_SIZE, "Клиент %d отключён.", type);
            printf("%s\n", discon_msg);
            broadcast(type, discon_msg);
            return;
        }
    }
}

void broadcast(int sender_type, const char *msg_text)
{
    message out_msg;
    char full_msg[MSG_SIZE];
    snprintf(full_msg, MSG_SIZE, "\nКлиент %d: %s", sender_type, msg_text);
    out_msg.mtype = 0;
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] != sender_type)
        {
            out_msg.mtype = clients[i];
            strncpy(out_msg.mtext, full_msg, MSG_SIZE);
            if (msgsnd(msqid, &out_msg, strlen(out_msg.mtext) + 1, 0) == -1)
            {
                perror("msgsnd broadcast");
            }
        }
    }
}