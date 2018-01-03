#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>


void initialize(void);
void int_to_page_offset(int);
int tlb_check(void);
int page_table_check(void);
FILE *ifile, *ifile_bin, *ofile_value, *ofile_addr;

int tlb_counter;
int frame_counter;
int decoded_page;
int decoded_offset;
int page_table[256];
char frame[256][257]; //physical memory

struct table{
    int page_num;
    int frame_num;
}tlb[15];

int main(int argc, char *argv[]){
    
    //input check
    if(argc != 3){
        printf("[Usage] ./main <addresses.txt> <BACKING_STORE.bin>");
        exit(-1);
    }
    initialize();
    
    if((ifile=fopen(argv[1], "r")) == NULL){
        printf("[Error] Failed in file addr\n");
        exit(-2);
    }
    if((ifile_bin=fopen(argv[2], "r")) == NULL){
        printf("[Error] Failed in open bin\n");
        exit(-2);
    }
    ofile_value = fopen("0416250_value.txt", "w");
    ofile_addr = fopen("0416250_address.txt", "w");
    
    int num, frame_number, word, addr;
    while(1){    
        fscanf(ifile, "%d\n", &num);
        int_to_page_offset(num);
        
        frame_number = tlb_check();

        word = frame[frame_number][decoded_offset];
        fprintf(ofile_value, "%d\n", word);
        //printf("%c", frame[frame_number][decoded_offset]);

        addr = (frame_number*256) + decoded_offset;
        fprintf(ofile_addr, "%d\n", addr);

        if(feof(ifile))
            break;
    }
    
    return 0;
}

int tlb_check(void){
    int i, value;
    for(i = 0; i < 16; i++)
        if(tlb[i].page_num == decoded_page)
            return tlb[i].frame_num;
    
    value = page_table_check();

    tlb[tlb_counter].page_num = decoded_page;
    tlb[tlb_counter].frame_num = value;
    tlb_counter++;
    tlb_counter = tlb_counter%16;
    
    return value;
}

int page_table_check(void){
    //page fault, it doesn't exist, trying to search in BACKING_STORE.bin
    if(page_table[decoded_page] == -1){
        char temp[257];
        memset(temp, 0, 257);
        fseek(ifile_bin, decoded_page*256, SEEK_SET);
        fread(temp, sizeof(char), 256, ifile_bin);
        
        int i;
        for(i = 0; i < 257; i++){
            frame[frame_counter][i] = temp[i];

        }
                

        
        
        page_table[decoded_page] = frame_counter;
        frame_counter++;
    }
    
    return page_table[decoded_page];
}

void int_to_page_offset(int num){
    int i;
    char temp[33];
    memset(temp, 0, 33);

    //int to binary
    for(i = 31; i >= 0; i--, num = num>>1)
        temp[i] = (num & 1) + '0';
    
    //transform to page
    decoded_page = 0;
    for(i = 0; i < 8; i++)
        if(temp[23-i] == '1')
            decoded_page += (int)pow(2, i);  
    //printf("page = %d\n", decoded_page);
        
    //transform to offset
    decoded_offset = 0;
    for(i = 0; i < 8; i++)
        if(temp[31-i] == '1')
            decoded_offset += (int)pow(2, i);     
    //printf("offset = %d\n", decoded_offset);
    return;
}

void initialize(void){
    int i;

    tlb_counter = 0;
    frame_counter = 0;
    for(i = 0; i < 16; i++){
        tlb[i].page_num = -1;
        tlb[1].frame_num = -1;
    }

    for(i = 0; i < 256; i++){
        page_table[i] = -1;
        memset(frame[i], 0, 256);
    }
    return;
}
