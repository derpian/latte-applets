/* button.c:
 * BUTTON applet
 * Copyright (C) 2017 Vitaliy Kopylov
 * http://latte-desktop.org/
 *
 *  button applet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  button applet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with button applet.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <mate-panel-applet.h>
#include <mate-panel-applet-gsettings.h>

#define PANEL_OBJECT_PATH             "/org/mate/panel/objects/"
#define PANEL_OBJECT_SCHEMA                  "org.mate.panel.object"

typedef struct
{
    MatePanelApplet   *applet;
    GtkWidget         *label;
    
} buttonApplet;

static void button_deint (GtkButton* button, gpointer data)
{
	g_value_unset((GValue *) data);
	g_free((GValue *) data);
}

static void button_clicked_int (GtkButton* button, gpointer data)
{
g_warning("FUCK");
	char* format = g_value_get_string((GValue *) data);
/*
gchar *fxx;
	fxx = g_strdup_printf("gxmessage %s", format);
g_spawn_command_line_async(fxx, NULL);
	g_free(fxx);

gchar *fxy;
	fxy = g_strdup_printf("%s", format);
g_spawn_command_line_async(fxy, NULL);
	g_free(fxy); */

	g_print ("button pressed: %s\n", format);
/* --------pointer ---- */
if (g_str_has_suffix(format, " %")){
	gchar *f, *fx;
    int ox, oy, ax, ay, wx, wy;
	int             menu_x = 0;
	int             menu_y = 0;
	int             pointer_x;
	int             pointer_y;
	GtkAllocation   allocation;
	GtkWidget *widget;
	GdkDevice      *device;
	widget = GTK_WIDGET(button);

	gdk_window_get_origin (gtk_widget_get_window (widget),
			       &menu_x, &menu_y);
#if GTK_CHECK_VERSION (3, 20, 0)
	device = gdk_seat_get_pointer (gdk_display_get_default_seat (gtk_widget_get_display (widget)));
	gdk_window_get_device_position (gtk_widget_get_window (widget), device, &pointer_x, &pointer_y, NULL);
#elif GTK_CHECK_VERSION (3, 0, 0)
	device = gdk_device_manager_get_client_pointer (gdk_display_get_device_manager (gtk_widget_get_display (widget)));
	gdk_window_get_device_position (gtk_widget_get_window (widget), device, &pointer_x, &pointer_y, NULL);
#else
	gtk_widget_get_pointer (widget, &pointer_x, &pointer_y);
#endif
	gtk_widget_get_allocation (widget, &allocation);

	menu_x += allocation.x;
	menu_y += allocation.y;

/*		if (gtk_widget_get_direction (GTK_WIDGET (menu)) != GTK_TEXT_DIR_RTL) {
			if (pointer_x < allocation.width &&
			    requisition.width < pointer_x)
				menu_x += MIN (pointer_x,
					       allocation.width - requisition.width);
		} else {
			menu_x += allocation.width - requisition.width;
			if (pointer_x > 0 && pointer_x < allocation.width &&
			    pointer_x < allocation.width - requisition.width) {
				menu_x -= MIN (allocation.width - pointer_x,
					       allocation.width - requisition.width);
			}
		}
		menu_x = MIN (menu_x, gdk_screen_get_width (screen) - requisition.width);

menu_y -= req.height; */

    f = g_strndup(format, strlen(format) - 1);
	fx = g_strdup_printf("%s%i %i %i %i %i %i", f, pointer_x, pointer_y, menu_x, menu_y, allocation.width, allocation.height);
	g_free(f);
g_spawn_command_line_async(fx, NULL);
	g_free(fx);
} else {
/* -------------------- */
    g_spawn_command_line_async(format, NULL);
}
}

static GtkWidget* pc_start_instantiate(MatePanelApplet* applet)
{
	//------------------------get-settings---
/* ---------------------- */
char *prefs_path; int ix = 0; int iy = 0;
char *p; char *ps;
prefs_path = mate_panel_applet_get_preferences_path (applet);
if (g_str_has_suffix(prefs_path, "/prefs/")) {
	p = g_strndup(prefs_path, (strlen(prefs_path) - 7));
} else {
	p = g_strndup(prefs_path, 1);
}

g_free(prefs_path);

//ps = g_strrstr(p, "/") + 1;
ps = g_strdup_printf(" %s", (g_strrstr(p, "/") + 1));

g_free(p);

    //gchar       *layout_file = NULL;
    GKeyFile    *keyfile = NULL;
    gchar      **groups = NULL;
    GError      *error = NULL;
    int          i;
    int          j;
    //char      **keyfile_keys = NULL;
    char       *value_str;
    char       *value_strb;
    
        keyfile = g_key_file_new ();
        if (g_key_file_load_from_file (keyfile,
                                       "/usr/share/mate-panel/layouts/default.layout",
                                       G_KEY_FILE_NONE,
                                       &error))
        {
            groups = g_key_file_get_groups (keyfile, NULL);
            
            for (i = 0; groups[i] != NULL; i++) {
                
                if (g_strcmp0 (groups[i], "Object") == 0 ||
                        g_str_has_prefix (groups[i], "Object ")) {
                
/*                    panel_layout_append_group_helper (
                                        keyfile, groups[i],
                                        -1,
                                        "Object",
                                        PANEL_OBJECT_ID_LIST_KEY,
                                        PANEL_OBJECT_SCHEMA,
                                        PANEL_OBJECT_PATH,
                                        PANEL_OBJECT_DEFAULT_PREFIX,
                                        panel_layout_object_keys,
                                        G_N_ELEMENTS (panel_layout_object_keys),
                                        "object"); */

if (g_str_has_suffix (groups[i], ps)) {
if (ix == 0) {
	                    value_str = g_key_file_get_string (keyfile,
                                                       groups[i], "launcher-location",
                                                       NULL);
   ix = 1;
}
if (iy == 0) {
	                    value_strb = g_key_file_get_string (keyfile,
                                                       groups[i], "menu-path",
                                                       NULL);
   iy = 1;
}

}                   
                } else
                    
                    g_warning ("Unknown group in default layout: '%s'",
                               groups[i]);
            }
        }
        else
        {
            g_warning ("Error while parsing default layout from '%s': %s\n",
                       "layout_file", error->message);
            g_error_free (error);
        }
    
    if (groups)
        g_strfreev (groups);
    
    if (keyfile)
        g_key_file_free (keyfile);
/*
gchar *fx;
	fx = g_strdup_printf("gxmessage %s %s", ps, value_str);
// /org/mate/panel/objects/universityapplet-1/prefs/ /org/mate/panel/objects/universityapplet/ /usr/share/applications/caja-RRRR.desktop_fuck duck /org/mate/panel/applet/UniversityApplet/0
// /org/mate/panel/objects/universityapplet/prefs/ /org/mate/panel/objects/universityapplet/ /usr/share/applications/caja-RRRR.desktop_fuck duck /org/mate/panel/applet/UniversityApplet/1
g_spawn_command_line_async(fx, NULL);
	g_free(fx); */
//	g_free(launcher_location);
	g_free (ps); 
/* ---------------------- */

	//----------------
	const gchar* format = "pancake_menu /usr/share/pancake/menu.sh %"; //cfg_getstr(config, "exec");
//	const gchar* imgs = cfg_getstr(config, "img");
	int si;// = cfg_getint(config, "size");
	if (si == 0) {si = GTK_ICON_SIZE_BUTTON;}
	GtkWidget* widget = gtk_button_new();
	GtkWidget *img;

	GValue* value = g_new0(GValue, 1);
	g_value_init(value, G_TYPE_STRING);

if (ix == 1) {
	g_value_set_string(value, value_str);	
	g_free (value_str);
	}
else {
	g_value_set_string(value, format);
}
/*	g_object_set_property(G_OBJECT(widget), "label", &value);
	g_value_unset(&value); */
	
	gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
	gtk_container_set_border_width(GTK_CONTAINER(widget), 0);

//    img = gtk_image_new_from_icon_name (imgs, 32);
//    img = gtk_image_new_from_icon_name (imgs, si);

if (iy == 1) {
	if (value_strb) {
		img = gtk_image_new_from_icon_name (value_strb, GTK_ICON_SIZE_BUTTON);
		g_free(value_strb);
	} else {
			img = gtk_image_new_from_icon_name ("network-server", GTK_ICON_SIZE_BUTTON);
		}
	}
else {
    img = gtk_image_new_from_icon_name ("network-server", GTK_ICON_SIZE_BUTTON);
}

	gtk_button_set_image(GTK_BUTTON(widget), img);
	
	g_signal_connect (G_OBJECT (widget), "clicked",
		G_CALLBACK (button_clicked_int),
		(gpointer) value);
	g_signal_connect (G_OBJECT (widget), "destroy",
		G_CALLBACK (button_deint),
		(gpointer) value);
/*
 g_signal_connect (G_OBJECT (widget), "clicked", 
							   (GtkSignalFunc)gtk_widget_destroy,
							   G_OBJECT(widget)); */
//		gtk_widget_show(widget);
	return widget;
}

static gboolean
button_applet_fill (MatePanelApplet* applet)
{
    buttonApplet *university_applet;

    /* set mate-panel applet options */
    mate_panel_applet_set_flags (applet, MATE_PANEL_APPLET_EXPAND_MINOR);
    mate_panel_applet_set_background_widget(applet, GTK_WIDGET(applet));

    /* create the UniversityApplet struct */
    university_applet = g_malloc0(sizeof(buttonApplet));
    university_applet->applet = applet;
    university_applet->label = pc_start_instantiate(applet);//gtk_label_new("");
    
    /* we add the Gtk label into the applet */
    gtk_container_add (GTK_CONTAINER(applet), university_applet->label);

    gtk_widget_show_all (applet);

    return TRUE;
}

/* This function, called by mate-panel, will create the applet */
static gboolean
button_factory (MatePanelApplet* applet, const char* iid, gpointer data)
{
    gboolean retval = FALSE;

//    if (!g_strcmp0(iid, "buttonApplet"))
        retval = button_applet_fill (applet);

    return retval;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY("buttonAppletFactory",
                                      PANEL_TYPE_APPLET,
                                      "Sample applet",
                                      button_factory,
                                      NULL)
