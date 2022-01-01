#include<stdio.h>

static int get_file_size(char* filename)
{
    FILE *fp=fopen(filename,"rb");
    if(!fp) return -1;
    fseek(fp, 0L, SEEK_END);
    int size=ftell(fp);
    fclose(fp);

    return size;
}

static bool check_record(unsigned char* record_buf)
{
	bool ret = true;
    unsigned int packet_size = record_buf[1]*256 + record_buf[2];
    unsigned int payload_size = record_buf[10]*65536 + record_buf[9]*4096 + record_buf[8]*256 + record_buf[7];
    if((packet_size - payload_size != 9) || (record_buf[packet_size +3] != 0x55))
    {
		ret = false;
	}
    return ret;	
}

int main(int argc, char*argv[])
{
    char* file_name = argv[1];
    int size = 0;
    unsigned char buf[8000];
    int file_size = get_file_size(file_name);
    printf("file size = %d bytes\n", file_size);
    int start_position = 0;

    FILE* fp = fopen(file_name, "rb");
    unsigned char a, b, c;
    while(ftell(fp) < file_size - 1)
    {
        start_position = ftell(fp);
        fread(buf, 1, 1, fp);
        if(buf[0] == 0xaa)
        {
            fread(&buf[1], 1, 2, fp);
            size = buf[1]*256 + buf[2] + 1;
            if(size >= 8000)
            {
                printf("size check error 0x%x\n", start_position);
            }
            fread(&buf[3], 1, size, fp);
            if(check_record(buf) != true)
            {
            	printf("size = 0x%x buf[size+2] = 0x%x\n", size - 1, buf[size + 2]);
                printf("packet check error 0x%x\n", start_position);
                fseek(fp, start_position + 1, SEEK_SET);
            }
        }
        else
        {
            printf("record start error 0x%x\n", ftell(fp));
            while(buf[0] != 0xaa)
            {
                fread(buf, 1, 1, fp);
            }
            fseek(fp, -1, SEEK_CUR);   // rewind 1 byte to start the process again from 0xAA
        }

    }
    printf("process finished \n");
    return 0;
}

