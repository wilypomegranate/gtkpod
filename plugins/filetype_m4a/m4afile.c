/*
 |  Copyright (C) 2007 Jorg Schuler <jcsjcs at users sourceforge net>
 |  Part of the gtkpod project.
 |
 |  URL: http://www.gtkpod.org/
 |  URL: http://gtkpod.sourceforge.net/
 |
 |  Gtkpod is free software; you can redistribute it and/or modify
 |  it under the terms of the GNU General Public License as published by
 |  the Free Software Foundation; either version 2 of the License, or
 |  (at your option) any later version.
 |
 |  Gtkpod is distributed in the hope that it will be useful,
 |  but WITHOUT ANY WARRANTY; without even the implied warranty of
 |  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |  GNU General Public License for more details.
 |
 |  You should have received a copy of the GNU General Public License
 |  along with gtkpod; if not, write to the Free Software
 |  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 |
 |  iTunes and iPod are trademarks of Apple
 |
 |  This product is not supported/written/published by Apple!
 |
 |  $Id$
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "libgtkpod/charset.h"
#include "libgtkpod/gp_itdb.h"
#include "libgtkpod/prefs.h"
#include "plugin.h"
#include "m4afile.h"
#include "mp4file.h"

/* Info on how to implement new file formats: see mp3file.c for more info */


Track *m4a_get_file_info(const gchar *m4aFileName) {
    gchar *path_utf8;
    gchar *suf;
    Track *track = mp4_get_file_info(m4aFileName);
    g_return_val_if_fail(track, NULL);

    path_utf8 = charset_to_utf8(m4aFileName);
    suf = strrchr(path_utf8, '.') + 1;
    if (g_str_equal(suf, "m4b"))
        track->mediatype = ITDB_MEDIATYPE_AUDIOBOOK;
    else
        track->mediatype = ITDB_MEDIATYPE_AUDIO;

    g_free(path_utf8);
    return track;
}

gboolean m4a_write_file_info (const gchar *filename, Track *track) {
    return mp4_write_file_info(filename, track);
}

gboolean m4a_read_soundcheck (const gchar *filename, Track *track) {
    return mp4_read_soundcheck(filename, track);
}

gboolean m4a_can_convert() {
    gchar *cmd = m4a_get_conversion_cmd();
    return cmd && cmd[0] && prefs_get_int("convert_m4a");
}

gchar *m4a_get_conversion_cmd() {
    return prefs_get_string("path_conv_m4a");
}

gchar *m4a_get_gain_cmd() {
    return prefs_get_string ("path_aacgain");
}