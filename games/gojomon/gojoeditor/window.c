#include <gtk/gtk.h>
#include "toolbar.h"
#include "draw.h"
#include "fileops.h"

GtkWidget *canvas[2];

static gboolean window_delete_event(GtkWidget *window, gpointer user_data)
{
	if(!file_get_saved())
	{
		GtkWidget *dialog = gtk_message_dialog_new(NULL,
												 GTK_DIALOG_DESTROY_WITH_PARENT,
												 GTK_MESSAGE_WARNING,
												 GTK_BUTTONS_YES_NO|GTK_BUTTONS_CANCEL,
												 "The current map hasn't been saved!\n\nDo you wish to save it?");
		int resp = gtk_dialog_run (GTK_DIALOG (dialog));
		if(resp == GTK_RESPONSE_YES)
		{
			file_save();
			gtk_widget_destroy (dialog);
		}
		else if(resp == GTK_RESPONSE_CANCEL)
		{
			gtk_widget_destroy (dialog);
			return TRUE;
		}
		gtk_widget_destroy (dialog);
	}
	gtk_main_quit();
	return FALSE;
}

static GtkWidget *window_create()
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window),600,400);
	GtkWidget *vbox = gtk_vbox_new(0,0);
	GtkWidget *hbox = gtk_hpaned_new();
	
	GtkWidget *scrolled_window[2];
	/*
		Both of the image holding contents, 
		canvas[0] = map
		canvas[1] = tileset
	*/
	
	canvas[0] = gtk_drawing_area_new();
	gtk_widget_set_size_request(canvas[0], 640, 640);
	canvas[1] = gtk_drawing_area_new();
	gtk_widget_set_size_request(canvas[1], 832, 832);
	int i;
	for(i = 0; i < 2; i++)
	{
		scrolled_window[i] = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window[i]),GTK_SHADOW_IN);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window[i]),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window[i]),canvas[i]);
		gtk_widget_add_events(canvas[i],GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);
	}
	gtk_paned_pack1(GTK_PANED(hbox),scrolled_window[0],FALSE,FALSE);
	gtk_paned_pack2(GTK_PANED(hbox),scrolled_window[1],FALSE,FALSE);
	
	gtk_box_pack_start(GTK_BOX(vbox),toolbar_create(),FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	/* Callbacks */
	g_signal_connect(G_OBJECT(canvas[0]),"expose-event",G_CALLBACK(draw_level_expose),NULL);
	g_signal_connect(G_OBJECT(canvas[1]),"expose-event",G_CALLBACK(draw_tset_expose),NULL);
	
	g_signal_connect(G_OBJECT(canvas[0]),"button-press-event",G_CALLBACK(draw_level_click),NULL);
	g_signal_connect(G_OBJECT(canvas[0]),"motion-notify-event",G_CALLBACK(draw_level_move),NULL);
	g_signal_connect(G_OBJECT(canvas[1]),"button-press-event",G_CALLBACK(draw_tset_click),NULL);
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_delete_event),NULL);
	
	return window;
}


void window_show()
{
	gtk_widget_show_all(window_create());
}
