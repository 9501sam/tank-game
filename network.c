#include <pthread.h>

#include "tankio.h"

void *recv_thread(void *arg)
{
    struct package pkg;

    while (recv(client_sock, &pkg, sizeof(pkg), 0)) {
        pthread_mutex_lock(&lock);
        switch (pkg.kind) {
        case NEW_TANK:
            add_enemy(&pkg.data.newtk);
            break;
        case TANK:
            int id = pkg.data.tk.id;
            tank oldtk = enemies[id];
            enemies[id] = pkg.data.tk;
            erase_tank(&oldtk);
            print_tank(&enemies[id]);
            refresh_screen();
            break;
        case BULLET:
            break;
        case ATTACKED:
            break;
        case DIE:
            break;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
