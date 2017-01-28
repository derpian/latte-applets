/* aclock.c:
 * CLOCK applet
 * Copyright (C) 2017 Vitaliy Kopylov
 * http://latte-desktop.org/
 *
 *  clock applet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  clock applet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with clock applet.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <config.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <mate-panel-applet.h>
#include <mate-panel-applet-gsettings.h>

#define PC_CLOCK_DEFAULTFORMAT "%H:%M:%S"

typedef struct
{
    MatePanelApplet   *applet;
    GtkWidget         *label;
} aclockApplet;

static gboolean pc_clock_update(gpointer data)
{
	aclockApplet* clock = data;

	char buf[64];
	time_t tt = time(NULL);
	struct tm* tm = localtime(&tt);
	if(strftime(buf, sizeof(buf), PC_CLOCK_DEFAULTFORMAT, tm))
		gtk_label_set_text(GTK_LABEL(clock->label), buf);

	return TRUE;
}

static gboolean
aclock_applet_fill (MatePanelApplet* applet)
{
    aclockApplet *university_applet;

	char buf[64];
	time_t tt = time(NULL);
	struct tm* tm = localtime(&tt);

    /* set mate-panel applet options */
    mate_panel_applet_set_flags (applet, MATE_PANEL_APPLET_EXPAND_MINOR);
    mate_panel_applet_set_background_widget(applet, GTK_WIDGET(applet));

    /* create the UniversityApplet struct */
    university_applet = g_malloc0(sizeof(aclockApplet));
    university_applet->applet = applet;
    university_applet->label = gtk_label_new("");
    
    gtk_widget_set_can_focus(university_applet->label, FALSE);
    gtk_widget_set_can_default(university_applet->label, FALSE);

    /* we add the Gtk label into the applet */
    gtk_container_add (GTK_CONTAINER(applet), university_applet->label);

	if(strftime(buf, sizeof(buf), PC_CLOCK_DEFAULTFORMAT, tm))
		gtk_label_set_text(GTK_LABEL(university_applet->label), buf);

    gtk_widget_show_all (applet);

	g_timeout_add_seconds(1, pc_clock_update, university_applet);

    return TRUE;
}

/* This function, called by mate-panel, will create the applet */
static gboolean
aclock_factory (MatePanelApplet* applet, const char* iid, gpointer data)
{
    gboolean retval = FALSE;

//    if (!g_strcmp0(iid, "aclockApplet"))
        retval = aclock_applet_fill (applet);

    return retval;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY("aclockAppletFactory",
                                      PANEL_TYPE_APPLET,
                                      "aclock applet",
                                      aclock_factory,
                                      NULL)
