/* Time-stamp: <2003-08-23 12:24:06 jcs>
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
*/

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "song.h"
#include "playlist.h"

/* Categories in each sort tab (page numbers) */
typedef enum {
  ST_CAT_ARTIST = 0,
  ST_CAT_ALBUM,
  ST_CAT_GENRE,
  ST_CAT_COMPOSER,
  ST_CAT_TITLE,
  ST_CAT_SPECIAL,
  ST_CAT_NUM
} ST_CAT_item;

/* Number of search tabs to be supported. */
#define SORT_TAB_MAX ST_CAT_NUM

/* Number of GtkPaned elements in the main window. The positions of
 * these elements will be stored in the prefs file and be set to the
 * last value when starting gtkpod again */
/* Number defined with glade ("paned%d") */
enum {
    PANED_PLAYLIST = 0,
    PANED_SONGLIST,
    PANED_STATUS1,
    PANED_STATUS2,
    PANED_NUM_GLADE
};
/* Number created in display.c (for sort tabs, stored in st_paned[]) */
#define PANED_NUM_ST (SORT_TAB_MAX-1)
/* Total number */
#define PANED_NUM (PANED_NUM_GLADE+PANED_NUM_ST)

/* max. number of stars */
#define RATING_MAX 5

/* time between display refreshs in ms */
#define REFRESH_MS 200
/* initial count number between display refreshs -- will be
   re-calculated to match the time interval specified above */
#define REFRESH_INIT_COUNT 5

/* struct for each entry in sort tab */
typedef struct {
  gchar *name;
  gboolean master; /* set if this is the "All" entry */
  GList *members;  /* GList with member songs (pointer to "Song") */
} TabEntry;

/* struct for time info (created, played, modified) */
typedef struct {
    gchar *int_str;   /* copy of string specified in the sort tab */
    gboolean valid;   /* is current string valid? */
    guint32 lower;    /* MAC timestamp for lower limit */
    guint32 upper;    /* MAC timestamp for upper limit ("-1": no limit) */
    GtkWidget *entry; /* pointer to GtkEntry in sort tab */
    GtkWidget *active;/* pointer to toggle button */
} TimeInfo;

/* struct with data corresponding to each sort tab */
typedef struct {
  guint current_category;            /* current page (category) selected) */
  gboolean final;                    /* have all songs been added? */
  GtkWidget *window[ST_CAT_NUM];     /* pointer to scrolled window */
  /* The following are used for "normal" categories (not ST_CAT_SPECIAL) */
  GtkTreeModel *model;               /* pointer to model used */
  GtkNotebook *notebook;             /* pointer to notebook used */
  GtkTreeView *treeview[ST_CAT_NUM]; /* pointer to treeviews used */
  GList *entries;                    /* list with entries */
  TabEntry *current_entry;           /* pointer to currently selected entry */
  gchar *lastselection[ST_CAT_NUM];  /* name of entry last selected */
  GHashTable *entry_hash;            /* table for quick find of tab entries */
  gboolean unselected;               /* unselected item since last st_init? */
  /* The following are used for "special" categories (ST_CAT_SPECIAL) */
  GList *sp_members;                 /* list of songs in sorttab */
  GList *sp_selected;                /* list of songs selected */
  gboolean is_go;                    /* pass new members on automatically */
  TimeInfo ti_modified, ti_played;   /* Sort information for modification
                                        (file time stamp) and played time */
  GtkTooltips *sp_tooltips;          /* pointer to tooltips in special st */
} SortTab;

/* "Column numbers" in sort tab model */
typedef enum  {
  ST_COLUMN_ENTRY = 0,
  ST_NUM_COLUMNS
} ST_item;

/* Column numbers in song model */
/* Note: the toggle buttons for tag_autoset and display_col
 * in the prefs_window are
 * named after the numbers (Title: tag_autoset0, Artist: tag_autoset1
 * etc.) Therefore, if you change the order of the first
 * SM_NUM_TAGS_PREFS entries, you should also adjust the names in
 * gtkpod.glade and rebuild the interface */
typedef enum  {
  SM_COLUMN_TITLE = 0,
  SM_COLUMN_ARTIST,
  SM_COLUMN_ALBUM,
  SM_COLUMN_GENRE,
  SM_COLUMN_COMPOSER,
  SM_COLUMN_TRACK_NR,
  SM_COLUMN_IPOD_ID,
  SM_COLUMN_PC_PATH,
  SM_COLUMN_TRANSFERRED,
  SM_COLUMN_SIZE,
  SM_COLUMN_SONGLEN,
  SM_COLUMN_BITRATE,
  SM_COLUMN_PLAYCOUNT,
  SM_COLUMN_RATING,
  SM_COLUMN_TIME_PLAYED,
  SM_COLUMN_TIME_MODIFIED,
  SM_COLUMN_VOLUME,
  SM_NUM_COLUMNS
} SM_item;

/* number of colums for prefs size storage */
#define SM_NUM_COLUMNS_PREFS SM_NUM_COLUMNS
/* number of entries with "autoset empty tag to filename " feature */
#define SM_NUM_TAGS_PREFS (5)

/* "Column numbers" in playlist model */
enum  {
  PM_COLUMN_PLAYLIST = 0,
  PM_NUM_COLUMNS
};

/* Drag and drop types */
enum {
    DND_GTKPOD_IDLIST = 1000,
    DND_GTKPOD_SM_PATHLIST,
    DND_GTKPOD_PM_PATHLIST,
    DND_TEXT_PLAIN
};

/* used for the ST_CAT_SPECIAL user_data (see st_create_special and
 * the corresponding signal functions) */
#define SP_SHIFT 9
#define SP_MASK ((1<<SP_SHIFT)-1)

void display_create (GtkWidget *gtkpod);
void display_cleanup (void);
void display_show_hide_toolbar (void);
void display_reset (gint inst);
GList *display_get_selection (guint32 inst);

void pm_remove_playlist (Playlist *playlist, gboolean select);
void pm_add_playlist (Playlist *playlist, gint position);
void pm_select_playlist (Playlist *playlist);
void pm_remove_song (Playlist *playlist, Song *song);
void pm_add_song (Playlist *playlist, Song *song, gboolean display);
void pm_name_changed (Playlist *playlist);
void pm_song_changed (Song *song);
void pm_sort (GtkSortType order);

void st_page_selected (GtkNotebook *notebook, guint page);
void st_sort (guint32 inst, GtkSortType order);
void st_remove_entry (TabEntry *entry, guint32 inst);
gint st_get_instance_from_treeview (GtkTreeView *tv);
void st_show_visible (void);
void st_arrange_visible_sort_tabs (void);
TabEntry *st_get_selected_entry (gint inst);

void cal_open_calendar (gint inst, S_item item);

void sp_go (guint32 inst);
void sp_conditions_changed (guint32 inst);

void on_sm_dnd_get_id_foreach(GtkTreeModel *tm, GtkTreePath *tp, 
			      GtkTreeIter *i, gpointer data);
void on_dnd_get_path_foreach(GtkTreeModel *tm, GtkTreePath *tp, 
			     GtkTreeIter *i, gpointer data);
void on_sm_dnd_get_file_foreach(GtkTreeModel *tm, GtkTreePath *tp, 
				GtkTreeIter *i, gpointer data);
void on_pm_dnd_get_id_foreach(GtkTreeModel *tm, GtkTreePath *tp, 
			      GtkTreeIter *i, gpointer data);
void on_pm_dnd_get_file_foreach(GtkTreeModel *tm, GtkTreePath *tp, 
				GtkTreeIter *i, gpointer data);
void on_st_listing_drag_foreach(GtkTreeModel *tm, GtkTreePath *tp, 
				GtkTreeIter *i, gpointer data);
TimeInfo *st_update_date_interval_from_string (guint32 inst,
					       S_item item,
					       gboolean force_update);
void sm_add_song_to_song_model (Song *song, GtkTreeIter *into_iter);
guint sm_get_nr_of_songs(void);
void sm_rows_reordered(void);
void pm_rows_reordered(void);
gboolean sm_move_pathlist (gchar *data, GtkTreePath *path,
			   GtkTreeViewDropPosition pos);
gboolean pm_move_pathlist (gchar *data, GtkTreePath *path,
			   GtkTreeViewDropPosition pos);
gboolean sm_add_filelist (gchar *data, GtkTreePath *path,
			  GtkTreeViewDropPosition pos);
void sm_show_preferred_columns(void);

Playlist* pm_get_selected_playlist(void);
GList* sm_get_selected_songids(void);
GList* sm_get_selected_songs(void);
GList* sm_get_all_songids(void);
GList* sm_get_all_songs(void);
void display_update_default_sizes (void);
void display_set_default_sizes (void);
void display_show_hide_tooltips (void);
void sm_store_col_order (void);

void stop_display_update (gint inst);
#endif 
