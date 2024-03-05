// C Program for Message Queue (Writer Process)
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX 10

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} m1, m2, m3, m4;

int main()
{
    key_t key;
    int msgid;

    // ftok to generate unique key
    key = ftok("progfile", 65);

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);
    m1.mesg_type = 1;
    m2.mesg_type = 2;
    m3.mesg_type = 3;
    m4.mesg_type = 4;

    printf("Write Data : ");
    fgets(m1.mesg_text, MAX, stdin);
    fgets(m2.mesg_text, MAX, stdin);
    fgets(m3.mesg_text, MAX, stdin);
    fgets(m4.mesg_text, MAX, stdin);

    // msgsnd to send message
    msgsnd(msgid, &m1, sizeof(m1), 0);
    msgsnd(msgid, &m2, sizeof(m2), 0);
    msgsnd(msgid, &m3, sizeof(m3), 0);
    msgsnd(msgid, &m4, sizeof(m4), 0);

    // display the message
    // printf("Data send is : %s \n", message.mesg_text);

    return 0;
}
