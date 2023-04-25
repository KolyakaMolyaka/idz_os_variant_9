# include "../utils/msglib.h"
# include "../globals/globals.h"
# include "server_tools.h"
# include <malloc.h>
# include <unistd.h> // sleep
# include <string.h>

# define START_CHAT_CHANNEL 3

void init_chats(struct chat *chats, int chats_amount) {
    // инициализация доступных чатов 
    for (int i = 0; i < chats_amount; ++i) {
        chats[i].id = START_CHAT_CHANNEL + i;
        chats[i].max_users = 10;
        chats[i].current_users = 0;

        chats[i].users = (struct user*) malloc(sizeof(struct user) * chats[i].max_users);
        for (int j = 0; j < chats[i].max_users; ++j) {
            memset(chats[i].users[j].username, 32, 0);
            chats[i].users[j].msq_channel_id = -1;
        }
    }
};

int is_user_in_chat(char *username, struct chat *chats, int chat_id) {
    // проверка, есть ли другой пользователь с username в чате

    const int chat_ind = chat_id - START_CHAT_CHANNEL;
    int c = chats[chat_ind].current_users;
    for (int i = 0; i < chats[chat_ind].current_users; ++i) {
        if ( strcmp(chats[chat_ind].users[i].username, username) == 0) return 1;
    }

    return 0;
}

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
    static int GID_COUNTER = 3 + 3 + 7;

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


