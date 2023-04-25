# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "client_tools.h"
# include <unistd.h> // sleep
# include "../utils/msglib.h"

# define START_CHAT_CHANNEL 3
# define  CHATS_AMOUNT 3
# define AUTH_CHANNEL 2

int is_valid_chat_id(int chat_id) {
    // проверка, существует ли чат с таким id
    if (chat_id < START_CHAT_CHANNEL) return 0;
    if (chat_id > START_CHAT_CHANNEL + CHATS_AMOUNT) return 0;
    return 1;
}

int request_for_signin_chat(int chat_id, int msqid, char *username) {
    struct mymsgbuf qbuf;
    qbuf.mtype = AUTH_CHANNEL;      // авторизация происходит в выделенном канале
    qbuf.auth = 0;                  // установка флага авторизации
    qbuf.chat_id = chat_id;         // указание чата для авторизации

    printf("Enter username: ");

    memset(username, 32, 0);
    fgets(username, 32, stdin);
    username[strcspn(username, "\n")] = 0; // удаление \n в конце

    strcpy(qbuf.username, username);
    send_message(msqid, &qbuf);     // отправка запроса авторизации
    read_message(msqid, 1, &qbuf);  // получение ответа о прохождении авторизации

    if (qbuf.auth != 0) {
        // авторизация успешна, 
        // возвращается выделенный канал для получения сообщений из чата
        return qbuf.auth;
    }
    else {
        return 0;
    }
}



