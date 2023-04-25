# include <unistd.h>
# include <malloc.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include "client_tools.h"
# include "../utils/msglib.h"

# define AUTH_CHANNEL 2
# define START_CHAT_CHANNEL 3

int main() {

    // получение адреса сервера
    key_t secret_msq_key = ftok(MESSAGES_QUEUE_KEY, 0);

    // подключение к серверу
    int msqid = open_queue(secret_msq_key);

    // конфигурация пользователя
    //int chat_id = START_CHAT_CHANNEL;   // 
    int communicate_channel = -1;
    char *username = (char*)malloc(sizeof(char)*32) ;
    memset(username, 32, 0);

    // чат, к которому хочет присоединиться пользователь
    int requested_chat;
    {
        int was_error = 0;
        do {
            if ( was_error ) {
                printf("Can't connect to chat with id = %d \n", requested_chat);
            }
            printf("Enter chat id you want to connect: ");
            scanf("%d", &requested_chat);
            getchar();
            was_error = 1;
        } while ( !is_valid_chat_id(requested_chat) ); 
    }

    printf("CLIENT ON.\n");
    int logged_in = 0;
    for (;;) {

        struct mymsgbuf qbuf;

        if ( !logged_in ) {
            communicate_channel = request_for_signin_chat(requested_chat, msqid, username);

            if ( communicate_channel == 0) {
                printf("You are NOT logged in chat with id %d. Maybe, the same user exists.\n", requested_chat);
            // даем пользователю еще одну попытку авторизации
                continue;
            } else {
                printf("You are now logged in chat with id %d.\n", requested_chat);
                logged_in = 1;
            }

        }

        // авторизованный пользователь
        
        printf("Your channel is: %d\n", communicate_channel);
        // чтение сообщений из чата
        int res = read_message_no_wait(msqid, communicate_channel, &qbuf);
        if (res != -1) {
            printf("Got message from chat with id = %d.\n", requested_chat);
            printf("%s sent: %s\n", qbuf.username, qbuf.msgtext);
            while ( res =  read_message_no_wait(msqid, communicate_channel, &qbuf) != -1 ) {
                printf("%s sent: %s", qbuf.username, qbuf.msgtext);
                printf("\n");
            }
        
        }


        
        // пользователь вводит сообщение для отправки
        printf("Type message to send in chat: ");

        memset(qbuf.msgtext, 64, 0);
        fgets(qbuf.msgtext, 64, stdin);
        qbuf.msgtext[strcspn(qbuf.msgtext, "\n")] = 0;

        // установка чата, в который будет отправлено сообщение
        qbuf.mtype = requested_chat;
        qbuf.chat_id = requested_chat;
        qbuf.auth = communicate_channel;
        strcpy(qbuf.username, username);
        // отправка сообщения в чат
        send_message(msqid, &qbuf);
    }

    // осовбождение памяти
    // пользователей в чатах
    // чатов
    return 0;
}
