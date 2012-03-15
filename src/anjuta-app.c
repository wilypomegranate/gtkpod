/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * anjuta.c Copyright (C) 2003 Naba Kumar  <naba@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <sys/wait.h>

#include <gtk/gtk.h>

#include <gdl/gdl.h>
#include <glib-2.0/gio/gio.h>

#include <libanjuta/anjuta-shell.h>
#include <libanjuta/anjuta-ui.h>
#include <libanjuta/anjuta-utils.h>
#include <libanjuta/resources.h>
#include <libanjuta/anjuta-plugin-manager.h>
#include <libanjuta/anjuta-debug.h>

#include "anjuta-app.h"
#include "anjuta-actions.h"
#include "anjuta-about.h"

#include "gtkpod.h"
#include "libgtkpod/directories.h"
#include "libgtkpod/gtkpod_app_iface.h"
#include "libgtkpod/prefs.h"
#include "libgtkpod/misc.h"

#define ICON_FILE "anjuta-preferences-general-48.png"
#define PREF_SCHEMA "org.gtkpod"
#define GDL_STYLE "gdl-style"
#define TOOLBAR_VISIBLE "toolbar-visible"
#define TOOLBAR_STYLE "toolbar-style"

static void anjuta_app_layout_load(AnjutaApp *app, const gchar *layout_filename, const gchar *name);
static void anjuta_app_layout_save(AnjutaApp *app, const gchar *layout_filename, const gchar *name);

static gpointer parent_class = NULL;
static gchar *uifile = NULL;

static GHashTable *id_hash = NULL;

typedef struct {
    GtkWidget *window;
    GtkBuilder *window_builder;
    gboolean scrolled;
    gchar *option1_key;
    gboolean option1_invert;
    gchar *option2_key;
    gboolean option2_invert;
    gchar *confirm_again_key;
    ConfHandler ok_handler;
    ConfHandler apply_handler;
    ConfHandler cancel_handler;
    gpointer user_data1;
    gpointer user_data2;
} ConfData;

void anjuta_set_ui_file_path(gchar *path) {
    uifile = path;
}

static void menu_item_select_cb(GtkMenuItem *proxy, AnjutaApp *app) {
    GtkAction *action;
    char *message;

    action = gtk_activatable_get_related_action(GTK_ACTIVATABLE(proxy));
    g_return_if_fail(action != NULL);

    g_object_get(G_OBJECT(action), "tooltip", &message, NULL);
    if (message) {
        anjuta_status_push(app->status, "%s", message);
        g_free(message);
    }
}

static void menu_item_deselect_cb(GtkMenuItem *proxy, AnjutaApp *app) {
    anjuta_status_pop(app->status);
}

static void connect_proxy_cb(GtkUIManager *manager, GtkAction *action, GtkWidget *proxy, AnjutaApp *app) {
    if (GTK_IS_MENU_ITEM(proxy)) {
        g_signal_connect(proxy, "select", G_CALLBACK(menu_item_select_cb), app);
        g_signal_connect(proxy, "deselect", G_CALLBACK(menu_item_deselect_cb), app);
    }
}

static void disconnect_proxy_cb(GtkUIManager *manager, GtkAction *action, GtkWidget *proxy, AnjutaApp *app) {
    if (GTK_IS_MENU_ITEM(proxy)) {
        g_signal_handlers_disconnect_by_func(proxy, G_CALLBACK(menu_item_select_cb), app);
        g_signal_handlers_disconnect_by_func(proxy, G_CALLBACK(menu_item_deselect_cb), app);
    }
}

static void anjuta_app_iconify_dockable_widget(AnjutaShell *shell, GtkWidget *widget, GError **error) {
    AnjutaApp *app = NULL;
    GtkWidget *dock_item = NULL;

    /* Argumments assertions */
    g_return_if_fail(ANJUTA_IS_APP (shell));
    g_return_if_fail(GTK_IS_WIDGET(widget));

    app = ANJUTA_APP (shell);
    g_return_if_fail(app->widgets != NULL);

    dock_item = g_object_get_data(G_OBJECT(widget), "dockitem");
    g_return_if_fail(dock_item != NULL);

    /* Iconify the dockable item */
    gdl_dock_item_iconify_item(GDL_DOCK_ITEM(dock_item));
}

static void anjuta_app_hide_dockable_widget(AnjutaShell *shell, GtkWidget *widget, GError **error) {
    AnjutaApp *app = NULL;
    GtkWidget *dock_item = NULL;

    /* Arguments assertions */
    g_return_if_fail(ANJUTA_IS_APP (shell));
    g_return_if_fail(GTK_IS_WIDGET(widget));

    app = ANJUTA_APP (shell);
    g_return_if_fail(app->widgets != NULL);

    dock_item = g_object_get_data(G_OBJECT(widget), "dockitem");
    g_return_if_fail(dock_item != NULL);

    /* Hide the dockable item */
    gdl_dock_item_hide_item(GDL_DOCK_ITEM(dock_item));
}

static void anjuta_app_show_dockable_widget(AnjutaShell *shell, GtkWidget* widget, GError **error) {
    AnjutaApp *app = NULL;
    GtkWidget *dock_item = NULL;

    /* Argumments assertions */
    g_return_if_fail(ANJUTA_IS_APP (shell));
    g_return_if_fail(GTK_IS_WIDGET(widget));

    app = ANJUTA_APP (shell);
    g_return_if_fail(app->widgets != NULL);

    dock_item = g_object_get_data(G_OBJECT(widget), "dockitem");
    g_return_if_fail(dock_item != NULL);

    /* Show the dockable item */
    gdl_dock_item_show_item(GDL_DOCK_ITEM(dock_item));
}

static void anjuta_app_maximize_widget(AnjutaShell *shell, const char *widget_name, GError **error) {
    AnjutaApp *app = NULL;
    GtkWidget *dock_item = NULL;
    gpointer value, key;
    GtkWidget *widget = NULL;
    GHashTableIter iter;

    /* AnjutaApp assertions */
    g_return_if_fail(ANJUTA_IS_APP (shell));
    app = ANJUTA_APP (shell);

    /* If app->maximized is TRUE then another widget is already maximized.
     Restoring the UI for a new maximization. */
    if (app->maximized)
        gdl_dock_layout_load_layout(app->layout_manager, "back-up");

    /* Back-up the layout so it can be restored */
    gdl_dock_layout_save_layout(app->layout_manager, "back-up");

    /* Mark the app as maximized (the other widgets except center are hidden) */
    app->maximized = TRUE;

    /* Hide all DockItem's except the ones positioned in the center */
    g_hash_table_iter_init(&iter, app->widgets);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        if (value == NULL)
            continue;

        /* If it's the widget requesting maximization then continue */
        if (!g_strcmp0((gchar*) key, widget_name))
            continue;

        /* Widget assertions */
        widget = GTK_WIDGET(value);
        if (!GTK_IS_WIDGET(widget))
            continue;

        /* DockItem assertions */
        dock_item = g_object_get_data(G_OBJECT(widget), "dockitem");
        if (dock_item == NULL || !GDL_IS_DOCK_ITEM(dock_item))
            continue;

        /* Hide the item */
        gdl_dock_item_hide_item(GDL_DOCK_ITEM(dock_item));
    }
}

static void anjuta_app_unmaximize(AnjutaShell *shell, GError **error) {
    AnjutaApp *app = NULL;

    /* AnjutaApp assertions */
    g_return_if_fail(ANJUTA_IS_APP (shell));
    app = ANJUTA_APP (shell);

    /* If not maximized then the operation doesn't make sence. */
    g_return_if_fail(app->maximized);

    /* Load the backed-up layout */
    gdl_dock_layout_load_layout(app->layout_manager, "back-up");
    gdl_dock_layout_delete_layout(app->layout_manager, "back-up");

    /* Un-mark maximized */
    app->maximized = FALSE;
}

static void on_gdl_style_changed(GSettings* settings, const gchar* key, gpointer user_data) {
    AnjutaApp* app = ANJUTA_APP (user_data);
    GdlSwitcherStyle style = GDL_SWITCHER_STYLE_BOTH;

    gchar* pr_style = g_settings_get_string(settings, key);

    if (g_strcmp0(pr_style, "Text") == 0)
        style = GDL_SWITCHER_STYLE_TEXT;
    else if (g_strcmp0(pr_style, "Icon") == 0)
        style = GDL_SWITCHER_STYLE_ICON;
    else if (g_strcmp0(pr_style, "Both") == 0)
        style = GDL_SWITCHER_STYLE_BOTH;
    else if (g_strcmp0(pr_style, "Toolbar") == 0)
        style = GDL_SWITCHER_STYLE_TOOLBAR;
    else if (g_strcmp0(pr_style, "Tabs") == 0)
        style = GDL_SWITCHER_STYLE_TABS;

    g_object_set(G_OBJECT(app->layout_manager->master), "switcher-style", style, NULL);
    g_free(pr_style);
}

static void on_toggle_widget_view(GtkCheckMenuItem *menuitem, GtkWidget *dockitem) {
    gboolean state;
    state = gtk_check_menu_item_get_active(menuitem);
    if (state)
        gdl_dock_item_show_item(GDL_DOCK_ITEM(dockitem));
    else
        gdl_dock_item_hide_item(GDL_DOCK_ITEM(dockitem));
}

static void on_update_widget_view_menuitem(gpointer key, gpointer wid, gpointer data) {
    GtkCheckMenuItem *menuitem;
    GdlDockItem *dockitem;

    dockitem = g_object_get_data(G_OBJECT(wid), "dockitem");
    menuitem = g_object_get_data(G_OBJECT(wid), "menuitem");

    g_signal_handlers_block_by_func(menuitem, G_CALLBACK(on_toggle_widget_view), dockitem);

    if (GDL_DOCK_OBJECT_ATTACHED(dockitem))
        gtk_check_menu_item_set_active(menuitem, TRUE);
    else
        gtk_check_menu_item_set_active(menuitem, FALSE);

    g_signal_handlers_unblock_by_func(menuitem, G_CALLBACK(on_toggle_widget_view), dockitem);
}

static void on_layout_dirty_notify(GObject *object, GParamSpec *pspec, gpointer user_data) {
    if (!strcmp(pspec->name, "dirty")) {
        gboolean dirty;
        g_object_get(object, "dirty", &dirty, NULL);
        if (dirty) {
            /* Update UI toggle buttons */
            g_hash_table_foreach(ANJUTA_APP (user_data)->widgets, on_update_widget_view_menuitem, NULL);
        }
    }
}

static void on_layout_locked_notify(GdlDockMaster *master, GParamSpec *pspec, AnjutaApp *app) {
    AnjutaUI *ui;
    GtkAction *action;
    gint locked;

    ui = app->ui;
    action = anjuta_ui_get_action(ui, "ActionGroupToggleView", "ActionViewLockLayout");

    g_object_get(master, "locked", &locked, NULL);
    gtk_toggle_action_set_active(GTK_TOGGLE_ACTION (action), (locked == 1));
}

static void on_session_save(AnjutaShell *shell, AnjutaSessionPhase phase, AnjutaSession *session, AnjutaApp *app) {
    gchar *geometry, *layout_file;
    GdkWindowState state;

    if (phase != ANJUTA_SESSION_PHASE_NORMAL)
        return;

    /* Save geometry */
    state = gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(app)));
    if (state & GDK_WINDOW_STATE_MAXIMIZED) {
        anjuta_session_set_int(session, "Anjuta", "Maximized", 1);
    }
    if (state & GDK_WINDOW_STATE_FULLSCREEN) {
        anjuta_session_set_int(session, "Anjuta", "Fullscreen", 1);
    }

    /* Save geometry only if window is not maximized or fullscreened */
    if (!(state & GDK_WINDOW_STATE_MAXIMIZED) || !(state & GDK_WINDOW_STATE_FULLSCREEN)) {
        geometry = anjuta_app_get_geometry(app);
        if (geometry)
            anjuta_session_set_string(session, "Anjuta", "Geometry", geometry);
        g_free(geometry);
    }

    /* Save layout */
    layout_file = g_build_filename(anjuta_session_get_session_directory(session), "dock-layout.xml", NULL);
    anjuta_app_layout_save(app, layout_file, NULL);
    g_free(layout_file);
}

static void on_session_load(AnjutaShell *shell, AnjutaSessionPhase phase, AnjutaSession *session, AnjutaApp *app) {
    /* We load layout at last so that all plugins would have loaded by now */
    if (phase == ANJUTA_SESSION_PHASE_LAST) {
        gchar *geometry;
        gchar *layout_file;

        /* Restore geometry */
        geometry = anjuta_session_get_string(session, "Anjuta", "Geometry");
        anjuta_app_set_geometry(app, geometry);

        /* Restore window state */
        if (anjuta_session_get_int(session, "Anjuta", "Fullscreen")) {
            /* bug #304495 */
            AnjutaUI* ui = anjuta_shell_get_ui(shell, NULL);
            GtkAction* action = anjuta_ui_get_action(ui, "ActionGroupToggleView", "ActionViewFullscreen");
            gtk_toggle_action_set_active(GTK_TOGGLE_ACTION (action), TRUE);

            gtk_window_fullscreen(GTK_WINDOW (shell));

        }
        else if (anjuta_session_get_int(session, "Anjuta", "Maximized")) {
            gtk_window_maximize(GTK_WINDOW (shell));
        }

        /* Restore layout */
        layout_file = g_build_filename(anjuta_session_get_session_directory(session), "dock-layout.xml", NULL);
        anjuta_app_layout_load(app, layout_file, NULL);
        g_free(layout_file);
    }
}

static void anjuta_app_dispose(GObject *widget) {
    AnjutaApp *app;

    g_return_if_fail (ANJUTA_IS_APP (widget));

    app = ANJUTA_APP (widget);

    if (app->widgets) {
        if (g_hash_table_size(app->widgets) > 0) {
            /*
             g_warning ("Some widgets are still inside shell (%d widgets), they are:",
             g_hash_table_size (app->widgets));
             g_hash_table_foreach (app->widgets, (GHFunc)puts, NULL);
             */
        }
        g_hash_table_destroy(app->widgets);
        app->widgets = NULL;
    }

    if (app->values) {
        if (g_hash_table_size(app->values) > 0) {
            /*
             g_warning ("Some Values are still left in shell (%d Values), they are:",
             g_hash_table_size (app->values));
             g_hash_table_foreach (app->values, (GHFunc)puts, NULL);
             */
        }
        g_hash_table_destroy(app->values);
        app->values = NULL;
    }

    if (app->layout_manager) {
        g_object_unref(app->layout_manager);
        app->layout_manager = NULL;
    }
    if (app->profile_manager) {
        g_object_unref(G_OBJECT (app->profile_manager));
        app->profile_manager = NULL;
    }
    if (app->plugin_manager) {
        g_object_unref(G_OBJECT (app->plugin_manager));
        app->plugin_manager = NULL;
    }
    if (app->status) {
        g_object_unref(G_OBJECT (app->status));
        app->status = NULL;
    }

    if (app->settings) {
        g_object_unref(app->settings);
        app->settings = NULL;
    }

    G_OBJECT_CLASS (parent_class)->dispose(widget);
}

static void anjuta_app_finalize(GObject *widget) {
    AnjutaApp *app;

    g_return_if_fail(ANJUTA_IS_APP (widget));

    app = ANJUTA_APP (widget);

    g_object_unref(app->ui);
    g_object_unref(app->preferences);

    G_OBJECT_CLASS(parent_class)->finalize(widget);
}

static void anjuta_app_instance_init(AnjutaApp *app) {
    GtkWidget *menubar, *about_menu;
    GtkWidget *view_menu, *hbox;
    GtkWidget *main_box;
    GtkWidget *dockbar;
    GList *plugins_dirs = NULL;
    GdkGeometry size_hints =
        { 100, 100, 0, 0, 100, 100, 1, 1, 0.0, 0.0, GDK_GRAVITY_NORTH_WEST };

    DEBUG_PRINT("%s", "Initializing gtkpod...");

    gtk_window_set_geometry_hints(GTK_WINDOW (app), GTK_WIDGET (app), &size_hints, GDK_HINT_RESIZE_INC);
    gtk_window_set_resizable(GTK_WINDOW (app), TRUE);

    /*
     * Main box
     */
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER (app), main_box);
    gtk_widget_show(main_box);

    app->values = NULL;
    app->widgets = NULL;
    app->maximized = FALSE;

    /* Settings */
    app->settings = g_settings_new (PREF_SCHEMA);

    /* Status bar */
    app->status = ANJUTA_STATUS(anjuta_status_new());
    anjuta_status_set_title_window(app->status, GTK_WIDGET (app));
    gtk_widget_show(GTK_WIDGET (app->status));
    gtk_box_pack_end(GTK_BOX (main_box), GTK_WIDGET (app->status), FALSE, TRUE, 0);
    g_object_ref(G_OBJECT (app->status));
    g_object_add_weak_pointer(G_OBJECT (app->status), (gpointer) &app->status);

    /* configure dock */
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(hbox);
    app->dock = gdl_dock_new();
    gtk_widget_show(app->dock);
    gtk_box_pack_end(GTK_BOX (hbox), app->dock, TRUE, TRUE, 0);

    dockbar = gdl_dock_bar_new(GDL_DOCK(app->dock));
    gtk_widget_show(dockbar);
    gtk_box_pack_start(GTK_BOX (hbox), dockbar, FALSE, FALSE, 0);

    app->layout_manager = gdl_dock_layout_new(GDL_DOCK (app->dock));
    g_signal_connect (app->layout_manager, "notify::dirty",
            G_CALLBACK (on_layout_dirty_notify), app);
    g_signal_connect (app->layout_manager->master, "notify::locked",
            G_CALLBACK (on_layout_locked_notify), app);

    /* UI engine */
    app->ui = anjuta_ui_new();
    g_object_add_weak_pointer(G_OBJECT(app->ui), (gpointer) &app->ui);
    /* show tooltips in the statusbar */
    g_signal_connect(app->ui, "connect_proxy", G_CALLBACK(connect_proxy_cb), app);
    g_signal_connect(app->ui, "disconnect_proxy", G_CALLBACK(disconnect_proxy_cb), app);

    /* Plugin Manager */
    plugins_dirs = g_list_prepend(plugins_dirs, get_plugin_dir());
    app->plugin_manager = anjuta_plugin_manager_new(G_OBJECT (app), app->status, plugins_dirs);
    app->profile_manager = anjuta_profile_manager_new(app->plugin_manager);
    g_list_free(plugins_dirs);

    /* Preferences */
    app->preferences = anjuta_preferences_new(app->plugin_manager);
    g_object_add_weak_pointer(G_OBJECT (app->preferences), (gpointer) &app->preferences);

    g_signal_connect(app->settings, "changed::" GDL_STYLE, G_CALLBACK(on_gdl_style_changed), app);
    on_gdl_style_changed(app->settings, GDL_STYLE, app);

    /* Register actions */
    anjuta_ui_add_action_group_entries(app->ui, "ActionGroupMusic", _("Music"), menu_entries_music, G_N_ELEMENTS (menu_entries_music), GETTEXT_PACKAGE, TRUE, app);
    anjuta_ui_add_action_group_entries(app->ui, "ActionGroupEdit", _("Edit"), menu_entries_edit, G_N_ELEMENTS (menu_entries_edit), GETTEXT_PACKAGE, TRUE, app);
    anjuta_ui_add_action_group_entries(app->ui, "ActionGroupView", _("View"), menu_entries_view, G_N_ELEMENTS (menu_entries_view), GETTEXT_PACKAGE, TRUE, app);
    anjuta_ui_add_toggle_action_group_entries(app->ui, "ActionGroupToggleView", _("View"), menu_entries_toggle_view, G_N_ELEMENTS (menu_entries_toggle_view), GETTEXT_PACKAGE, TRUE, app);
    anjuta_ui_add_action_group_entries(app->ui, "ActionGroupTools", _("Tools"), menu_entries_tools, G_N_ELEMENTS (menu_entries_tools), GETTEXT_PACKAGE, TRUE, app);
    anjuta_ui_add_action_group_entries(app->ui, "ActionGroupHelp", _("Help"), menu_entries_help, G_N_ELEMENTS (menu_entries_help), GETTEXT_PACKAGE, TRUE, app);

    /* Merge UI */
    anjuta_ui_merge(app->ui, uifile);

    /* Adding accels group */
    gtk_window_add_accel_group(GTK_WINDOW (app), gtk_ui_manager_get_accel_group(GTK_UI_MANAGER (app->ui)));

    /* create main menu */
    menubar = gtk_ui_manager_get_widget(GTK_UI_MANAGER (app->ui), "/MenuMain");
    gtk_box_pack_start(GTK_BOX (main_box), menubar, FALSE, FALSE, 0);
    gtk_widget_show(menubar);

    /* Create widgets menu */
    view_menu = gtk_ui_manager_get_widget(GTK_UI_MANAGER(app->ui), "/MenuMain/MenuView");
    app->view_menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM (view_menu));

    /* Create about plugins menu */
    about_menu = gtk_ui_manager_get_widget(GTK_UI_MANAGER(app->ui), "/MenuMain/PlaceHolderHelpMenus/MenuHelp/"
        "PlaceHolderHelpAbout/AboutPlugins");
    about_create_plugins_submenu(ANJUTA_SHELL(app), about_menu);

    /* Add main view */
    gtk_box_pack_start(GTK_BOX (main_box), hbox, TRUE, TRUE, 0);

    /* Connect to session */
    g_signal_connect (G_OBJECT (app), "save_session",
            G_CALLBACK (on_session_save), app);
    g_signal_connect (G_OBJECT (app), "load_session",
            G_CALLBACK (on_session_load), app);

    /* Loading accels */
    anjuta_ui_load_accels(NULL);

    app->save_count = 0;
}

/*
 * GtkWindow catches keybindings for the menu items _before_ passing them to
 * the focused widget. This is unfortunate and means that pressing ctrl+V
 * in an entry on a panel ends up pasting text in the TextView.
 * Here we override GtkWindow's handler to do the same things that it
 * does, but in the opposite order and then we chain up to the grand
 * parent handler, skipping gtk_window_key_press_event.
 */
static gboolean anjuta_app_key_press_event(GtkWidget *widget, GdkEventKey *event) {
    static gpointer grand_parent_class = NULL;
    GtkWindow *window = GTK_WINDOW(widget);
    gboolean handled = FALSE;

    if (grand_parent_class == NULL)
        grand_parent_class = g_type_class_peek_parent(parent_class);

    /* handle focus widget key events */
    if (!handled)
        handled = gtk_window_propagate_key_event(window, event);

    /* handle mnemonics and accelerators */
    if (!handled)
        handled = gtk_window_activate_key(window, event);

    /* Chain up, invokes binding set */
    if (!handled)
        handled = GTK_WIDGET_CLASS(grand_parent_class)->key_press_event(widget, event);

    return handled;
}

static void anjuta_app_class_init(AnjutaAppClass *class) {
    GObjectClass *object_class;
    GtkWidgetClass *widget_class;

    parent_class = g_type_class_peek_parent(class);
    object_class = (GObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;

    object_class->finalize = anjuta_app_finalize;
    object_class->dispose = anjuta_app_dispose;

    widget_class->key_press_event = anjuta_app_key_press_event;
}

GtkWidget *
anjuta_app_new(void) {
    AnjutaApp *app;

    app = ANJUTA_APP (g_object_new (ANJUTA_TYPE_APP,
                    "title", "gtkpod",
                    NULL));
    return GTK_WIDGET (app);
}

gchar*
anjuta_app_get_geometry(AnjutaApp *app) {
    gchar *geometry;
    gint width, height, posx, posy;

    g_return_val_if_fail (ANJUTA_IS_APP (app), NULL);

    geometry = NULL;
    width = height = posx = posy = 0;
    if (gtk_widget_get_window(GTK_WIDGET (app))) {
        gtk_window_get_size(GTK_WINDOW (app), &width, &height);
        gtk_window_get_position(GTK_WINDOW(app), &posx, &posy);

        geometry = g_strdup_printf("%dx%d+%d+%d", width, height, posx, posy);
    }
    return geometry;
}

void anjuta_app_set_geometry(AnjutaApp *app, const gchar *geometry) {
    gint width, height, posx, posy;
    gboolean geometry_set = FALSE;

    if (geometry && strlen(geometry) > 0) {
        DEBUG_PRINT("Setting geometry: %s", geometry);

        if (sscanf(geometry, "%dx%d+%d+%d", &width, &height, &posx, &posy) == 4) {
            if (gtk_widget_get_realized (GTK_WIDGET (app))) {
                gtk_window_resize(GTK_WINDOW (app), width, height);
            }
            else {
                gtk_window_set_default_size(GTK_WINDOW (app), width, height);
                gtk_window_move(GTK_WINDOW (app), posx, posy);
            }
            geometry_set = TRUE;
        }
        else {
            g_warning ("Failed to parse geometry: %s", geometry);
        }
    }
    if (!geometry_set) {
        posx = 10;
        posy = 10;
        width = gdk_screen_width() - 10;
        height = gdk_screen_height() - 25;
        width = (width < 790) ? width : 790;
        height = (height < 575) ? width : 575;
        if (gtk_widget_get_realized (GTK_WIDGET (app)) == FALSE) {
            gtk_window_set_default_size(GTK_WINDOW (app), width, height);
            gtk_window_move(GTK_WINDOW (app), posx, posy);
        }
    }
}

static void anjuta_app_layout_save(AnjutaApp *app, const gchar *filename, const gchar *name) {
    g_return_if_fail (ANJUTA_IS_APP (app));
    g_return_if_fail (filename != NULL);

    /* If maximized, the layout should be loaded from the back-up first */
    if (app->maximized)
        gdl_dock_layout_load_layout(app->layout_manager, "back-up");

    /* Continue with the saving */
    gdl_dock_layout_save_layout(app->layout_manager, name);
    if (!gdl_dock_layout_save_to_file(app->layout_manager, filename))
        g_warning ("Saving dock layout to '%s' failed!", filename);

    /* This is a good place to save the accels too */
    anjuta_ui_save_accels(NULL);
}

static void anjuta_app_layout_load(AnjutaApp *app, const gchar *layout_filename, const gchar *name) {
    g_return_if_fail (ANJUTA_IS_APP (app));

    if (!layout_filename || !gdl_dock_layout_load_from_file(app->layout_manager, layout_filename)) {
        gchar *filename;

        filename = g_build_filename(get_data_dir(), "dock-layout.xml", NULL);
        DEBUG_PRINT("Layout = %s", filename);
        if (!gdl_dock_layout_load_from_file(app->layout_manager, filename))
            g_warning ("Loading layout from '%s' failed!!", filename);
        g_free(filename);
    }

    if (!gdl_dock_layout_load_layout(app->layout_manager, name))
        g_warning ("Loading layout failed!!");
}

void anjuta_app_layout_reset(AnjutaApp *app) {
    anjuta_app_layout_load(app, NULL, NULL);
}

void anjuta_app_install_preferences(AnjutaApp *app) {
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    GtkWidget *notebook, *splash_toggle, *shortcuts, *plugins, *remember_plugins;

    /* Create preferences page */
    gchar *glade_path = g_build_filename(get_glade_dir(), CORE_GTKPOD_XML, NULL);
    gtk_builder_add_from_file(builder, glade_path, &error);
    g_free(glade_path);
    if (error) {
        g_warning("Could not load general preferences: %s", error->message);
        g_error_free(error);
        return;
    }

    splash_toggle = GTK_WIDGET(gtk_builder_get_object(builder, "preferences_disable_splash_screen"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(splash_toggle), prefs_get_int(DISABLE_SPLASH_SCREEN));

    gtk_builder_connect_signals(builder, NULL);

    gchar *icon_path = g_build_filename(get_icon_dir(), ICON_FILE, NULL);
    anjuta_preferences_add_from_builder(app->preferences, builder, app->settings, "General", _("General"), ICON_FILE);
    g_free(icon_path);

    notebook = GTK_WIDGET(gtk_builder_get_object(builder, "General"));
    shortcuts = anjuta_ui_get_accel_editor(ANJUTA_UI(app->ui));
    plugins = anjuta_plugin_manager_get_plugins_page(app->plugin_manager);
    remember_plugins = anjuta_plugin_manager_get_remembered_plugins_page(app->plugin_manager);

    gtk_widget_show(shortcuts);
    gtk_widget_show(plugins);
    gtk_widget_show(remember_plugins);

    gtk_notebook_append_page(GTK_NOTEBOOK (notebook), plugins, gtk_label_new(_("Installed plugins")));
    gtk_notebook_append_page(GTK_NOTEBOOK (notebook), remember_plugins, gtk_label_new(_("Preferred plugins")));
    gtk_notebook_append_page(GTK_NOTEBOOK (notebook), shortcuts, gtk_label_new(_("Shortcuts")));

    g_object_unref(builder);
}

/* AnjutaShell Implementation */

static void on_value_removed_from_hash(gpointer value) {
    g_value_unset((GValue*) value);
    g_free(value);
}

static void anjuta_app_add_value(AnjutaShell *shell, const char *name, const GValue *value, GError **error) {
    GValue *copy;
    AnjutaApp *app;

    g_return_if_fail (ANJUTA_IS_APP (shell));
    g_return_if_fail (name != NULL);
    g_return_if_fail (G_IS_VALUE(value));

    app = ANJUTA_APP (shell);

    if (app->values == NULL) {
        app->values = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, on_value_removed_from_hash);
    }
    anjuta_shell_remove_value(shell, name, error);

    copy = g_new0 (GValue, 1);
    g_value_init(copy, value->g_type);
    g_value_copy(value, copy);

    g_hash_table_insert(app->values, g_strdup(name), copy);
    g_signal_emit_by_name(shell, "value_added", name, copy);
}

static void anjuta_app_get_value(AnjutaShell *shell, const char *name, GValue *value, GError **error) {
    GValue *val;
    AnjutaApp *app;

    g_return_if_fail (ANJUTA_IS_APP (shell));
    g_return_if_fail (name != NULL);
    /* g_return_if_fail (G_IS_VALUE (value)); */

    app = ANJUTA_APP (shell);

    val = NULL;
    if (app->values)
        val = g_hash_table_lookup(app->values, name);
    if (val) {
        if (!value->g_type) {
            g_value_init(value, val->g_type);
        }
        g_value_copy(val, value);
    }
    else {
        if (error) {
            *error = g_error_new(ANJUTA_SHELL_ERROR, ANJUTA_SHELL_ERROR_DOESNT_EXIST, _("Value doesn't exist"));
        }
    }
}

static void anjuta_app_remove_value(AnjutaShell *shell, const char *name, GError **error) {
    AnjutaApp *app;
    GValue *value;
    char *key;

    g_return_if_fail (ANJUTA_IS_APP (shell));
    g_return_if_fail (name != NULL);

    app = ANJUTA_APP (shell);

    /*
     g_return_if_fail (app->values != NULL);
     if (app->widgets && g_hash_table_lookup_extended (app->widgets, name,
     (gpointer*)&key,
     (gpointer*)&w)) {
     GtkWidget *item;
     item = g_object_get_data (G_OBJECT (w), "dockitem");
     gdl_dock_item_hide_item (GDL_DOCK_ITEM (item));
     gdl_dock_object_unbind (GDL_DOCK_OBJECT (item));
     g_free (key);
     }
     */

    if (app->values && g_hash_table_lookup_extended(app->values, name, (gpointer) &key, (gpointer) &value)) {
        g_signal_emit_by_name(app, "value_removed", name);
        g_hash_table_remove(app->values, name);
    }
}

static void anjuta_app_saving_push(AnjutaShell* shell) {
    AnjutaApp* app = ANJUTA_APP (shell);
    app->save_count++;
}

static void anjuta_app_saving_pop(AnjutaShell* shell) {
    AnjutaApp* app = ANJUTA_APP (shell);
    app->save_count--;
}

static gboolean remove_from_widgets_hash(gpointer name, gpointer hash_widget, gpointer widget) {
    if (hash_widget == widget)
        return TRUE;
    return FALSE;
}

static void on_widget_destroy(GtkWidget *widget, AnjutaApp *app) {
    DEBUG_PRINT("%s", "Widget about to be destroyed");
    g_hash_table_foreach_remove(app->widgets, remove_from_widgets_hash, widget);
}

static void on_widget_remove(GtkWidget *container, GtkWidget *widget, AnjutaApp *app) {
    GtkWidget *dock_item;

    dock_item = g_object_get_data(G_OBJECT (widget), "dockitem");
    if (dock_item) {
        gchar* unique_name = g_object_get_data(G_OBJECT(dock_item), "unique_name");
        g_free(unique_name);
        g_signal_handlers_disconnect_by_func (G_OBJECT (dock_item),
                G_CALLBACK (on_widget_remove), app);
        gdl_dock_item_unbind(GDL_DOCK_ITEM(dock_item));
    }
    if (g_hash_table_foreach_remove(app->widgets, remove_from_widgets_hash, widget)) {
        DEBUG_PRINT("%s", "Widget removed from container");
    }
}

static void on_widget_removed_from_hash(gpointer widget) {
    AnjutaApp *app;
    GtkWidget *menuitem;
    GdlDockItem *dockitem;

    DEBUG_PRINT("%s", "Removing widget from hash");

    app = g_object_get_data(G_OBJECT (widget), "app-object");
    dockitem = g_object_get_data(G_OBJECT (widget), "dockitem");
    menuitem = g_object_get_data(G_OBJECT (widget), "menuitem");

    gtk_widget_destroy(menuitem);

    g_object_set_data(G_OBJECT(widget), "dockitem", NULL);
    g_object_set_data(G_OBJECT(widget), "menuitem", NULL);

    g_signal_handlers_disconnect_by_func(G_OBJECT(widget), G_CALLBACK(on_widget_destroy), app);
    g_signal_handlers_disconnect_by_func(G_OBJECT(dockitem), G_CALLBACK(on_widget_remove), app);

    g_object_unref(G_OBJECT(widget));
}

static void anjuta_app_setup_widget(AnjutaApp* app, const gchar* name, GtkWidget *widget, GtkWidget* item, const gchar* title, gboolean locked) {
    GtkCheckMenuItem* menuitem;

    /* Add the widget to hash */
    if (app->widgets == NULL) {
        app->widgets = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, on_widget_removed_from_hash);
    }
    g_hash_table_insert(app->widgets, g_strdup(name), widget);
    g_object_ref(widget);

    /* Add toggle button for the widget */
    menuitem = GTK_CHECK_MENU_ITEM(gtk_check_menu_item_new_with_label(title));
    gtk_widget_show(GTK_WIDGET(menuitem));
    gtk_check_menu_item_set_active(menuitem, TRUE);
    gtk_menu_shell_append(GTK_MENU_SHELL(app->view_menu), GTK_WIDGET(menuitem));

    if (locked)
        g_object_set(G_OBJECT(menuitem), "visible", FALSE, NULL);

    g_object_set_data(G_OBJECT(widget), "app-object", app);
    g_object_set_data(G_OBJECT(widget), "menuitem", menuitem);
    g_object_set_data(G_OBJECT(widget), "dockitem", item);

    /* For toggling widget view on/off */
    g_signal_connect(G_OBJECT(menuitem), "toggled", G_CALLBACK(on_toggle_widget_view), item);

    /*
     Watch for widget removal/destruction so that it could be
     removed from widgets hash.
     */
    g_signal_connect(G_OBJECT(item), "remove", G_CALLBACK(on_widget_remove), app);
    g_signal_connect_after(G_OBJECT(widget), "destroy", G_CALLBACK(on_widget_destroy), app);

    gtk_widget_show(item);
}

static void anjuta_app_add_widget_full(AnjutaShell *shell, GtkWidget *widget, const char *name, const char *title, const char *stock_id, AnjutaShellPlacement placement, gboolean locked, GError **error) {
    AnjutaApp *app;
    GtkWidget *item;

    g_return_if_fail(ANJUTA_IS_APP (shell));
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(name != NULL);
    g_return_if_fail(title != NULL);

    app = ANJUTA_APP (shell);

    /* Add the widget to dock */
    if (stock_id == NULL)
        item = gdl_dock_item_new(name, title, GDL_DOCK_ITEM_BEH_NORMAL);
    else
        item = gdl_dock_item_new_with_stock(name, title, stock_id, GDL_DOCK_ITEM_BEH_NORMAL);
    if (locked) {
        guint flags = 0;
        flags |= GDL_DOCK_ITEM_BEH_NEVER_FLOATING;
        flags |= GDL_DOCK_ITEM_BEH_CANT_CLOSE;
        flags |= GDL_DOCK_ITEM_BEH_CANT_ICONIFY;
        flags |= GDL_DOCK_ITEM_BEH_NO_GRIP;
        g_object_set(G_OBJECT(item), "behavior", flags, NULL);
    }

    gtk_container_add(GTK_CONTAINER (item), widget);
    gdl_dock_add_item(GDL_DOCK (app->dock), GDL_DOCK_ITEM (item), placement);

    if (locked)
        gdl_dock_item_set_default_position(GDL_DOCK_ITEM(item), GDL_DOCK_OBJECT(app->dock));

    anjuta_app_setup_widget(app, name, widget, item, title, locked);
}

static void anjuta_app_add_widget_custom(AnjutaShell *shell, GtkWidget *widget, const char *name, const char *title, const char *stock_id, GtkWidget *label, AnjutaShellPlacement placement, GError **error) {
    AnjutaApp *app;
    GtkWidget *item;
    GtkWidget *grip;

    g_return_if_fail(ANJUTA_IS_APP (shell));
    g_return_if_fail(GTK_IS_WIDGET(widget));
    g_return_if_fail(name != NULL);
    g_return_if_fail(title != NULL);

    app = ANJUTA_APP (shell);

    /* Add the widget to dock */
    /* Add the widget to dock */
    if (stock_id == NULL)
        item = gdl_dock_item_new(name, title, GDL_DOCK_ITEM_BEH_NORMAL);
    else
        item = gdl_dock_item_new_with_stock(name, title, stock_id, GDL_DOCK_ITEM_BEH_NORMAL);

    gtk_container_add(GTK_CONTAINER(item), widget);
    gdl_dock_add_item(GDL_DOCK(app->dock), GDL_DOCK_ITEM(item), placement);

    grip = gdl_dock_item_get_grip(GDL_DOCK_ITEM(item));

    gdl_dock_item_grip_set_label(GDL_DOCK_ITEM_GRIP(grip), label);

    anjuta_app_setup_widget(app, name, widget, item, title, FALSE);
}

static void anjuta_app_remove_widget(AnjutaShell *shell, GtkWidget *widget, GError **error) {
    AnjutaApp *app;
    GtkWidget *dock_item;

    g_return_if_fail (ANJUTA_IS_APP (shell));
    g_return_if_fail (GTK_IS_WIDGET (widget));

    app = ANJUTA_APP (shell);

    g_return_if_fail (app->widgets != NULL);

    dock_item = g_object_get_data(G_OBJECT (widget), "dockitem");
    g_return_if_fail (dock_item != NULL);

    /* Remove the widget from container */
    g_object_ref(widget);
    /* It should call on_widget_remove() and clean up should happen */
    gtk_container_remove(GTK_CONTAINER (dock_item), widget);
    g_object_unref(widget);
}

static void anjuta_app_present_widget(AnjutaShell *shell, GtkWidget *widget, GError **error) {
    AnjutaApp *app;
    GdlDockItem *dock_item;
    GtkWidget *parent;

    g_return_if_fail (ANJUTA_IS_APP (shell));
    g_return_if_fail (GTK_IS_WIDGET (widget));

    app = ANJUTA_APP (shell);

    g_return_if_fail (app->widgets != NULL);

    dock_item = g_object_get_data(G_OBJECT(widget), "dockitem");
    g_return_if_fail (dock_item != NULL);

    /* Hack to present the dock item if it's in a notebook dock item */
    parent = gtk_widget_get_parent(GTK_WIDGET(dock_item));
    if (GTK_IS_NOTEBOOK (parent)) {
        gint pagenum;
        pagenum = gtk_notebook_page_num(GTK_NOTEBOOK (parent), GTK_WIDGET (dock_item));
        gtk_notebook_set_current_page(GTK_NOTEBOOK (parent), pagenum);
    }
    else if (!GDL_DOCK_OBJECT_ATTACHED (dock_item)) {
        gdl_dock_item_show_item(GDL_DOCK_ITEM (dock_item));
    }

    /* FIXME: If the item is floating, present the window */
    /* FIXME: There is no way to detect if a widget was floating before it was
     detached since it no longer has a parent there is no way to access the
     floating property of the GdlDock structure.*/
}

static GObject*
anjuta_app_get_object(AnjutaShell *shell, const char *iface_name, GError **error) {
    g_return_val_if_fail (ANJUTA_IS_APP (shell), NULL);
    g_return_val_if_fail (iface_name != NULL, NULL);
    return anjuta_plugin_manager_get_plugin(ANJUTA_APP (shell)->plugin_manager, iface_name);
}

static AnjutaStatus*
anjuta_app_get_status(AnjutaShell *shell, GError **error) {
    g_return_val_if_fail (ANJUTA_IS_APP (shell), NULL);
    return ANJUTA_APP (shell)->status;
}

static AnjutaUI *
anjuta_app_get_ui(AnjutaShell *shell, GError **error) {
    g_return_val_if_fail (ANJUTA_IS_APP (shell), NULL);
    return ANJUTA_APP (shell)->ui;
}

static AnjutaPreferences *
anjuta_app_get_preferences(AnjutaShell *shell, GError **error) {
    g_return_val_if_fail (ANJUTA_IS_APP (shell), NULL);
    return ANJUTA_APP (shell)->preferences;
}

static AnjutaPluginManager *
anjuta_app_get_plugin_manager(AnjutaShell *shell, GError **error) {
    g_return_val_if_fail (ANJUTA_IS_APP (shell), NULL);
    return ANJUTA_APP (shell)->plugin_manager;
}

static AnjutaProfileManager *
anjuta_app_get_profile_manager(AnjutaShell *shell, GError **error) {
    g_return_val_if_fail (ANJUTA_IS_APP (shell), NULL);
    return ANJUTA_APP (shell)->profile_manager;
}

static void anjuta_shell_iface_init(AnjutaShellIface *iface) {
    iface->add_widget_full = anjuta_app_add_widget_full;
    iface->add_widget_custom = anjuta_app_add_widget_custom;
    iface->remove_widget = anjuta_app_remove_widget;
    iface->present_widget = anjuta_app_present_widget;
    iface->iconify_dockable_widget = anjuta_app_iconify_dockable_widget;
    iface->hide_dockable_widget = anjuta_app_hide_dockable_widget;
    iface->show_dockable_widget = anjuta_app_show_dockable_widget;
    iface->maximize_widget = anjuta_app_maximize_widget;
    iface->unmaximize = anjuta_app_unmaximize;
    iface->add_value = anjuta_app_add_value;
    iface->get_value = anjuta_app_get_value;
    iface->remove_value = anjuta_app_remove_value;
    iface->get_object = anjuta_app_get_object;
    iface->get_status = anjuta_app_get_status;
    iface->get_ui = anjuta_app_get_ui;
    iface->get_preferences = anjuta_app_get_preferences;
    iface->get_plugin_manager = anjuta_app_get_plugin_manager;
    iface->get_profile_manager = anjuta_app_get_profile_manager;
    iface->saving_push = anjuta_app_saving_push;
    iface->saving_pop = anjuta_app_saving_pop;
}

/*
 * -------------------------------------------------------------------------
 * --       GtkPodAppInterface implementations                  --
 * -------------------------------------------------------------------------
 */

static void anjuta_gtkpod_statusbar_reset_progress(GtkPodApp *obj, gint total) {
    g_return_if_fail(ANJUTA_IS_APP(gtkpod_app));
    AnjutaStatus *status = anjuta_shell_get_status(ANJUTA_SHELL(gtkpod_app), NULL);
    anjuta_status_progress_reset(status);
    anjuta_status_progress_add_ticks(status, total);
}

static void anjuta_gtkpod_statusbar_increment_progress_ticks(GtkPodApp *obj, gint ticks, gchar* text) {
    g_return_if_fail(ANJUTA_IS_APP(gtkpod_app));
    AnjutaStatus *status = anjuta_shell_get_status(ANJUTA_SHELL(gtkpod_app), NULL);
    anjuta_status_progress_increment_ticks(status, ticks, text);
}

static void anjuta_gtkpod_app_statusbar_message(GtkPodApp *gtkpod_app, gchar* message, ...) {
    g_return_if_fail(ANJUTA_IS_APP(gtkpod_app));

    gchar* msg;
    va_list args;
    va_start (args, message);
    msg = g_strdup_vprintf(message, args);
    va_end (args);

    AnjutaStatus *status = anjuta_shell_get_status(ANJUTA_SHELL(gtkpod_app), NULL);
    anjuta_status_set(status, "%s", msg);
    g_free(msg);
}

static void anjuta_gtkpod_app_statusbar_busy_push(GtkPodApp *gtkpod_app) {
    g_return_if_fail(ANJUTA_IS_APP(gtkpod_app));
    AnjutaStatus *status = anjuta_shell_get_status(ANJUTA_SHELL(gtkpod_app), NULL);
    anjuta_status_busy_push(status);
}

static void anjuta_gtkpod_app_statusbar_busy_pop(GtkPodApp *gtkpod_app) {
    g_return_if_fail(ANJUTA_IS_APP(gtkpod_app));
    AnjutaStatus *status = anjuta_shell_get_status(ANJUTA_SHELL(gtkpod_app), NULL);
    anjuta_status_busy_pop(status);
}

static void anjuta_gtkpod_app_warning(GtkPodApp *gtkpod_app, gchar *message, ...) {
    g_return_if_fail(GTK_IS_WINDOW(gtkpod_app));
    gchar* msg;
    va_list args;
    va_start (args, message);
    msg = g_strdup_vprintf(message, args);
    va_end (args);

    anjuta_util_dialog_warning(GTK_WINDOW(gtkpod_app), "%s", msg);
    g_free(msg);
}

static void anjuta_gtkpod_app_warning_hig(GtkPodApp *gtkpod_app, GtkMessageType icon, const gchar *primary_text, const gchar *secondary_text) {
    GtkWidget *dialog =
            gtk_message_dialog_new(GTK_WINDOW(gtkpod_app), GTK_DIALOG_MODAL, icon, GTK_BUTTONS_OK, "%s", primary_text);

    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), "%s", secondary_text);

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}

static gint anjuta_gtkpod_app_confirmation_hig(GtkPodApp *gtkpod_app, GtkMessageType icon, const gchar *primary_text, const gchar *secondary_text, const gchar *accept_button_text, const gchar *cancel_button_text, const gchar *third_button_text, const gchar *help_context) {
    g_return_val_if_fail(GTK_IS_WINDOW(gtkpod_app), GTK_RESPONSE_CANCEL);

    gint result;

    GtkWidget
            *dialog =
                    gtk_message_dialog_new(GTK_WINDOW(gtkpod_app), GTK_DIALOG_MODAL, icon, GTK_BUTTONS_NONE, "%s", primary_text);

    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), "%s", secondary_text);

    if (third_button_text)
        gtk_dialog_add_button(GTK_DIALOG(dialog), third_button_text, GTK_RESPONSE_APPLY);

    gtk_dialog_add_buttons(GTK_DIALOG(dialog), cancel_button_text ? cancel_button_text : GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, accept_button_text ? accept_button_text : GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

    result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    switch (result) {
    case GTK_RESPONSE_OK:
    case GTK_RESPONSE_APPLY:
        return result;
    default:
        return GTK_RESPONSE_CANCEL;
    };
}

static void on_never_again_toggled(GtkToggleButton *t, gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        if (cd->confirm_again_key)
            prefs_set_int(cd->confirm_again_key, !gtk_toggle_button_get_active(t));
    }
}

static void on_option1_toggled(GtkToggleButton *t, gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        if (cd->option1_key) {
            gboolean state = gtk_toggle_button_get_active(t);
            if (cd->option1_invert)
                prefs_set_int(cd->option1_key, !state);
            else
                prefs_set_int(cd->option1_key, state);
        }
    }
}

static void on_option2_toggled(GtkToggleButton *t, gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        if (cd->option2_key) {
            gboolean state = gtk_toggle_button_get_active(t);
            if (cd->option2_invert)
                prefs_set_int(cd->option2_key, !state);
            else
                prefs_set_int(cd->option2_key, state);
        }
    }
}

static void cleanup(gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        gint defx, defy;
        gtk_window_get_size(GTK_WINDOW (cd->window), &defx, &defy);
        if (cd->scrolled) {
            prefs_set_int("size_conf_sw.x", defx);
            prefs_set_int("size_conf_sw.y", defy);
        }
        else {
            prefs_set_int("size_conf.x", defx);
            prefs_set_int("size_conf.y", defy);
        }

        gtk_widget_destroy(cd->window);
        g_free(cd->option1_key);
        g_free(cd->option2_key);
        g_free(cd->confirm_again_key);

        g_hash_table_remove(id_hash, id);
    }
}

static void on_apply_clicked(GtkWidget *w, gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        gtk_widget_set_sensitive(cd->window, FALSE);
        if (cd->apply_handler)
            cd->apply_handler(cd->user_data1, cd->user_data2);
        gtk_widget_set_sensitive(cd->window, TRUE);
    }
}

static void on_ok_clicked(GtkWidget *w, gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        gtk_widget_set_sensitive(cd->window, FALSE);
        if (cd->ok_handler)
            cd->ok_handler(cd->user_data1, cd->user_data2);
        cleanup(id);
    }
}

static void on_cancel_clicked(GtkWidget *w, gpointer id) {
    ConfData *cd;

    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        gtk_widget_set_sensitive(cd->window, FALSE);
        if (cd->cancel_handler)
            cd->cancel_handler(cd->user_data1, cd->user_data2);
        cleanup(id);
    }
}

static void on_response(GtkWidget *w, gint response, gpointer id) {
    ConfData *cd;
    /*     printf ("r: %d, i: %d\n", response, id); */
    cd = g_hash_table_lookup(id_hash, id);
    if (cd) {
        switch (response) {
        case GTK_RESPONSE_OK:
            on_ok_clicked(w, id);
            break;
        case GTK_RESPONSE_NONE:
        case GTK_RESPONSE_CANCEL:
        case GTK_RESPONSE_DELETE_EVENT:
            on_cancel_clicked(w, id);
            break;
        case GTK_RESPONSE_APPLY:
            on_apply_clicked(w, id);
            break;
        default:
            g_warning ("Programming error: response '%d' received in on_response()\n", response);
            on_cancel_clicked(w, id);
            break;
        }
    }
}

static void confirm_append_text(GtkBuilder *builder, const gchar *text) {
    g_return_if_fail(builder);

    int i;
    gchar **strings = g_strsplit(text, "\n", 0);
    GtkTreeIter iter;
    GtkAdjustment *adjustment;
    GtkWidget *w = gtkpod_builder_xml_get_widget(builder, "tree");
    GtkListStore *store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (w)));

    for (i = 0; strings[i]; i++) {
        if (strings[i][0]) {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, strings[i], -1);
        }
    }

    w = gtkpod_builder_xml_get_widget(builder, "scroller");
    adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW (w));
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment)
            - gtk_adjustment_get_page_size(adjustment));

    g_strfreev(strings);
}

static GtkResponseType anjuta_gtkpod_app_confirmation(GtkPodApp *obj, gint id, gboolean modal, const gchar *title, const gchar *label, const gchar *text, const gchar *option1_text, CONF_STATE option1_state, const gchar *option1_key, const gchar *option2_text, CONF_STATE option2_state, const gchar *option2_key, gboolean confirm_again, const gchar *confirm_again_key, ConfHandler ok_handler, ConfHandler apply_handler, ConfHandler cancel_handler, gpointer user_data1, gpointer user_data2) {
    GtkWidget *window, *w;
    ConfData *cd;
    gint defx, defy;
    GtkBuilder *confirm_builder;
    GtkListStore *store;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    gchar *full_label;

    if (id_hash == NULL) { /* initialize hash table to store IDs */
        id_hash = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
    }

    if (id >= 0) {
        if ((cd = g_hash_table_lookup(id_hash, GINT_TO_POINTER(id)))) { /* window with same ID already open -- add @text and return */
            if (text && *text && cd->window) {
                confirm_append_text(cd->window_builder, text);
            }
            return GTK_RESPONSE_REJECT;
        }
    }
    else /* find free ID */
    {
        id = 0;

        do {
            --id;
            cd = g_hash_table_lookup(id_hash, GINT_TO_POINTER(id));
        }
        while (cd != NULL);
    }

    if (!confirm_again) {
        /* This question was supposed to be asked "never again" ("don't
         confirm again" -- so we just call the ok_handler */
        if (ok_handler && !modal)
            ok_handler(user_data1, user_data2);

        if (!modal)
            return GTK_RESPONSE_ACCEPT;

        return GTK_RESPONSE_OK;
    }

    gchar *glade_path = g_build_filename(get_glade_dir(), CORE_GTKPOD_XML, NULL);
    confirm_builder = gtkpod_builder_xml_new(glade_path);
    g_free(glade_path);

    window = gtkpod_builder_xml_get_widget(confirm_builder, "confirm_dialog");
    gtk_builder_connect_signals(confirm_builder, NULL);

    /* insert ID into hash table */
    cd = g_new0 (ConfData, 1);
    cd->window = window;
    cd->window_builder = confirm_builder;
    cd->option1_key = g_strdup(option1_key);
    cd->option2_key = g_strdup(option2_key);
    cd->confirm_again_key = g_strdup(confirm_again_key);
    cd->ok_handler = ok_handler;
    cd->apply_handler = apply_handler;
    cd->cancel_handler = cancel_handler;
    cd->user_data1 = user_data1;
    cd->user_data2 = user_data2;
    g_hash_table_insert(id_hash, GINT_TO_POINTER(id), cd);

    full_label
            = g_markup_printf_escaped("<span weight='bold' size='larger'>%s</span>\n\n%s", title ? title : _("Confirmation"), label ? label : "");

    /* Set label */
    w = gtkpod_builder_xml_get_widget(confirm_builder, "label");
    gtk_label_set_markup(GTK_LABEL(w), full_label);
    g_free(full_label);

    /* Set text */
    w = gtkpod_builder_xml_get_widget(confirm_builder, "tree");
    store = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW (w), GTK_TREE_MODEL (store));
    g_object_unref(store);

    column = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_text_new();

    /*
     gint mode, width;

     g_object_get (G_OBJECT (renderer),
     "wrap-mode", &mode,
     "wrap-width", &width,
     NULL);

     printf ("wrap-mode: %d wrap-width:%d\n", mode, width);
     */

    /* I have no idea why 400, but neither 1000 nor 0 result in the
     right behavior... */
    g_object_set(G_OBJECT (renderer), "wrap-mode", PANGO_WRAP_WORD_CHAR, "wrap-width", 400, NULL);

    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    g_object_set_data(G_OBJECT (w), "renderer", renderer);

    gtk_tree_view_append_column(GTK_TREE_VIEW (w), column);

    if (text) {
        confirm_append_text(confirm_builder, text);
        cd->scrolled = TRUE;
        defx = prefs_get_int("size_conf_sw.x");
        defy = prefs_get_int("size_conf_sw.y");
    }
    else {
        /* no text -> hide widget */
        if ((w = gtkpod_builder_xml_get_widget(confirm_builder, "scroller")))
            gtk_widget_hide(w);

        cd->scrolled = FALSE;
        defx = prefs_get_int("size_conf.x");
        defy = prefs_get_int("size_conf.y");
    }

    gtk_widget_set_size_request(GTK_WIDGET (window), defx, defy);

    /* Set "Option 1" checkbox */
    w = gtkpod_builder_xml_get_widget(confirm_builder, "option_vbox");

    if (w && option1_key && option1_text) {
        gboolean state, invert;
        GtkWidget *option1_button = gtk_check_button_new_with_mnemonic(option1_text);

        state = ((option1_state == CONF_STATE_INVERT_TRUE) || (option1_state == CONF_STATE_TRUE));
        invert = ((option1_state == CONF_STATE_INVERT_FALSE) || (option1_state == CONF_STATE_INVERT_TRUE));
        cd->option1_invert = invert;

        gtk_widget_show(option1_button);
        gtk_box_pack_start(GTK_BOX (w), option1_button, FALSE, FALSE, 0);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option1_button), state);
        g_signal_connect ((gpointer)option1_button,
                "toggled",
                G_CALLBACK (on_option1_toggled),
                GINT_TO_POINTER(id));
    }

    /* Set "Option 2" checkbox */
    w = gtkpod_builder_xml_get_widget(confirm_builder, "option_vbox");
    if (w && option2_key && option2_text) {
        gboolean state, invert;
        GtkWidget *option2_button = gtk_check_button_new_with_mnemonic(option2_text);

        state = ((option2_state == CONF_STATE_INVERT_TRUE) || (option2_state == CONF_STATE_TRUE));
        invert = ((option2_state == CONF_STATE_INVERT_FALSE) || (option2_state == CONF_STATE_INVERT_TRUE));
        cd->option2_invert = invert;

        gtk_widget_show(option2_button);
        gtk_box_pack_start(GTK_BOX (w), option2_button, FALSE, FALSE, 0);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(option2_button), state);
        g_signal_connect ((gpointer)option2_button,
                "toggled",
                G_CALLBACK (on_option2_toggled),
                GINT_TO_POINTER(id));
    }

    /* Set "Never Again" checkbox */
    w = gtkpod_builder_xml_get_widget(confirm_builder, "never_again");

    if (w && confirm_again_key) {
        /* connect signal */
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), !confirm_again);
        g_signal_connect ((gpointer)w,
                "toggled",
                G_CALLBACK (on_never_again_toggled),
                GINT_TO_POINTER(id));
    }
    else if (w) {
        /* hide "never again" button */
        gtk_widget_hide(w);
    }

    /* Hide and set "default" button that can be activated by pressing
     ENTER in the window (usually OK)*/
    /* Hide or default CANCEL button */
    if ((w = gtkpod_builder_xml_get_widget(confirm_builder, "cancel"))) {
        gtk_widget_set_can_default(w, TRUE);
        gtk_widget_grab_default(w);

        if (!cancel_handler)
            gtk_widget_hide(w);
    }

    /* Hide or default APPLY button */
    if ((w = gtkpod_builder_xml_get_widget(confirm_builder, "apply"))) {
        gtk_widget_set_can_default(w, TRUE);
        gtk_widget_grab_default(w);

        if (!apply_handler)
            gtk_widget_hide(w);
    }

    /* Hide or default OK button */
    if ((w = gtkpod_builder_xml_get_widget(confirm_builder, "ok"))) {
        gtk_widget_set_can_default(w, TRUE);
        gtk_widget_grab_default(w);

        if (!ok_handler)
            gtk_widget_hide(w);
    }

    /* Connect Close window */
    g_signal_connect (G_OBJECT (window),
            "delete_event",
            G_CALLBACK (on_cancel_clicked),
            GINT_TO_POINTER(id));

    /* Stay on top of main window */
    gtk_window_set_transient_for(GTK_WINDOW (window), GTK_WINDOW (gtkpod_app));

    if (modal) {
        /* use gtk_dialog_run() to block the application */
        gint response = gtk_dialog_run(GTK_DIALOG (window));
        /* cleanup hash, store window size */
        cleanup(GINT_TO_POINTER(id));

        switch (response) {
        case GTK_RESPONSE_OK:
        case GTK_RESPONSE_APPLY:
            return response;
        default:
            return GTK_RESPONSE_CANCEL;
        }
    }
    else {
        /* Make sure we catch the response */
        g_signal_connect (G_OBJECT (window),
                "response",
                G_CALLBACK (on_response),
                GINT_TO_POINTER(id));
        gtk_widget_show(window);

        return GTK_RESPONSE_ACCEPT;
    }
}

static void anjuta_gtkpod_app_display_widget(GtkPodApp *obj, GtkWidget *widget) {
    g_return_if_fail(widget);
    g_return_if_fail(ANJUTA_IS_APP(gtkpod_app));

    anjuta_app_show_dockable_widget(ANJUTA_SHELL(gtkpod_app), widget, NULL);
}

static void gtkpod_app_iface_init(GtkPodAppInterface *iface) {
    iface->statusbar_reset_progress = anjuta_gtkpod_statusbar_reset_progress;
    iface->statusbar_increment_progress_ticks = anjuta_gtkpod_statusbar_increment_progress_ticks;
    iface->statusbar_message = anjuta_gtkpod_app_statusbar_message;
    iface->statusbar_busy_push = anjuta_gtkpod_app_statusbar_busy_push;
    iface->statusbar_busy_pop = anjuta_gtkpod_app_statusbar_busy_pop;
    iface->gtkpod_warning = anjuta_gtkpod_app_warning;
    iface->gtkpod_warning_hig = anjuta_gtkpod_app_warning_hig;
    iface->gtkpod_confirmation_hig = anjuta_gtkpod_app_confirmation_hig;
    iface->gtkpod_confirmation = anjuta_gtkpod_app_confirmation;
    iface->display_widget = anjuta_gtkpod_app_display_widget;
    iface->export_tracks_as_gchar = NULL;
    iface->export_tracks_as_glist = NULL;
    iface->repository_editor = NULL;
    iface->details_editor = NULL;
}

G_MODULE_EXPORT void on_confirm_tree_size_allocate(GtkWidget *sender, GtkAllocation *allocation, gpointer e) {
    GtkCellRenderer *renderer = GTK_CELL_RENDERER (g_object_get_data (G_OBJECT (sender), "renderer"));
    g_object_set(renderer, "wrap-width", allocation->width, NULL);
}

G_MODULE_EXPORT void on_disable_splash_screen_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    prefs_set_int(DISABLE_SPLASH_SCREEN, gtk_toggle_button_get_active(togglebutton));
}

ANJUTA_TYPE_BEGIN(AnjutaApp, anjuta_app, GTK_TYPE_WINDOW);
ANJUTA_TYPE_ADD_INTERFACE( anjuta_shell, ANJUTA_TYPE_SHELL);
ANJUTA_TYPE_ADD_INTERFACE( gtkpod_app, GTKPOD_APP_TYPE);
ANJUTA_TYPE_END;
