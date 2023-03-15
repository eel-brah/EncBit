#define _XOPEN_SOURCE // fileno(): _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<getopt.h>
#include<ctype.h>
#include<errno.h>
#include<termios.h>
#include <sys/stat.h>

#include "AES_256.h"
#include "SHA_256.h"

#define enc 0b1
#define dec 0b10
#define Min_nmb 1
#define Max_nmb 999999
#define nmb_defult_value 1
#define PasswordMaxLength 1001

int nmb = nmb_defult_value;
uint8_t flag, flags;

uint8_t open_dir(char *path, unsigned char *key);
uint8_t encrypt_decrypt_file(char *file, unsigned char *key);
void encrypt_text(char *text, unsigned char *key);
void decrypt_text(char *text, unsigned char *key,int text_lenght);
void hash_file(char *file);
void hash_data(char *data);
void print_hex(unsigned char x);
char *hexToText(int len, char **input, char *output);
char *get_str(char *str, int DEFAULT_BUFFER_SIZE);
int get_int(char *str);
char *generate_file_name(char *file);
char *edit_path(char *file, char *name);

int main(int argc, char *argv[]){

    // Options
    int c;
    while ((c = getopt (argc, argv, "edfkpHSorwh")) != -1)
    {
        switch (c)
        {
            case 'e': // encryption
                flag |= enc;
                break;
            case 'd': // decryption
                flag |= dec;
                break;
            case 'f': // only one file
                flags |= 0b1;
                break;
            case 'k': // use a keyfile
                flags |= 0b10;
                break;
            case 'H': // hash a file
                flags |= 0b100;
            break;
            case 'S': // hash a string
                flags |= 0b1000;
            break;
            case 'p': // use nmb
                flags |= 0b10000;
                break;
            case 'o': // keep the original file
                flags |= 0b100000;
                break;
            case 'r': // rename a file
                flags |= 0b1000000;
                break;
            case 'w': // display password
                flags |= 0b10000000;
                break;
            case 'h': // help
                printf("encrypt and decrypt a directory, file, or string using AES-256 using a password or a keyfile,"
                "and hash a file or a string using SHA-256.\nUsage: ./EncBit option *entity [keyfile]\n"
                "\n"
                "*entity could be a path, directory, file, or string.\n"
                "\n"
                "The keyfile is required as an argument when using the -k option, except in -dSk when a keyfile is used to decrypt a string.\n"
                "\n"
                "Options:\n"
                "-h help\n-e encrypt\n-d decrypt\n-f encrypt or decrypt a single file\n"
                "-k use a keyfile instead of a password\n-H hash a file\n"
                "-S hash a string\n-efH hash a file and encrypt it\n"
                "-eS encrypt a string\n-dS decrypt a string (string in hexadecimal without quotes (''))\n"
                "   e.g. ./EncBit -dS 1D C5 48 93 16 C8 08 6s 21 DA A2 E3 AD 8F 5C 7A\n"
                "-ekS encrypt a string with a keyfile\n-p use nmb\n"
                "-r name the output file\n-o keep the original file\n-w display the password and nmb\n");
                return 0;
            case '?':
            if (isprint (optopt))
              printf ("Unknown option `-%c'.\n", optopt);
            else
              printf ("Unknown option character `\\x%x'.\n", optopt);
            return 1;
        }
    }

    // Check inputs
    if(flag == 0b11){
        printf("Warning! -e and -d can't be used togather\nUse -h for help\n");
        return 1;
    }
    else if((flags & 0b100) && (flags & 0b1000)){
        printf("Warning! -H and -S can't be used togather\nUse -h for help\n");
        return 1;
    }
    else if((flags & 0b1) && (flags & 0b1000)){
        printf("Warning! -f and -S can't be used togather\nUse -h for help\n");
        return 1;
    }

    // Hash a file
    if((flags & 0b100) && !flag){
        if(argc != 3){
			printf("Warning! Incorrect number of arguments\nUsage: ./test -H file_name\n");
			return 1;
		}
        hash_file(argv[2]);
        return 0;
    }
    // Hash a string
    else if ((flags & 0b1000) && !flag){
        if(argc != 3 && argc != 2){
			printf("Warning! incorrect number of arguments\nUsage: ./test -S 'string'\n");
			return 1;
		}
		if(argc == 2) argv[2] = "";
        hash_data(argv[2]);
        return 0;
    }

    // Check inputs, without a keyfile argc must be 3 and with a keyfile it must be 4
    // Except in the case of decrypting a string using a keyfile
    if (!flag || (((flags & 0b10) != 0b10 || (argc != 4 && !(((flags & 0b1000) == 0b1000) && flag == dec))) && ((flags & 0b10) == 0b10 || (argc != 3 && !(((flags & 0b1000) == 0b1000) && flag == dec))))){
        printf("Warning! incorrect number of arguments\nUsage: ./test Option Entity [Keyfile]\nUse -h for help\n");
        return 1;
    }

    // Declare pointers
    unsigned char *key = NULL;
    char *password = NULL;

    // For disabling echo
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    uint8_t x;
    // If key file not used get password form user
    if ((flags & 0b10) != 0b10)
    {

        // Disabling echo (hide the password)
        if ((flags & 0b10000000) != 0b10000000){
            if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
                perror("tcsetattr");
            }
        }
        // Get a password
        do
        {
            password = get_str("Password: ", PasswordMaxLength);
            if(password == NULL){
                printf("Failed to get the password\n");
                return 1;
            }

            int passLen = strlen(password);
            x = passLen < 8;
            if(x){
                printf("Your password must be at least 8 characters\n");
                free(password);
                password = NULL;
            }
        }while (x);

        // Restore terminal
        if((flags & 0b10000000) != 0b10000000){
            if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
                perror("tcsetattr");
            }
            printf("\n");
        }

        // Hash the password to get the key
        key = sha_256_data(password);
        if(key == NULL){
            printf("Key generation failed\n");
            free(password);
            return 1;
        }
        free(password);
        password = NULL;
    }
    // Get a keyfile if decrypting a string with keyfile
    else if((flags & 0b10) == 0b10 && (flags & 0b1000) == 0b1000 && flag == dec)
    {
        // Get a keyfile from the user
        char *key_file = get_str("Keyfile: ", 100);
        if(key_file == NULL){
            printf("Failed to get the keyfile\n");
            return 1;
        }
        // Hash the keyfile
        key = sha_256_file(key_file);
        free(key_file);
        key_file = NULL;
        if(key == NULL){
            printf("Key generation failed\n");
            return 1;
        }
    }
    else // If keyfile used
    {
        // Hash the keyfile
        key = sha_256_file(argv[3]);
        if(key == NULL){
            printf("Key generation failed\n");
            return 1;
        }
    }

    // Get MNB
    if ((flags & 0b10000) == 0b10000)
    {
        // Disabling echo (hide nmb)
        if ((flags & 0b10000000) != 0b10000000){
            if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
                perror("tcsetattr");
            }
        }
        // Get nmb from the user
        do
        {
            nmb = get_int("nmb: ");
            x = nmb > Max_nmb || nmb < Min_nmb;
            if(x)
                printf("nmb must be between 1 and 999999\n");
        }while(x);

        // Restore terminal
        if((flags & 0b10000000) != 0b10000000){
            if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
                perror("tcsetattr");
            }
            printf("\n");
        }
    }

    // Encrypt or Decrypt a string
    if((flags & 0b1000) == 0b1000){
        char *str = NULL;
        if (flag == enc) // Encrypt
            encrypt_text(argv[2], key);
        else if (flag == dec) // Decrypt
        {
            // Check inputs
            if(argc < 3){
                printf("Usage: ./test -dS[kpw] string in hexadecimal\nUse -h for help\n");
                free(key);
                return 1;
            }
            // Convert input from hexadecimal to ASCII text
            str = hexToText(argc-2, argv+2, str);
            if(str == NULL){
                printf("Failed to Convert input from hexadecimal to ASCII text\n");
                free(key);
                return 1;
            }
            decrypt_text(str, key, argc-2);
        }

        free(key);
        free(str);
        return 0;
    }

    // Ecrypt or Decrypt a directory
    uint8_t failed = 0;
    if ((flags & 0b1) != 0b1)
        failed = open_dir(argv[2], key);
    else //Ecrypt or Decrypt a file
    {
        // hash a file
        if((flags & 0b100) == 0b100) hash_file(argv[2]);
        failed = encrypt_decrypt_file(argv[2], key);
    }

    if(failed)
    {
        printf("Encryption/Decreption failed\n");
        free(key);
        return 1;
    }

    free(key);
    return 0;
}

char *hexToText(int len, char **input, char *output){
    int plen = len;

    // Make sure the length of the hexadecimal input is a multiple of 16
    while(plen % 16 != 0)
        plen++;

    output = (char *)malloc(plen+1);
    if (output == NULL) {
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        return NULL;
    }

    for (int i = 0; i < len; i++)
        output[i] = strtol(input[i], NULL, 16) & 0xff;

    // If the length wasn't a multiple of 16, fill in the difference with zeros
    if(len != plen)
    {
        for(; len < plen; len++)
            output[len] = 0;
    }

    output[plen] = '\0';
    return output;
}

void hash_file(char *file){

    unsigned char *file_Hash = sha_256_file(file);
    if(file_Hash == NULL){
        printf("File hashing failed\n");
        return;
    }

    // Print hash in hex
    printf("Hash of %s: ", file);
	for(int i = 0; i < 32; i++){
		print_hex(file_Hash[i]);
		printf(" ");
	}

	printf("\n");
    free(file_Hash);
}

void hash_data(char *data){
    unsigned char *data_Hash = sha_256_data(data);
    if(data_Hash == NULL){
        printf("String hashing failed\n");
        return;
    }

    // Print hash in hex
	for(int i = 0; i < 32; i++){
		print_hex(data_Hash[i]);
		printf(" ");
	}

	printf("\n");
    free(data_Hash);
}

void encrypt_text(char *text, unsigned char *key){
    // Text lenght
    int text_lenght = strlen(text);

    // Encrypted text lenght
    int ptext_lenght = text_lenght;
    while(ptext_lenght % 16 != 0)
        ptext_lenght++;

    // Allocate memory
    unsigned char *encrypted_text = (unsigned char *)malloc(ptext_lenght * sizeof(unsigned char));
    if(encrypted_text == NULL){
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        printf("Encryption failed\n");
        return;
    }

    int p = 0;
    unsigned char c[16];
    unsigned int N = nmb;
    int remain_text_lenght = text_lenght;
    while(remain_text_lenght > 0){
        // Copy text to c[16] to encrypt it
        if(remain_text_lenght >= 16)
        {
            memcpy(c, &text[text_lenght-remain_text_lenght], 16);
            remain_text_lenght-= 16;
        }
        else // Padding
        {
            for (int i = 0; i < 16; i++)
            {
                c[i] = text[text_lenght-remain_text_lenght];
                remain_text_lenght--;
                if(remain_text_lenght == 0)
                {
                    for(int j = i+1; j < 16; j++, i++)
                            c[j] = 0;
                }
            }
        }
        // Encrypt
        if(N > 1){
            for(int i = 0; i < 32; i++)
                key[i] = key[i] ^ N--;
            if(N == 1) N = nmb;
        }
        for(int i = 0; i < nmb; i++)
            AES_256_Encryption(c, key);
        memcpy(&encrypted_text[p], c, 16);
        p += 16;
    }

    // Print hash in hex
    printf("Encrypted string: ");
	for(int i = 0; i < ptext_lenght; i++){
		print_hex(encrypted_text[i]);
		printf(" ");
	}

	printf("\n");

    free(encrypted_text);
}

void decrypt_text(char *text, unsigned char *key, int text_lenght){

    // Make text_lenght a multiple of 16
    while(text_lenght % 16 != 0)
        text_lenght++;

    // Allocate memory
    unsigned char *decrypted_text = (unsigned char *)malloc((text_lenght+1) * sizeof(unsigned char));
    if(decrypted_text == NULL){
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        printf("Decryption failed\n");
        return;
    }

    int p = 0;
    unsigned char c[16];
    unsigned int N = nmb;
    int remain_text_lenght = text_lenght;

    while(remain_text_lenght > 0){
        // Copy text to c[16] to decrypt it
        memcpy(c, &text[text_lenght-remain_text_lenght], 16);
        remain_text_lenght-= 16;

        // Decrypt
        if(N > 1){
            for(int i = 0; i < 32; i++)
                key[i] = key[i] ^ N--;
            if(N == 1) N = nmb;
        }
        for(int i = 0; i < nmb; i++)
            AES_256_Decryption(c, key);
        memcpy(&decrypted_text[p], c, 16);
        p += 16;
    }
    decrypted_text[text_lenght] = '\0';

    // Print the output
    printf("String: %s", decrypted_text);
	printf("\n");

    free(decrypted_text);
}

uint8_t open_dir(char *path, unsigned char *key){
    struct dirent *entry;
    DIR *dp;

    // Open directory
    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        printf("Use -f for a file\nUse -h for help\n");
        return 1;
    }

    int status;
    struct stat st_buf;

    uint8_t failed = 0;
    while((entry = readdir(dp)))
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            char ent[1000] = {0};
            strcat(ent, path);
            strcat(ent, "/");
            strcat(ent, entry->d_name);
            status = stat (ent, &st_buf); // Get the status
            if (status != 0) {
                printf ("Error %d\n", errno);
                return 1;
            }
            if (S_ISDIR (st_buf.st_mode)) // if it is a file, encrypt it
                failed = open_dir(ent, key);
            else // if it is a directory, open it
            {
                if((flags & 0b100) == 0b100) hash_file(ent);
                failed = encrypt_decrypt_file(ent, key);
            }

        }
    }

    closedir(dp);
    return failed;
}

uint8_t encrypt_decrypt_file(char *file, unsigned char *key){

    // Open a file for reading
    FILE *fps = fopen(file, "rb");
    if (fps == NULL){
        printf("File %s can't open\n", file);
        return 1;
    }

    // Open a file for writing
    FILE *fpo = fopen(".tmp_enc22", "wb");
    if (fpo == NULL){
        printf("Erorr\n");
        fclose(fps);
        return 1;
    }

    unsigned char c[16];
    unsigned int N = nmb;
    int len;
    if(flag == enc) // if encrypting a file
    {
        while((len = fread(c, sizeof(char), 16, fps)) == 16)
        {
            if(N > 1)
            {
                for(int i = 0; i < 32; i++)
                    key[i] = key[i] ^ N--;
                if(N == 1) N = nmb;
            }
            for(int i = 0; i < nmb; i++)
                AES_256_Encryption(c, key);

            fwrite(c, sizeof(char), 16, fpo);
        }
        if(len > 0) // Padding
        {
            if(N > 1){
                for(int i = 0; i < 32; i++)
                    key[i] = key[i] ^ N--;
                if(N == 1) N = nmb;
            }
            for(int i = len; i < 16; i++)
                c[i] = 0;

            for(int i = 0; i < nmb; i++)
                    AES_256_Encryption(c, key);

            fwrite(c, sizeof(char), 16, fpo);
        }
    }
    else if (flag == dec) // if decrypting a file
    {
        while(fread(c, sizeof(char), 16, fps)){
            if(N > 1){
                for(int i = 0; i < 32; i++)
                    key[i] = key[i] ^ N--;
                if(N == 1) N = nmb;
            }
            for(int i = 0; i < nmb; i++)
                AES_256_Decryption(c, key);

            fwrite(c, sizeof(char), 16, fpo);
        }
    }

    // close files
    fclose(fpo);
    fclose(fps);

    // Open for reading
    fpo = fopen(".tmp_enc22", "rb");
    if (fpo == NULL){
        printf("Erorr\n");
        return 1;
    }

    char *file_name = NULL;
    char *path = NULL;
    uint8_t x;
    unsigned char fl = 1;

    // Keep the original files
    if((flags & 0b100000) == 0b100000)
    {
        if((flags & 0b1000000) == 0b1000000 || flag == dec) // New name
        {
            do{
                if(flag == enc) printf("Name the encrypted version of %s", file);
                else printf("Name the decrypted version of %s", file);
                file_name = get_str(": ", 100);
                if(file_name == NULL){
                    printf("Naming failed\n");
                    fl = 0;
                    break;
                }
                fps = fopen(file_name, "wb");
                x = strlen(file_name) <= 0 || fps == NULL;
                if(x)
                    free(file_name);
            }while(x);
            fclose(fps);
            if(!(flags & 0b1) && fl) // If encrypting or decrypting a directory
            {
                path = edit_path(file, file_name);
                if (path != NULL){
                    free(file_name);
                    file_name = path;
                    path = NULL;
                }
            }
        }
        else if (flag == enc) // Change the file extension
        {
            file_name = generate_file_name(file);
            if(file_name == NULL) fl = 0;
        }
    }

    // Open for writing
    if(fl && (flags & 0b100000) == 0b100000) fps = fopen(file_name, "wb");
    else fps = fopen(file, "wb");
    if (fps == NULL){
        printf("File %s can't open\n", file);
        fclose(fpo);
        if(fl) free(file_name);
        if(path != NULL) free(path);
        return 1;
    }

    char h[16];
    while(fread(h, sizeof(char), 16, fpo) == 16)
        fwrite(h, sizeof(char), 16, fps);

    fclose(fpo);
    fclose(fps);

    uint8_t rename_failed  = 0;
    // name the output file
    if((flags & 0b1000000) == 0b1000000 && (flags & 0b100000) != 0b100000){
        do{
            printf("Rename %s", file);
            file_name = get_str(": ", 100);
            if(file_name == NULL) break;
            x = strlen(file_name) <= 0;
            if(x)
                free(file_name);
        }while(x);
        if(file_name != NULL)
        {
            // If encrypting or decrypting a directory
            if(!(flags & 0b1))
            {
                path = edit_path(file, file_name);
                if (path != NULL){
                    if (rename(file, path)){
                        printf("%s could not be renamed.\n", file);
                        rename_failed = 1;
                    }
                }
            }
            else
            {
                if (rename(file, file_name)){
                    printf("%s could not be renamed.\n", file);
                    rename_failed = 1;
                }
            }
        }
        else
        {
            printf("Renaming %s failed\n", file);
            rename_failed = 1;
        }
    }
    //Change the file extension
    if((flag == enc && (flags & 0b1000000) != 0b1000000 && (flags & 0b100000) != 0b100000) || rename_failed)
    {
        file_name = generate_file_name(file);
        if(file_name != NULL) rename(file, file_name);
        else fl = 0;

    }


    if(flag == enc) printf("File %s Encrypted Successfully!\n", file);
    else if (flag == dec) printf("File %s Decrypted Successfully!\n", file);

    if(fl) free(file_name);
    if(path != NULL) free(path);
    remove(".tmp_enc22");
    return 0;
}

char *generate_file_name(char *file){
    int name_len = strlen(file), fl = 0;
    char *name = NULL;
    for(int i = name_len; i > 0; i--){
        if(file[i] == '.')
        {
            name_len = i;
            fl = 1;
            break;
        }
        else if (file[i] == '/') {
            fl = 1;
            break;
        }
    }
    if(name_len <= 1) return NULL;
    char *ext = ".bin";
    name = (char *)malloc((name_len+5) * sizeof(char));
    if(name == NULL) return NULL;
    memcpy(name, file, name_len);
    memcpy(name+name_len, ext, 5);
    return name;

}

char *edit_path(char *file, char *name){
    int len = strlen(file), fl = 0;
    char *path = NULL;
    for(int i = len; i > 0; i--){
        if(file[i] == '/')
        {
            len = i+1;
            fl = 1;
            break;
        }
    }
    if(len <= 1) return NULL;
    int name_len = strlen(name);
    path = (char *)malloc((name_len+len+1) * sizeof(char));
    if(path == NULL) return NULL;
    memcpy(path, file, len);
    memcpy(path+len, name, name_len+1);
    return path;

}


char *get_str(char *text, int SIZE)
{
    printf("%s", text);
    char *inputBuffer = (char *)malloc(sizeof(char) * SIZE);
    if(inputBuffer == NULL){
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        return NULL;
    }
    memset(inputBuffer, 0, SIZE);

    char *result = NULL;

    while (result == NULL)
    {
        result = fgets(inputBuffer, SIZE, stdin);

        if (inputBuffer[strlen(inputBuffer) - 1] != '\n')
        {
            if(SIZE == PasswordMaxLength) fprintf(stderr, "The password was too long, please try again.\n");
            else fprintf(stderr, "The input was too long, please try again.\n");
            printf("%s", text);
            char c;
            while ((c = getchar()) != '\n' && c != EOF) {}

            result = NULL;
        }
    }
    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
    return inputBuffer;
}

int get_int(char *str)
{
    int num;
    char term;

    printf("%s", str);
    if (scanf("%d%c", &num, &term) != 2 || term != '\n')
    {
        do
        {
            printf("\n%s", str);
            while ((getchar()) != '\n');
        } while (scanf("%d%c", &num, &term) != 2 || term != '\n');

    }
    return num;
}


void print_hex(unsigned char x)
{
    if(x / 16 < 10) printf("%c", (char)((x/16)+'0'));
    if(x / 16 >= 10) printf("%c", (char)((x/16-10)+'A'));

    if(x % 16 < 10) printf("%c", (char)((x%16)+'0'));
    if(x % 16 >= 10) printf("%c", (char)((x%16-10)+'A'));
}