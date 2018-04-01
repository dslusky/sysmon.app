#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#include "sysmon.h"
#include "wmgeneral.h"

#include "sysmon-master.xpm"
#include "sysmon-mask.xbm"

void createWindow(int argc, char *argv[]);
void refreshDisplay(void);

void createWindow(int argc, char *argv[]) {
    openXwindow(argc, argv,
        sysmon_master_xpm, sysmon_mask_bits, WIN_WIDTH, WIN_HEIGHT);
    setMaskXY(0, 0);
}

void refreshDisplay(void) {
    copyXPMArea(CPU_SRC_X, CPU_SRC_Y, CPU_WIDTH, CPU_HEIGHT, CPU_DST_X, CPU_DST_Y);
    copyXPMArea(MEM_SRC_X, MEM_SRC_Y, MEM_WIDTH, MEM_HEIGHT, MEM_DST_X, MEM_DST_Y);
    copyXPMArea(IO_SRC_X, IO_SRC_Y, IO_WIDTH, IO_HEIGHT, IO_DST_X, IO_DST_Y);

    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, CPU_DST_X+CPU_WIDTH, CPU_DST_Y);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, MEM_DST_X+MEM_WIDTH, MEM_DST_Y);
    copyXPMArea(METER_BG_X, METER_BG_Y, METER_WIDTH, METER_HEIGHT, IO_DST_X+IO_WIDTH, IO_DST_Y);

    copyXPMArea(SPACER_SRC_X, SPACER_SRC_Y, SPACER_WIDTH, SPACER_HEIGHT, SPACER_DST_X, SPACER_DST_Y);

    RedrawWindowXY(0, 0);
}

int main(int argc, char *argv[]) {
    XEvent Event;
    createWindow(argc, argv);
    while (1) {
        refreshDisplay();
        while (XPending(display)) {
            XNextEvent(display, &Event);
            switch (Event.type) {
                case Expose:
                    RedrawWindowXY(0, 0);
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
