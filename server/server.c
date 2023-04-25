# include <unistd.h>
# include <malloc.h>
# include <string.h>
# include "../utils/msglib.h"
# include "../globals/globals.h"
# include "server_tools.h"

int GID_COUNTER = 3 + 3 + 7;

int listen(int msqid, struct mymsgbuf *request) {
    int res = -1;       // -1 означает, что новых сообщений нет
    int channel = -1;   // в случае, res != -1, означает каннал из-которого пришло сообщение

    while (res == -1) {
        sleep(1);
        printf("Waiting for requests...");
        // прослушивание каналов: авторизации, чата №1, чата №2, ...
        for (int chan = AUTH_CHANNEL; chan < START_CHAT_CHANNEL + CHATS_AMOUNT; ++chan)  {
            res = read_message_no_wait(msqid, chan, request);
            
            // после первого полученного сообщения начинается его обработка
            if (res != -1) {
                channel = chan;
                break;
            }
        }
        printf("Sleep for 1 sec...\n");
    }

    return channel;
}

int convert_chat_id_to_chat_index(int chat_id) {
    if (chat_id < START_CHAT_CHANNEL) return -1;
    if (chat_id > START_CHAT_CHANNEL + CHATS_AMOUNT) return -1;
    return chat_id - START_CHAT_CHANNEL;
}


void authentificate_user(struct mymsgbuf *request, struct mymsgbuf *response, struct chat *chats) {
    // проверка, что пользователь с таким же именем отстутствует в чате 
    printf("Validating username...\n");
    int invalid_username = is_user_in_chat(request->username, chats, request->chat_id);

    if ( invalid_username ) {
        // Пользователь с таким именем уже присутствует в чате - отмена добавления
        response->auth = 0; // установка флага неуспешной авторизации
    } else {
        // Добавление пользователя в чат 
        printf("Register user...\n");

        // канал для отправки сообщений, отправленных в чат пользователя
        response->auth = ++GID_COUNTER;
        
        int chat_ind = convert_chat_id_to_chat_index(request->chat_id);
        int cur_user  = chats[chat_ind].current_users;

        // обновление данных о чате (добавление нового пользователя
        strcpy(chats[chat_ind].users[cur_user].username, request->username);
        chats[chat_ind].users[cur_user].msq_channel_id = response->auth;
        // обновление счетчика текущих пользователей 
        chats[chat_ind].current_users++;
        printf("User '%s' connected to the chat %d.\n", response->username, request->chat_id);
    }

    // установка канала доставки "ответа от запроса авторизации"
    response->mtype = 1;

};
void mail_users(struct mymsgbuf *request, int msqid, struct chat *chats) {
    int chat_ind,
        receiving_users;
    
    chat_ind = convert_chat_id_to_chat_index(request->chat_id);
    receiving_users = chats[chat_ind].current_users; 

    for (int i = 0; i < receiving_users; ++i) {
        int rcv_channel = chats[chat_ind].users[i].msq_channel_id;
        if ( rcv_channel == request->auth ) {
            //printf("The user don't get message becase he sent it.\n");
            continue;
        }
        //printf("Sending msg for user %d\n", rcv_channel) ;
        struct mymsgbuf response;
        response.mtype = rcv_channel;
        strcpy(response.msgtext, request->msgtext);
        strcpy(response.username, request->username); // пользователь, отправивший сообщение
        send_message_no_wait(msqid, &response);
    }
            
}

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
