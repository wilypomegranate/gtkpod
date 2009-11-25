/*
 |Copyright (C) 2007 P.G. Richardson <phantom_sf at users.sourceforge.net>
 |Part of the gtkpod project.
 |
 |URL: http://www.gtkpod.org/
 |URL: http://gtkpod.sourceforge.net/
 |
 |This program is free software; you can redistribute it and/or modify
 |it under the terms of the GNU General Public License as published by
 |the Free Software Foundation; either version 2 of the License, or
 |(at your option) any later version.
 |
 |This program is distributed in the hope that it will be useful,
 |but WITHOUT ANY WARRANTY; without even the implied warranty of
 |MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 |GNU General Public License for more details.
 |
 |You should have received a copy of the GNU General Public License
 |along with this program; if not, write to the Free Software
 |Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 |
 |iTunes and iPod are trademarks of Apple
 |
 |This product is not supported/written/published by Apple!
 |
 */

#include "stock_icons.h"

void register_stock_icon (const gchar *name, const gchar *stockid)
{
	GtkIconSet *pl_iconset;
	GtkIconSource *source;

	g_return_if_fail (name);
	g_return_if_fail (stockid);

	pl_iconset = gtk_icon_set_new ();
	source = gtk_icon_source_new ();

	gtk_icon_source_set_icon_name (source, name);
	gtk_icon_set_add_source (pl_iconset, source);
	gtk_icon_source_free (source); /* _add_source() copies source */

	GtkIconFactory *factory = gtk_icon_factory_new ();
	gtk_icon_factory_add (factory, stockid, pl_iconset);

	gtk_icon_factory_add_default (factory);
}