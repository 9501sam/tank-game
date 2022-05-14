#include "tankio.h"

void *recv_thread(void *arg)
{
    struct package pkg;
    int id;

    while (recv(client_sock, &pkg, sizeof(pkg), 0)) {
        pthread_mutex_lock(&lock);
        switch (pkg.kind) {
        case NEW_TANK:
            id = pkg.data.tk.id;
            add_enemy(&pkg.data.tk);
            attron_tank(id);
            print_tank(&pkg.data.tk);
            attroff_tank(id);
            refresh_screen();
            break;
        case TANK:
            id = pkg.data.tk.id;
            tank oldtk = enemies[id];
            enemies[id] = pkg.data.tk;
            erase_tank(&oldtk);
            attron_tank(id);
            print_tank(&enemies[id]);
            attroff_tank(id);
            refresh_screen();
            break;
        case BULLET:
            break;
        case ATTACKED:
            break;
        case DIE:
            id = pkg.data.die_id;
            tank dietk = enemies[id];
            if (dietk.id != NOT_USED) {
                erase_tank(&dietk);
                refresh_screen();
            }
            break;
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
