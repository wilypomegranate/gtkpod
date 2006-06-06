/* Time-stamp: <2006-05-22 23:24:27 jcs>
|
|  Copyright (C) 2002 Corey Donohoe <atmos at atmos.org>
|  Copyright (C) 2002-2005 Jorg Schuler <jcsjcs at users sourceforge net>
|  Part of the gtkpod project.
|
|  URL: http://www.gtkpod.org/
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

#include <stdio.h>
#include <string.h>
#include "charset.h"
#include "display_itdb.h"
#include "info.h"
#include "fileselection.h"
#include "misc.h"
#include "misc_track.h"
#include "prefs.h"
#include "prefs_window.h"
#include "repository.h"


GladeXML *prefs_window_xml;
GladeXML *sort_window_xml;

static GtkWidget *prefs_window = NULL;
static GtkWidget *sort_window = NULL;
static struct cfg *tmpcfg = NULL;
static struct cfg *origcfg = NULL;
static struct sortcfg *tmpsortcfg = NULL;
static struct sortcfg *origsortcfg = NULL;

/* New prefs temp handling */
static TempPrefs *temp_prefs;
static TempLists *temp_lists;
static TempPrefs *sort_temp_prefs;
static TempLists *sort_temp_lists;

/* keeps the check buttons for "Select Entry 'All' in Sorttab %d" */
static GtkWidget *autoselect_widget[SORT_TAB_MAX];

static void prefs_window_set_st_autoselect (guint32 inst, gboolean autoselect);
static void prefs_window_set_autosettags (gint category, gboolean autoset);
static void prefs_window_set_col_visible (gint column, gboolean visible);
static void prefs_window_set_sort_tab_num (gint num);

/* Some declarations */
static void standard_toggle_toggled (GtkToggleButton *togglebutton,
				     const gchar *key);



/* Definition of path button names.
   E.g. path_button_names[PATH_PLAY_ENQUEUE] is
   "play_enqueue_path_button".
   path_fileselector_titles[] specifies the title for the file
   chooser. path_type[] specifies whether to browse for dirs or for
   files.
*/
static const gchar *path_button_names[] =
{
    "play_now_path_button",
    "play_enqueue_path_button",
    "mp3gain_path_button",
    "",
    "",
    "mserv_music_root_button",
    "mserv_trackinfo_root_button",
    "",
    NULL
};
static const gchar *path_key_names[] =
{
    "path_play_now",
    "path_play_enqueue",
    "path_mp3gain",
    "",
    "",
    "path_mserv_music_root",
    "path_mserv_trackinfo_root",
    "",
    NULL
};
const gchar *path_entry_names[] =
{
    "play_now_path_entry",
    "play_enqueue_path_entry",
    "mp3gain_path_entry",
    "",
    "",
    "mserv_music_root_entry",
    "mserv_trackinfo_root_entry",
    "",
    NULL
};
static const gchar *path_fileselector_titles[] =
{
    N_("Please select command for 'Play Now'"),
    N_("Please select command for 'Enqueue'"),
    N_("Please select the mp3gain executable"),
    "",
    "",
    N_("Select the mserv music root directory"),
    N_("Select the mserv trackinfo root directory"),
    "",
    NULL
};
static const GtkFileChooserAction path_type[] =
{
    GTK_FILE_CHOOSER_ACTION_OPEN,  /* select file */
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, /* select folder */
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
    GTK_FILE_CHOOSER_ACTION_OPEN,
    -1
};


static void on_cfg_st_autoselect_toggled (GtkToggleButton *togglebutton,
					  gpointer         user_data)
{
    prefs_window_set_st_autoselect (
	GPOINTER_TO_UINT(user_data),
	gtk_toggle_button_get_active(togglebutton));
}

static void on_cfg_autosettags_toggled (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    prefs_window_set_autosettags (
	GPOINTER_TO_UINT(user_data),
	gtk_toggle_button_get_active(togglebutton));
}


static void on_cfg_col_visible_toggled (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    prefs_window_set_col_visible (
	GPOINTER_TO_UINT(user_data),
	gtk_toggle_button_get_active(togglebutton));
}


/* one of the "..." buttons has been pressed -> open a file chooser
   dialog and let the user select a file or directory */
static void on_path_button_pressed (GtkButton *button, gpointer user_data)
{
    PathType i = (PathType)user_data;
    gchar *oldpath, *newpath;

    g_return_if_fail (temp_prefs);
    g_return_if_fail (i>=0 && i<PATH_NUM);

    oldpath = temp_prefs_get_string (temp_prefs, path_key_names[i]);

    switch (path_type[i])
    {
    case GTK_FILE_CHOOSER_ACTION_OPEN:
	/* script */
	newpath = fileselection_select_script (
	    oldpath,
	    NULL,
	    _(path_fileselector_titles[i]),
	    NULL);
	break;
    case GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER:
	/* directory */
	newpath = fileselection_get_file_or_dir (
	    _(path_fileselector_titles[i]),
	    oldpath,
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	break;
    default:
	g_return_if_reached ();
    }
    g_free (oldpath);

    if (newpath)
    {
	GtkWidget *w = gtkpod_xml_get_widget (prefs_window_xml,
					      path_entry_names[i]);
	if (w)
	{
	    gchar *newpath_utf8 = g_filename_to_utf8 (newpath, -1, NULL, NULL, NULL);
	    gtk_entry_set_text(GTK_ENTRY(w), newpath_utf8);
	    g_free (newpath_utf8);
	}
	g_free (newpath);
    }
}


static void on_path_entry_changed (GtkEditable     *editable,
				   gpointer         user_data)
{
    PathType i = (PathType)user_data;
    gchar *buf = gtk_editable_get_chars(editable, 0, -1);

    temp_prefs_set_string (temp_prefs, path_key_names[i], buf);
    g_free (buf);
}


/* turn the prefs window insensitive (if it's open) */
void prefs_window_block (void)
{
    if (prefs_window)
	gtk_widget_set_sensitive (prefs_window, FALSE);
}

/* turn the prefs window sensitive (if it's open) */
void prefs_window_release (void)
{
    if (prefs_window)
	gtk_widget_set_sensitive (prefs_window, TRUE);
}


/* make the tooltips visible or hide it depending on the value set in
 * the prefs (tooltips_prefs) */
void prefs_window_show_hide_tooltips (void)
{
    GtkTooltips *tt;
    GtkTooltipsData *tooltipsdata;

    if (!prefs_window)   return; /* we may get called even when window
				    is not open */
    tooltipsdata = gtk_tooltips_data_get (gtkpod_xml_get_widget (prefs_window_xml, "cfg_write_extended"));
    g_return_if_fail (tooltipsdata);
    tt = tooltipsdata->tooltips;
    g_return_if_fail (tt);
    if (prefs_get_display_tooltips_prefs ()) gtk_tooltips_enable (tt);
    else                                     gtk_tooltips_disable (tt);
}



/**
 * create_gtk_prefs_window
 * Create, Initialize, and Show the preferences window
 * allocate a static cfg struct for temporary variables
 *
 * If the window is already open, it is raised to the front and @page
 * is selected (unless it's -1).
 *
 * @page: -1 for 'last page'.
 */
void
prefs_window_create (gint page)
{
    gint i;
    gint defx, defy;
    GtkWidget *w = NULL;
    GtkTooltips *tt;
    GtkTooltipsData *tooltipsdata;
    /* List of standard toggle widget names */
    const gchar *toggle_widget_names[] = {
	"sync_confirm_dirs_toggle",
	"sync_delete_tracks_toggle",
	"sync_show_summary_toggle",
	NULL
    };
    /* ... and corresponding keys */
    const gchar *toggle_key_names[] = {
	KEY_SYNC_CONFIRM_DIRS,
	KEY_SYNC_DELETE_TRACKS,
	KEY_SYNC_SHOW_SUMMARY,
	NULL
    };

    if (prefs_window)
    {   /* prefs window already open -- raise to the top */
	gdk_window_raise (prefs_window->window);
	if (page != -1)
	{
	    g_return_if_fail (prefs_window_xml);
	    if ((w = gtkpod_xml_get_widget (prefs_window_xml, "notebook")))
	    {
		gtk_notebook_set_current_page (GTK_NOTEBOOK (w), page);
	    }
	}
	return;
    }
		
		/* Initialize temp prefs structures */
		temp_prefs = temp_prefs_create();
		temp_lists = temp_lists_create();

    if(!tmpcfg && !origcfg)
    {
	tmpcfg = clone_prefs();
	origcfg = clone_prefs();
    }
    else
    {
	g_warning ("Programming error: tmpcfg is not NULL!!\n");
	return;
    }

    prefs_window_xml = glade_xml_new (xml_file, "prefs_window", NULL);
    glade_xml_signal_autoconnect (prefs_window_xml);

    prefs_window = gtkpod_xml_get_widget(prefs_window_xml,"prefs_window");

    g_return_if_fail (prefs_window);
		
    tooltipsdata = gtk_tooltips_data_get (gtkpod_xml_get_widget (prefs_window_xml, "cfg_write_extended"));
    g_return_if_fail (tooltipsdata);
    tt = tooltipsdata->tooltips;
    g_return_if_fail (tt);

    prefs_get_size_prefs (&defx, &defy);
    gtk_window_set_default_size (GTK_WINDOW (prefs_window), defx, defy);

    /* Code to add subscriptions list box */

    /* Set up standard toggles */
    for (i=0; toggle_widget_names[i]; ++i)
    {
	w = gtkpod_xml_get_widget (prefs_window_xml,
				   toggle_widget_names[i]);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w),
				      prefs_get_int (toggle_key_names[i]));
	g_signal_connect (w, "toggled",
			  G_CALLBACK (standard_toggle_toggled),
			  (gpointer)toggle_key_names[i]);
    }

    w = gtkpod_xml_get_widget (prefs_window_xml, "sync_confirm_delete_toggle");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int (KEY_SYNC_CONFIRM_DELETE));

    w = gtkpod_xml_get_widget (prefs_window_xml, "sync_confirm_delete_toggle2");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int (KEY_SYNC_CONFIRM_DELETE));


    w = gtkpod_xml_get_widget (prefs_window_xml, "charset_combo");
    charset_init_combo (GTK_COMBO (w));
    
    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_md5tracks");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->md5tracks);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_update_existing");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("update_existing"));

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_show_duplicates");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("show_duplicates"));
    if (!tmpcfg->md5tracks) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_show_updated");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("show_updated"));
    if (!prefs_get_int("update_existing")) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_show_non_updated");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("show_non_updated"));
    if (!prefs_get_int("update_existing")) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_display_toolbar");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->display_toolbar);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_toolbar_style_icons");
    if (tmpcfg->toolbar_style == GTK_TOOLBAR_ICONS)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
    if (!tmpcfg->display_toolbar) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_toolbar_style_text");
    if (tmpcfg->toolbar_style == GTK_TOOLBAR_TEXT)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
    if (!tmpcfg->display_toolbar) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_toolbar_style_both");
    if (tmpcfg->toolbar_style == GTK_TOOLBAR_BOTH)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
    if (!tmpcfg->display_toolbar) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_display_tooltips_main");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->display_tooltips_main);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_display_tooltips_prefs");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->display_tooltips_prefs);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_multi_edit");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->multi_edit);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_not_played_track");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->not_played_track);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_misc_track_nr");
    gtk_spin_button_set_range (GTK_SPIN_BUTTON (w), 0, 0xffffffff);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (w),
			       prefs_get_misc_track_nr ());
    tmpcfg->misc_track_nr = tmpcfg->misc_track_nr;

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_multi_edit_title");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->multi_edit_title);
    gtk_widget_set_sensitive (w, tmpcfg->multi_edit);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_update_charset");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->update_charset);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_block_display");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->block_display);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_id3_write");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("id3_write"));

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_id3_write_id3v24");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("id3_write_id3v24"));
    if (!prefs_get_int("id3_write")) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_write_charset");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->write_charset);
    if (!prefs_get_int("id3_write")) gtk_widget_set_sensitive (w, FALSE);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_add_recursively");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->add_recursively);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_delete_track_from_playlist");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("delete_track"));

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_delete_track_from_ipod");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("delete_ipod"));

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_track_local_file_deletion");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("delete_local_file"));

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_track_database_deletion");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("delete_database"));

    w = gtkpod_xml_get_widget (prefs_window_xml,  "cfg_autoimport");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->autoimport);

    w = gtkpod_xml_get_widget (prefs_window_xml, "autoselect_hbox");
    for (i=0; i<SORT_TAB_MAX; ++i)
    {
	gchar *buf;
	GtkWidget *as;
	gint padding;
	    
	buf = g_strdup_printf ("%d", i+1);
	as = gtk_check_button_new_with_mnemonic (buf);
	autoselect_widget[i] = as;
	gtk_widget_show (as);
	if (i==0) padding = 0;
	else      padding = 5;
	gtk_box_pack_start (GTK_BOX (w), as, FALSE, FALSE, padding);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(as),
				     prefs_get_int_index("st_autoselect", i));
	g_signal_connect ((gpointer)as,
			  "toggled",
			  G_CALLBACK (on_cfg_st_autoselect_toggled),
			  GUINT_TO_POINTER(i));
	g_free (buf);
    }
    /* connect signals for path entrys and selectors */
    for (i=0; i<PATH_NUM; ++i)
    {
	gchar *path;
	/* "" is not a valid button name -> skip */
	if (strlen (path_button_names[i]) == 0) continue;
	/* Otherwise connect handler */
	w = gtkpod_xml_get_widget (prefs_window_xml, path_button_names[i]);
	g_signal_connect ((gpointer)w,
			  "clicked",
			  G_CALLBACK (on_path_button_pressed),
			  GUINT_TO_POINTER(i));

	w = gtkpod_xml_get_widget (prefs_window_xml, path_entry_names[i]);

	path = temp_prefs_get_string (temp_prefs, path_button_names[i]);
	if (path)
	{
	    gtk_entry_set_text(GTK_ENTRY(w), path);
	    g_free (path);
	}
	g_signal_connect ((gpointer)w,
			  "changed",
			  G_CALLBACK (on_path_entry_changed),
			  GUINT_TO_POINTER(i));
    }

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_mpl_autoselect");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->mpl_autoselect);

    for (i=0; i<TM_NUM_TAGS_PREFS; ++i)
    {
	gchar *buf;
	buf = g_strdup_printf ("tag_autoset%d", i);
	if((w = gtkpod_xml_get_widget (prefs_window_xml, buf)))
	{
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
					 tmpcfg->autosettags[i]);
	    /* glade makes a "GTK_OBJECT (i)" which segfaults
	       because "i" is not a GTK object. So we have to set up
	       the signal handlers ourselves */
	    g_signal_connect ((gpointer)w,
			      "toggled",
			      G_CALLBACK (on_cfg_autosettags_toggled),
			      GUINT_TO_POINTER(i));
	}
	g_free (buf);
    }

    w = gtkpod_xml_get_widget (prefs_window_xml, "readtags");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->readtags);

    w = gtkpod_xml_get_widget (prefs_window_xml, "parsetags");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->parsetags);

    w = gtkpod_xml_get_widget (prefs_window_xml, "parsetags_overwrite");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->parsetags_overwrite);
    gtk_widget_set_sensitive (w, tmpcfg->parsetags);

    w = gtkpod_xml_get_widget (prefs_window_xml, "parsetags_template");
    if (tmpcfg->parsetags_template)
    {  /* we should copy the new path first because by setting
	  the text we might get a callback destroying the old
	  value... */
	gchar *buf = g_strdup (tmpcfg->parsetags_template);
	gtk_entry_set_text(GTK_ENTRY(w), buf);
	g_free (buf);
    }
    gtk_widget_set_sensitive (w, tmpcfg->parsetags);

    w = gtkpod_xml_get_widget (prefs_window_xml, "coverart");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->coverart);

    w = gtkpod_xml_get_widget (prefs_window_xml, "coverart_template");
    if (tmpcfg->coverart_template)
    {  /* we should copy the new path first because by setting
	  the text we might get a callback destroying the old
	  value... */
	gchar *buf = g_strdup (tmpcfg->coverart_template);
	gtk_entry_set_text(GTK_ENTRY(w), buf);
	g_free (buf);
    }
    gtk_widget_set_sensitive (w, tmpcfg->coverart);

    for (i=0; i<TM_NUM_COLUMNS; ++i)
    {
	gchar *buf = g_strdup_printf ("col_visible%d", i);
	if((w = gtkpod_xml_get_widget (prefs_window_xml, buf)))
	{
	    /* set label */
	    gtk_button_set_label (GTK_BUTTON (w),
				  gettext (get_tm_string (i)));
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
					 prefs_get_int_index("col_visible", i));
	    /* set tooltip if available */
	    if (get_tm_tooltip (i))
	    {
		gtk_tooltips_set_tip (tt, w, 
				      gettext (get_tm_tooltip (i)),
				      NULL);
	    }
	    /* glade makes a "GTK_OBJECT (i)" which segfaults
	       because "i" is not a GTK object. So we have to set
	       up the signal handlers ourselves */
	    g_signal_connect ((gpointer)w,
			      "toggled",
			      G_CALLBACK (on_cfg_col_visible_toggled),
			      GUINT_TO_POINTER(i));
	}
	g_free (buf);
    }
    
    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_write_extended");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->write_extended_info);


if ((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_automount_ipod")))
    {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				     prefs_get_automount());
    }
    if ((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_sort_tab_num_sb")))
    {
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (w),
				   0, SORT_TAB_MAX);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (w),
				   prefs_get_sort_tab_num ());
	prefs_window_set_sort_tab_num (tmpcfg->sort_tab_num);
    }
    if ((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_group_compilations")))
    {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				     prefs_get_group_compilations());
    }

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_tmp_disable_sort");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->tmp_disable_sort);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_startup_messages");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->startup_messages);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_mserv_use");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 tmpcfg->mserv_use);

    w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_mserv_report_probs");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
				 prefs_get_int("mserv_report_probs"));

    w = gtkpod_xml_get_widget (prefs_window_xml, "mserv_username_entry");
    if (tmpcfg->mserv_username)
    {  /* we should copy the new path first because by setting
	  the text we might get a callback destroying the old
	  value... */
	gchar *buf = g_strdup (tmpcfg->mserv_username);
	gtk_entry_set_text(GTK_ENTRY(w), buf);
	g_free (buf);
    }


/* 	if((w = lookup_widget(prefs_window, "cfg_unused_gboolean3"))) */
/* 	{ */
/* 	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), */
/* 					 tmpcfg->unused_gboolean3); */
/* 	} */

    w = gtkpod_xml_get_widget (prefs_window_xml, "notebook");
    if (page == -1)
    {
	gtk_notebook_set_current_page (GTK_NOTEBOOK (w),
				       prefs_get_last_prefs_page ());
    }
    else
    {
	gtk_notebook_set_current_page (GTK_NOTEBOOK (w), page);
    }
    prefs_window_show_hide_tooltips ();
    gtk_widget_show(prefs_window);
}


/**
 * save_gtkpod_prefs_window
 * UI has requested preferences update(by clicking ok on the prefs window)
 * Frees the tmpcfg variable
 */
static void
prefs_window_set(void)
{
    if (tmpcfg)
    {
	gint i;
	/* Need this in case user reordered column order (we don't
	 * catch the reorder signal) */
	tm_store_col_order ();
	prefs_set_charset(tmpcfg->charset);
	prefs_set_autoimport(tmpcfg->autoimport);
	for (i=0; i<TM_NUM_TAGS_PREFS; ++i) {
	    prefs_set_autosettags (i, tmpcfg->autosettags[i]);
	}
	prefs_set_readtags(tmpcfg->readtags);
	prefs_set_parsetags(tmpcfg->parsetags);
	prefs_set_parsetags_overwrite(tmpcfg->parsetags_overwrite);
	prefs_set_parsetags_template(tmpcfg->parsetags_template);
	prefs_set_coverart(tmpcfg->coverart);
	prefs_set_coverart_template(tmpcfg->coverart_template);
	prefs_set_mpl_autoselect (tmpcfg->mpl_autoselect);
	prefs_set_write_extended_info(tmpcfg->write_extended_info);

	/* this call well automatically destroy/setup the md5 hash table */
	prefs_set_md5tracks(tmpcfg->md5tracks);
	prefs_set_block_display(tmpcfg->block_display);
	prefs_set_sort_tab_num(tmpcfg->sort_tab_num, TRUE);
	prefs_set_group_compilations(tmpcfg->group_compilations, TRUE);
	prefs_set_toolbar_style(tmpcfg->toolbar_style);
	prefs_set_display_toolbar(tmpcfg->display_toolbar);
	prefs_set_display_tooltips_main (tmpcfg->display_tooltips_main);
	prefs_set_display_tooltips_prefs (tmpcfg->display_tooltips_prefs);
	prefs_set_multi_edit (tmpcfg->multi_edit);
	prefs_set_multi_edit_title (tmpcfg->multi_edit_title);
	prefs_set_misc_track_nr (tmpcfg->misc_track_nr);
	prefs_set_not_played_track (tmpcfg->not_played_track);
	prefs_set_update_charset(tmpcfg->update_charset);
	prefs_set_write_charset(tmpcfg->write_charset);
	prefs_set_add_recursively(tmpcfg->add_recursively);
	prefs_set_automount(tmpcfg->automount);
	prefs_set_tmp_disable_sort(tmpcfg->tmp_disable_sort);
	prefs_set_startup_messages(tmpcfg->startup_messages);
	prefs_set_mserv_use(tmpcfg->mserv_use);
	prefs_set_mserv_username(tmpcfg->mserv_username);
	prefs_set_unused_gboolean3(tmpcfg->unused_gboolean3);

	tm_show_preferred_columns();
    }
}


/* save current window size */
void prefs_window_update_default_sizes (void)
{
    if (prefs_window)
    {
	gint defx, defy;
	gtk_window_get_size (GTK_WINDOW (prefs_window), &defx, &defy);
	prefs_set_size_prefs (defx, defy);
    }
}


/**
 * cancel_gtk_prefs_window
 * UI has requested preference changes be ignored -- write back the
 * original values
 * Frees the tmpcfg and origcfg variable
 */
void
prefs_window_cancel(void)
{
    cfg_free (tmpcfg);
    /* exchange tmpcfg for origcfg */
    tmpcfg = origcfg;
    origcfg = NULL;

    /* "save" (i.e. reset) original configs */
    prefs_window_set ();

    /* delete cfg struct */
    cfg_free (tmpcfg);
    tmpcfg = NULL;

    /* save current window size */
    prefs_window_update_default_sizes ();

    /* close the window */
    if(prefs_window)
	gtk_widget_destroy(prefs_window);
    prefs_window = NULL;
}

/* when window is deleted, we keep the currently applied prefs and
   save the notebook page */
void prefs_window_delete(void)
{
    gint defx, defy;
    GtkWidget *nb;

    /* delete cfg structs */
    cfg_free (tmpcfg);
    tmpcfg = NULL;
    cfg_free (origcfg);
    origcfg = NULL;
	
		/* Delete temp prefs structures */
		temp_prefs_destroy(temp_prefs);
		temp_prefs = NULL;
		temp_lists_destroy(temp_lists);
		temp_lists = NULL;

    /* save current notebook page */
    nb = gtkpod_xml_get_widget (prefs_window_xml, "notebook");
    prefs_set_last_prefs_page (gtk_notebook_get_current_page (
				   GTK_NOTEBOOK (nb)));

    /* save current window size */
    gtk_window_get_size (GTK_WINDOW (prefs_window), &defx, &defy);
    prefs_set_size_prefs (defx, defy);

    /* close the window */
    if(prefs_window)
	gtk_widget_destroy(prefs_window);
    prefs_window = NULL;
}

/* apply the current settings and close the window */
/* Frees the tmpcfg and origcfg variable */
void
prefs_window_ok (void)
{
    gint defx, defy;
    GtkWidget *nb;

    /* save current settings */
    prefs_window_set ();

    /* delete cfg structs */
    cfg_free (tmpcfg);
    tmpcfg = NULL;
    cfg_free (origcfg);
    origcfg = NULL;
	
    /* Committ temp prefs to prefs table */
    temp_prefs_apply(temp_prefs);
    temp_lists_apply(temp_lists);

    /* save current notebook page */
    nb = gtkpod_xml_get_widget (prefs_window_xml, "notebook");
    prefs_set_last_prefs_page (gtk_notebook_get_current_page (
				   GTK_NOTEBOOK (nb)));

    /* save current window size */
    gtk_window_get_size (GTK_WINDOW (prefs_window), &defx, &defy);
    prefs_set_size_prefs (defx, defy);

    /* close the window */
    if(prefs_window)
	gtk_widget_destroy(prefs_window);
    prefs_window = NULL;
}


/* apply the current settings, don't close the window */
void
prefs_window_apply (void)
{
    gint defx, defy;
    GtkWidget *nb, *w;

    /* save current settings */
    prefs_window_set ();
	
		/* Committ temp prefs to prefs table */
		temp_prefs_apply(temp_prefs);
		temp_lists_apply(temp_lists);

    if((w = gtkpod_xml_get_widget (prefs_window_xml, "mserv_username_entry")))
    {
	gtk_entry_set_text(GTK_ENTRY(w), prefs_get_mserv_username ());
	/* tmpcfg gets set by the "changed" callback */
    }

    /* save current notebook page */
    nb = gtkpod_xml_get_widget (prefs_window_xml, "notebook");
    prefs_set_last_prefs_page (gtk_notebook_get_current_page (
				   GTK_NOTEBOOK (nb)));

    /* save current window size */
    gtk_window_get_size (GTK_WINDOW (prefs_window), &defx, &defy);
    prefs_set_size_prefs (defx, defy);
}



/* -----------------------------------------------------------------

   Callbacks

   ----------------------------------------------------------------- */

void
on_sorting_clicked                     (GtkButton       *button,
					gpointer         user_data)
{
    sort_window_create ();
}


void
on_edit_repository_clicked           (GtkButton       *button,
				      gpointer         user_data)
{
    repository_edit (gp_get_active_itdb(), NULL);
}

void
on_calendar_contact_notes_options_clicked (GtkButton       *button,
					   gpointer         user_data)
{
    /* Select an iPod repository. If the currently selected repository
       is not an iPod repository, select the first iPod repository. */
    iTunesDB *itdb = gp_get_active_itdb();
    if (itdb)
    {
	if (! (itdb->usertype & GP_ITDB_TYPE_IPOD))
	{
	    itdb = gp_get_ipod_itdb();
	}
    }
    repository_edit (itdb, NULL);
}


gboolean
on_prefs_window_delete_event           (GtkWidget       *widget,
					GdkEvent        *event,
					gpointer         user_data)
{
  prefs_window_delete ();
  gtkpod_statusbar_message(_("Preferences not updated"));
  return FALSE;
}


void
on_prefs_ok_clicked                    (GtkButton       *button,
					gpointer         user_data)
{
    prefs_window_ok();
}


void
on_prefs_cancel_clicked                (GtkButton       *button,
					gpointer         user_data)
{
    prefs_window_cancel();
    gtkpod_statusbar_message(_("Preferences not updated"));
}


void
on_prefs_apply_clicked                 (GtkButton       *button,
					gpointer         user_data)
{
    prefs_window_apply ();
    gtkpod_statusbar_message(_("Preferences applied"));
}


void
on_cfg_md5tracks_toggled                (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active(togglebutton);
    GtkWidget *w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_show_duplicates");

    tmpcfg->md5tracks = val;
    if(w)	gtk_widget_set_sensitive (w, val);
}

void
 on_cfg_block_display_toggled           (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->block_display = gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_update_existing_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active (togglebutton);
    GtkWidget *w;

    temp_prefs_set_int(temp_prefs, "update_existing", val);

    if((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_show_updated")))
	gtk_widget_set_sensitive (w, val);
    if((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_show_non_updated")))
	gtk_widget_set_sensitive (w, val);
}

void
on_cfg_id3_write_toggled                (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active (togglebutton);
    GtkWidget *w;

    temp_prefs_set_int(temp_prefs, "id3_write", val);
    if((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_id3_write_id3v24")))
	gtk_widget_set_sensitive (w, val);
    if((w = gtkpod_xml_get_widget (prefs_window_xml, "cfg_write_charset")))
	gtk_widget_set_sensitive (w, val);
}


void
on_cfg_id3_write_id3v24_toggled            (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    temp_prefs_set_int(temp_prefs,"id3_write_id3v24",
                       gtk_toggle_button_get_active (togglebutton));
}


void
on_cfg_write_extended_info_toggled     (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->write_extended_info = 
	gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_delete_track_from_playlist_toggled (GtkToggleButton *togglebutton,
					   gpointer         user_data)
{
	temp_prefs_set_int(temp_prefs, "delete_file",
                     gtk_toggle_button_get_active(togglebutton));
}


void
on_cfg_delete_track_from_ipod_toggled  (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
	temp_prefs_set_int(temp_prefs, "delete_ipod", 
                     gtk_toggle_button_get_active(togglebutton));
}

void
on_cfg_track_local_file_deletion_toggled (GtkToggleButton *togglebutton,
					  gpointer         user_data)
{
	temp_prefs_set_int(temp_prefs, "delete_local_file",
                     gtk_toggle_button_get_active(togglebutton));
}

void
on_cfg_track_database_deletion_toggled (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
	temp_prefs_set_int(temp_prefs, "delete_database",
                     gtk_toggle_button_get_active(togglebutton));
}

static void standard_toggle_toggled (GtkToggleButton *togglebutton,
				     const gchar *key)
{
    g_return_if_fail (key);

    temp_prefs_set_int (temp_prefs, key, 
			gtk_toggle_button_get_active (togglebutton));
}

void
on_sync_confirm_delete_toggled     (GtkToggleButton *togglebutton,
				    gpointer         user_data)
{
    GtkToggleButton *w;
    gboolean active = gtk_toggle_button_get_active(togglebutton);

    temp_prefs_set_int(temp_prefs, KEY_SYNC_CONFIRM_DELETE, active);
		       

    w = GTK_TOGGLE_BUTTON(
	gtkpod_xml_get_widget (prefs_window_xml,
			       "sync_confirm_delete_toggle"));
    if (w != togglebutton)
	gtk_toggle_button_set_active(w, active);

    w = GTK_TOGGLE_BUTTON(
	gtkpod_xml_get_widget (prefs_window_xml,
			       "sync_confirm_delete_toggle2"));
    if (w != togglebutton)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), active);
}


void
on_cfg_autoimport_toggled              (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->autoimport =
	gtk_toggle_button_get_active(togglebutton);
}

void
on_charset_combo_entry_changed          (GtkEditable     *editable,
					gpointer         user_data)
{
    gchar *descr, *charset;

    descr = gtk_editable_get_chars (editable, 0, -1);
    charset = charset_from_description (descr);
    prefs_cfg_set_charset (tmpcfg, charset);
    g_free (descr);
    g_free (charset);
}

void prefs_window_set_st_autoselect (guint32 inst, gboolean autoselect)
{
    if ((inst >= 0) && (inst < SORT_TAB_MAX))
    {
      temp_prefs_set_int_index(temp_prefs, "st_autoselect", inst, 
                               autoselect);
    }
}

void
on_cfg_mpl_autoselect_toggled          (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->mpl_autoselect =
	gtk_toggle_button_get_active(togglebutton);
}

void prefs_window_set_autosettags (gint category, gboolean autoset)
{
    if (category < TM_NUM_TAGS_PREFS)
	tmpcfg->autosettags[category] = autoset;
}

void
on_readtags_toggled                    (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->readtags = 
	gtk_toggle_button_get_active(togglebutton);
}

void
on_parsetags_toggled                   (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active(togglebutton);
    GtkWidget *w;

    tmpcfg->parsetags = val;
    w = gtkpod_xml_get_widget (prefs_window_xml, "parsetags_overwrite");
    gtk_widget_set_sensitive (w, val);
    w = gtkpod_xml_get_widget (prefs_window_xml, "parsetags_template");
    gtk_widget_set_sensitive (w, val);
}

void
on_parsetags_overwrite_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->parsetags_overwrite = 
	gtk_toggle_button_get_active(togglebutton);
}

void
on_parsetags_template_changed             (GtkEditable     *editable,
					   gpointer         user_data)
{
    g_free (tmpcfg->parsetags_template);
    tmpcfg->parsetags_template = gtk_editable_get_chars (editable,0, -1);
}

void
on_coverart_toggled                   (GtkToggleButton *togglebutton,
				       gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active(togglebutton);
    GtkWidget *w;

    tmpcfg->coverart = val;
    w = gtkpod_xml_get_widget (prefs_window_xml, "coverart_template");
    gtk_widget_set_sensitive (w, val);
}

void
on_coverart_template_changed             (GtkEditable     *editable,
					gpointer         user_data)
{
    g_free (tmpcfg->coverart_template);
    tmpcfg->coverart_template = gtk_editable_get_chars (editable,0, -1);
}

void prefs_window_set_col_visible (gint column, gboolean visible)
{
    if (column < TM_NUM_COLUMNS)
	    temp_prefs_set_int_index(temp_prefs, "col_visible", column, visible);
}

void
on_cfg_show_duplicates_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
	temp_prefs_set_int(temp_prefs, "show_duplicates", 
                     gtk_toggle_button_get_active (togglebutton));

}


void
on_cfg_show_updated_toggled            (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    temp_prefs_set_int(temp_prefs, "show_updated",
                       gtk_toggle_button_get_active (togglebutton));
}


void
on_cfg_show_non_updated_toggled        (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    temp_prefs_set_int(temp_prefs, "show_non_updated", 
                       gtk_toggle_button_get_active (togglebutton));
}

void
on_cfg_display_tooltips_main_toggled   (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->display_tooltips_main =
	gtk_toggle_button_get_active  (togglebutton);
}

void
on_cfg_display_tooltips_prefs_toggled  (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->display_tooltips_prefs =
	gtk_toggle_button_get_active  (togglebutton);
}


void
on_cfg_display_toolbar_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active (togglebutton);
    GtkWidget *w1 = gtkpod_xml_get_widget (prefs_window_xml, "cfg_toolbar_style_icons");
    GtkWidget *w2 = gtkpod_xml_get_widget (prefs_window_xml, "cfg_toolbar_style_text");
    GtkWidget *w3 = gtkpod_xml_get_widget (prefs_window_xml, "cfg_toolbar_style_both");

    tmpcfg->display_toolbar = val;

    if (w1) gtk_widget_set_sensitive (w1, val);
    if (w2) gtk_widget_set_sensitive (w2, val);
    if (w3) gtk_widget_set_sensitive (w3, val);
}

void
on_cfg_multi_edit_toggled              (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    gboolean val = gtk_toggle_button_get_active (togglebutton);
    GtkWidget *w = gtkpod_xml_get_widget  (prefs_window_xml, "cfg_multi_edit_title");

    tmpcfg->multi_edit = val;
    gtk_widget_set_sensitive (w, val);
}


void
on_cfg_multi_edit_title_toggled        (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->multi_edit_title =
	gtk_toggle_button_get_active  (togglebutton);
}

void
on_cfg_update_charset_toggled          (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->update_charset = gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_write_charset_toggled           (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->write_charset = gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_add_recursively_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->add_recursively = gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_not_played_track_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->not_played_track =
	gtk_toggle_button_get_active  (togglebutton);
}

void
on_cfg_misc_track_nr_value_changed      (GtkSpinButton   *spinbutton,
					gpointer         user_data)
{
    tmpcfg->misc_track_nr = gtk_spin_button_get_value  (spinbutton);
}

static void prefs_window_set_sort_tab_num (gint num)
{
    gint i;

    tmpcfg->sort_tab_num = num;
    for (i=0; i<SORT_TAB_MAX; ++i)
    {   /* make all checkboxes with i<num sensitive, the others
	   insensitive */
	gtk_widget_set_sensitive (autoselect_widget[i], i<num);
    }
}

void
on_cfg_sort_tab_num_sb_value_changed   (GtkSpinButton   *spinbutton,
					gpointer         user_data)
{
    prefs_window_set_sort_tab_num (
	gtk_spin_button_get_value (spinbutton));
}

void
on_cfg_group_compilations_toggled      (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->group_compilations = gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_toolbar_style_both_toggled      (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active (togglebutton))
    {
	tmpcfg->toolbar_style = GTK_TOOLBAR_BOTH;
    }
}


void
on_cfg_toolbar_style_text_toggled      (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active (togglebutton))
    {
	tmpcfg->toolbar_style = GTK_TOOLBAR_TEXT;
    }
}


void
on_cfg_toolbar_style_icons_toggled      (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active (togglebutton))
    {
	tmpcfg->toolbar_style = GTK_TOOLBAR_ICONS;
    }
}

void prefs_window_set_toolbar_style (GtkToolbarStyle style)
{
}

void
on_cfg_automount_ipod_toggled          (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    tmpcfg->automount = gtk_toggle_button_get_active (togglebutton);
}

void
on_cfg_temporarily_disable_sorting     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    tmpcfg->tmp_disable_sort =
	gtk_toggle_button_get_active (togglebutton);
}
void
on_cfg_startup_messages                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    tmpcfg->startup_messages =
	gtk_toggle_button_get_active (togglebutton);
}

void
on_mserv_from_file_playlist_menu_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gp_do_selected_playlist (mserv_from_file_tracks);
}


void
on_mserv_use_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    tmpcfg->mserv_use = gtk_toggle_button_get_active (togglebutton);
}

void
on_mserv_report_probs_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    temp_prefs_set_int(temp_prefs, "mserv_report_probs", 
                       gtk_toggle_button_get_active (togglebutton));
}

void
on_mserv_username_entry_changed              (GtkEditable     *editable,
					      gpointer         user_data)
{
    gchar *val = gtk_editable_get_chars (editable,0, -1);

    if (!val) return;
    g_free (tmpcfg->mserv_username);
    tmpcfg->mserv_username = val;
}

void
prefs_window_set_unused_gboolean3(gboolean val)
{
    tmpcfg->unused_gboolean3 = val;
}


/* ------------------------------------------------------------ *\
 *                                                              *
 * Sort-Prefs Window                                            *
 *                                                              *
\* ------------------------------------------------------------ */

/* the following checkboxes exist */
static const gint sort_ign_fields[] = {
    T_TITLE, T_ARTIST,
    T_ALBUM, T_COMPOSER,
    -1
};


/* Copy the current ignore fields and ignore strings into scfg */
static void sort_window_read_sort_ign (struct sortcfg *scfg)
{
    gint i;
    GtkTextIter ts, te;
    GtkTextView *tv;
    GtkTextBuffer *tb;

    g_return_if_fail (scfg);

    /* remove old list */
    g_list_free (scfg->tmp_sort_ign_fields);
    scfg->tmp_sort_ign_fields = NULL;

    /* read sort field states */
    for (i=0; sort_ign_fields[i] != -1; ++i)
    {
	gchar *buf = g_strdup_printf ("sort_ign_field_%d",
				      sort_ign_fields[i]);
	GtkWidget *w = gtkpod_xml_get_widget (sort_window_xml, buf);
	g_return_if_fail (w);
	scfg->tmp_sort_ign_fields = g_list_append (
	    scfg->tmp_sort_ign_fields,
	    GUINT_TO_POINTER( gtk_toggle_button_get_active (
		GTK_TOGGLE_BUTTON (w))));
	g_free (buf);
    }

    /* remove old strings */
    g_free (scfg->tmp_sort_ign_strings);
    scfg->tmp_sort_ign_strings = NULL;

    /* read new ignore strings */
    tv = GTK_TEXT_VIEW (gtkpod_xml_get_widget (sort_window_xml,
					      "sort_ign_strings"));
    g_return_if_fail (tv);
    tb = gtk_text_view_get_buffer (tv);
    g_return_if_fail (tb);

    gtk_text_buffer_get_bounds (tb, &ts, &te);
    scfg->tmp_sort_ign_strings = gtk_text_buffer_get_text (tb,
							   &ts, &te,
							   TRUE);
}

/**
 * sort_window_create
 * Create, Initialize, and Show the sorting preferences window
 * allocate a static sort struct for temporary variables
 */
void sort_window_create (void)
{
    if (sort_window)
    {  /* sort options already open --> simply raise to the top */
	gdk_window_raise(sort_window->window);
    }
    else
    {
	GList *collist = NULL;
	GtkWidget *w;
	GtkTextView *tv;
	GtkTextBuffer *tb;
	gint i;

	if(!tmpsortcfg && !origsortcfg)
	{
	    origsortcfg = clone_sortprefs();
	}
	else
	{
	    g_warning ("Programming error: tmpsortcfg is not NULL!!\n");
	    g_return_if_reached ();
	}
  
	sort_temp_prefs = temp_prefs_create();
	sort_temp_lists = temp_lists_create();

	sort_window_xml = glade_xml_new (xml_file, "sort_window", NULL);
	glade_xml_signal_autoconnect (sort_window_xml);

	sort_window = gtkpod_xml_get_widget (sort_window_xml, "sort_window");

	/* label the ignore-field checkbox-labels */
	for (i=0; sort_ign_fields[i] != -1; ++i)
	{
	    gchar *buf = g_strdup_printf ("sort_ign_field_%d",
					  sort_ign_fields[i]);
	    GtkWidget *w = gtkpod_xml_get_widget (sort_window_xml, buf);
	    g_return_if_fail (w);
	    gtk_button_set_label (
		GTK_BUTTON (w),
		gettext (get_t_string (sort_ign_fields[i])));
	    gtk_toggle_button_set_active (
		GTK_TOGGLE_BUTTON (w),
		prefs_get_int (buf));
	    /* set tooltip if available */
/* 	    if (tm_col_tooltips[sort_ign_fields[i]]) */
/* 	    { */
/* 		gtk_tooltips_set_tip ( */
/* 		    tt, w, */
/* 		    gettext (tm_col_tooltips[sort_ign_fields[i]]), */
/* 		    NULL); */
/* 	    } */
	    g_free (buf);
	}
	/* set the ignore strings */
	tv = GTK_TEXT_VIEW (gtkpod_xml_get_widget (sort_window_xml,
						  "sort_ign_strings"));
	tb = gtk_text_view_get_buffer (tv);
	if (!tb)
	{   /* text buffer doesn't exist yet */
	    tb = gtk_text_buffer_new (NULL);
	    gtk_text_view_set_buffer(tv, tb);
	    gtk_text_view_set_editable(tv, FALSE);
	    gtk_text_view_set_cursor_visible(tv, FALSE);
	}
	for (i=0; ; ++i)
	{
	    GtkTextIter ti;
	    gchar *buf = g_strdup_printf ("sort_ign_string_%d", i);
	    gchar *str = prefs_get_string (buf);

	    g_free (buf);

           /* end loop if no string is set or if the the string
	    * corresponds to the end marker */
	    if (!str)  break;  
	    if (strcmp (str, LIST_END_MARKER) == 0)
	    {
		g_free (str);
		break;
	    }

	    /* append new text to the end */
	    gtk_text_buffer_get_end_iter (tb, &ti);
	    gtk_text_buffer_insert (tb, &ti, str, -1);
	    /* append newline */
	    gtk_text_buffer_get_end_iter (tb, &ti);
	    gtk_text_buffer_insert (tb, &ti, "\n", -1);
	    g_free (str);
	}
	/* update the origsortcfg with the original settings (ignore
	 * fields and ignore strings) */
	sort_window_read_sort_ign (origsortcfg);

	/* Set Sort-Column-Combo */
	/* create the list in the order of the columns displayed */
	tm_store_col_order ();
	for (i=0; i<TM_NUM_COLUMNS; ++i)
	{   /* first the visible columns */
	    TM_item col = prefs_get_col_order (i);
	    if (col != -1)
	    {
		if (prefs_get_int_index("col_visible", col))
		    collist = g_list_append (collist,
					     gettext (get_tm_string (col)));
	    }
	}
	for (i=0; i<TM_NUM_COLUMNS; ++i)
	{   /* first the visible columns */
	    TM_item col = prefs_get_col_order (i);
	    if (col != -1)
	    {
		if (!prefs_get_int_index("col_visible", col))
		    collist = g_list_append (collist,
					     gettext (get_tm_string (col)));
	    }
	}
	w = gtkpod_xml_get_widget (sort_window_xml, "sort_combo");
	gtk_combo_set_popdown_strings (GTK_COMBO (w), collist);
	g_list_free (collist);
	collist = NULL;

	sort_window_update ();

	sort_window_show_hide_tooltips ();
	gtk_widget_show (sort_window);
    }
}



/* Update sort_window's settings (except for ignore list and ignore
 * fields) */
void sort_window_update (void)
{
    if (sort_window)
    {
	gchar *str;
	GtkWidget *w = NULL;

	/* update or create tmpsortcfg */
	if (tmpsortcfg)
	    sortcfg_free (tmpsortcfg);
	tmpsortcfg = clone_sortprefs();

	switch (tmpsortcfg->pm_sort)
	{
	case SORT_ASCENDING:
	    w = gtkpod_xml_get_widget (sort_window_xml, "pm_ascend");
	    break;
	case SORT_DESCENDING:
	    w = gtkpod_xml_get_widget (sort_window_xml, "pm_descend");
	    break;
	case SORT_NONE:
	    w = gtkpod_xml_get_widget (sort_window_xml, "pm_none");
	    break;
	}
	if (w)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);

	w = NULL;
	switch (tmpsortcfg->st_sort)
	{
	case SORT_ASCENDING:
	    w = gtkpod_xml_get_widget (sort_window_xml, "st_ascend");
	    break;
	case SORT_DESCENDING:
	    w = gtkpod_xml_get_widget (sort_window_xml, "st_descend");
	    break;
	case SORT_NONE:
	    w = gtkpod_xml_get_widget (sort_window_xml, "st_none");
	    break;
	}
	if (w)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);

	switch (tmpsortcfg->tm_sort)
	{
	case SORT_ASCENDING:
	    w = gtkpod_xml_get_widget (sort_window_xml, "tm_ascend");
	    break;
	case SORT_DESCENDING:
	    w = gtkpod_xml_get_widget (sort_window_xml, "tm_descend");
	    break;
	case SORT_NONE:
	    w = gtkpod_xml_get_widget (sort_window_xml, "tm_none");
	    break;
	}
	if (w)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);

	w = gtkpod_xml_get_widget (sort_window_xml, "pm_autostore");
	if (w)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
					 tmpsortcfg->pm_autostore);

	w = gtkpod_xml_get_widget (sort_window_xml, "tm_autostore");
	if (w)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
					 tmpsortcfg->tm_autostore);

	if((w = gtkpod_xml_get_widget (sort_window_xml, "cfg_case_sensitive")))
	{
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w),
					 tmpsortcfg->case_sensitive);
	}
	/* set standard entry in combo */
	str = gettext (get_tm_string (prefs_get_tm_sortcol ()));
	w = gtkpod_xml_get_widget (sort_window_xml, "sort_combo");
	gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (w)->entry), str);
    }
}


/* turn the sort window insensitive (if it's open) */
void sort_window_block (void)
{
    if (sort_window)
	gtk_widget_set_sensitive (sort_window, FALSE);
}

/* turn the sort window sensitive (if it's open) */
void sort_window_release (void)
{
    if (sort_window)
	gtk_widget_set_sensitive (sort_window, TRUE);
}


/* make the tooltips visible or hide it depending on the value set in
 * the prefs (tooltips_prefs) */
void sort_window_show_hide_tooltips (void)
{
    if (sort_window)
    {
	GtkTooltips *tt;
	GtkTooltipsData *tooltips_data;
	tooltips_data = gtk_tooltips_data_get (gtkpod_xml_get_widget (sort_window_xml, "sort_combo-entry"));
	tt = tooltips_data->tooltips;
	if (tt)
	{
	    if (prefs_get_display_tooltips_prefs ()) gtk_tooltips_enable (tt);
	    else                                     gtk_tooltips_disable (tt);
	}
	else
	{
	    g_warning ("***tt is NULL***");
	}
    }
}


/* get the sort_column selected in the combo */
static TM_item sort_window_get_sort_col (void)
{
    const gchar *str;
    GtkWidget *w;
    gint i = -1;

    w = gtkpod_xml_get_widget (sort_window_xml, "sort_combo");
    str = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (w)->entry));
    /* Check which string is selected in the combo */
    if (str)
	for (i=0; get_tm_string (i); ++i)
	    if (strcmp (gettext (get_tm_string (i)), str) == 0)  break;
    if ((i<0) || (i>= TM_NUM_COLUMNS))
    {
	fprintf (stderr,
		 "Programming error: cal_get_category () -- item not found.\n");
	/* set to something reasonable at least */
	i = TM_COLUMN_TITLE;
    }

    return i;
}


/* copy newly set values from tmpsortcfg to the original cfg struct */
static void sort_window_set (struct sortcfg *scfg)
{
    struct sortcfg *tsc;
    gchar *buf;
    gint i;

    g_return_if_fail (scfg);

    tsc = clone_sortprefs ();

    prefs_set_pm_sort (scfg->pm_sort);
    prefs_set_pm_autostore (scfg->pm_autostore);
    prefs_set_st_sort (scfg->st_sort);
    prefs_set_tm_sort (scfg->tm_sort);
    prefs_set_tm_autostore (scfg->tm_autostore);
    prefs_set_case_sensitive (scfg->case_sensitive);
    scfg->tm_sortcol = sort_window_get_sort_col ();
    prefs_set_tm_sortcol (scfg->tm_sortcol);

    /* set sort field states */
    for (i=0; sort_ign_fields[i] != -1; ++i)
    {
	gchar *buf = g_strdup_printf ("sort_ign_field_%d",
				      sort_ign_fields[i]);
	prefs_set_int (
	    buf,
	    GPOINTER_TO_UINT(g_list_nth_data (
		scfg->tmp_sort_ign_fields, i)));
	g_free (buf);
    }
    /* clean up old sort strings */
    for (i=0; i>=0; ++i)
    {
	gchar *buf = g_strdup_printf ("sort_ign_string_%d", i);
	if (prefs_get_string (buf))
	{
	    prefs_set_string (buf, NULL);
	}
	else
	{
	    i=-2;  /* end loop */
	}
	g_free (buf);
    }
    /* set new sort strings */
    i=0;
    if (scfg->tmp_sort_ign_strings)
    {
	gchar **strings = g_strsplit(scfg->tmp_sort_ign_strings,
				     "\n", -1);
	gchar **strp = strings;
	while (*strp)
	{
	    g_strstrip (*strp);
	    if (strlen (*strp) != 0)
	    {
		/* add space to the ignore string */
		gchar *str = g_strdup_printf ("%s ", *strp);
		buf = g_strdup_printf ("sort_ign_string_%d", i);
		prefs_set_string (buf, str);
		g_free (str);
		g_free (buf);
		++i;
	    }
	    ++strp;
	}
	g_strfreev (strings);
    }
    /* set end marker */
    buf = g_strdup_printf ("sort_ign_string_%d", i);
    prefs_set_string (buf, LIST_END_MARKER);
    g_free (buf);
    /* update compare string keys */
    compare_string_fuzzy_generate_keys ();

    /* if sort type has changed, initialize display */
    if (tsc->pm_sort != scfg->pm_sort)
	pm_sort (scfg->pm_sort);
    if (tsc->st_sort != scfg->st_sort)
	st_sort (scfg->st_sort);
    if ((tsc->tm_sort != scfg->tm_sort) ||
	(tsc->tm_sortcol != scfg->tm_sortcol))
    {
	tm_sort_counter (-1);
	tm_sort (prefs_get_tm_sortcol (), scfg->tm_sort);
    }
    /* if auto sort was changed to TRUE, store order */
    if (!tsc->pm_autostore && scfg->pm_autostore)
	pm_rows_reordered ();
    if (!tsc->tm_autostore && scfg->tm_autostore)
	tm_rows_reordered ();

    sortcfg_free (tsc);
}


/* -----------------------------------------------------------------

   Callbacks

   ----------------------------------------------------------------- */

void
on_st_ascend_toggled                   (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_st_sort (SORT_ASCENDING);
}


void
on_st_descend_toggled                  (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_st_sort (SORT_DESCENDING);
}


void
on_st_none_toggled                     (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_st_sort (SORT_NONE);
}


void
on_pm_ascend_toggled                   (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_pm_sort (SORT_ASCENDING);
}


void
on_pm_descend_toggled                  (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_pm_sort (SORT_DESCENDING);
}


void
on_pm_none_toggled                     (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_pm_sort (SORT_NONE);
}


void
on_pm_autostore_toggled                (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    sort_window_set_pm_autostore (gtk_toggle_button_get_active(togglebutton));
}



void
on_tm_ascend_toggled                   (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_tm_sort (SORT_ASCENDING);
}


void
on_tm_descend_toggled                  (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_tm_sort (SORT_DESCENDING);
}


void
on_tm_none_toggled                     (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
	sort_window_set_tm_sort (SORT_NONE);
}

void
on_tm_autostore_toggled                (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    sort_window_set_tm_autostore (gtk_toggle_button_get_active(togglebutton));
}


void
on_sort_case_sensitive_toggled         (GtkToggleButton *togglebutton,
					gpointer         user_data)
{
    sort_window_set_case_sensitive(
	gtk_toggle_button_get_active(togglebutton));
}


void
on_sort_apply_clicked                  (GtkButton       *button,
					gpointer         user_data)
{
    sort_window_apply ();
}


void
on_sort_cancel_clicked                 (GtkButton       *button,
					gpointer         user_data)
{
    sort_window_cancel ();
}


void
on_sort_ok_clicked                     (GtkButton       *button,
					gpointer         user_data)
{
    sort_window_ok ();
}


gboolean
on_sort_window_delete_event            (GtkWidget       *widget,
					GdkEvent        *event,
					gpointer         user_data)
{
    sort_window_delete ();
    return FALSE;
}



/**
 * sort_window_cancel
 * UI has requested sort prefs changes be ignored -- write back the
 * original values
 * Frees the tmpsortcfg and origsortcfg variable
 */
void sort_window_cancel (void)
{
    g_return_if_fail (tmpsortcfg);
    g_return_if_fail (origsortcfg);

    /* "save" (i.e. reset) original configs */
    sort_window_set (origsortcfg);

    /* delete cfg struct */
    sortcfg_free (tmpsortcfg);
    tmpsortcfg = NULL;
    sortcfg_free (origsortcfg);
    origsortcfg = NULL;

    /* close the window */
    if(sort_window)
	gtk_widget_destroy(sort_window);
    sort_window = NULL;
}

/* when window is deleted, we keep the currently applied prefs */
void sort_window_delete(void)
{
    g_return_if_fail (tmpsortcfg);
    g_return_if_fail (origsortcfg);
  
    temp_prefs_destroy(sort_temp_prefs);
    sort_temp_prefs = NULL;
    temp_lists_destroy(sort_temp_lists);
    sort_temp_lists = NULL;

    /* delete sortcfg structs */
    sortcfg_free (tmpsortcfg);
    tmpsortcfg = NULL;
    sortcfg_free (origsortcfg);
    origsortcfg = NULL;

    /* close the window */
    if(sort_window)
	gtk_widget_destroy(sort_window);
    sort_window = NULL;
}

/* apply the current settings and close the window */
/* Frees the tmpsortcfg and origsortcfg variable */
void sort_window_ok (void)
{
    g_return_if_fail (tmpsortcfg);
    g_return_if_fail (origsortcfg);

    /* update the sort ignore strings */
    sort_window_read_sort_ign (tmpsortcfg);
    /* save current settings */
    sort_window_set (tmpsortcfg);
  
    temp_prefs_apply(sort_temp_prefs);
    temp_lists_apply(sort_temp_lists);

    /* delete sortcfg structs */
    sortcfg_free (tmpsortcfg);
    tmpsortcfg = NULL;
    sortcfg_free (origsortcfg);
    origsortcfg = NULL;

    /* close the window */
    if(sort_window)
	gtk_widget_destroy(sort_window);
    sort_window = NULL;
}


/* apply the current settings, don't close the window */
void sort_window_apply (void)
{
    g_return_if_fail (tmpsortcfg);
    g_return_if_fail (origsortcfg);
  
    temp_prefs_apply(sort_temp_prefs);
    temp_lists_apply(sort_temp_lists);

    /* update the sort ignore strings */
    sort_window_read_sort_ign (tmpsortcfg);
    /* save current settings */
    sort_window_set (tmpsortcfg);
}

void sort_window_set_pm_autostore (gboolean val)
{
    tmpsortcfg->pm_autostore = val;
}

void sort_window_set_tm_autostore (gboolean val)
{
    tmpsortcfg->tm_autostore = val;
}

void sort_window_set_pm_sort (gint val)
{
    tmpsortcfg->pm_sort = val;
}

void sort_window_set_st_sort (gint val)
{
    tmpsortcfg->st_sort = val;
}

void sort_window_set_tm_sort (gint val)
{
    tmpsortcfg->tm_sort = val;
}

void sort_window_set_case_sensitive (gboolean val)
{
    tmpsortcfg->case_sensitive = val;
}
