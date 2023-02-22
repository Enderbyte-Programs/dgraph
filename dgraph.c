#include <curses.h>
#include <sys/vfs.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define MAXINFOSIZE 40//Maximum size of text-only info

long double rround(long double in,int places) {
    long double ir = in * pow(10,places);
    return roundl(ir) / pow(10,places);
}

char* parse_size(long indata) {
    char* final = (char*)malloc(MAXINFOSIZE-10);
    if (indata < 0) {
        indata = -indata;
    }
    indata = (long double)indata;//Force long double over long with no floating point
    if (indata > 2000000000) {
        sprintf(final,"%.2Lf GB",rround(indata/1000000000.0,2));
    } else if (indata > 2000000) {
        sprintf(final,"%.2Lf MB",rround(indata/1000000.0,2));
    } else if (indata > 2000) {
        sprintf(final,"%.2Lf KB",rround(indata/(long double)1000.0,2));
    } else {
        sprintf(final,"%.2Lf bytes",rround(indata,2));
    }

    return final;
}

int msleep(long tms)
{
    struct timespec ts;
    int ret;

    if (tms < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = tms / 1000;
    ts.tv_nsec = (tms % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}

int main(int argc,char *argv[]) {
    printf("Getting ready...");
    //Init screen
    initscr();
    keypad(stdscr,true);
    noecho();
    cbreak();
    nodelay(stdscr,true);
    int my = getmaxy(stdscr);
    int mx = getmaxx(stdscr);
    long graph[mx+5];
    long diff = 0;
    long totaldiff = 0;
    char tick = 0;
    long ftick = 0;
    long ofs;
    while (true)
    {
        tick++;
        ftick++;
        
        if (tick == 10) {
            tick = 0;
            diff = 0;
        }
        struct statfs data;
        statfs(argv[0],&data);
        long blocksize = data.f_bsize;
        long totalsize = data.f_blocks*blocksize;
        long freespace = data.f_bavail*blocksize;
        long used = totalsize - freespace;
        if (ftick == 1) {
            ofs = freespace;
        }

        //Print text info to screen

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
        char __diff[MAXINFOSIZE];
        snprintf(__diff,MAXINFOSIZE,"Write Speed: %s/sec",parse_size(diff));
        mvaddstr(0,MAXINFOSIZE,__diff);
        char __tdiff[MAXINFOSIZE];
        snprintf(__tdiff,MAXINFOSIZE,"Net Change: %s",parse_size(totaldiff));
        mvaddstr(1,MAXINFOSIZE,__tdiff);

        //Debug data
        char __ticks[10];
        snprintf(__ticks,10,"t: %ld",ftick);
        mvaddstr(my-1,0,__ticks);
        //End Debug Data

        //Compare data
        if (freespace < ofs) {
            diff += -(ofs - freespace);
            totaldiff += -(ofs - freespace);
        } else {
            diff += freespace - ofs;
            totaldiff += freespace - ofs;
        }
        
        //Check keys

        int ch = getch();
        if (ch == KEY_F(10)) {
            break;
        }

        //Wait for 0.1 seconds

        msleep(100);
        ofs = freespace; //Update old data after comparisons
        erase();//Clear screen
    }

    reset_shell_mode();//Un-init screen
    return 0;
}