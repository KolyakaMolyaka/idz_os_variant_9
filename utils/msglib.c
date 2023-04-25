#include "msglib.h"
#include <sys/ipc.h>
#include <sys/msg.h>

int open_queue(key_t keyval) {
    // открытие или создание очереди сообщений
    int qid;
    if ((qid = msgget( keyval, IPC_CREAT | 0660 )) == -1) return (-1);
    return qid;
}

int send_message(int qid, struct mymsgbuf *qbuf) {
    // отправка сообщения в очередь
    int result, length;
    length = sizeof(struct mymsgbuf) - sizeof(long);
    if ((result = msgsnd( qid, qbuf, length, 0)) == -1) return (-1);
    return result;
}

int send_message_no_wait(int qid, struct mymsgbuf *qbuf) {
    // отправка сообщения в очередь
    int result, length;
    length = sizeof(struct mymsgbuf) - sizeof(long);
    if ((result = msgsnd( qid, qbuf, length, IPC_NOWAIT)) == -1) return (-1);
    return result;
}


int read_message(int qid, long type, struct mymsgbuf *qbuf) {
    // чтение сообщения из очереди
    int result, length;
    length = sizeof(struct mymsgbuf) - sizeof(long);
    if ((result = msgrcv(qid, qbuf, length, type, 0)) == -1) return (-1);
    return result;
}

int read_message_no_wait(int qid, long type, struct mymsgbuf *qbuf) {
    // чтение сообщения из очереди
    int result, length;
    length = sizeof(struct mymsgbuf) - sizeof(long);
    if ((result = msgrcv(qid, qbuf, length, type, IPC_NOWAIT)) == -1) return (-1);
    return result;
}


/*
int peek_message(int qid, long type) {
    int result, length;
    if ((result = msgrcv(qid, NULL, 0, type, IPC_NOWAIT)) == -1) {
        if (errno == E2BIG) return 1;
    }
    return 0;
}

int get_queue_ds(int qid, struct msgqid_ds *qbuf) {
    if (msgctl(qid, IPC_STAT, qbuf) == -1) return (-1);
    return 0;
}

int change_queue_mode(int qid, char *mode) {
    struct msgqid_ds tmpbuf;
    get_queue_ds(qid, &tmpbuf);
    sscanf(mode, "%ho", &tmpbuf.msg_perm.mode);
    if (msgctl(qid, IPC_SET, &tmpbuf) == -1) return (-1);
    return 0;
}
*/
int remove_queue(int qid) {
    if (msgctl(qid, IPC_RMID, 0) == -1) return (-1);
    return 0;
}
