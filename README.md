# EncBit

EncBit is a command-line tool for encrypting, decrypting, and hashing data. It supports AES-256 encryption and decryption for directories, files, and strings using a password or keyfile. Additionally, it provides SHA-256 hashing for files and strings.

## Features
- AES-256 encryption and decryption
- SHA-256 hashing for files and strings
- Encrypt or decrypt individual files or entire directories
- Password-based encryption with key derivation
- Multi-layer encryption with the `-p` option
- Secure handling of passwords and keys

## Installation
Clone this repository and compile it using GCC:

```bash
git clone https://github.com/eel-brah/EncBit
cd EncBit
gcc -o encbit EncBit.c AES_256.c SHA_256.c
```

## Usage

```
./EncBit [options] <entity> [keyfile]
```

- `<entity>` can be a path, directory, file, or string.
- `[keyfile]` is required when using the `-k` option, except when decrypting a string with `-dSk`.
- `EncBit.c` is for Linux, and `EncBit_w.c` is for Windows.

## Options

```
-h   Show help information
-e   Encrypt
-d   Decrypt
-f   Process a single file instead of a directory
-k   Use a keyfile instead of a password
-p   Use NMB for additional security
-H   Hash a file using SHA-256
-S   Hash a string using SHA-256
-r   Rename the output file
-o   Preserve the original file
-w   Display the password and NMB
```

## Examples

### Encrypting & Decrypting

Encrypt a directory with a password:

```
./EncBit -e directory_name
```

Encrypt a file with a password:

```
./EncBit -ef file_name
```

Decrypt a file using a keyfile:

```
./EncBit -dfk file_name keyfile
```

### Using NMB

NMB enhances encryption by altering the key periodically. The larger the NMB, the stronger the encryption and the longer encryption takes.

```
./EncBit -efp file_name
```

### Handling Password Visibility

By default, passwords and NMB are hidden. Use `-w` to display them:

```
./EncBit -dpw directory_name
```

### File Naming

By default, encrypted files get a `.bin` extension. Use `-r` to specify a custom name:

```
./EncBit -efr file_name
```

### Preserving Original Files

Prevent overwriting by using `-o`:

```
./EncBit -eo directory_name
```

### Hashing Files & Strings

Hash a file with SHA-256:

```
./EncBit -H file_name
```

Hash and encrypt a file:

```
./EncBit -efH file_name
```

Hash a string:

```
./EncBit -S 'Hello, World!'
```

### Encrypting & Decrypting Strings

Encrypt a string:

```
./EncBit -eS 'Hello, World!'
```

Encrypt a string with a keyfile and NMB:

```
./EncBit -eSkp 'Hello, World!' keyfile
```

Decrypt a hexadecimal string:

```
./EncBit -dS 2E 3A 0D 6A E5 54 17 9C 90 E8 D0 29 01 AA D3 2D
```

Decrypt a string using a keyfile:

```
./EncBit -dSk 2E 3A 0D 6A E5 54 17 9C 90 E8 D0 29 01 AA D3 2D
```

## Project Files

EncBit consists of the following main source files:

- **AES\_256.c**: Handles AES-256 encryption and decryption.
- **SHA\_256.c**: Implements SHA-256 hashing.
- **EncBit.c**: Main logic for Linux.
- **EncBit\_w\.c**: Windows-specific implementation.

### Key Functions in EncBit.c

- `main`: Parses user input and calls appropriate functions.
- `open_dir`: Processes directories.
- `encrypt_decrypt_file`: Encrypts or decrypts files.
- `encrypt_text`: Encrypts a string.
- `decrypt_text`: Decrypts a string.
- `hash_file`: Hashes a file.
- `hash_data`: Hashes a string.
- `print_hex`: Prints a string in hexadecimal.
- `generate_file_name`: Renames encrypted files.
- `edit_path`: Modifies file paths for renamed outputs.

## How EncBit Works

1. **Validates Options:** Ensures compatible options are used.
2. **Handles Hashing:** If `-H` or `-S` is used alone, it hashes the given file or string and exits.
3. **Processes Password & Keyfile:** If encryption or decryption is requested, it prompts for a password or uses a keyfile.
4. **Handles NMB (if enabled):** Enhances encryption security by modifying the key dynamically.
5. **Executes Encryption/Decryption:**
   - If `-S` is used, processes strings.
   - If `-f` is used, processes a single file.
   - Otherwise, assumes the input is a directory and encrypts/decrypts all files inside.
6. **Handles Output Naming & Preservation:**
   - Overwrites files by default unless `-o` is used.
   - Adds `.bin` to encrypted files unless `-r` specifies a new name.

## How AES-256 Works

1. **Key Expansion:** The original key expands to 240 bytes using transformations.
2. **Encryption:**
   - Initial round: `AddRoundKey`.
   - 13 main rounds: `SubBytes`, `ShiftRows`, `MixColumns`, `AddRoundKey`.
   - Final round: `SubBytes`, `ShiftRows`, `AddRoundKey`.
3. **Decryption:** Performs inverse transformations in reverse order.

## How SHA-256 Works

1. **Padding:** Input is padded to be a multiple of 512 bits.
2. **Hash Computation:**
   - Initializes hash values and constants.
   - Processes input in 512-bit chunks.
   - Uses bitwise operations to generate the final hash.

EncBit provides strong encryption and hashing, making it a powerful tool for data security.

[For in-depth explanation](INDEPTH.md).

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


