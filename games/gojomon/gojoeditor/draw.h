#ifndef __DRAW_H__
#define __DRAW_H__

#include <gtk/gtk.h>

extern char *level_data;

void draw_init();
gboolean draw_level_expose(GtkWidget *widget, gpointer user_data);
gboolean draw_tset_expose(GtkWidget *widget, gpointer user_data);
char *draw_get_level_data(GtkWidget *widget, gpointer user_data);
void draw_level_click(GtkWidget *widget, GdkEventButton *ev, gpointer user_data);
void draw_level_move(GtkWidget *widget, GdkEventMotion *ev, gpointer user_data);
void draw_tset_click(GtkWidget *widget, GdkEventButton *ev, gpointer user_data);
void draw_clear_level();

#endif
