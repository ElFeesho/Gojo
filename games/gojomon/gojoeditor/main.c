#include <gtk/gtk.h>
#include "window.h"
#include "draw.h"

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	g_set_application_name("Gojomon Level Editor");
	draw_init();
	window_show();
	
	gtk_main();
	return 0;
}

