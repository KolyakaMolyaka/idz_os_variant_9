# include "server_tools.h"
# include <malloc.h>
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
}

int is_user_in_chat(char *username, struct chat *chats, int chat_id) {
    // проверка, есть ли другой пользователь с username в чате

    const int chat_ind = chat_id - START_CHAT_CHANNEL;
    int c = chats[chat_ind].current_users;
    for (int i = 0; i < chats[chat_ind].current_users; ++i) {
        if ( strcmp(chats[chat_ind].users[i].username, username) == 0) return 1;
    }

    return 0;
}
