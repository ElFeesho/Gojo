#ifndef __FILEOPS_H__
#define __FILEOPS_H__

#include <gtk/gtk.h>

void file_new();
void file_open();
void file_save();
void file_save_as();

void file_set_unsaved();
int file_get_saved();

#endif
