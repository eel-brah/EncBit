# EncBit
<!-- #### Video Demo:  <https://youtu.be/Wi7WFuxHjq0> -->
<!-- #### Description: -->
EncBit is a command-line program.\
It can encrypt and decrypt a directory, file, or string using AES-256 with a password or a keyfile.\
It can also hash a file or a string using SHA-256.

Usage:
./EncBit option *entity [keyfile]\
*entity could be a path, directory, file, or string.

Note: 
The file EncBit.c for Linux and EncBit_w.c for Windows.

#### Options:

    -h
        display a quick help guide
    -e
        encrypt
    -d
        decrypt
    -f
        encrypt or decrypt a single file
    -k
        use a keyfile instead of a password
    -p
        use NMB
    -H
        hash a file
    -S
        hash a string
    -r
        name the output file
    -o
        keep the original file
    -w
        don't hide the password and NMB

---

## How to use EncBit
1\. To encrypt or decrypt a directory, the option -e for encryption or -d for decryption must be provided.

- to ecrypt a derectory with a password

        ./EncBit -e directory_name 

2\. By default, EncBit takes a directory as an argument after options, unless the -f option is used, where it will take a file.

- to encrypt a file with a password

        ./EncBit -ef file_name

3\. To use a keyfile instead of a password, the -k option must be used.

- to decrypt a file with a keyfile

        ./EncBit -dfk file_name keyfile


4\. NMB makes encryption more secure, it is a number that specifies the number of times a file will be encrypted, and every 16 bytes the key change by adding NMB to the key.\
NMB can be used with a password and a keyfile.\
The larger the NMB, the longer the file takes to be encrypted or decrypted.\
To use NMB, the option -p must be used.

- to encrypt a file and use NMB

        ./EncBit -efp file_name

6\. By default the password and NMB are hidden, the option -w makes them visible.

- to see the password and NMB

        ./EncBit -dpw directory_name

7\. When a file is encrypted, by default its extension is replaced with ".bin", if it doesn't have an extension, ".bin" is added to the end.\
The option -r is used to give the output file a new name, instead of adding ".bin".

- to encrypt a file and gives it a new name

        ./EncBit -efr file_name

8\. While encrypting and decrypting files, they will be overwritten, to avoid this, the -o option is used.

- to encrypt a directory while preserving the unencrypted copy of its files

        ./EncBit -eo directory_name

8\. The -H option is used to hash a file with SHA-256

- to hash a file with SHA-256

        ./EncBit -H file_name
- to get a file hash and encrypt it

        ./EncBit -efH file_name

9\. The -S option is used to hash a string, string must be between quotes ('') if it contains space(s).

- to hash a string with SHA-256

        ./EncBit -S 'Hello, World!'

10\. Strings can also be encrypted.

- to encrypt a string with a password

        ./EncBit -eS 'Hello, World!'
- to encrypt a string with a keyfile and using NMB

        ./EncBit -eSkp 'Hello, World!' keyfile

11\. To decrypt a string, the input must be in hexadecimal.

- to decrypt a string with a password

        ./EncBit -dS 2E 3A 0D 6A E5 54 17 9C 90 E8 D0 29 01 AA D3 2D
- to decrypt a string with keyfile

        ./EncBit -dSk 2E 3A 0D 6A E5 54 17 9C 90 E8 D0 29 01 AA D3 2D

The keyfile is required as an argument when using the -k option, except in this case.

12\. Password must be at least 8 characters long, and on Linux version, the maximum number of characters is 1000.

13\. While encrypting or decrypting a single file, if the input argument is its path and the -r option is used then the user has to type the path and replace the old name in the path with the new one when the new name is requested.

- Example

        ./EncBit -ef ./dir/dir2/file_name
        Passwor: XXXXXXXX
        Rename ./dir/dir2/file_name: ./dir/dir2/new_file_name

14\. Some options cannot be used together, such as -e and -d, and some others will be ignored if used in the wrong context, such as -o and -r if they are used in case of hashing a sting or a file, or the encrypting or decrypting of a string.

---

## Project files
There are four main files that contain the code of EncBit, **AES_256.c**, **SHA_256.c**, **EncBit.c**, and  **EncBit_w.c**.

**AES_256.c:**\
As the name suggests, this file contains code for encrypting and decrypting files and data using AES-256.

**SHA_256.c:**\
This file contains code to hash data and files using SHA-256.

**EncBit.c:**
EncBit.c contains several functions:
- *main*: main function collects the arguments provided by the user, checks whether the options are valid, and if the number of arguments is correct, then it calls the required functions, which depend on which option was used.
- *open_dir*: This function opens directories.
- *encrypt_decrypt_file*: This function is responsible for opening files, padding them if needed, and then encrypting or decrypting them, it also checks if the output file needs a new name and Whether to keep the original copy of the file.

- *encrypt_text*: Encrypt a string.

- *decrypt_text*: Decrypt a string.

- *hash_file*: Hash a file and print the output in hexadecimal.

- *hash_data*: Hash a string and print the output in hexadecimal.

- *get_str*: This function gets string input from the user.

- *get_int*: This function gets integer input from the user.

- *print_hex*: Print a ASCII Text string in hexadecimal.

- *hexToText*: Convert input from hexadecimal to ASCII text.

- *generate_file_name*: Replace the file extension with ".bin" or add ".bin".

- *edit_path*: This function helps to give the output file a new name while encrypting or decrypting a directory, by replacing the old name with the new one in the file path.

**EncBit_w.c:**
EncBit_w.c is almost the same as EncBit.c, but it is for Windows since EncBit.c only work on Linux.
EncBit_w.c contains two more functions:
*get_h_str*: It gets string input from the user, by default, the string doesn't appear on the terminal.
*get_nmb*: This function gets nmb from the user, it also doesn't let nmb be displayed on the terminal unless the option -w is used.

---

## How EncBit works:
First, it starts with checking options (if there are options that can't be used together). 
Then it checks if the -H option is used without -e or -d, if it is, it will assume that the argument after the option is a file name and will try to hash that file using SHA-256, if it succeeds, it will print the hash in hexadecimal and exit, if not, it will print why it failed and exit.
Else if the -S option is used without -e or -d, it will assume that the argument after the option is a string and will try to hash it, like file hashing, if it succeeds the hash will be printed in hexadecimal and exit, otherwise, it will print why it failed and exit.

If none of these conditions occur, it will check the input again, at this stage -e or -d must exist for EncBit to continue, if that's not the case or the number of arguments was not equal to 3, or 4 if keyfile was used, EncBit will print how it should be used and exit, except if keyfile is used to decrypt a sritng (-dSk), where the input string must be in hexadecimal (without quotes ('')).
Otherwise if -e or -d is used and the number of argument was correct, EncBit will continue.

after checking the inputs, if the keyfile was not used then the user will be prompted for a password, then the password or keyfile (if used) will be hashed, and this hash will be the key. 
If the -p option is used, the user will be asked for NMB, which is a number specifies how many times the file will be encrypted, and every 16 bytes the key change by adding NMB to the key. This makes the encryption more secure, but the larger the NMB, the longer it takes to encrypt or decrypt the file.

While getting a password and NMB from the user, the echo of the terminal will be disabled and enabled after that (to hide the password and NMB), unless the -w option is used. This is on Linux, But on Windows to hide the password and nmb, فhey are obtained through *getch* function. 

Next, it will check if the -S option is used with -e or -d, if so it will encrypt or decrypt a string (in encryption, there are only three arguments, the third one is the string, while in decryption, the string is in hexadecimal from argv[2] to argv[argc]) using AES-256 and then exit, but before dicrypting a string, it will check if there are at least three arguments and convert the string from hexadecimal to ASCII text. 
Otherwise it will check if the -f option is used, if it is, it will assume that the third argument is a file and try to encrypt it or decrypt it, else if the -f option is not used, it will assume that it is a directory and will try to open it and encrypt or decrypt every file in it, or open other directories within it to do the same.

Before encrypting or decrypting a file if the -H option was used, it will hash that file before.

By default, every encrypted file is overwritten and its extention change to ".bin", if it has no extention, ".bin" will be added to the end of the file name. but during decryption the files are also overwritten but the extension does't change.

To avoid losing the original version after encrypting or decrypting it, the -o option should be used, this option will make sure that the file is not overwritten.

To change the file name to a new one instead of adding ".bin" to its end in encryption or staying the same in decryption, the option -r should be used.

The options -r and -o can be combined, which means that EncBit will not overwrite the original file and will ask the user for a new name for the output file.

## How AES-256 works:
First, Key Expansion, we expand the key from 32 bytes to 240 bytes, we set the rcon iteration value to 1, then the first 32 bytes of the expanded key are the encryption key, to generate 32 bytes of the expanded key, we take the last 4 bytes (32-bit) and apply *RotWord, *SubWord, and *Rcon to it every 32 bytes or just *SubWord every 16 bytes, and combining it with the 4 bytes that are in the same position in the previous 32 bytes using bitwise xor, this becomes the next 4 bytes in the expanded key.
if 4 bytes weren't the last in 16 or 32 bytes, it's only combined with the 4 bytes that are in the same position in the previous 16 or 32 bytes using bitwise xor.
We do this until we have 240 bytes.

RotWord: rotates 4 bytes by 1 byte to the left.
SubWord: each byte is replaced with another according to s-box table.
Rcon: add (^) round constants into the key, these round constants are generated using a recursive function.

Next, every 16 bytes (state) will be encrypted separately.
There are 14 transformation rounds in AES-256 that are applied to the state (16 bytes), starting with the initial round *AddroundKey, then 13 rounds of *SubBytes, *ShiftRows, *MixColumns, and *AddRoundKey, finally, the final round *SubBytes, *ShiftRows, and *AddRoundKey.

AddroundKey: each byte of the state is combined with a byte of the round key using bitwise xor.
SubBytes: each byte is replaced with another according to s-box table.
ShiftRows: shift 2nd row to the left by 1, 3rd row by 2, and 4th row by 3.
MixColumns: all bytes of each column of the state are combined using an invertible linear transformation.

    Initial round: AddroundKey (first 16 bytes)
    Rounds   (Rounds = 13)
        SubBytes
        ShiftRows
        MixColumns
        AddRoundKey (16 * (current_round + 1))
    The finle round without MixColumns
        SubBytes
        ShiftRows
        AddRoundKey (last 16 bytes)


AES-256 decryption is just going in reverse order of the encryption process and using InvShiftRows, InvSubBytes, and InvMixColumns.

        Reverse the last round
            AddRoundKey (last 16 bytes)
            InvShiftRows
            InvSubBytes 
        Rounds
            AddRoundKey (16 * (rounds - current_round))
            InvMixColumns
            InvShiftRows
            InvSubBytes
        AddroundKey (first 16 bytes)

## How SHA-256 works:
First, padding bits, we appended 0 bits to the original input to make it compatible with SHA-256, the total bits must be 64 bits shorter than the nearest multiple of 512, and in the last 64 bits, we add the length of the original input in 64 binary representation.

Second, before we begin the hash computation, we need to initialize hash values h[8], they are 32-bit words, come from the first 32 bits of the fractional part of the square roots of the eight prime numbers, and also initialize an array of round constants k[64], these come from the first 32-bit of the fractional parts of the cube root of the first 64 prime number in hexadecimal.

Third, hash computation, we compute every 512 bits separately, we split each one into 32-bit words, which gives us 16 32-bit words,
then, we create a 64-entry message schedule array of 32-bit words w[64], the first 16 32-bit come from the splitting of the 512 bits.

The remaining 48 words of the 64 message schedule array, come from extending the first 16 words.
for i from 16 to 63

    w[i] = w[i-16] + s0 + w[i-7] + s1

    s0 = (w[i-15] rightrotate  7) xor (w[i-15] rightrotate 18) xor (w[i-15] rightrotate  3)
    s1 = (w[i-2] rightrotate 17) xor (w[i-2] rightrotate 19) xor (w[i-2] rightrotate 10)

After that, we initialize the eight working variables

    a = h[0]
    b = h[1]
    c = h[2]
    d = h[3]
    e = h[4]
    f = h[5]
    g = h[6]
    h = h[7]

Next, Compression function main loop

for i from 0 to 63

    temp1 = h + S1 + ch + k[i] + w[i]
    temp2 = S0 + maj

    S1 = (e rightrotate 6) xor (e rightrotate 11) xor (e rightrotate 25)
    ch = (e and f) xor ((not e) and g)
    S0 = (a rightrotate 2) xor (a rightrotate 13) xor (a rightrotate 22)
    maj = (a and b) xor (a and c) xor (b and c)

    h = g
    g = f
    f = e
    e = d + temp1
    d = c
    c = b
    b = a
    a = temp1 + temp2

Add the compressed chunk to the current hash value

    h[0] += a
    h[1] += b
    h[2] += c
    h[3] += d
    h[4] += e
    h[5] += f
    h[6] += g
    h[7] += h

Finly, we append hash values to get the final hash value.


## sources: 
1. [SHA-2 - Wikipedia](https://en.wikipedia.org/wiki/SHA-2)
2. [SHA-256 | COMPLETE Step-By-Step Explanation - YouTube](https://www.youtube.com/watch?v=orIgy2MjqrA)
3. [Advanced Encryption Standard - Wikipedia](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_ShiftRows_step)
4. [AES – The Advanced Encryption Standard Explained - YouTube](https://www.youtube.com/watch?v=h6wvqm0aXco)
5. [AES key schedule - Wikipedia](https://en.wikipedia.org/wiki/AES_key_schedule)
6. [AES – Key Schedule/Key Expansion Explained - YouTube](https://www.youtube.com/watch?v=rmqWaktEpcw)
7. [Rijndael MixColumns - Wikipedia](https://en.wikipedia.org/wiki/Rijndael_MixColumns)
8. [Rijndael mix columns | Crypto Wiki | Fandom](https://cryptography.fandom.com/wiki/Rijndael_mix_columns)
9. [Rijndael S-box | Crypto Wiki | Fandom](https://cryptography.fandom.com/wiki/Rijndael_S-box)


