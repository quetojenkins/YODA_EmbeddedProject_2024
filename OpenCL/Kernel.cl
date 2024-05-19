__global int found = 0;

#define MAX_PADDED_LEN 64
#define MAX_MESSAGE_LEN 64
#define BLOCK_SIZE 64

uint leftrotate(uint x, uint c) {
    return ((x << c) | (x >> (32 - c)));
}

uint F(uint x, uint y, uint z) {
    return (x & y) | (~x & z);
}

uint G(uint x, uint y, uint z) {
    return (x & z) | (y & ~z);
}

uint H(uint x, uint y, uint z) {
    return x ^ y ^ z;
}

uint I(uint x, uint y, uint z) {
    return y ^ (x | ~z);
}

uint FF(uint a, uint b, uint c, uint d, uint x, uint s, uint ac) {
    a = (a + F(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint GG(uint a, uint b, uint c, uint d, uint x, uint s, uint ac) {
    a = (a + G(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint HH(uint a, uint b, uint c, uint d, uint x, uint s, uint ac) {
    a = (a + H(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

uint II(uint a, uint b, uint c, uint d, uint x, uint s, uint ac) {
    a = (a + I(b, c, d) + x + ac);
    a = leftrotate(a, s);
    a = a + b;
    return a;
}

// get the length of a c_string
int char_str_len(const uchar* charArray) {
    int count = 0;
    while (charArray[count] != '\0') {
        count++;
    }
    return count;
}

// check if two strings are the same
int same_string(const uchar* str1, const uchar* str2, int len) {
    bool same = true;

    // get the length of the strings
    int str1_len = char_str_len(str1);
    int str2_len = char_str_len(str2);

    for(int i = 0; i < str1_len; i++){
        printf("%c    %c\n",str1[i],str2[i]);
        if (str1[i] == '\0' || str2[i] == '\0' ) {
            if (str1[i] == '\0' && str2[i] == '\0' ) {
                same = true;
                break;
            } else {
                same = false;
                break;
            }
        } else {
            

        }
        if (str1[i] != str2[i]){
            same = false;
        }
    }
    
    return same;
}



// Function to convert hexadecimal character to decimal
int hexToDec(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'a' && hex <= 'f') {
        return 10 + (hex - 'a');
    } else if (hex >= 'A' && hex <= 'F') {
        return 10 + (hex - 'A');
    } else {
        return -1; // Invalid hexadecimal character
    }
}


int compareHashes(const char* target, const char* outputhash) {
    for (int i = 0; i < 32; i += 2) {
        char targetByte = (char)((hexToDec(target[i]) << 4) + hexToDec(target[i + 1]));
        char outputByte = outputhash[i / 2];
        if (targetByte != outputByte) {
            return 0; // Hashes don't match
        }
    }
    return 1; // Hashes match
}


// md5 hashing function
void md5(char* data, char* hash, char* outputhash, int word_len) {
    //for (int i = 0; i < char_str_len(data); ++i) {
    //    hash[i] = data[i] + 1;
    //}
    //hash[char_str_len(data)] = '\0';

    //uchar padded_message[MAX_PADDED_LEN];
    //uint padded_len = message_len + 1 + (BLOCK_SIZE - ((message_len + 9) % BLOCK_SIZE)) % BLOCK_SIZE + 8;
    //printf("Before Padding\n");
    // Copy the original message
    for (int i = 0; i < MAX_PADDED_LEN; i++) {
        if (data[i] == '\0') {
            //wordendpos = i;
            break;
        }
        hash[i] = data[i];
    }

        // Append 0x80 byte
    hash[word_len] = 0x80;

    //  Append 0x00 bytes until length is congruent to 56 mod 64
    for (uint i = word_len + 1; i < MAX_PADDED_LEN - 8; i++) {
        hash[i] = 0x00;
    }


    long length_bits = word_len * 8;
    for (uint i = 0; i < 8; i++) {
        hash[MAX_PADDED_LEN - 8 + i] = (uchar)(length_bits >> (i * 8));
        //printf("%i, %02x\n", i,(length_bits >> (i*8)) & 0xff);
    }


    // printf("Hash:");
    // for (int i = 0; i < MAX_PADDED_LEN; ++i) {
    //     printf("%02x", (unsigned char) hash[i]);            
    // }
    // printf("\n");
    uint a0 = 0x67452301;
    uint b0 = 0xEFCDAB89;
    uint c0 = 0x98BADCFE;
    uint d0 = 0x10325476;

    uint Round1values[] = {
        0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
        0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
        0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
        0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821
    };

    uint Round2values[] = {
        0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
        0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
        0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
        0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A
    };

    uint Round3values[] = {
        0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
        0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
        0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
        0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665
    };

    uint Round4values[] = {
        0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
        0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
        0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
        0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
    };
    //printf("Before Algorithm\n");

    for (uint chunk_start = 0; chunk_start < MAX_PADDED_LEN; chunk_start += BLOCK_SIZE) {
        uint a = a0, b = b0, c = c0, d = d0;
        uint chunk[BLOCK_SIZE / 4];

        // for (uint i = 0; i < 16; i++) {
        //     chunk[i] = ((uint)hash[chunk_start + i * 4 + 3] << 24) |
        //                ((uint)hash[chunk_start + i * 4 + 2] << 16) |
        //                ((uint)hash[chunk_start + i * 4 + 1] << 8) |
        //                ((uint)hash[chunk_start + i * 4]);
        // }

        for (uint i = 0; i < 16; i++) {
        // Cast to unsigned char for individual bytes
            unsigned char byte0 = hash[chunk_start + i * 4];
            unsigned char byte1 = hash[chunk_start + i * 4 + 1];
            unsigned char byte2 = hash[chunk_start + i * 4 + 2];
            unsigned char byte3 = hash[chunk_start + i * 4 + 3];

            // Use bit masking to isolate relevant bits and avoid overflow
            chunk[i] = ((uint)byte3 << 24) | ((uint)(byte2 & 0x0000FF) << 16) |
                        ((uint)(byte1 & 0x0000FF) << 8) | (uint)byte0;
        }

        //printf("Chunks: %u,%u,%u,%u,%u,%u,%u,%u\n",chunk[0],chunk[1],chunk[2],chunk[3],chunk[4],chunk[5],chunk[6],chunk[7]);
        //printf("Chunks: %u,%u,%u,%u,%u,%u,%u,%u\n",chunk[8],chunk[9],chunk[10],chunk[11],chunk[12],chunk[13],chunk[14],chunk[15]);

        for (uint i = 0; i < 4; i++) {
            a = FF(a, b, c, d, chunk[0 + i * 4], 7, Round1values[0 + i* 4]);
            d = FF(d, a, b, c, chunk[1 + i * 4], 12, Round1values[1 + i * 4]);
            c = FF(c, d, a, b, chunk[2 + i * 4], 17, Round1values[2 + i * 4]);
            b = FF(b, c, d, a, chunk[3 + i * 4], 22, Round1values[3 + i * 4]);
        }
       // printf("Round 1: %i,%i,%i,%i\n",a,b,c,d);
        for (uint i = 0; i < 4; i++) {
            a = GG(a, b, c, d, chunk[(((i * 4) * 5) + 1) % 16], 5, Round2values[0 + i * 4]);
            d = GG(d, a, b, c, chunk[(((i * 4 + 1) * 5) + 1) % 16], 9, Round2values[1 + i * 4]);
            c = GG(c, d, a, b, chunk[(((i * 4 + 2) * 5) + 1) % 16], 14, Round2values[2 + i * 4]);
           b = GG(b, c, d, a, chunk[(((i * 4 + 3) * 5) + 1) % 16], 20, Round2values[3 + i * 4]);
       }

    //   printf("Round 2: %i,%i,%i,%i\n",a,b,c,d);
       for (uint i = 0; i < 4; i++) {
           a = HH(a, b, c, d, chunk[(((i * 4) * 3) + 5) % 16], 4, Round3values[0 + i * 4]);
           d = HH(d, a, b, c, chunk[(((i * 4 + 1) * 3) + 5) % 16], 11, Round3values[1 + i * 4]);
           c = HH(c, d, a, b, chunk[(((i * 4 + 2) * 3) + 5) % 16], 16, Round3values[2 + i * 4]);
           b = HH(b, c, d, a, chunk[(((i * 4 + 3) * 3) + 5) % 16], 23, Round3values[3 + i * 4]);
       }
      // printf("Round 3: %i,%i,%i,%i\n",a,b,c,d);
       for (uint i = 0; i < 4; i++) {
           a = II(a, b, c, d, chunk[(((i * 4) * 7)) % 16], 6, Round4values[0 + i * 4]);
           d = II(d, a, b, c, chunk[(((i * 4 + 1) * 7)) % 16], 10, Round4values[1 + i * 4]);
           c = II(c, d, a, b, chunk[(((i * 4 + 2) * 7)) % 16], 15, Round4values[2 + i * 4]);
           b = II(b, c, d, a, chunk[(((i * 4 + 3) * 7)) % 16], 21, Round4values[3 + i * 4]);
       }
      // printf("Round 4: %i,%i,%i,%i\n",a,b,c,d);
       a0 += a;
       b0 += b;
       c0 += c;
       d0 += d;
   }


   uchar digest_bytes[16];
   digest_bytes[0] = (uchar)(a0 & 0xFF);
   digest_bytes[1] = (uchar)((a0 >> 8) & 0xFF);
   digest_bytes[2] = (uchar)((a0 >> 16) & 0xFF);
   digest_bytes[3] = (uchar)((a0 >> 24) & 0xFF);

   digest_bytes[4] = (uchar)(b0 & 0xFF);
   digest_bytes[5] = (uchar)((b0 >> 8) & 0xFF);
   digest_bytes[6] = (uchar)((b0 >> 16) & 0xFF);
   digest_bytes[7] = (uchar)((b0 >> 24) & 0xFF);

   digest_bytes[8] = (uchar)(c0 & 0xFF);
   digest_bytes[9] = (uchar)((c0 >> 8) & 0xFF);
   digest_bytes[10] = (uchar)((c0 >> 16) & 0xFF);
   digest_bytes[11] = (uchar)((c0 >> 24) & 0xFF);

   digest_bytes[12] = (uchar)(d0 & 0xFF);
   digest_bytes[13] = (uchar)((d0 >> 8) & 0xFF);
   digest_bytes[14] = (uchar)((d0 >> 16) & 0xFF);
   digest_bytes[15] = (uchar)((d0 >> 24) & 0xFF);
   //printf("Output:");
   for (int i = 0; i < 16; i++) {
        //printf("Saving\n");
        outputhash[i] = digest_bytes[i];
        //printf("%02x", digest_bytes[i]);
    }
    //printf("\n");
}


// sha-1 hashing function
void sha1(char* data, char* hash) {
    for (int i = 0; i < char_str_len(data); ++i) {
        hash[i] = data[i] - 1;
    }
    hash[char_str_len(data)] = '\0';
}

// execute the hash cracking
__kernel void crackHash(__global const char* words,
                        __global const int* word_pos,
                        __global const char* target,
                        const int num_words,
                        const int hash_type) {
    
    // get the current workers globalid
    int global_id = get_global_id(0);

    if (global_id < num_words && found == 0) {
        // printf("%i\n",global_id);

        // get the start and end of the word
        int word_start = word_pos[global_id];
        int word_end = word_pos[global_id + 1];

        // extract the single word from the global array
        uchar word[255];  // assuming max word length of 255 characters
        int word_length = 0;
        for (int i = word_start; i < word_end; ++i) {
            word[word_length++] = words[i];
        }
        word[word_length] = '\0';  // Null-terminate the word
        //printf("==========================================\n");
        //printf("Word Length: %i\n", word_length);
        //printf("Last char of word:%casd\n", word[word_length-1]);
        //printf("Last char of target:%casd\n", target[word_length-1]);

        if (word_length == 0) {
            return;
        }

        // hash the word using appropriate hashing function

        // Need to fix this somehow, will only be correct for md5?
        uchar hash[128];
        uchar outputhash[16];


        if (hash_type == 0){

            // printf("Word:");
            // for (int i = 0; i < char_str_len(word); ++i) {
            //      printf("%c", word[i]);
            // }
            // printf("\n");
            md5((char*)word, (char*)hash,(char*)outputhash, word_length);
            // printf("Hash:");
            // for (int i = 0; i < 16; ++i) {
            //      printf("%02x", (unsigned char) outputhash[i]);            
            // }
            // printf("\nTarget:");
            // for (int i = 0; i < 32; ++i) {
            //     if (target[i] == '\0') {
            //         break;
            //     }
            //     printf("%c", target[i]);
            // }
            //  printf("\n");

        }else if (hash_type == 1){
            sha1(word,hash);
        }
        //printf("Last char of hash:%casd\n", hash[word_length-1]);
        //Store the hash in the output array

        if (compareHashes(target, outputhash)) {
            printf("==========================================\n");
            printf("Found Match!\n");
            printf("Recovered Password:");
            for (int i = 0; i < char_str_len(word); ++i) {
                 printf("%c", word[i]);
            }
            printf("\n");
            printf("==========================================\n");
        }
        //printf("==========================================\n");

    }

    //  printf("Exitting Kernel\n");


}