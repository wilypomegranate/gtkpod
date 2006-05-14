/* Time-stamp: <2006-05-15 00:54:13 jcs>
|
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

#ifndef __SYNCDIR_H__
#define __SYNCDIR_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
 
#include <gtk/gtk.h>
#include "itdb.h"


void sync_playlist (Playlist *playlist,
		    const gchar *syncdir,
		    const gchar *key_sync_confirm_dirs,
		    gboolean sync_confirm_dirs,
		    const gchar *key_sync_delete_tracks,
		    gboolean sync_delete_tracks,
		    const gchar *key_sync_confirm_delete,
		    gboolean sync_confirm_delete,
		    const gchar *key_sync_show_summary,
		    gboolean sync_show_summary);
#endif
