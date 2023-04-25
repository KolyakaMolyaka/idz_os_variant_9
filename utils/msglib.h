#ifndef MSGLIB_H_
#define MSGLIB_H_ 

#include <time.h>
#include <sys/ipc.h>

#define MESSAGES_QUEUE_KEY "message_secret"

struct mymsgbuf {
    long mtype;         // номер канала в очереди сообщений
    char username[32];  // имя пользователя 
    int chat_id;        // номер канала в очереди сообщений 
    int auth;           // 0 - прохождение авторизации, иначе номер канала для чтения сообщений из чата
    char msgtext[64];   // сообщение пользователя
};

int open_queue(key_t keyval);
int send_message(int qid, struct mymsgbuf *qbuf);
int send_message_no_wait(int qid, struct mymsgbuf *qbuf);
int read_message(int qid, long type, struct mymsgbuf *qbuf);
int read_message_no_wait(int qid, long type, struct mymsgbuf *qbuf);
//int peek_message(int qid, long type);
//int get_queue_ds(int qid, struct msgqid_ds *qbuf);
//int change_queue_mode(int qid, char *mode);
int remove_queue(int qid);

#endif



