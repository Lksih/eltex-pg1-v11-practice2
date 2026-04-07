#include <stdio.h>
#include <string.h>
#include <mqueue.h>

#define OUTCOMING_QUEUE_NAME "/first_queue"
#define INCOMING_QUEUE_NAME "/second_queue"
#define PRIORITY 1
#define SIZE 256

int main()
{
    mqd_t ds_outcoming;
    mqd_t ds_second;

    char text[] = "Hello Posix World";
    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = 32;
    queue_attr.mq_msgsize = SIZE;
    if ((ds_outcoming = mq_open(OUTCOMING_QUEUE_NAME, O_CREAT | O_RDWR, 0600, &queue_attr)) == (mqd_t)-1)
    {
        perror("Ошибка открытия выходной очереди");
        return -1;
    }

    if ((ds_incoming = mq_open(INCOMING_QUEUE_NAME, O_RDWR | O_NONBLOCK, 0600, NULL)) == (mqd_t)-1)
    {
        perror("Ошибка открытия входной очереди");
        return -1;
    }

    if (mq_send(ds_outcoming, text, strlen(text), PRIORITY) == -1)
    {
        perror("Sending message error");
        return -1;
    }

    if (mq_close(ds_incoming) == -1)
    {
        perror("Ошибка закрытия входной очереди");
    }

    if (mq_close(ds_outcoming) == -1)
    {
        perror("Ошибка закрытия выходной очереди");
    }
    return 0;
}