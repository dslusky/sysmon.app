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

#ifndef __SYSMON_H__
#define __SYSMON_H__

typedef struct {
    long int active;
    long int idle;
    long int total;
} cpu_stat_t;

typedef struct {
    cpu_stat_t cpu;
} stat_t;

enum {
    STATS_CPU,
    STATS_MEM,
    STATS_IO
};

#define PROC_STATS "/proc/stat"

#define WIN_WIDTH  64
#define WIN_HEIGHT 64

#define CPU_SRC_X   1
#define CPU_SRC_Y   74
#define CPU_DST_X   7
#define CPU_DST_Y   7
#define CPU_METER_X 27
#define CPU_METER_Y 7
#define CPU_WIDTH   17
#define CPU_HEIGHT  7

#define MEM_SRC_X   19
#define MEM_SRC_Y   74
#define MEM_DST_X   7
#define MEM_DST_Y   17
#define MEM_METER_X 27
#define MEM_METER_Y 17
#define MEM_WIDTH   17
#define MEM_HEIGHT  7

#define IO_SRC_X   37
#define IO_SRC_Y   74
#define IO_DST_X   7
#define IO_DST_Y   27
#define IO_METER_X 27
#define IO_METER_Y 27
#define IO_WIDTH   17
#define IO_HEIGHT  7

#define METER_BG_X   32
#define METER_BG_Y   82
#define METER_FG_X   1
#define METER_FG_Y   82
#define METER_WIDTH  30
#define METER_HEIGHT 7

#define SPACER_SRC_X  0
#define SPACER_SRC_Y  90
#define SPACER_DST_X  5
#define SPACER_DST_Y  37
#define SPACER_WIDTH  54
#define SPACER_HEIGHT 1

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#endif // __SYSMON_H__
