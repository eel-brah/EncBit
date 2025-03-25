### How EncBit works:

First, it starts with checking options (if there are options that can't be used together). Then it checks if the -H option is used without -e or -d, if it is, it will assume that the argument after the option is a file name and will try to hash that file using SHA-256, if it succeeds, it will print the hash in hexadecimal and exit, if not, it will print why it failed and exit. Else if the -S option is used without -e or -d, it will assume that the argument after the option is a string and will try to hash it, like file hashing, if it succeeds the hash will be printed in hexadecimal and exit, otherwise, it will print why it failed and exit.

If none of these conditions occur, it will check the input again. At this stage, -e or -d must exist for EncBit to continue. If that's not the case, or the number of arguments was not equal to 3, or 4 if a keyfile was used, EncBit will print how it should be used and exit, except if a keyfile is used to decrypt a string (-dSk), where the input string must be in hexadecimal (without quotes ('')). Otherwise, if -e or -d is used and the number of arguments is correct, EncBit will continue.

After checking the inputs, if the keyfile was not used, then the user will be prompted for a password. The password or keyfile (if used) will be hashed, and this hash will be the key. If the -p option is used, the user will be asked for NMB, which specifies how many times the file will be encrypted. Every 16 bytes, the key changes by adding NMB to the key. This makes the encryption more secure, but the larger the NMB, the longer it takes to encrypt or decrypt the file.

While getting a password and NMB from the user, the echo of the terminal will be disabled and enabled afterward (to hide the password and NMB), unless the -w option is used. This is on Linux, but on Windows, to hide the password and NMB, they are obtained through the getch function.

Next, it will check if the -S option is used with -e or -d. If so, it will encrypt or decrypt a string (in encryption, there are only three arguments; the third one is the string, while in decryption, the string is in hexadecimal from argv[2] to argv[argc]) using AES-256 and then exit. Before decrypting a string, it will check if there are at least three arguments and convert the string from hexadecimal to ASCII text. Otherwise, it will check if the -f option is used. If it is, it will assume that the third argument is a file and try to encrypt it or decrypt it. Otherwise, if the -f option is not used, it will assume that it is a directory and will try to open it and encrypt or decrypt every file in it, or open other directories within it to do the same.

Before encrypting or decrypting a file, if the -H option was used, it will hash that file beforehand.

By default, every encrypted file is overwritten and its extension changes to ".bin". If it has no extension, ".bin" will be added to the end of the file name. However, during decryption, the files are also overwritten, but the extension doesn't change.

To avoid losing the original version after encrypting or decrypting, the -o option should be used. This option ensures that the file is not overwritten.

To change the file name to a new one instead of adding ".bin" to its end in encryption or staying the same in decryption, the -r option should be used.

The options -r and -o can be combined, which means that EncBit will not overwrite the original file and will ask the user for a new name for the output file.

---

### How AES-256 works:

First, Key Expansion: we expand the key from 32 bytes to 240 bytes. We set the rcon iteration value to 1, then the first 32 bytes of the expanded key are the encryption key. To generate 32 bytes of the expanded key, we take the last 4 bytes (32-bit) and apply *RotWord, *SubWord, and *Rcon to it every 32 bytes or just *SubWord every 16 bytes, and combine it with the 4 bytes that are in the same position in the previous 32 bytes using bitwise XOR. This becomes the next 4 bytes in the expanded key. If 4 bytes weren't the last in 16 or 32 bytes, they're only combined with the 4 bytes that are in the same position in the previous 16 or 32 bytes using bitwise XOR. We do this until we have 240 bytes.

RotWord: rotates 4 bytes by 1 byte to the left.
SubWord: each byte is replaced with another according to the S-box table.
Rcon: adds (^) round constants into the key, generated using a recursive function.

Next, every 16 bytes (state) will be encrypted separately. There are 14 transformation rounds in AES-256 that are applied to the state (16 bytes), starting with the initial round *AddroundKey, then 13 rounds of *SubBytes, *ShiftRows, *MixColumns, and *AddRoundKey, and finally, the final round *SubBytes, *ShiftRows, and *AddRoundKey.

AES-256 decryption is just going in reverse order of the encryption process and using InvShiftRows, InvSubBytes, and InvMixColumns.

---

### How SHA-256 works:

First, padding bits: we append 0 bits to the original input to make it compatible with SHA-256. The total bits must be 64 bits shorter than the nearest multiple of 512, and in the last 64 bits, we add the length of the original input in a 64-bit binary representation.

Second, before we begin the hash computation, we need to initialize hash values h[8], which are 32-bit words derived from the first 32 bits of the fractional part of the square roots of the first eight prime numbers. We also initialize an array of round constants k[64], which come from the first 32 bits of the fractional parts of the cube root of the first 64 prime numbers in hexadecimal.

Third, hash computation: we compute every 512 bits separately. We split each one into 32-bit words, which gives us 16 32-bit words. Then, we create a 64-entry message schedule array of 32-bit words w[64]. The first 16 32-bit words come from splitting the 512-bit chunk, and the remaining 48 words are generated using a specific formula involving previous words, rotations, and bitwise operations.

Next, we initialize eight working variables and process the data through 64 rounds of computation using bitwise operations and modular addition. Finally, we append the computed hash values to produce the final SHA-256 hash.

This ensures a secure and unique representation of the original data.

