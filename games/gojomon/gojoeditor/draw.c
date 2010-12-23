#include <gtk/gtk.h>
#include <memory.h>
#include "fileops.h"
#include "window.h"

char *level_data = NULL;
static GdkPixmap *level_tileset = NULL;

static int ctilex = 0;
static int ctiley = 0;

static void draw_grid(int width, int height, GdkDrawable *drawable)
{
	GdkGC *canv_gc = gdk_gc_new(drawable);
	gdk_gc_set_function(canv_gc,GDK_INVERT);
	int w = 0, h = 0;
	gdk_drawable_get_size(drawable,&w, &h);
	
	int i = 0;
	for(i = 0;i<h;i+=height)
	{
		gdk_draw_line(drawable,canv_gc, 0, i, w, i);
	}
	
	for(i = 0;i<w;i+=width)
	{
		gdk_draw_line(drawable,canv_gc, i, 0, i, h);
	}	
}

static void draw_tile(GdkDrawable *window, int xpos, int ypos)
{
	if(xpos >= 0 && xpos < 20 && ypos >= 0 && ypos < 20)
	{
		GdkGC *gc = gdk_gc_new(window);
		gdk_draw_drawable(window, gc, level_tileset, ctilex, ctiley, xpos*32, ypos*32,32,32);
		level_data[xpos*2+ypos*40] = (char)((ctilex/32)+65);
		level_data[(xpos*2+ypos*40)+1] = (char)((ctiley/32)+65);
	}
}

void draw_init()
{
	level_tileset = gdk_pixmap_new(NULL, 832, 832, 24);
	GdkPixbuf *tset = gdk_pixbuf_new_from_file("tset.png",NULL);
	if(!tset)
	{
		fprintf(stderr, "Failed loading the tileset - exitting!\n");
		gtk_main_quit();
	}
	
	GdkGC *tset_gc = gdk_gc_new(level_tileset);
	
	gdk_draw_pixbuf(level_tileset, tset_gc, tset,0,0,0,0, 832, 832, GDK_RGB_DITHER_NONE, 0,0);
	
	level_data = g_malloc0(40*2*20);
	memset(level_data, 'A', 40*2*20);
}

/* Callbacks */

void draw_level_click(GtkWidget *widget, GdkEventButton *ev, gpointer user_data)
{
	draw_tile(widget->window,(int)(ev->x/32),(int)(ev->y/32));
	file_set_unsaved();
}

void draw_level_move(GtkWidget *widget, GdkEventMotion *ev, gpointer user_data)
{
	static int x = 0; static int y = 0;
	if((int)(ev->x/32)!=x && ((int)(ev->y/32))!=y)
	{
		x = (int)(ev->x/32);
		y = (int)(ev->y/32);
	}
	draw_tile(widget->window,(int)(ev->x/32),(int)(ev->y/32));
	file_set_unsaved();
}

void draw_tset_click(GtkWidget *widget, GdkEventButton *ev, gpointer user_data)
{
	ctilex = ((int)ev->x/32)*32;
	ctiley = ((int)ev->y/32)*32;
}

gboolean draw_level_expose(GtkWidget *widget, gpointer user_data)
{
	if(level_data != NULL)
	{	
		int cols = 0;
		int rows = 0;
		GdkGC *gc = gdk_gc_new(widget->window);
		for(rows = 0; rows<20; rows++)
		{
			for(cols = 0; cols < 20;cols++)
			{
				int tilex = 0;
				int tiley = 0;
				tilex = level_data[(cols*2)+(rows*40)]-65;
				tiley = level_data[(cols*2)+1+(rows*40)]-65;
				gdk_draw_drawable(widget->window, gc, level_tileset, tilex*32, tiley*32, cols*32, rows*32,32,32);
			}
		}
	}
	return FALSE;
}

gboolean draw_tset_expose(GtkWidget *widget, gpointer user_data)
{
	if(level_tileset!=NULL)
	{
		GdkGC *canv_gc = gdk_gc_new(widget->window);
		gdk_draw_drawable(widget->window, canv_gc, level_tileset,0,0,0,0,832,832);
		draw_grid(32, 32, widget->window);
	}
	return FALSE;
}

char *draw_get_level_data()
{
	return g_strdup(level_data);
}

void draw_clear_level()
{
	memset(level_data,'A',40*2*20);
	GdkRectangle rect;
	rect.x = 0;
	rect.y = 0;
	rect.width = 640;
	rect.height = 640;
	gdk_window_invalidate_rect(canvas[0]->window,&rect,FALSE);
}

