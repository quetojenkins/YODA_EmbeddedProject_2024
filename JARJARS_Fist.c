#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BLOCK_SIZE 64

uint32_t leftrotate(uint32_t x, uint32_t c) {
    return ((x << c) | (x >> (32 - c)));
}

uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) | (~x & z);
}

uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
    return (x & z) | (y & ~z);
}

uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
    return x ^ y ^ z;
}

uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
    return y ^ (x | ~z);
}

uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = (a + F(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = (a + G(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = (a + H(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = (a + I(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint8_t* pad_message(uint8_t *message, size_t message_len, uint64_t* padded_len) {
    size_t padded_len_val = message_len + 1 + (BLOCK_SIZE - ((message_len + 9) % BLOCK_SIZE)) % BLOCK_SIZE + 8;
    uint8_t* padded_message = (uint8_t*)malloc(padded_len_val);
    size_t i;

    // Copy the original message
    memcpy(padded_message, message, message_len);

    // Append 0x80 byte
    padded_message[message_len] = 0x80;

    // Append 0x00 bytes until length is congruent to 56 mod 64
    for (i = message_len + 1; i < padded_len_val - 8; i++) {
        padded_message[i] = 0x00;
    }

    uint64_t length_bits = message_len * 8;
    for (i = 0; i < 8; i++) {
        padded_message[padded_len_val - 8 + i] = (length_bits >> (i * 8)) & 0xff;
    }

    *padded_len = padded_len_val;
    return padded_message;
}



char *md5(uint8_t *message, size_t message_len) {
    
    static char digest[33];
    //uint8_t padded_message[BLOCK_SIZE * 2];

    uint64_t padded_len1;
    uint8_t* padded_message = pad_message(message, message_len,&padded_len1);

    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xEFCDAB89;
    uint32_t c0 = 0x98BADCFE;
    uint32_t d0 = 0x10325476;

    uint32_t Round1values[] = {
        0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
        0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
        0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
        0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821
    };

    uint32_t Round2values[] = {
        0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
        0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
        0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
        0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A
    };

    uint32_t Round3values[] = {
        0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
        0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
        0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
        0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665
    };

    uint32_t Round4values[] = {
        0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
        0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
        0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
        0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
    };

    for (size_t chunk_start = 0; chunk_start < padded_len1; chunk_start += BLOCK_SIZE) {
        uint32_t a = a0, b = b0, c = c0, d = d0;
        uint32_t chunk[BLOCK_SIZE / 4];

        for (size_t i = 0; i < 16; i++) {
            chunk[i] = ((uint32_t)padded_message[chunk_start + i * 4 + 3] << 24) |
                       ((uint32_t)padded_message[chunk_start + i * 4 + 2] << 16) |
                       ((uint32_t)padded_message[chunk_start + i * 4 + 1] << 8) |
                       ((uint32_t)padded_message[chunk_start + i * 4]);
           
        }


        for (size_t i = 0; i < 4; i++) {
            a = FF(a, b, c, d, chunk[0 + i * 4], 7, Round1values[0 + i* 4]);
            d = FF(d, a, b, c, chunk[1 + i * 4], 12, Round1values[1 + i * 4]);
            c = FF(c, d, a, b, chunk[2 + i * 4], 17, Round1values[2 + i * 4]);
            b = FF(b, c, d, a, chunk[3 + i * 4], 22, Round1values[3 + i * 4]);
        }
        
        for (size_t i = 0; i < 4; i++) {
            a = GG(a, b, c, d, chunk[(((i * 4) * 5) + 1) % 16], 5, Round2values[0 + i * 4]);
            d = GG(d, a, b, c, chunk[(((i * 4 + 1) * 5) + 1) % 16], 9, Round2values[1 + i * 4]);
            c = GG(c, d, a, b, chunk[(((i * 4 + 2) * 5) + 1) % 16], 14, Round2values[2 + i * 4]);
            b = GG(b, c, d, a, chunk[(((i * 4 + 3) * 5) + 1) % 16], 20, Round2values[3 + i * 4]);
        }
        
        for (size_t i = 0; i < 4; i++) {
            a = HH(a, b, c, d, chunk[(((i * 4) * 3) + 5) % 16], 4, Round3values[0 + i * 4]);
            d = HH(d, a, b, c, chunk[(((i * 4 + 1) * 3) + 5) % 16], 11, Round3values[1 + i * 4]);
            c = HH(c, d, a, b, chunk[(((i * 4 + 2) * 3) + 5) % 16], 16, Round3values[2 + i * 4]);
            b = HH(b, c, d, a, chunk[(((i * 4 + 3) * 3) + 5) % 16], 23, Round3values[3 + i * 4]);
        }

        for (size_t i = 0; i < 4; i++) {
            a = II(a, b, c, d, chunk[(((i * 4) * 7)) % 16], 6, Round4values[0 + i * 4]);
            d = II(d, a, b, c, chunk[(((i * 4 + 1) * 7)) % 16], 10, Round4values[1 + i * 4]);
            c = II(c, d, a, b, chunk[(((i * 4 + 2) * 7)) % 16], 15, Round4values[2 + i * 4]);
            b = II(b, c, d, a, chunk[(((i * 4 + 3) * 7)) % 16], 21, Round4values[3 + i * 4]);
        }
        
        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;

    }

    uint8_t digest_bytes[16];
    memcpy(digest_bytes, &a0, 4);
    memcpy(digest_bytes + 4, &b0, 4);
    memcpy(digest_bytes + 8, &c0, 4);
    memcpy(digest_bytes + 12, &d0, 4);

    // Convert each byte to its hexadecimal representation
    for (int i = 0; i < 16; i++) {
        sprintf(digest + i * 2, "%02x", digest_bytes[i]);
    }
    digest[32] = '\0';
    
    //printf(digest);
    free(padded_message);
    return digest;
}

void append_hash_to_file(const char *hash) {
    FILE *output_file = fopen("dictionaries/test_hashed.txt", "a");
    if (!output_file) {
        perror("Unable to open output file");
        return;
    }
    fprintf(output_file, "%s\n", hash);
    fclose(output_file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <password_file> <hashed_password>\n", argv[0]);
        return 1;
    }

    char *password_file_path = argv[1];
    char *target_hashed_password = argv[2];
    printf("%s\n",target_hashed_password);

    FILE *file = fopen(password_file_path, "r");
    if (!file) {
        perror("Unable to open password file");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character from the end of the line
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // Compute MD5 hash of the password
        char *hashed_password = md5((uint8_t*)line, strlen(line));
        // append_hash_to_file(hashed_password);

        // Compare the hashed password with the target hashed password
        if (strcmp(hashed_password, target_hashed_password) == 0) {
            printf("==========================================\n");
            printf("Found Match!\n");
            printf("Recovered Password: %s\n", line);
            printf("==========================================\n");
            fclose(file);
            return 0;
        }
    }

    printf("-1\n");
    fclose(file);
    return 0;
}


