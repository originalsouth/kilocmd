/* By BC van Zuiden -- Leiden, September 2014 */
/* Probably very buggy USE AT OWN RISK this will brick everything you own */
/* NOBODY but YOU is liable for anything that happened in result of using this */
/* WARNING: DON'T RUN THIS PROGRAM THIS WILL DESTROY YOUR COMPUTER AND/OR KILOBOTS */
/* Based (loosly) on kilogui by Alex Cornejo https://github.com/acornejo/kilogui */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftdi.h>
#include <unistd.h>
#include "intelhex.h"

struct ftdi_context *ftdic;
intelhex::hex_data data;

bool silent=false;

char VOLTAGE[132]={};
char PAUSE[132]={};
char RUN[132]={};
char RESET[132]={};
char SLEEP[132]={};
char LEDTOGGLE[132]={};
char BOOTLOAD[132]={};
char CHARGE[132]={};
char STOP[132]={};

void init()
{
    VOLTAGE[0]=0x55,VOLTAGE[1]=0x02,VOLTAGE[11]=0x87,VOLTAGE[131]=0xD0;
    PAUSE[0]=0x55,PAUSE[1]=0x02,PAUSE[11]=0x85,PAUSE[131]=0xD2;
    RUN[0]=0x55,RUN[1]=0x02,RUN[11]=0x88,RUN[131]=0xDF;
    RESET[0]=0x55,RESET[1]=0x02,RESET[11]=0x83,RESET[131]=0xD4;
    SLEEP[0]=0x55,SLEEP[1]=0x02,SLEEP[11]=0x84,SLEEP[131]=0xD3;
    LEDTOGGLE[0]=0x55,LEDTOGGLE[1]=0x01,LEDTOGGLE[131]=0x54;
    BOOTLOAD[0]=0x55,BOOTLOAD[1]=0x02,BOOTLOAD[11]=0x80,BOOTLOAD[131]=0xD7;
    CHARGE[0]=0x55,CHARGE[1]=0x02,CHARGE[11]=0x86,CHARGE[131]=0xD1;
    STOP[0]=0x55,STOP[131]=0x55;
}

void close()
{
    if(ftdic)
    {
        ftdi_usb_close(ftdic);
        ftdi_free(ftdic);
        ftdic=nullptr;
        if(!silent) puts("kilocmd: disconnected");
    }
    else if(!silent) puts("kilocmd: already disconnected");
}

void open()
{
    unsigned int chipid;
    if(ftdic!=nullptr)
    {
        if(!ftdi_read_chipid(ftdic,&chipid))
        {
            if(!silent) puts("kilocmd: device already opened");
            return;
        }
        else close();
    }
    if(ftdic==nullptr)
    {
        ftdic=ftdi_new();
        ftdic->usb_read_timeout=5000;
        ftdic->usb_write_timeout=1000;
        if(ftdi_usb_open(ftdic,0x403,0x6001))
        {
           if(!silent) puts("kilocmd: device not found");
           ftdi_free(ftdic);
           ftdic=nullptr;
        }
        else
        {
            if(ftdi_set_baudrate(ftdic,38400))
            {
                if(!silent) puts("kilocmd: unable to set baudrate");
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic=nullptr;
            }
            else if(ftdi_setflowctrl(ftdic,SIO_DISABLE_FLOW_CTRL))
            {
                if(!silent) puts("kilocmd: unable to set flowctrl");
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic=nullptr;
            }
            else if(ftdi_set_line_property(ftdic,BITS_8,STOP_BIT_1,NONE))
            {
                if(!silent) puts("kilocmd: unable to set line property");
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic=nullptr;
            }
            else if(!silent) puts("kilocmd: connected!");
        }
    }
}

void send_cmd(char *cmd)
{
    if(ftdic)
    {
        if(ftdi_write_data(ftdic,(unsigned char *)cmd,132)==132)
        {
            if(!silent) puts("kilocmd: successfully sent command!");
        }
        else if(!silent) puts("kilocmd: error sending command");
    }
    else if(!silent) puts("kilocmd: not connected to device...");
}

void send_program(const char *fname)
{
    if(ftdic)
    {
        unsigned char prgm[132];
        FILE *file=fopen(fname,"r");
        if(!file)
        {
            if(!silent) printf("kilocmd: unable to open file \"%s\"\n",fname);
            return;
        }
        fclose(file);
        data.load(fname);
        uint8_t pg_t=1+data.size()/128;
        if(pg_t>220) pg_t=220;
        uint8_t pg=pg_t;
        for(unsigned int i=1;i<3U;)
        {
            if(pg>=pg_t)
            {
                pg=0;
                memset(prgm,0,132);
                prgm[0]=0x55,prgm[1]=0x2,prgm[2]=pg_t,prgm[11]=0x82,prgm[131]=0x55^0x2^pg_t^0x82;
                if(ftdi_write_data(ftdic,prgm,132)!=132)
                {
                    if(!silent) puts("kilocmd: error uploading program");
                    return;
                }
                i++;
            }
            else
            {
                prgm[0]=0x55,prgm[1]=0x4,prgm[2]=pg;
                uint8_t cksm=0x55^0x4^pg,dbyte;
                for(unsigned int j=0;j<128;j++)
                {
                    dbyte=data.get(pg*128+j);
                    prgm[j+3]=dbyte;
                    cksm^=dbyte;
                }
                prgm[131]=cksm;
                if(ftdi_write_data(ftdic,prgm,132)!=132)
                {
                    if(!silent) puts("kilocmd: error uploading program");
                    return;
                }
                else pg++;
            }
            usleep(130000);
        }
    }
    else
    {
        if(!silent) puts("kilocmd: not connected to device...");
        return;
    }
    if(!silent) printf("kilocmd: successfully uploaded \"%s\"\n!",fname);
}

char* strtokar(char *source,char delim,unsigned int pos)
{
    static char dest[4096];
    unsigned int i=0,dlim=0;
    for(;dlim<pos;i++)
    {
        if(source[i]==delim) dlim++;
        else if(source[i]==0)
        {
            dest[0]=0;
            return dest;
        }
    }
    strcpy(dest,&source[i]);
    for(i=0;dest[i]>0;i++) if(dest[i]==delim) dest[i]=0;
    return dest;
}

void proccess(FILE *file)
{
    char stroutel[4096];
    init();
    open();
    while(!feof(file))
    {
        stroutel[0]=0;
        fgets(stroutel,4096,file);
        for(size_t i=0;i<strlen(stroutel);i++) if(stroutel[i]=='\n') stroutel[i]=0;
        if(!strcmp(stroutel,"\0")) continue;
        else if(!strcmp(strtokar(stroutel,' ',0),"VOLTAGE")) send_cmd(VOLTAGE);
        else if(!strcmp(strtokar(stroutel,' ',0),"CHARGE")) send_cmd(CHARGE);
        else if(!strcmp(strtokar(stroutel,' ',0),"PAUSE")) send_cmd(PAUSE);
        else if(!strcmp(strtokar(stroutel,' ',0),"RUN")) send_cmd(RUN);
        else if(!strcmp(strtokar(stroutel,' ',0),"RESET")) send_cmd(RESET);
        else if(!strcmp(strtokar(stroutel,' ',0),"SLEEP")) send_cmd(SLEEP);
        else if(!strcmp(strtokar(stroutel,' ',0),"TOGGLE")) send_cmd(LEDTOGGLE);
        else if(!strcmp(strtokar(stroutel,' ',0),"STOP")) send_cmd(STOP);
        else if(!strcmp(strtokar(stroutel,' ',0),"BOOTLOAD")) send_cmd(BOOTLOAD);
        else if(!strcmp(strtokar(stroutel,' ',0),"UPLOAD")) send_program(strtokar(stroutel,' ',1));
        else if(!strcmp(strtokar(stroutel,' ',0),"STFU"))
        {
            silent=not silent;
            if(!silent) puts("kilocmd: are we talking again?");
        }
        else fprintf(stderr,"kilocmd: error parsing command \"%s\"\n",stroutel);
    }
    close();
}

void help()
{
    puts("kilocmd: WILL DESTROY YOUR COMPUTER AND/OR KILOBOTS!");
    puts("kilocmd: USE AT OWN RISK!\n");
    puts("usage: kilocmd [options] \"filename\"");
    puts("if no filename is provided stdin is used (use ^D to exit)");
    puts("options:");
    puts("\t-s or --stfu\tstartup in silent mode");
    puts("\t-h or --help\tproduce this output");
    puts("commands:");
    puts("\tVOLTAGE\t\tdisplay voltage level using LED (blue/green = fully charged, yellow/red = need battery)");
    puts("\tCHARGE\t\tsets the kilobot in charge mode");
    puts("\tPAUSE\t\tpause the user program");
    puts("\tRUN\t\truns the currently uploaded program");
    puts("\tRESET\t\tjump to the user program starting point");
    puts("\tSLEEP\t\tswitch to low-power sleep mode");
    puts("\tTOGGLE\t\ttoggle LEDs on controller");
    puts("\tSTOP\t\tsends an empty message");
    puts("\tBOOTLOAD\tjump to their bootloader to accept a new program");
    puts("\tUPLOAD filename\tsend a new program to kilobots that are in bootloader mode");
    puts("\tSTFU\t\ttoggle silent mode");
}

int main(int argc,char *argv[])
{
    if(argc==1) proccess(stdin);
    else if(argc==2)
    {
        if(argv[1][0]=='-' and argv[1][1]=='s') silent=true;
        else if(argv[1][0]=='-' and argv[1][1]=='-' and !strcmp(&argv[1][2],"stfu")) silent=true;
        else if(argv[1][0]=='-' and argv[1][1]=='h')
        {
            help();
            return EXIT_SUCCESS;
        }
        else if(argv[1][0]=='-' and argv[1][1]=='-' and !strcmp(&argv[1][2],"help"))
        {
            help();
            return EXIT_SUCCESS;
        }
        else
        {
            FILE *file=fopen(argv[1],"r");
            if(!file)
            {
                fprintf(stderr,"kilocmd: unable to open file \"%s\"\n",argv[1]);
                return EXIT_FAILURE;
            }
            proccess(file);
            return EXIT_SUCCESS;
        }
        proccess(stdin);
    }
    else if(argc==3)
    {
        if(argv[1][0]=='-' and argv[1][1]=='s') silent=true;
        else if(argv[1][0]=='-' and argv[1][1]=='h')
        {
            help();
            return EXIT_SUCCESS;
        }
        else if(argv[1][0]=='-' and argv[1][1]=='-' and !strcmp(&argv[1][2],"help"))
        {
            help();
            return EXIT_SUCCESS;
        }
        FILE *file=fopen(argv[2],"r");
        if(!file)
        {
            fprintf(stderr,"kilocmd: unable to open file \"%s\"\n",argv[2]);
            return EXIT_FAILURE;
        }
        proccess(file);
    }
    return EXIT_SUCCESS;
}

/* By BC van Zuiden -- Leiden, September 2014 */
/* Probably very buggy USE AT OWN RISK this will brick everything you own */
/* NOBODY but YOU is liable for anything that happened in result of using this */
/* WARNING: DON'T RUN THIS PROGRAM THIS WILL DESTROY YOUR COMPUTER */
/* Based (loosly) on kilogui by Alex Cornejo https://github.com/acornejo/kilogui */
