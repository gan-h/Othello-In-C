all:
	gcc -o Chess.exe test.c -I./include/gtk-3.0 -I./include/pango-1.0 -I./include -I./include/glib-2.0 -I./lib/glib-2.0/include -I./include/harfbuzz -I./include/freetype2 -I./include/libpng16 -mms-bitfields -I./include/fribidi -I./include/cairo -I./include/lzo -I./include/pixman-1 -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -mms-bitfields -I./include/gdk-pixbuf-2.0 -mms-bitfields -mms-bitfields -mms-bitfields -I./include/atk-1.0 -mms-bitfields -mms-bitfields -mms-bitfields -pthread -mms-bitfields -L./lib -lgtk-3 -lgdk-3 -lz -lgdi32 -limm32 -lshell32 -lole32 -Wl,-luuid -lwinmm -ldwmapi -lsetupapi -lcfgmgr32 -lpangowin32-1.0 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl 

run:
	./Chess.exe

