# ifndef SERVER_TOOLS_H_
# define SERVER_TOOS_H_

// структура авторизованного пользователя 
struct user {
    char username[32];      // имя пользователя
    int msq_channel_id;     // номер канала в очереди сообщений для отправки сообщений из чата
};

// информация о доступных чатах
struct chat {
    int id;                 // айди канала в очереди сообщений
    struct user *users;     // пользователи привязанные к конкретному чату
    int max_users;          // максимальное количество пользователей в чате
    int current_users;      // текущее количетсво пользователей в чате
};

void init_chats(struct chat *chats, int chats_amount);
int is_user_in_chat(char *username, struct chat *chats, int chat_id);
# endif
