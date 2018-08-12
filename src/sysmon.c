/*
    Sysmon.app - system monitoring dockapp for WindowMaker
    Copyright (C) 2018  David Slusky

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#include "sysmon.h"
#include "wmgeneral.h"

#ifdef SIZE_SMALL
#  include "sysmon-small-master.xpm"
#  include "sysmon-small-mask.xbm"
#else
#  include "sysmon-master.xpm"
#  include "sysmon-mask.xbm"
#endif

void createWindow(int argc, char *argv[]);
void refreshDisplay(void);
void drawMeter(int x, int y, int amount);
void drawLoadAvg(loadavg_t *loadavg);
void updateCpuMeter(cpu_stat_t *current, cpu_stat_t *last);
void updateMemMeter();
void updateIoMeter(io_stat_t *current, io_stat_t *last);
void updateLoadMeter(loadavg_t *loadavg);


/* ========================================================================
 = CREATE_WINDOW
 =
 = Create main dockapp window
 ======================================================================= */

void createWindow(int argc, char *argv[]) {
#ifdef SIZE_SMALL
    openXwindow(argc, argv,
        sysmon_small_master_xpm, sysmon_small_mask_bits, WIN_WIDTH, WIN_HEIGHT);
#else
    openXwindow(argc, argv,
        sysmon_master_xpm, sysmon_mask_bits, WIN_WIDTH, WIN_HEIGHT);
#endif
    setMaskXY(0, 0);
}


/* ========================================================================
 = REFRESH_DISPLAY
 =
 = Redraw the whole display area
 ======================================================================= */

void refreshDisplay(void) {
    copyXPMArea(CPU_SRC_X, CPU_SRC_Y, CPU_WIDTH, CPU_HEIGHT, CPU_DST_X, CPU_DST_Y);
    copyXPMArea(MEM_SRC_X, MEM_SRC_Y, MEM_WIDTH, MEM_HEIGHT, MEM_DST_X, MEM_DST_Y);

    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, CPU_DST_X+CPU_WIDTH+3, CPU_DST_Y);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, MEM_DST_X+MEM_WIDTH+3, MEM_DST_Y);

#ifndef SIZE_SMALL
    copyXPMArea(IO_SRC_X, IO_SRC_Y, IO_WIDTH, IO_HEIGHT, IO_DST_X, IO_DST_Y);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, IO_DST_X+IO_WIDTH+3, IO_DST_Y);
#endif

    copyXPMArea(SPACER_SRC_X, SPACER_SRC_Y, SPACER_WIDTH, SPACER_HEIGHT, SPACER_DST_X, SPACER_DST_Y);
    RedrawWindow();
}


/* ========================================================================
 = DRAW_METER
 =
 = Display meter at XY coordinates
 ======================================================================= */

void drawMeter(int x, int y, int amount) {
    amount = CLAMP(amount, 0, 100);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, x, y);
    copyXPMArea(METER_FG_X, METER_FG_Y, amount*METER_WIDTH/100, METER_HEIGHT, x, y);
    RedrawRegion(x, y, METER_WIDTH, METER_HEIGHT);
}


/* ========================================================================
 = DRAW_LOADAVG
 =
 = Display load average graph
 ======================================================================= */

void drawLoadAvg(loadavg_t *loadavg) {
    float max = 1.0F;

    // find highest value for scale
    for (int i = 0; i < LOAD_HIST_LEN; i++)
        if (loadavg->history[i] > max) max = loadavg->history[i];

    // clear graph area
    for (int i = 0; i < LOADAVG_HEIGHT; i++)
        copyXPMArea(VIEW_BG_X, VIEW_BG_Y, VIEW_WIDTH, 1, VIEW_DST_X, LOADAVG_DST_Y+i);

    // draw updated graph
    for (int i = 0; i < LOAD_HIST_LEN; i++) {
        int index = loadavg->isWrapped ? (loadavg->index+i) % LOAD_HIST_LEN : i;
        int height = (int)(loadavg->history[index] / max * LOADAVG_HEIGHT);
        int y = LOADAVG_DST_Y + LOADAVG_HEIGHT - height;

        copyXPMArea(LOADAVG_SRC_X, LOADAVG_SRC_Y,
            LOADAVG_WIDTH, height, LOADAVG_DST_X+i, y);
    }

    RedrawRegion(VIEW_DST_X, VIEW_DST_Y, VIEW_WIDTH, VIEW_HEIGHT);
}


/* ========================================================================
 = UPDATE_CPU_METER
 =
 = Gather CPU stats and update meter
 ======================================================================= */

void updateCpuMeter(cpu_stat_t *current, cpu_stat_t *last) {
    long int user, nice, sys, idle;
    long int dt, da, usage;
    FILE *procFile;

    if ((procFile = fopen(PROC_STATS, "r")) == NULL) {
        fprintf(stderr, "Cannot open '%s' for reading: %s\n", PROC_STATS, strerror(errno));
        exit(1);
    }

    fscanf(procFile, "cpu %ld %ld %ld %ld", &user, &nice, &sys, &idle);
    fclose(procFile);

    memcpy(last, current, sizeof(cpu_stat_t));
    current->active = (user + nice + sys);
    current->idle = idle;
    current->total = current->active + current->idle;

    dt = MAX(1, current->total - last->total);
    da = MAX(0, current->active - last->active);
    usage = da*100 / dt;

    drawMeter(CPU_METER_X, CPU_METER_Y, usage);
}


/* ========================================================================
 = UPDATE_MEM_METER
 =
 = Gather memory stats and update meter
 ======================================================================= */

void updateMemMeter() {
    long int total = -1, active = -1, unused = -1, buffers = -1, cached = -1;
    FILE *procFile;
    char buf[128];

    if ((procFile = fopen(PROC_MEMINFO, "r")) == NULL) {
        fprintf(stderr, "Cannot open '%s' for reading: %s\n", PROC_MEMINFO, strerror(errno));
        exit(1);
    }

    while (fgets(buf, sizeof(buf), procFile) != NULL) {
        char *token;

        if ((token = strtok(buf, " ")) == NULL)
            continue;

        if (!strncmp(token, "MemTotal:", 9)) {
            token = strtok(NULL, " ");
            total = MAX(1, atoi(token));
        }
        else if (!strncmp(token, "MemFree:", 8)) {
            token = strtok(NULL, " ");
            unused = MAX(0, atoi(token));
        }
        else if (!strncmp(token, "Buffers:", 8)) {
            token = strtok(NULL, " ");
            buffers = MAX(0, atoi(token));
        }
        else if (!strncmp(token, "Cached:", 7)) {
            token = strtok(NULL, " ");
            cached = MAX(0, atoi(token));
        }
    }

    fclose(procFile);

    if (total == -1 || unused == -1 || buffers == -1 || cached == -1) {
        fprintf(stderr, "Failed to read required fields from '%s'!\n", PROC_MEMINFO);
        exit(1);
    }

    active = total - (unused + buffers + cached);
    drawMeter(MEM_METER_X, MEM_METER_Y, (active*100 / total));
}


/* ========================================================================
 = UPDATE_IO_METER
 =
 = Gather disk IO stats and update meter
 ======================================================================= */

void updateIoMeter(io_stat_t *current, io_stat_t *last) {
    FILE *procFile;
    char buf[128];
    long int delta;

    if ((procFile = fopen(PROC_DISKSTATS, "r")) == NULL) {
        fprintf(stderr, "Cannot open '%s' for reading: %s\n", PROC_DISKSTATS, strerror(errno));
        exit(1);
    }

    // TODO: allow user to specify disk(s) to monitor
    memcpy(last, current, sizeof(io_stat_t));
    memset(current, 0, sizeof(io_stat_t));
    while (fgets(buf, sizeof(buf), procFile) != NULL) {
        char *token;

        if ((token = strtok(buf, " ")) == NULL) continue; // major
        if ((token = strtok(NULL, " ")) == NULL) continue; // minor
        if ((token = strtok(NULL, " ")) == NULL) continue; // device

        if (!strncmp(token, "sd", 2) && strlen(token) == 3) {
            for (int i = 0; i < 11; i++)
                if ((token = strtok(NULL, " ")) == NULL) break;
            current->weighted += MAX(0, atoi(token));
        }
    }

    fclose(procFile);

    // max was reset, wait until enough data has been cycled through
    if (current->max == -1 || last->max == -1) return;

    delta = current->weighted - last->weighted;
    current->max = MAX(1, delta > last->max ? delta : last->max);

    drawMeter(IO_METER_X, IO_METER_Y, delta*100 / current->max);
}


/* ========================================================================
 = UPDATE_LOAD_METER
 =
 = Gather load average stats and update meter
 ======================================================================= */

void updateLoadMeter(loadavg_t *loadavg) {
    FILE *procFile;
    float value;

    if ((procFile = fopen(PROC_LOADAVG, "r")) == NULL) {
        fprintf(stderr, "Cannot open '%s' for reading: %s\n", PROC_LOADAVG, strerror(errno));
        exit(1);
    }

    fscanf(procFile, "%f", &value);
    fclose(procFile);

    loadavg->history[loadavg->index++] = value;

    if (loadavg->index >= LOAD_HIST_LEN) {
        loadavg->index = 0;
        loadavg->isWrapped = 1;
    }

    drawLoadAvg(loadavg);
}


/* ========================================================================
 = MAIN
 =
 = You have entered a maze of twisty passages, all alike
 ======================================================================= */

int main(int argc, char *argv[]) {
    XEvent Event;
    stat_t current, last;
    loadavg_t loadavg;
    time_t now;

    memset(&current, 0, sizeof(current));
    memset(&last, 0, sizeof(last));
    memset(&loadavg, 0, sizeof(loadavg));

    current.io.max = -1; // signal that max has been reset

    createWindow(argc, argv);
    refreshDisplay();

    while (1) {
        updateCpuMeter(&current.cpu, &last.cpu);
        updateMemMeter();
#ifndef SIZE_SMALL
        updateIoMeter(&current.io, &last.io);
#endif

        now = time(NULL);
        if ((now - loadavg.lastUpdate) > LOADAVG_INTERVAL) {
            updateLoadMeter(&loadavg);
            loadavg.lastUpdate = now;
        }

        while (XPending(display)) {
            XNextEvent(display, &Event);
            switch (Event.type) {
                case Expose:
                    refreshDisplay();
                    break;
                case DestroyNotify:
                    XCloseDisplay(display);
                    exit(0);
                    break;
                default:
                    // printf("unhandled event: %d\n", Event.type);
                    break;
            }
        }
        usleep(250000L);
    }
    return 0;
}
