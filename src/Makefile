LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11
OBJS =  sysmon.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/list.o \
		../wmgeneral/misc.o


.c.o:
	gcc -c -O2 -Wall $< -o $*.o

sysmon: $(OBJS)
	gcc -o sysmon $^ $(LIBDIR) $(LIBS)

clean::
	for i in $(OBJS) ; do \
		rm -f $$i;\
	done
	rm -f sysmon
