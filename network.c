#include "tankio.h"

#define BUF_SIZE 16

int recv_packet(int fd, struct packet *pkt)
{
    char buffer[BUF_SIZE];
    int ret;
    packet_kind k;
    if ((ret = recv(fd, buffer, sizeof(buffer), 0)) <= 0)
        return ret;
    k = buffer[0];
    pkt->kind = k;
    switch (k) {
    case NEW_TANK:
    case TANK:
    case SHOOT:
        // uint16_t x;
        pkt->data.tk.x = (((uint16_t) buffer[1]) << 8) | (uint16_t) buffer[2];
        // pkt->data.tk.x = (uint16_t) buffer[2];

        // uint16_t y;
        pkt->data.tk.y = (((uint16_t) buffer[3]) << 8) | (uint16_t) buffer[4];
        // uint8_t dir;
        pkt->data.tk.dir = buffer[5];
        // int8_t hp;
        pkt->data.tk.hp = buffer[6];
        // int8_t nblts;
        pkt->data.tk.nblts = buffer[7];
        // int8_t id;
        pkt->data.tk.id = buffer[8];
        break;
    case REFILL:
    case ATTACKED:
    case DIE:
        pkt->data.id = buffer[1];
        break;
    default:
        return -1;
    }
    return ret;
}

int send_packet(int fd, struct packet *pkt)
{
    char buffer[BUF_SIZE];
    packet_kind k = pkt->kind;
    // uint8_t kind
    buffer[0] = k;
    switch (k) {
    case NEW_TANK:
    case TANK:
    case SHOOT:
        // uint16_t x;
        buffer[1] = pkt->data.tk.x >> 8;
        buffer[2] = pkt->data.tk.x;
        // uint16_t y;
        buffer[3] = pkt->data.tk.y >> 8;
        buffer[4] = pkt->data.tk.y;
        // uint8_t dir;
        buffer[5] = pkt->data.tk.dir;
        // int8_t hp;
        buffer[6] = pkt->data.tk.hp;
        // int8_t nblts;
        buffer[7] = pkt->data.tk.nblts;
        // int8_t id;
        buffer[8] = pkt->data.tk.id;
        break;
    case REFILL:
    case ATTACKED:
    case DIE:
        buffer[1] = pkt->data.id;
        break;
    default:
        return -1;
    }
    return send(fd, buffer, sizeof(buffer), 0);
}
