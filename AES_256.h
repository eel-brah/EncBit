#include<stdio.h>


void AES_256_Encryption(unsigned char *plan, unsigned char *key);
void AES_256_Decryption(unsigned char *cipher, unsigned char *key);
void keyExpantion(unsigned char *inputKey, unsigned char *outputKey);
unsigned char Rcon(int in);
void aes_encryption(unsigned char *plan, unsigned char *key);
void addRoundKey(unsigned char *state, unsigned char *rkey);
void subBytes(unsigned char *state);
void shiftRows(unsigned char *state);
void mixColumns(unsigned char *state);
void aes_decryption(unsigned char *cipher, unsigned char *key);
void invSubBytes(unsigned char *state);
void invShiftRows(unsigned char *state);
void invMixColumns(unsigned char *state);