/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_gtkpod (void)
{
  GtkWidget *gtkpod;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *menuitem10;
  GtkWidget *menuitem10_menu;
  GtkWidget *import_itunes1;
  GtkWidget *add_files1;
  GtkWidget *add_directory1;
  GtkWidget *export_itunes1;
  GtkWidget *separatormenuitem1;
  GtkWidget *quit1;
  GtkWidget *menuitem11;
  GtkWidget *menuitem11_menu;
  GtkWidget *new_playlist1;
  GtkWidget *image1;
  GtkWidget *separator1;
  GtkWidget *cut1;
  GtkWidget *copy1;
  GtkWidget *paste1;
  GtkWidget *delete1;
  GtkWidget *menuitem12;
  GtkWidget *menuitem13;
  GtkWidget *menuitem13_menu;
  GtkWidget *about1;
  GtkWidget *toolbar2;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;
  GtkWidget *button4;
  GtkWidget *button5;
  GtkWidget *hpaned1;
  GtkWidget *scrolledwindow8;
  GtkWidget *playlist_treeview;
  GtkWidget *vpaned1;
  GtkWidget *hpaned2;
  GtkWidget *sorttab0;
  GtkWidget *scrolledwindow9;
  GtkWidget *st0_cat0_treeview;
  GtkWidget *label8;
  GtkWidget *scrolledwindow12;
  GtkWidget *st0_cat1_treeview;
  GtkWidget *label9;
  GtkWidget *scrolledwindow13;
  GtkWidget *st0_cat2_treeview;
  GtkWidget *label10;
  GtkWidget *scrolledwindow16;
  GtkWidget *st0_cat3_treeview;
  GtkWidget *label14;
  GtkWidget *sorttab1;
  GtkWidget *scrolledwindow15;
  GtkWidget *st1_cat0_treeview;
  GtkWidget *label11;
  GtkWidget *scrolledwindow14;
  GtkWidget *st1_cat1_treeview;
  GtkWidget *label12;
  GtkWidget *scrolledwindow10;
  GtkWidget *st1_cat2_treeview;
  GtkWidget *label13;
  GtkWidget *scrolledwindow17;
  GtkWidget *st1_cat3_treeview;
  GtkWidget *label15;
  GtkWidget *scrolledwindow11;
  GtkWidget *song_treeview;
  GtkWidget *statusbar1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  gtkpod = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (gtkpod), _("gtkpod"));
  gtk_window_set_default_size (GTK_WINDOW (gtkpod), 600, 500);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (gtkpod), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  menuitem10 = gtk_menu_item_new_with_mnemonic (_("_File"));
  gtk_widget_show (menuitem10);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem10);

  menuitem10_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem10), menuitem10_menu);

  import_itunes1 = gtk_menu_item_new_with_mnemonic (_("_Import iTunes"));
  gtk_widget_show (import_itunes1);
  gtk_container_add (GTK_CONTAINER (menuitem10_menu), import_itunes1);

  add_files1 = gtk_menu_item_new_with_mnemonic (_("Add _Files"));
  gtk_widget_show (add_files1);
  gtk_container_add (GTK_CONTAINER (menuitem10_menu), add_files1);

  add_directory1 = gtk_menu_item_new_with_mnemonic (_("Add _Directory"));
  gtk_widget_show (add_directory1);
  gtk_container_add (GTK_CONTAINER (menuitem10_menu), add_directory1);

  export_itunes1 = gtk_menu_item_new_with_mnemonic (_("_Export iTunes"));
  gtk_widget_show (export_itunes1);
  gtk_container_add (GTK_CONTAINER (menuitem10_menu), export_itunes1);

  separatormenuitem1 = gtk_menu_item_new ();
  gtk_widget_show (separatormenuitem1);
  gtk_container_add (GTK_CONTAINER (menuitem10_menu), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  quit1 = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  gtk_widget_show (quit1);
  gtk_container_add (GTK_CONTAINER (menuitem10_menu), quit1);

  menuitem11 = gtk_menu_item_new_with_mnemonic (_("_Edit"));
  gtk_widget_show (menuitem11);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem11);

  menuitem11_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem11), menuitem11_menu);

  new_playlist1 = gtk_image_menu_item_new_with_mnemonic (_("_New Playlist"));
  gtk_widget_show (new_playlist1);
  gtk_container_add (GTK_CONTAINER (menuitem11_menu), new_playlist1);

  image1 = gtk_image_new_from_stock ("gtk-justify-left", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image1);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (new_playlist1), image1);

  separator1 = gtk_menu_item_new ();
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (menuitem11_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  cut1 = gtk_image_menu_item_new_from_stock ("gtk-cut", accel_group);
  gtk_widget_show (cut1);
  gtk_container_add (GTK_CONTAINER (menuitem11_menu), cut1);

  copy1 = gtk_image_menu_item_new_from_stock ("gtk-copy", accel_group);
  gtk_widget_show (copy1);
  gtk_container_add (GTK_CONTAINER (menuitem11_menu), copy1);

  paste1 = gtk_image_menu_item_new_from_stock ("gtk-paste", accel_group);
  gtk_widget_show (paste1);
  gtk_container_add (GTK_CONTAINER (menuitem11_menu), paste1);

  delete1 = gtk_image_menu_item_new_from_stock ("gtk-delete", accel_group);
  gtk_widget_show (delete1);
  gtk_container_add (GTK_CONTAINER (menuitem11_menu), delete1);

  menuitem12 = gtk_menu_item_new_with_mnemonic (_("_View"));
  gtk_widget_show (menuitem12);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem12);

  menuitem13 = gtk_menu_item_new_with_mnemonic (_("_Help"));
  gtk_widget_show (menuitem13);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem13);

  menuitem13_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem13), menuitem13_menu);

  about1 = gtk_menu_item_new_with_mnemonic (_("_About"));
  gtk_widget_show (about1);
  gtk_container_add (GTK_CONTAINER (menuitem13_menu), about1);

  toolbar2 = gtk_toolbar_new ();
  gtk_widget_show (toolbar2);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar2, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar2), GTK_TOOLBAR_BOTH);

  tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-goto-top", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar2)));
  button1 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar2),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Import"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar2)->children)->data))->label), TRUE);
  gtk_widget_show (button1);

  tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-add", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar2)));
  button2 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar2),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Files"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar2)->children)->data))->label), TRUE);
  gtk_widget_show (button2);

  tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-add", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar2)));
  button3 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar2),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Dirs"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar2)->children)->data))->label), TRUE);
  gtk_widget_show (button3);

  tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-justify-left", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar2)));
  button4 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar2),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("New PL"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar2)->children)->data))->label), TRUE);
  gtk_widget_show (button4);

  tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-goto-bottom", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar2)));
  button5 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar2),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Export"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar2)->children)->data))->label), TRUE);
  gtk_widget_show (button5);

  hpaned1 = gtk_hpaned_new ();
  gtk_widget_show (hpaned1);
  gtk_box_pack_start (GTK_BOX (vbox1), hpaned1, TRUE, TRUE, 0);
  gtk_paned_set_position (GTK_PANED (hpaned1), 150);

  scrolledwindow8 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow8);
  gtk_paned_pack1 (GTK_PANED (hpaned1), scrolledwindow8, FALSE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow8), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  playlist_treeview = gtk_tree_view_new ();
  gtk_widget_show (playlist_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow8), playlist_treeview);

  vpaned1 = gtk_vpaned_new ();
  gtk_widget_show (vpaned1);
  gtk_paned_pack2 (GTK_PANED (hpaned1), vpaned1, TRUE, TRUE);
  gtk_paned_set_position (GTK_PANED (vpaned1), 180);

  hpaned2 = gtk_hpaned_new ();
  gtk_widget_show (hpaned2);
  gtk_paned_pack1 (GTK_PANED (vpaned1), hpaned2, FALSE, FALSE);

  sorttab0 = gtk_notebook_new ();
  gtk_widget_show (sorttab0);
  gtk_paned_pack1 (GTK_PANED (hpaned2), sorttab0, TRUE, TRUE);

  scrolledwindow9 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow9);
  gtk_container_add (GTK_CONTAINER (sorttab0), scrolledwindow9);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st0_cat0_treeview = gtk_tree_view_new ();
  gtk_widget_show (st0_cat0_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow9), st0_cat0_treeview);

  label8 = gtk_label_new (_("Artist"));
  gtk_widget_show (label8);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab0), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab0), 0), label8);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

  scrolledwindow12 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow12);
  gtk_container_add (GTK_CONTAINER (sorttab0), scrolledwindow12);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow12), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st0_cat1_treeview = gtk_tree_view_new ();
  gtk_widget_show (st0_cat1_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow12), st0_cat1_treeview);

  label9 = gtk_label_new (_("Album"));
  gtk_widget_show (label9);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab0), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab0), 1), label9);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  scrolledwindow13 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow13);
  gtk_container_add (GTK_CONTAINER (sorttab0), scrolledwindow13);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow13), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st0_cat2_treeview = gtk_tree_view_new ();
  gtk_widget_show (st0_cat2_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow13), st0_cat2_treeview);

  label10 = gtk_label_new (_("Genre"));
  gtk_widget_show (label10);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab0), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab0), 2), label10);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);

  scrolledwindow16 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow16);
  gtk_container_add (GTK_CONTAINER (sorttab0), scrolledwindow16);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow16), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st0_cat3_treeview = gtk_tree_view_new ();
  gtk_widget_show (st0_cat3_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow16), st0_cat3_treeview);

  label14 = gtk_label_new (_("Title"));
  gtk_widget_show (label14);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab0), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab0), 3), label14);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);

  sorttab1 = gtk_notebook_new ();
  gtk_widget_show (sorttab1);
  gtk_paned_pack2 (GTK_PANED (hpaned2), sorttab1, TRUE, TRUE);

  scrolledwindow15 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow15);
  gtk_container_add (GTK_CONTAINER (sorttab1), scrolledwindow15);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow15), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st1_cat0_treeview = gtk_tree_view_new ();
  gtk_widget_show (st1_cat0_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow15), st1_cat0_treeview);

  label11 = gtk_label_new (_("Artist"));
  gtk_widget_show (label11);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab1), 0), label11);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  scrolledwindow14 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow14);
  gtk_container_add (GTK_CONTAINER (sorttab1), scrolledwindow14);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow14), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st1_cat1_treeview = gtk_tree_view_new ();
  gtk_widget_show (st1_cat1_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow14), st1_cat1_treeview);

  label12 = gtk_label_new (_("Album"));
  gtk_widget_show (label12);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab1), 1), label12);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

  scrolledwindow10 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow10);
  gtk_container_add (GTK_CONTAINER (sorttab1), scrolledwindow10);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow10), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st1_cat2_treeview = gtk_tree_view_new ();
  gtk_widget_show (st1_cat2_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow10), st1_cat2_treeview);

  label13 = gtk_label_new (_("Genre"));
  gtk_widget_show (label13);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab1), 2), label13);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  scrolledwindow17 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow17);
  gtk_container_add (GTK_CONTAINER (sorttab1), scrolledwindow17);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow17), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  st1_cat3_treeview = gtk_tree_view_new ();
  gtk_widget_show (st1_cat3_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow17), st1_cat3_treeview);

  label15 = gtk_label_new (_("Title"));
  gtk_widget_show (label15);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (sorttab1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (sorttab1), 3), label15);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);

  scrolledwindow11 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow11);
  gtk_paned_pack2 (GTK_PANED (vpaned1), scrolledwindow11, TRUE, TRUE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow11), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  song_treeview = gtk_tree_view_new ();
  gtk_widget_show (song_treeview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow11), song_treeview);
  gtk_widget_add_accelerator (song_treeview, "select_all", accel_group,
                              GDK_a, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) gtkpod, "delete_event",
                    G_CALLBACK (on_gtkpod_delete_event),
                    NULL);
  g_signal_connect ((gpointer) import_itunes1, "activate",
                    G_CALLBACK (on_import_itunes1_activate),
                    NULL);
  g_signal_connect ((gpointer) add_files1, "activate",
                    G_CALLBACK (on_add_files1_activate),
                    NULL);
  g_signal_connect ((gpointer) add_directory1, "activate",
                    G_CALLBACK (on_add_directory1_activate),
                    NULL);
  g_signal_connect ((gpointer) export_itunes1, "activate",
                    G_CALLBACK (on_export_itunes1_activate),
                    NULL);
  g_signal_connect ((gpointer) quit1, "activate",
                    G_CALLBACK (on_quit1_activate),
                    NULL);
  g_signal_connect ((gpointer) new_playlist1, "activate",
                    G_CALLBACK (on_new_playlist1_activate),
                    NULL);
  g_signal_connect ((gpointer) cut1, "activate",
                    G_CALLBACK (on_cut1_activate),
                    NULL);
  g_signal_connect ((gpointer) copy1, "activate",
                    G_CALLBACK (on_copy1_activate),
                    NULL);
  g_signal_connect ((gpointer) paste1, "activate",
                    G_CALLBACK (on_paste1_activate),
                    NULL);
  g_signal_connect ((gpointer) delete1, "activate",
                    G_CALLBACK (on_delete1_activate),
                    NULL);
  g_signal_connect ((gpointer) about1, "activate",
                    G_CALLBACK (on_about1_activate),
                    NULL);
  g_signal_connect ((gpointer) button1, "clicked",
                    G_CALLBACK (on_import_itunes1_button),
                    NULL);
  g_signal_connect ((gpointer) button2, "clicked",
                    G_CALLBACK (on_add_files1_button),
                    NULL);
  g_signal_connect ((gpointer) button3, "clicked",
                    G_CALLBACK (on_add_directory1_button),
                    NULL);
  g_signal_connect ((gpointer) button4, "clicked",
                    G_CALLBACK (on_new_playlist_button),
                    NULL);
  g_signal_connect ((gpointer) button5, "clicked",
                    G_CALLBACK (on_export_itunes1_button),
                    NULL);
  g_signal_connect ((gpointer) playlist_treeview, "drag_data_received",
                    G_CALLBACK (on_playlist_treeview_drag_data_received),
                    NULL);
  g_signal_connect ((gpointer) sorttab0, "switch_page",
                    G_CALLBACK (on_sorttab_switch_page),
                    NULL);
  g_signal_connect ((gpointer) sorttab1, "switch_page",
                    G_CALLBACK (on_sorttab_switch_page),
                    NULL);
  g_signal_connect ((gpointer) song_treeview, "drag_data_get",
                    G_CALLBACK (on_song_treeview_drag_data_get),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (gtkpod, gtkpod, "gtkpod");
  GLADE_HOOKUP_OBJECT (gtkpod, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (gtkpod, menubar1, "menubar1");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem10, "menuitem10");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem10_menu, "menuitem10_menu");
  GLADE_HOOKUP_OBJECT (gtkpod, import_itunes1, "import_itunes1");
  GLADE_HOOKUP_OBJECT (gtkpod, add_files1, "add_files1");
  GLADE_HOOKUP_OBJECT (gtkpod, add_directory1, "add_directory1");
  GLADE_HOOKUP_OBJECT (gtkpod, export_itunes1, "export_itunes1");
  GLADE_HOOKUP_OBJECT (gtkpod, separatormenuitem1, "separatormenuitem1");
  GLADE_HOOKUP_OBJECT (gtkpod, quit1, "quit1");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem11, "menuitem11");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem11_menu, "menuitem11_menu");
  GLADE_HOOKUP_OBJECT (gtkpod, new_playlist1, "new_playlist1");
  GLADE_HOOKUP_OBJECT (gtkpod, image1, "image1");
  GLADE_HOOKUP_OBJECT (gtkpod, separator1, "separator1");
  GLADE_HOOKUP_OBJECT (gtkpod, cut1, "cut1");
  GLADE_HOOKUP_OBJECT (gtkpod, copy1, "copy1");
  GLADE_HOOKUP_OBJECT (gtkpod, paste1, "paste1");
  GLADE_HOOKUP_OBJECT (gtkpod, delete1, "delete1");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem12, "menuitem12");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem13, "menuitem13");
  GLADE_HOOKUP_OBJECT (gtkpod, menuitem13_menu, "menuitem13_menu");
  GLADE_HOOKUP_OBJECT (gtkpod, about1, "about1");
  GLADE_HOOKUP_OBJECT (gtkpod, toolbar2, "toolbar2");
  GLADE_HOOKUP_OBJECT (gtkpod, button1, "button1");
  GLADE_HOOKUP_OBJECT (gtkpod, button2, "button2");
  GLADE_HOOKUP_OBJECT (gtkpod, button3, "button3");
  GLADE_HOOKUP_OBJECT (gtkpod, button4, "button4");
  GLADE_HOOKUP_OBJECT (gtkpod, button5, "button5");
  GLADE_HOOKUP_OBJECT (gtkpod, hpaned1, "hpaned1");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow8, "scrolledwindow8");
  GLADE_HOOKUP_OBJECT (gtkpod, playlist_treeview, "playlist_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, vpaned1, "vpaned1");
  GLADE_HOOKUP_OBJECT (gtkpod, hpaned2, "hpaned2");
  GLADE_HOOKUP_OBJECT (gtkpod, sorttab0, "sorttab0");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow9, "scrolledwindow9");
  GLADE_HOOKUP_OBJECT (gtkpod, st0_cat0_treeview, "st0_cat0_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label8, "label8");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow12, "scrolledwindow12");
  GLADE_HOOKUP_OBJECT (gtkpod, st0_cat1_treeview, "st0_cat1_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label9, "label9");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow13, "scrolledwindow13");
  GLADE_HOOKUP_OBJECT (gtkpod, st0_cat2_treeview, "st0_cat2_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label10, "label10");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow16, "scrolledwindow16");
  GLADE_HOOKUP_OBJECT (gtkpod, st0_cat3_treeview, "st0_cat3_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label14, "label14");
  GLADE_HOOKUP_OBJECT (gtkpod, sorttab1, "sorttab1");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow15, "scrolledwindow15");
  GLADE_HOOKUP_OBJECT (gtkpod, st1_cat0_treeview, "st1_cat0_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label11, "label11");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow14, "scrolledwindow14");
  GLADE_HOOKUP_OBJECT (gtkpod, st1_cat1_treeview, "st1_cat1_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label12, "label12");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow10, "scrolledwindow10");
  GLADE_HOOKUP_OBJECT (gtkpod, st1_cat2_treeview, "st1_cat2_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label13, "label13");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow17, "scrolledwindow17");
  GLADE_HOOKUP_OBJECT (gtkpod, st1_cat3_treeview, "st1_cat3_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, label15, "label15");
  GLADE_HOOKUP_OBJECT (gtkpod, scrolledwindow11, "scrolledwindow11");
  GLADE_HOOKUP_OBJECT (gtkpod, song_treeview, "song_treeview");
  GLADE_HOOKUP_OBJECT (gtkpod, statusbar1, "statusbar1");

  gtk_window_add_accel_group (GTK_WINDOW (gtkpod), accel_group);

  return gtkpod;
}

GtkWidget*
create_gtkpod_about_window (void)
{
  GtkWidget *gtkpod_about_window;
  GtkWidget *vbox2;
  GtkWidget *about_top_box;
  GtkWidget *about_label;
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow2;
  GtkWidget *credits_textview;
  GtkWidget *label3;
  GtkWidget *scrolledwindow3;
  GtkWidget *translators_textview;
  GtkWidget *label4;
  GtkWidget *hbuttonbox2;
  GtkWidget *about_close;

  gtkpod_about_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (gtkpod_about_window, 350, 0);
  gtk_window_set_title (GTK_WINDOW (gtkpod_about_window), _("window1"));
  gtk_window_set_default_size (GTK_WINDOW (gtkpod_about_window), 450, 450);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (gtkpod_about_window), vbox2);

  about_top_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (about_top_box);
  gtk_box_pack_start (GTK_BOX (vbox2), about_top_box, FALSE, FALSE, 0);

  about_label = gtk_label_new ("");
  gtk_widget_show (about_label);
  gtk_box_pack_start (GTK_BOX (about_top_box), about_label, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (about_label), TRUE);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (vbox2), notebook1, TRUE, TRUE, 0);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow2);

  credits_textview = gtk_text_view_new ();
  gtk_widget_show (credits_textview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), credits_textview);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (credits_textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (credits_textview), GTK_WRAP_WORD);

  label3 = gtk_label_new (_("Credits"));
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label3);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow3);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow3);

  translators_textview = gtk_text_view_new ();
  gtk_widget_show (translators_textview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), translators_textview);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (translators_textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (translators_textview), GTK_WRAP_WORD);

  label4 = gtk_label_new (_("Translators"));
  gtk_widget_show (label4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label4);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox2, FALSE, FALSE, 0);

  about_close = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (about_close);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), about_close);
  GTK_WIDGET_SET_FLAGS (about_close, GTK_CAN_DEFAULT);

  g_signal_connect_swapped ((gpointer) gtkpod_about_window, "delete_event",
                            G_CALLBACK (on_about_window_close),
                            GTK_OBJECT (gtkpod_about_window));
  g_signal_connect_swapped ((gpointer) about_close, "clicked",
                            G_CALLBACK (on_about_window_close_button),
                            GTK_OBJECT (gtkpod_about_window));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (gtkpod_about_window, gtkpod_about_window, "gtkpod_about_window");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, about_top_box, "about_top_box");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, about_label, "about_label");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, scrolledwindow2, "scrolledwindow2");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, credits_textview, "credits_textview");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, label3, "label3");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, scrolledwindow3, "scrolledwindow3");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, translators_textview, "translators_textview");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, label4, "label4");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, hbuttonbox2, "hbuttonbox2");
  GLADE_HOOKUP_OBJECT (gtkpod_about_window, about_close, "about_close");

  return gtkpod_about_window;
}

