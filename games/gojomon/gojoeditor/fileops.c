#include <gtk/gtk.h>
#include "draw.h"
#include "window.h"

static int file_saved = 1;
static char *file_name = NULL;

static void file_save_action()
{
	FILE *map_file = fopen(file_name,"w");
	if(!map_file)
	{
		fprintf(stderr, "Failed openning file for saving %s\n",file_name);
	}
	else
	{
		int i = 0;
		while(i!=800)
		{
			fwrite(level_data+i,1,40,map_file);
			fputc('\n',map_file); // customary new line :D
			i+=40;
		}
		/*
			NPC and Warp Saving here
		*/
		fclose(map_file);
	}
}

void file_new()
{
	if(!file_saved)
	{
		/*
			You sure you want to make a new level?
			[yes] [no]
		*/
	}
	if(file_name!=NULL)
		g_free(file_name);
	
	file_name = NULL;
	draw_clear_level();
}

void file_open()
{
	GtkWidget *dialog;
	if(!file_saved)
	{
		 dialog = gtk_message_dialog_new(NULL,
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_YES_NO,
                                  "The current map hasn't been saved!\n\nDo you still wish to open another map?");
		if(gtk_dialog_run (GTK_DIALOG (dialog))==GTK_RESPONSE_YES)
		{
			gtk_widget_destroy (dialog);
		}
		else
		{
			gtk_widget_destroy (dialog);
			return;
		}
	}
	
	dialog = gtk_file_chooser_dialog_new ("Open Gojomon Map File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		file_name = g_strdup(filename);
		FILE *map_file = fopen(file_name,"r");
		if(!map_file)
		{
			fprintf(stderr, "Failed loading map %s\n",file_name);
		}
		else
		{
			int i = 0;
		
			while(i<800 || feof(map_file))
			{
				char cbyte = (char)fgetc(map_file);
				if(cbyte == EOF)
				{
					break;
				}
				if(cbyte>='A' && cbyte<='Z')
				{
					level_data[i]=cbyte;
					i++;
				}
			}
			fclose(map_file);
			if(i!=800)
			{
				fprintf(stderr, "Didn't read in a full map!\n");
			}
			GdkRectangle rect;
			rect.x = 0;
			rect.y = 0;
			rect.width = 640;
			rect.height = 640;
			gdk_window_invalidate_rect(canvas[0]->window,&rect,FALSE);
		}
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
	
	file_saved = 1;
}

void file_save_as()
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Save Gojomon Map File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		file_name = g_strdup(filename);
		file_save_action();
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
	file_saved = 1;
}

void file_save()
{
	if(!file_name)
	{
		file_save_as();
		return;
	}
	file_save_action();	
	file_saved = 1;
}

void file_set_unsaved()
{
	file_saved = 0;
}

int file_get_saved()
{
	return file_saved;
}
