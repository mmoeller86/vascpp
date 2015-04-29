CC=cc
LINK=cc
CFLAGS=
CPPFLAGS=

OBJS=vas.o posix.o
all: vas_test.exe

vas.o: vas.cpp vas.h win32.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) vas.cpp
	
posix.o: posix.cpp posix.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) posix.cpp
	
vas_test.o: vas_test.cpp win32.h vas.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) vas_test.cpp
	
vas_test.exe: $(OBJS) vas_test.o
	$(LINK) -o vas_test.exe $(OBJS) vas_test.o
