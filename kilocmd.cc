/* By BC van Zuiden -- Leiden, September 2014 */
/* Probably very buggy USE AT OWN RISK this will brick everything you own */
/* NOBODY but YOU is liable for anything that happened in result of using this */
/* WARNING: DON'T RUN THIS PROGRAM THIS WILL DESTROY YOUR COMPUTER AND/OR KILOBOTS */
/* Based (loosly) on kilogui by Alex Cornejo https://github.com/acornejo/kilogui */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftdi.h>
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
    VOLTAGE[0]=0x55;
    VOLTAGE[1]=0x02;
    VOLTAGE[11]=0x87;
    VOLTAGE[131]=0xD0;
    PAUSE[0]=0x55;
    PAUSE[1]=0x02;
    PAUSE[11]=0x85;
    PAUSE[131]=0xD2;
    RUN[0]=0x55;
    RUN[1]=0x02;
    RUN[11]=0x88;
    RUN[131]=0xDF;
    RESET[0]=0x55;
    RESET[1]=0x02;
    RESET[11]=0x83;
    RESET[131]=0xD4;
    SLEEP[0]=0x55;
    SLEEP[1]=0x02;
    SLEEP[11]=0x84;
    SLEEP[131]=0xD3;
    LEDTOGGLE[0]=0x55;
    LEDTOGGLE[1]=0x01;
    LEDTOGGLE[131]=0x54;
    BOOTLOAD[0]=0x55;
    BOOTLOAD[1]=0x02;
    BOOTLOAD[11]=0x80;
    BOOTLOAD[131]=0xD7;
    CHARGE[0]=0x55;
    CHARGE[1]=0x02;
    CHARGE[11]=0x86;
    CHARGE[131]=0xD1;
    STOP[0]=0x55;
    STOP[131]=0x55;
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
            else if(!silent) puts("kilcmd: connected!");
        }
    }
}

void send_cmd(char *cmd)
{
    if(ftdic)
    {
        if(ftdi_write_data(ftdic,(unsigned char *)cmd,132)==132)
        {
            if(!silent) puts("kilocmd: succesfully sent command!");
        }
        else if(!silent) puts("kilocmd: error sending command");
    }
    else if(!silent) puts("kilocmd: not connected to device...");
}

void send_program(char *fname); //TODO

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
        else if(!strcmp(stroutel,"VOLTAGE")) send_cmd(VOLTAGE);
        else if(!strcmp(stroutel,"CHARGE")) send_cmd(CHARGE);
        else if(!strcmp(stroutel,"PAUSE")) send_cmd(PAUSE);
        else if(!strcmp(stroutel,"RUN")) send_cmd(RUN);
        else if(!strcmp(stroutel,"RESET")) send_cmd(RESET);
        else if(!strcmp(stroutel,"SLEEP")) send_cmd(SLEEP);
        else if(!strcmp(stroutel,"TOGGLE")) send_cmd(LEDTOGGLE);
        else if(!strcmp(stroutel,"STOP")) send_cmd(STOP);
        else if(!strcmp(stroutel,"BOOTLOAD")) send_cmd(BOOTLOAD);
        else if(!strcmp(stroutel,"UPLOAD")) puts("kilocmd: to be implemented");
        else fprintf(stderr,"kilocmd: error parsing command %s\n",stroutel);
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
    puts("\t-s\t\t\tstfu or silent mode");
    puts("\t-h or --help\tproduce this output");
    puts("commands:");
    puts("\tVOLTAGE");
    puts("\tCHARGE");
    puts("\tPAUSE");
    puts("\tRUN");
    puts("\tRESET");
    puts("\tSLEEP");
    puts("\tTOGGLE");
    puts("\tSTOP");
    puts("\tBOOTLOAD");
    puts("\tUPLOAD filename (to be implemented)"); //TODO
}

int main(int argc,char *argv[])
{
    if(argc==1) proccess(stdin);
    else if(argc==2)
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
        else
        {
            FILE *file=fopen(argv[1],"r");
            if(!file)
            {
                fprintf(stderr,"kilocmd: unable to open file \"%s\"",argv[2]);
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
            fprintf(stderr,"kilocmd: unable to open file \"%s\"",argv[2]);
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
