/* Time-stamp: <2004-12-06 23:42:59 jcs>
|
|  Copyright (C) 2002-2003 Jorg Schuler <jcsjcs at users.sourceforge.net>
|  Part of the gtkpod project.
|
|  URL: http://gtkpod.sourceforge.net/
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
|
|  $Id$
*/

#ifndef __MISC_H__
#define __MISC_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include "file.h"
#include "track.h"
#include "playlist.h"
#include "display.h"
#include "confirmation.h"
#include "time.h"

#define C_FREE(a) {g_free(a); a=NULL;}

/* version of GTK_CHECK_VERSION that uses the runtime variables
   gtk_*_version... instead of the compile-time constants
   GTK_*_VERSION */
#define	RUNTIME_GTK_CHECK_VERSION(major,minor,micro)	\
    (gtk_major_version > (major) || \
     (gtk_major_version == (major) && gtk_minor_version > (minor)) || \
     (gtk_major_version == (major) && gtk_minor_version == (minor) && \
      gtk_micro_version >= (micro)))

/* compare a and b, return sign (a-b) -- it has to be this way rather
   than just calculate a-b, because a and b might be unsigned... */
#define COMP(a,b) (a<b ? -1:a>b ? +1:0)

/* Response for the Apply button (we cannot use GTK_RESPONSE_APPLY)
 * because that is only emitted if a filename is present */
enum { RESPONSE_APPLY = 5 };

/* indicates whether widgets are currently blocked */
extern gboolean widgets_blocked;

gchar *get_user_string (gchar *title, gchar *message, gchar *dflt,
			gchar *opt_msg, gboolean *opt_state);
Playlist *add_new_pl_user_name (gchar *dflt, gint position);
void add_new_pl_or_spl_user_name (gchar *dflt, gint position);
void create_add_files_fileselector (void);
void create_add_playlists_fileselector (void);
gchar *concat_dir (G_CONST_RETURN gchar *dir, G_CONST_RETURN gchar *file);
gchar *concat_dir_if_relative (G_CONST_RETURN gchar *base_dir,
			       G_CONST_RETURN gchar *rel_dir);
float get_ms_since (GTimeVal *old_time, gboolean update);
gint get_sort_tab_number (gchar *text);
void open_about_window (void);
void close_about_window (void);
gboolean parse_ipod_id_from_string(gchar **s, guint32 *id);
void add_idlist_to_playlist (Playlist *pl, gchar *str);
void add_text_plain_to_playlist (Playlist *pl, gchar *str, gint position,
				 AddTrackFunc trackaddfunc, gpointer data);
void cleanup_backup_and_extended_files (void);
gboolean gtkpod_main_quit(void);
void gtkpod_main_window_set_active(gboolean active);

T_item TM_to_T (TM_item sm);
T_item ST_to_T (ST_CAT_item st);
gchar *get_track_info (Track *track);

void ipod_directories_head (void);
void delete_playlist_head (gboolean delete_full);
void delete_track_head (gboolean delete_full);
void delete_entry_head (gint inst, gboolean delete_full);

void delete_populate_settings (Playlist *pl, GList *selected_trackids,
			       gchar **label, gchar **title,
			       gboolean *confirm_again,
			       ConfHandlerOpt *confirm_again_handler,
			       GString **str);

void block_widgets (void);
void release_widgets (void);
void update_blocked_widget (GtkWidget *w, gboolean sens);

void mount_ipod(void);
void unmount_ipod(void);
void call_script (gchar *script);

void delete_track_ok (gpointer user_data1, gpointer user_data2);

void gtkpod_warning (const gchar *format, ...);

gchar *time_time_to_string (time_t t);
gchar *time_fromtime_to_string (time_t t);
gchar *time_totime_to_string (time_t t);
time_t time_string_to_time (const gchar *str);
time_t time_string_to_fromtime (const gchar *str);
time_t time_string_to_totime (const gchar *str);
time_t time_get_time (Track *track, TM_item tm_item);
gchar *time_field_to_string (Track *track, TM_item tm_item);
void time_set_time (Track *track, time_t t, TM_item tm_item);

gchar *get_filesize_as_string (double size);

gint compare_string (gchar *str1, gchar *str2);
gint compare_string_fuzzy (gchar *str1, gchar *str2);
gint compare_string_case_insensitive (gchar *str1, gchar *str2);

gchar *filename_from_uri (const char *uri,
			  char      **hostname,
			  GError    **error);

void generate_category_playlists (T_item cat);
Playlist *generate_displayed_playlist (void);
Playlist *generate_selected_playlist (void);
void randomize_current_playlist (void);
Playlist *generate_random_playlist (void);
Playlist *generate_not_listed_playlist (void);
Playlist *generate_playlist_with_name (GList *tracks, gchar *pl_name,
				       gboolean del_old);
Playlist *generate_new_playlist (GList *tracks);
void most_listened_pl (void);
void never_listened_pl (void);
void last_listened_pl(void);
void most_rated_pl(void);
void since_last_pl(void);

guint32 utf16_strlen (gunichar2 *utf16);
gunichar2 *utf16_strdup (gunichar2 *utf16);

void check_db (void);

void do_selected_tracks (void (*do_func)(GList *trackids));
void do_selected_entry (void (*do_func)(GList *trackids), gint inst);
void do_selected_playlist (void (*do_func)(GList *trackids));

guint32 replaygain_to_soundcheck (gdouble gain);
gdouble soundcheck_to_replaygain (guint32 soundcheck);


void option_set_radio_button (GtkWidget *win,
			      const gchar *prefs_string,
			      const gchar **widgets,
			      gint dflt);
gint option_get_radio_button (GtkWidget *win,
			      const gchar *prefs_string,
			      const gchar **widgets);
void option_set_folder (GtkFileChooser *fc,
			const gchar *prefs_string);
void option_get_folder (GtkFileChooser *fc,
			const gchar *prefs_string,
			gchar **value);
void option_set_filename (GtkFileChooser *fc,
			  const gchar *prefs_string);
void option_get_filename (GtkFileChooser *fc,
			  const gchar *prefs_string,
			  gchar **value);
void option_set_string (GtkWidget *win,
			const gchar *name,
			const gchar *dflt);
void option_get_string (GtkWidget *win,
			const gchar *name,
			gchar **value);
void option_set_toggle_button (GtkWidget *win,
			       const gchar *name,
			       gboolean dflt);
gboolean option_get_toggle_button (GtkWidget *win,
				   const gchar *name);

gchar *get_string_from_template (Track *track,
				 const gchar *full_template,
				 gboolean is_filename);

GList *glist_duplicate (GList *list);

#endif
