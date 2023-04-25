# include <string.h>
# include "server_tools.h"
# include "../globals/globals.h"
# include <stdio.h>
# include <malloc.h>

void test_create_chats();

int main() {
    test_create_chats();
}

void test_create_chats() {
    int chats_amount = 2;
    int max_users = 10;

    //printf("Init chats...\n");
    struct chat *chats = (struct chat *) malloc (sizeof(struct chat) * chats_amount);

    init_chats(chats, chats_amount);

    int error = 0;
    for (int i = 0; i < chats_amount; ++i) {

        //fprintf(stderr, "Test chat #%d\n", i);

        //perror("Testing chat ID...\n");
        if ( chats[i].id != START_CHAT_CHANNEL + i) {
            perror("Chats has wrong IDs!\n");
            ++error;
            break;
        }

        //printf("Testing chat max_users amount...\n");
        if ( chats[i].max_users != max_users ) {
            printf("Chats has wrong max_users amount!\n");
            ++error;
            break;
        }

        //printf("Testing chat current_users amount...\n");
        if ( chats[i].current_users != 0 ) {
            printf("Chats has wrong current_users amount!\n");
            ++error;
            break;
        }

        char empty[32] = "";

        //perror("Testing chat users...\n");
        for (int j = 0; j < max_users; ++j) {
            //printf("Testing user #%d\n", j);

            //printf("Testing user username \n", j);
            if ( strcmp(chats[i].users[j].username, empty) != 0 ) {
                perror("Username must be empty by default!\n");
                fprintf(stderr, "Expected: '%s'\n", empty);
                fprintf(stderr, "Got: '%s'\n", chats[i].users[j].username);
                ++error;
                break;
            }
            //printf("Testing user msq_channel_id \n", j);
            if ( chats[i].users[j].msq_channel_id != -1 ) {
                perror("msq_channel_id must be -1  by default!\n");
                ++error;
                break;
            }
        }
        
    }

    if ( !error ) {
        printf("Test passed!\n");
    }
    else printf("\nERROR! TESTS ARE NOT PASSED!\n");

}
/*
void init_chats(struct chat *chats, int chats_amount) {
    // инициализация доступных чатов 
    for (int i = 0; i < chats_amount; ++i) {
        chats[i].id = START_CHAT_CHANNEL + i;
        chats[i].max_users = 10;
        chats[i].users = (struct user*) malloc(sizeof(struct user) * chats[i].max_users);
        for (int j = 0; j < chats[i].max_users; ++j) {
            memset(chats[i].users[j].username, 32, 0);
        }
        chats[i].current_users = 0;
    }
}

int is_user_in_chat(char *username, struct chat *chats, int chat_id) {
    // проверка, есть ли другой пользователь с username в чате

    int c = chats[chat_id - START_CHAT_CHANNEL].current_users;
    for (int i = 0; i < chats[chat_id - START_CHAT_CHANNEL].current_users; ++i) {
        if ( strcmp(chats[chat_id - START_CHAT_CHANNEL].users[i].username, username) == 0) return 1;
    }

    return 0;
}
*/
