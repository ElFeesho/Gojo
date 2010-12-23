#include <gtk/gtk.h>
#include "fileops.h"

GtkWidget *toolbar_create()
{
	GtkWidget *toolbar = gtk_toolbar_new();
	
	GtkToolItem *toolitem;
	
	toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	g_signal_connect(G_OBJECT(toolitem),"clicked",G_CALLBACK(file_new),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),toolitem,-1);
	
	toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(toolitem),"clicked",G_CALLBACK(file_open),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),toolitem,-1);
	
	toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	
	g_signal_connect(G_OBJECT(toolitem),"clicked",G_CALLBACK(file_save),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),toolitem,-1);
	
	toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
	g_signal_connect(G_OBJECT(toolitem),"clicked",G_CALLBACK(file_save_as),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),toolitem,-1);
	
	
	return toolbar;
}
