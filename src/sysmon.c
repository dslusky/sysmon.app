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
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#include "sysmon.h"
#include "wmgeneral.h"

#include "sysmon-master.xpm"
#include "sysmon-mask.xbm"

unsigned int stats[3];

void createWindow(int argc, char *argv[]);
void refreshDisplay(void);
void drawMeter(int x, int y, int amount);


/* ========================================================================
 = CREATE_WINDOW
 =
 = Create main dockapp window
 ======================================================================= */

void createWindow(int argc, char *argv[]) {
    openXwindow(argc, argv,
        sysmon_master_xpm, sysmon_mask_bits, WIN_WIDTH, WIN_HEIGHT);
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
    copyXPMArea(IO_SRC_X, IO_SRC_Y, IO_WIDTH, IO_HEIGHT, IO_DST_X, IO_DST_Y);

    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, CPU_DST_X+CPU_WIDTH+3, CPU_DST_Y);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, MEM_DST_X+MEM_WIDTH+3, MEM_DST_Y);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, IO_DST_X+IO_WIDTH+3, IO_DST_Y);

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
 = UPDATE_STATS
 =
 = Gather system stats and update display
 ======================================================================= */

void updateStats() {
    drawMeter(CPU_METER_X, CPU_METER_Y, stats[STATS_CPU]);
    drawMeter(MEM_METER_X, MEM_METER_Y, stats[STATS_MEM]);
    drawMeter(IO_METER_X, IO_METER_Y, stats[STATS_IO]);
}


/* ========================================================================
 = MAIN
 =
 = You have entered a maze of twisty passages, all alike
 ======================================================================= */

int main(int argc, char *argv[]) {
    XEvent Event;

    memset(stats, 0, sizeof(stats));

    createWindow(argc, argv);
    refreshDisplay();

    while (1) {
        updateStats();
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
