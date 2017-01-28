/* mount.c:
 * MOUNT applet
 * Copyright (C) 2017 Vitaliy Kopylov
 * http://latte-desktop.org/
 *
 *  mount applet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mount applet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mount applet.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <mate-panel-applet.h>
#include <mate-panel-applet-gsettings.h>

#define PC_MOUNT_DEFAULT ""
#define PC_MOUNT_IMAGE "desktop"

#define DEFAULT_MOUNT_COMMAND "/usr/share/latte/mount.sh"
#define DEFAULT_NMOUNT_COMMAND "/usr/share/latte/nmount.sh"
#define DEFAULT_UMOUNT_COMMAND "/usr/share/latte/umount.sh"
#define DEFAULT_INET_COMMAND "/usr/share/latte/internet.sh"

typedef struct
{
    MatePanelApplet   *applet;
    GtkWidget         *label;
} mountApplet;

typedef struct {
    GtkWidget *menu, *but, *label;
    gulong handler_id;
    int iconsize, paneliconsize;
        
	/* fabian on 2005-12-17 */
	char *mount_command;
	char *nmount_command;
	char *umount_command;
	/* end fabian */
	GPtrArray * pdisks ; /* contains pointers to struct t_disk */

	GtkWidget *sep, *mi, *hbox, *laba, *labb;
} mount_priv;

typedef struct
{
	GtkWidget * menu_item ;
	GtkWidget * hbox ;
	GtkWidget * label_disk ;
	GtkWidget * label_mount_info ;
	GtkWidget * progress_bar ;
} t_disk_display ;

#include "devices.h"
#include "devices.c"

/*---------------- on_activate_disk_display ---------------*/
static void on_activate_disk_display(GtkWidget * widget,t_disk * disk)
{
    mount_priv * mt ;
    if (disk != NULL)
    {
        mt = (mount_priv*) g_object_get_data (G_OBJECT(widget), "mounter");
    	if (disk->mount_info != NULL)
    	{/*disk is mounted*/
    		disk_umount (disk, mt->umount_command);
    		/* disk->mount_info is freed by disk_mount */
    	}
    	else
    	{/*disk is not mounted*/
			if (disk->is_new == TRUE)
				disk_mount (disk, mt->nmount_command);
			else
				disk_mount (disk, mt->mount_command);
    		/* needs a refresh, a global refresh is done whe the window becomes visible*/
    	}
    }
}

/*---------------------------------------------------------*/

/*-------------------- disk_display_new -----------------*/
/*create a new t_disk_display from t_disk infos */
static t_disk_display * disk_display_new(t_disk * disk, mount_priv * mounter)
{
	if (disk != NULL) 
	{
// if ((g_strlen(disk->mount_point) == 1) && (g_strchr(disk->mount_point, '/') != NULL)) {return NULL;}
if (disk->is_hide == TRUE) return NULL;
/* if (g_strcmp(disk->mount_point, "/") == 0) {return NULL ;} */
		t_disk_display * dd ;
		dd = g_new0(t_disk_display,1) ;
		dd->menu_item = gtk_menu_item_new();
		g_signal_connect(G_OBJECT(dd->menu_item),"activate",G_CALLBACK(on_activate_disk_display),disk);
		g_object_set_data(G_OBJECT(dd->menu_item),"mounter",(gpointer)mounter);
		
		dd->hbox = gtk_hbox_new(FALSE,10);
		gtk_container_add(GTK_CONTAINER(dd->menu_item), dd->hbox);
		
		dd->label_disk = gtk_label_new(g_strconcat(disk->device," -> ",disk->mount_point,NULL));
		/*change to uniform label size*/
		gtk_label_set_width_chars(GTK_LABEL(dd->label_disk),28);
		gtk_label_set_justify(GTK_LABEL(dd->label_disk),GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(dd->hbox),dd->label_disk,FALSE,TRUE,0);
		
		dd->label_mount_info = gtk_label_new("");
		/*change to uniform label size*/
		gtk_label_set_width_chars(GTK_LABEL(dd->label_mount_info),25);
		gtk_label_set_use_markup(GTK_LABEL(dd->label_mount_info),TRUE);
		gtk_label_set_justify(GTK_LABEL(dd->label_mount_info),GTK_JUSTIFY_RIGHT);
		gtk_box_pack_start(GTK_BOX(dd->hbox),dd->label_mount_info,TRUE,TRUE,0);
		
		dd->progress_bar = gtk_progress_bar_new();
/*					GTK_WIDGET_SET_FLAGS (dd->progress_bar, GTK_APP_PAINTABLE); */
		gtk_box_pack_start(GTK_BOX(dd->hbox),dd->progress_bar,TRUE,TRUE,0);
		return dd ;
	}
	return NULL ;
}
/*-----------------------------------------------------------*/


/*-------------------- disk_display_refresh -----------------*/
static void disk_display_refresh(t_disk_display * disk_display, t_mount_info * mount_info, int i)
{
	if (disk_display != NULL)
	{
		if (mount_info != NULL)
		{	/* device is mounted */
			char * text ;
			char * used = get_size_human_readable(mount_info->used);
			//DBG("used is now : %s",used);
			char * size = get_size_human_readable(mount_info->size);
			//DBG("size is now : %s",size);
			char * avail = get_size_human_readable(mount_info->avail);
			//DBG("avail is now : %s",size);
			text = g_strdup_printf("[%s/%s] %s free", used ,size,avail );
			//DBG("text is now : %s",text);
			
			g_free(used);
			g_free(size);
			g_free(avail);
			//text = g_strdup_printf("mounted on : %s\t[%g Mb/%g Mb] %g Mb free",mount_info->mounted_on, mount_info->used, mount_info->size, mount_info->avail);
			gtk_label_set_text(GTK_LABEL(disk_display->label_mount_info),text);
			
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(disk_display->progress_bar),((gdouble)mount_info->percent / 100) );
			gtk_progress_bar_set_text(GTK_PROGRESS_BAR(disk_display->progress_bar),g_strdup_printf("%d",mount_info->percent));
			gtk_widget_show(GTK_WIDGET(disk_display->progress_bar));
		}
		else /* mount_info == NULL */
		{
			/*remove mount info */
if (i == TRUE)
			gtk_label_set_markup(GTK_LABEL(disk_display->label_mount_info),"<span foreground=\"#FF0000\">new/not mounted</span>");
else		gtk_label_set_markup(GTK_LABEL(disk_display->label_mount_info),"<span foreground=\"#FF0000\">not mounted</span>");
			gtk_widget_hide(GTK_WIDGET(disk_display->progress_bar));

		}
	}
}
/*----------------------------------------------*/

/*--------------- mounter_data_free -----------------*/
static void mounter_data_free(mount_priv * mt)
{
	disks_free(&(mt->pdisks));
if (1 == 1){
	gtk_widget_destroy(GTK_WIDGET(mt->labb));
	gtk_widget_destroy(GTK_WIDGET(mt->laba));
	gtk_widget_destroy(GTK_WIDGET(mt->hbox));
	gtk_widget_destroy(GTK_WIDGET(mt->mi));
	gtk_widget_destroy(GTK_WIDGET(mt->sep));
}
	gtk_widget_destroy(GTK_WIDGET(mt->menu));
	mt->menu = NULL ;
}
/*----------------------------------------------*/

static void
mount_internet(GtkWidget *widget, gpointer data)
{
        g_spawn_command_line_async(DEFAULT_INET_COMMAND, NULL);
}

/*---------------- mounter_data_new --------------------------*/
static void mounter_data_new (mount_priv * mt)
{
	int i ;
	t_disk * disk ;
	t_disk_display * disk_display ;
	
	/*get static infos from /etc/fstab */
	mt->pdisks = disks_new(FALSE);
	
	/* get dynamic infos on mounts */
	disks_refresh(mt->pdisks);	
//g_print("is %i\n", mt->pdisks->len);
	procinf(mt->pdisks);
//g_print("is %i\n", mt->pdisks->len);
	
	/* menu with menu_item */
	mt->menu = gtk_menu_new();
	
	for(i=0;i < mt->pdisks->len;i++)
	{
		disk = g_ptr_array_index(mt->pdisks,i); //get the disk 
		disk_display = disk_display_new(disk,mt) ;// creates a disk_display
	if (disk_display) {
		disk_display_refresh(disk_display,disk->mount_info, disk->is_new) ;//fill in mount infos
		gtk_menu_shell_append(GTK_MENU_SHELL(mt->menu),disk_display->menu_item);//add the menu_item to the menu
}
	}
	
	
if (1 == 1){
/* nya nya nayn */

        mt->sep = gtk_separator_menu_item_new();
		mt->mi = gtk_menu_item_new();
		
		mt->hbox = gtk_hbox_new(FALSE,10);
		gtk_container_add(GTK_CONTAINER(mt->mi), mt->hbox);
		
		mt->laba = gtk_label_new("");
		/*change to uniform label size*/
		gtk_label_set_width_chars(GTK_LABEL(mt->laba),4);
		gtk_label_set_justify(GTK_LABEL(mt->laba),GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(mt->hbox),mt->laba,FALSE,TRUE,0);
		
		mt->labb = gtk_label_new("");
		/*change to uniform label size*/
		gtk_label_set_width_chars(GTK_LABEL(mt->labb),25);
		gtk_label_set_use_markup(GTK_LABEL(mt->labb),TRUE);
		gtk_label_set_justify(GTK_LABEL(mt->labb),GTK_JUSTIFY_RIGHT);
		gtk_label_set_markup (GTK_LABEL(mt->labb),"<span foreground=\"#0000FF\">network</span>");
		gtk_box_pack_start(GTK_BOX(mt->hbox),mt->labb,TRUE,TRUE,0);

			g_signal_connect(G_OBJECT(mt->mi), "activate", G_CALLBACK(mount_internet), NULL);

if (g_file_test (DEFAULT_INET_COMMAND, G_FILE_TEST_IS_EXECUTABLE)) {
		gtk_menu_shell_append (GTK_MENU_SHELL (mt->menu), mt->sep);
		gtk_menu_shell_append(GTK_MENU_SHELL(mt->menu), mt->mi);
}
/* naysh*/


}
	gtk_widget_show_all(mt->menu);	
	return ;
}
/*------------------------------------------------------*/

/*---------------------- mounter_refresh ---------------*/
static void mounter_refresh(mount_priv * mt)
{
	mounter_data_free(mt);
	mounter_data_new(mt);
	procinf(mt->pdisks);
}
/*---------------------------------------------------------*/
static void menu_pos(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, GtkWidget *widget)
{
    int ox, oy, w, h, ax, ay, wx, wy;
    gdk_window_get_origin(gtk_widget_get_window(widget), &ox, &oy);
	GtkAllocation alloc;
	GtkRequisition req;
	gtk_widget_get_allocation(widget, &alloc);
	gtk_widget_get_requisition (GTK_WIDGET(menu), &req);

    w = req.width;
    h = req.height;
    wx = alloc.x;
    wy = alloc.y;
        *x = ox;
        if (*x + w > gdk_screen_width())
            *x = ox + alloc.width - w;
        *y = oy - h;
        if (*y < 0)
            *y = oy + alloc.height;
            
    ay = *y; ax = *x;
    *y = ay + wy;
    *x = ax + wx;
    *push_in = TRUE;
}

static void
mate_panel_applet_position_menu (GtkMenu   *menu,
			    int       *x,
			    int       *y,
			    gboolean  *push_in,
			    GtkWidget *widget)
{
//	MatePanelApplet    *applet;
	GtkAllocation   allocation;
	GtkRequisition  requisition;
	GtkRequisition  req;
#if GTK_CHECK_VERSION (3, 0, 0)
	GdkDevice      *device;
#endif
	GdkScreen      *screen;
	int             menu_x = 0;
	int             menu_y = 0;
	int             pointer_x;
	int             pointer_y;

//	g_return_if_fail (PANEL_IS_APPLET (widget));

//	applet = MATE_PANEL_APPLET (widget);

	screen = gtk_widget_get_screen (widget);
	gtk_menu_set_screen (menu, screen);

	gtk_widget_get_preferred_size (GTK_WIDGET (menu), &requisition, &req);
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

/*	if (applet->priv->orient == MATE_PANEL_APPLET_ORIENT_UP ||
	    applet->priv->orient == MATE_PANEL_APPLET_ORIENT_DOWN) { */
		if (gtk_widget_get_direction (GTK_WIDGET (menu)) != GTK_TEXT_DIR_RTL) {
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

	//	if (menu_y > gdk_screen_get_height (screen) / 2)
	//		menu_y -= requisition.height;
	//	else
//			menu_y += allocation.height;
menu_y -= req.height;
/*	} else  {
		if (pointer_y < allocation.height &&
		    requisition.height < pointer_y)
			menu_y += MIN (pointer_y, allocation.height - requisition.height);
		menu_y = MIN (menu_y, gdk_screen_get_height (screen) - requisition.height);

		if (menu_x > gdk_screen_get_width (screen) / 2)
			menu_x -= requisition.width;
		else
			menu_x += allocation.width;
	} */

	*x = menu_x;
	*y = menu_y;
	*push_in = TRUE;
}

static gint clicked (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    mount_priv *m = ((mount_priv *) data);
    
    mounter_refresh(m); // refreshs infos regarding mounts data
    gtk_menu_set_screen(GTK_MENU(m->menu), gtk_widget_get_screen(GTK_WIDGET(widget)));
    gtk_menu_popup(GTK_MENU(m->menu),
              NULL, NULL, (GtkMenuPositionFunc)mate_panel_applet_position_menu, widget,
              event->button, event->time);
    return TRUE;
}

static void button_deint (GtkButton* button, gpointer data)
{
	mounter_data_free((mount_priv *) data);
	g_free((mount_priv *) data);
}

static GtkWidget* pc_mount_instantiate()
{
//	const gchar* imgs = cfg_getstr(config, "img");
	GtkWidget *img;
	mount_priv *m;

	m = g_new0(mount_priv, 1);
	m->but = gtk_button_new();
	g_return_val_if_fail(m != NULL, 0); 
	
	gtk_button_set_relief(GTK_BUTTON(m->but), GTK_RELIEF_NONE);
	gtk_container_set_border_width(GTK_CONTAINER(m->but), 0);

//    img = gtk_image_new_from_icon_name (imgs, GTK_ICON_SIZE_MENU);
    img = gtk_image_new_from_icon_name ("folder-remote", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(m->but), img);

	m->mount_command = DEFAULT_MOUNT_COMMAND;
	m->nmount_command = DEFAULT_NMOUNT_COMMAND;
	m->umount_command = DEFAULT_UMOUNT_COMMAND;
	mounter_data_new(m);    
	
    g_signal_connect(G_OBJECT(m->but), "button_press_event",
          G_CALLBACK(clicked), (gpointer) m);
	g_signal_connect (G_OBJECT (m->but), "destroy",
		G_CALLBACK (button_deint),
		(gpointer) m);

	return m->but;
}

static gboolean
mount_applet_fill (MatePanelApplet* applet)
{
    mountApplet *university_applet;

    /* set mate-panel applet options */
    mate_panel_applet_set_flags (applet, MATE_PANEL_APPLET_EXPAND_MINOR);
    mate_panel_applet_set_background_widget(applet, GTK_WIDGET(applet));

    /* create the UniversityApplet struct */
    university_applet = g_malloc0(sizeof(mountApplet));
    university_applet->applet = applet;
    university_applet->label = pc_mount_instantiate();//gtk_label_new("");
    
    /* we add the Gtk label into the applet */
    gtk_container_add (GTK_CONTAINER(applet), university_applet->label);

    gtk_widget_show_all (applet);

    return TRUE;
}

/* This function, called by mate-panel, will create the applet */
static gboolean
mount_factory (MatePanelApplet* applet, const char* iid, gpointer data)
{
    gboolean retval = FALSE;

    //if (!g_strcmp0(iid, "mountpplet"))
        retval = mount_applet_fill (applet);

    return retval;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY("mountAppletFactory",
                                      PANEL_TYPE_APPLET,
                                      "Sample applet",
                                      mount_factory,
                                      NULL)
