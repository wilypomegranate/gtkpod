/*
|  Copyright (C) 2002 Jorg Schuler <jcsjcs at sourceforge.net>
|  Part of the gtkpod project.
| 
|  URL: 
| 
|  This program is free software; you can redistribute it and/or modify
|  it under the terms of the GNU General Public License as published by
|  the Free Software Foundation; either version 2 of the License, or
|  (at your option) any later version.
| 
|  This program is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|  GNU General Public License for more details.
| 
|  You should have received a copy of the GNU General Public License
|  along with this program; if not, write to the Free Software
|  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
| 
|  iTunes and iPod are trademarks of Apple
| 
|  This product is not supported/written/published by Apple!
*/

#ifndef __PREFS_H__
#define __PREFS_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

/* m
enum {
    OPT_SHOW_DEFAULT=	0,
    OPT_SHOW_ARTIST = (1 << 0),
    OPT_SHOW_ALBUM  = (1 << 1),
    OPT_SHOW_YEAR   = (1 << 2),
    OPT_SHOW_TRACK  = (1 << 3),
    OPT_SHOW_GENRE  = (1 << 4),
    OPT_SHOW_ALL    = (1 << 5)	
};
*/

struct cfg
{
  gchar    *ipod_mount;   /* mount point of iPod */
  gboolean writeid3;      /* should changes to ID3 tags be written to file */
  gboolean md5songs;	  /* don't allow song duplication on your ipod */	
  struct {
      gboolean artist, album, year, track, genre;
  } song_list_show; /* what columns are displayed in the song list */	
  /* last directories used by the fileselections */
  struct {
      gchar *dir_browse, *file_browse;
  } last_dir;	
};

/* enum for reading of options */
enum {
  GP_HELP,
  GP_MOUNT,
  GP_WRITEID3,
  GP_MD5SONGS
};

extern struct cfg *cfg;

void prefs_print(void);
gboolean read_prefs (int argc, char *argv[]);
void cfg_free(struct cfg *c);
void write_prefs (void);
void discard_prefs (void);
void prefs_set_mount_point(const gchar *mp);
void prefs_set_md5songs_active(gboolean active);
void prefs_set_writeid3_active(gboolean active);
void prefs_set_last_dir_dir_browse_for_filename(gchar * dir);
void prefs_set_last_dir_file_browse_for_filename(gchar * dir);

void prefs_set_song_list_show_all(gboolean val);
void prefs_set_song_list_show_year(gboolean val);
void prefs_set_song_list_show_track(gboolean val);
void prefs_set_song_list_show_genre(gboolean val);
void prefs_set_song_list_show_album(gboolean val);
void prefs_set_song_list_show_artist(gboolean val);

gboolean prefs_get_song_list_show_all(void);
gboolean prefs_get_song_list_show_year(void);
gboolean prefs_get_song_list_show_album(void);
gboolean prefs_get_song_list_show_track(void);
gboolean prefs_get_song_list_show_genre(void);
gboolean prefs_get_song_list_show_artist(void);

#endif __PREFS_H__
