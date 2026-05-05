#ifndef _COMENTO_H_
#define _COMENTO_H_

#include <unistd.h>

int encrypt(uid_t uid, const char *in, size_t in_len, char *out, size_t *out_len);
int decrypt(uid_t uid, const char *in, size_t in_len, char *out, size_t *out_len);

int create_keyring(uid_t uid);
int destroy_keyring(uid_t uid);

#endif
