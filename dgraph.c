#include <curses.h>
#include <sys/vfs.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

#define MAXINFOSIZE 40

long double rround(long double in,int places) {
    long double ir = in * pow(10,places);
    return roundl(ir) / pow(10,places);
}

char* parse_size(long indata) {
    char* final = (char*)malloc(20);
    if (indata < 0) {
        indata = -indata;
    }
    indata = (long double)indata;//Force long double over long with no floating point
    if (indata > 2000000) {
        sprintf(final,"%ld GB",rround(indata,2));
    } else if (indata > 2000000) {
        sprintf(final,"%ld MB",rround(indata,2));
    } else if (indata > 2000) {
        sprintf(final,"%ld KB",rround(indata,2));
    } else {
        sprintf(final,"%ld bytes",rround(indata,2));
    }

    return final;
}

int main(int argc,char *argv[]) {
    //Init screen
    initscr();
    keypad(stdscr,true);
    noecho();
    cbreak();
    nodelay(stdscr,true);
    int my = getmaxy(stdscr);
    int mx = getmaxx(stdscr);
    long graph[mx+5];
    while (true)
    {
        struct statfs data;
        int my = getmaxy(stdscr);
        int mx = getmaxx(stdscr);
        statfs(argv[0],&data);
        long blocksize = data.f_bsize;
        long totalsize = data.f_blocks*blocksize;
        long freespace = data.f_bavail*blocksize;
        long used = totalsize - freespace;
        //char* __tlx = (char*)malloc(20);
        char __tlx[MAXINFOSIZE];
        snprintf(__tlx,MAXINFOSIZE,"Total Space: %ld bytes",totalsize);
        mvaddstr(0,0,__tlx);
        //char* __ulx = (char*)malloc(20);
        char __ulx[MAXINFOSIZE];
        snprintf(__ulx,MAXINFOSIZE,"Used Space: %ld bytes",used);
        mvaddstr(1,0,__ulx);
        char __flx[MAXINFOSIZE];
        snprintf(__flx,MAXINFOSIZE,"Free Space: %ld bytes",freespace);
        mvaddstr(2,0,__flx);


        int ch = getch();
        if (ch == KEY_F(10)) {
            break;
        }
        struct timespec __wait;
        __wait.tv_nsec = 100000;
        nanosleep(&__wait,&__wait);
    }

    reset_shell_mode();//Un-init screen
    return 0;
}