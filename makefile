all:
	gcc `pkg-config --cflags gtk+-3.0` -o Chess.exe main.c `pkg-config --libs gtk+-3.0`
run:
	./Chess.exe

