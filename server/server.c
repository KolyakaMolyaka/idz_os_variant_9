# include <unistd.h>
# include <malloc.h>
# include <string.h>
# include "../utils/msglib.h"
# include "../globals/globals.h"
# include "server_tools.h"

int GID_COUNTER = 3 + 3 + 7;

int main() {
    struct chat *chats = (struct chat*) malloc (sizeof(struct chat) * CHATS_AMOUNT);
    init_chats(chats, CHATS_AMOUNT);

    printf("created chats:\n");
    //print_chats(chats, CHATS_AMOUNT);

    key_t secret_msq_key = ftok(MESSAGES_QUEUE_KEY, 0);
    int msqid = open_queue(secret_msq_key);
    // очистка предыдущей работы
    {
        struct mymsgbuf qbuf;
        while ( read_message_no_wait(msqid, 0, &qbuf) != -1 );
    }

    printf("SERVER ON.\n");
    for (;;) {
        struct mymsgbuf qbuf;

        // чтение сообщения из любого канала
        printf("waiting for the message in queue...\n");


        int res = -1;
        int channel = -1;
        while (res == -1) {
            sleep(1);
            printf("Waiting for requests...");
            for (int chan = AUTH_CHANNEL; chan < START_CHAT_CHANNEL + CHATS_AMOUNT; ++chan)  {
                //printf("read channel: %d\n", chan);
                res = read_message_no_wait(msqid, chan, &qbuf);
                if (res != -1) {
                    printf("got message from channel %d\n", chan);
                    channel = chan;
                    break;
                }
                //printf("channel %d is empty\n", chan);
            }
            printf("Sleep for 2 sec...\n");
        }

        //read_message(msqid, channel, &qbuf);

        //printf("Got qbuf\n");
        //print_qbuf(&qbuf);

        if (channel == 2) {
            // авторизация пользователя

            printf("Validating username...\n");
            int invalid_username = is_user_in_chat(qbuf.username, chats, qbuf.chat_id);
            if ( invalid_username ) {
                // пользователь уже в чате
                struct mymsgbuf qbuf;
                qbuf.auth = 0;
            } 
            else
            {
                printf("Register user...\n");
                // пользователь добавлени в чат
                qbuf.auth = ++GID_COUNTER;
                int chat_ind = qbuf.chat_id - START_CHAT_CHANNEL;
                int cur_user  = chats[chat_ind].current_users;
                strcpy(chats[chat_ind].users[cur_user].username, qbuf.username);
                chats[chat_ind].users[cur_user].msq_channel_id = qbuf.auth;
                chats[chat_ind].current_users++;

                printf("User %s now be added.\n", qbuf.username);

            }

            // отправка ответа пользователю
            qbuf.mtype = 1;
            send_message(msqid, &qbuf);
        }
        else {
            // рассылка сообщений остальным пользователям
            
            // айди не канала чата qbuf.chat_id;
            // айди канала отправителя qbuf.auth
            // сообщение посланное в чат qbuf.msgtext
            //
           
            
            printf("Chat id: %d\n", qbuf.chat_id);
            printf("Chat id - START_CHAT_CHANNEL: %d\n", qbuf.chat_id - START_CHAT_CHANNEL);

            int chat_ind = START_CHAT_CHANNEL - qbuf.chat_id;
            int receiving_users = chats[chat_ind].current_users; 
            printf("Users who should  get message: %d\n", receiving_users);
            for (int i = 0; i < receiving_users; ++i) {
                int rcv_channel = chats[chat_ind].users[i].msq_channel_id;
                if ( rcv_channel == qbuf.auth ) {
                    printf("One user don't get message becase he sent it.\n");
                    continue;
                }
                printf("Sending msg for user %d\n", rcv_channel) ;
                struct mymsgbuf qbuffer;
                qbuffer.mtype = rcv_channel;
                strcpy(qbuffer.msgtext, qbuf.msgtext);
                strcpy(qbuffer.username, qbuf.username); // пользователь, отправивший сообщение
                send_message_no_wait(msqid, &qbuffer);
            }
            
            

            printf("Got message from user: %s\n", qbuf.username);
            printf("Message is: %s\n", qbuf.msgtext);
        }

    }

    // осовбождение памяти
    // пользователей в чатах
    // чатов
    if (remove_queue(msqid) != -1 ) printf("SERVER OFF.\n");
    return 0;
}
