#ifndef POSTMAN_COMMANDO_H
#define POSTMAN_COMMANDO_H

#define PORT 12345

int PostmanCommando_new(void);
extern void sendMsg();
extern void *receiveMsg(void *params);
void PostmanCommando_free(void);

#endif
