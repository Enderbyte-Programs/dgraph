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
    long graph[mx+1];
    for (int l = 0;l > mx+1;l++) {
        graph[l] = 0;
    }
    long diff = 0;
    long totaldiff = 0;
    char tick = 0;
    long ftick = 0;
    long ofs;
    long blocksize;
    long totalsize;
    long freespace;
    long used;
    int maxy;
    int miny;
    
    while (true)
    {
        tick++;
        ftick++;
        
        if (tick == 10) {
            tick = 0;
            totaldiff += diff;
            diff = 0;
        }
        struct statfs data;
        statfs(argv[0],&data);
        blocksize = data.f_bsize;
        totalsize = data.f_blocks*blocksize;
        freespace = data.f_bavail*blocksize;
        used = totalsize - freespace;
        if (ftick == 1) {
            ofs = freespace;
        }

        //get maximum and minimum graph values
        int tmin = graph[0];
        int tmax = graph[0];
        for (int __i = 1;__i < mx+1;__i++) {
            if (graph[__i] > tmax) {
                tmax = graph[__i];
            }
            if (graph[__i] < tmin) {
                tmin = graph[__i];
            }
        }
        maxy = tmax;
        miny = tmin;

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
        char __maxmin[MAXINFOSIZE];
        snprintf(__maxmin,MAXINFOSIZE,"+ %ld - %ld",parse_size((long)maxy),parse_size((long)miny));
        mvaddstr(2,MAXINFOSIZE,__maxmin);

        //Debug data
        char __ticks[10];
        snprintf(__ticks,10,"t: %ld",ftick);
        mvaddstr(my-1,0,__ticks);
        //End Debug Data

        //Compare data
        int xdiff;
        if (freespace < ofs) {
            diff += -(ofs - freespace);
            xdiff = (ofs - freespace);
        } else {
            diff += (freespace - ofs);
            xdiff = (freespace - ofs);
        }
        for (int i = 1;i < mx;i++) {
            graph[i-1] = graph[i];
        }
        graph[mx] = xdiff;
        
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