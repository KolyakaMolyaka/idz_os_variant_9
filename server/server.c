# include <unistd.h>
# include <malloc.h>
# include <string.h>
# include "../utils/msglib.h"
# include "../globals/globals.h"
# include "server_tools.h"


int main() {
    // создание чатов для контроля сервером
    struct chat *chats = (struct chat*) malloc (sizeof(struct chat) * CHATS_AMOUNT);
    init_chats(chats, CHATS_AMOUNT);

    key_t secret_msq_key = ftok(MESSAGES_QUEUE_KEY, 0);

    // создание сервера 
    int msqid = open_queue(secret_msq_key);

    // очистка предыдущей работы сервера
    {
        struct mymsgbuf qbuf;
        while ( read_message_no_wait(msqid, 0, &qbuf) != -1 );
    }

    // запуск работы сервера
    printf("SERVER ON.\n");
    for (;;) {

        // запрос клиента для сервера
        struct mymsgbuf request;    

        // чтение сообщений поочередно из каждого  канала:
        // авторизации, чата №1, чата №2, ...
        int channel = listen(msqid, &request);

        if (channel == 2) { 
            // канал аутендификации пользователей
            
            struct mymsgbuf response;
            authentificate_user(&request, &response, chats);

            // отправка ответа пользователю
            send_message(msqid, &response);
        }
        else {
            // новое сообщение в чате.
            // рассылка сообщений остальным пользователям
            
            mail_users(&request, msqid, chats);

            //printf("Got message from user: %s\n", qbuf.username);
            //printf("Message is: %s\n", qbuf.msgtext);
        }

    }

    // осовбождение памяти
    // пользователей в чатах
    // чатов
    if (remove_queue(msqid) != -1 ) printf("SERVER OFF.\n");
    return 0;
}
