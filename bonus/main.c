/*
 * latte-menu - read a description file and generate a menu.
 * Copyright (C) 2004-2011 John Vorthman
 * Copyright (C) 2017 Vitaliy Kopylov
 * http://latte-desktop.org/
 * 
 * -------------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 2, as published
 * by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59 
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * -------------------------------------------------------------------------
 * 
 * This program requires GTK+ 3.0 (or later) libraries.
 * 
 * main.c is used to generate latte-menu.
 *
 * gcc -Wall -o latte-menu main.c `pkg-config gtk+-3.0 --cflags --libs`
 * 
 */


#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <glib.h>
#include <glib/gprintf.h>

#if !GTK_CHECK_VERSION (2, 18, 0)
     #define gtk_widget_get_visible(w) GTK_WIDGET_VISIBLE (w)
#endif

#define MAX_LINE_LENGTH 200
#define MAX_MENU_ENTRIES 1000
#define MAX_SUBMENU_DEPTH 4
#define MAX_ICON_SIZE 200
#define MIN_ICON_SIZE 10
#define LOCK_NAME "myGtkMenu_lockfile"
#define MAX_PATH_LEN 256

// Function prototypes
int ReadLine (gchar **arcp);
static void RunItem (char *Cmd);
static void QuitMenu (char *Msg);
gboolean Get2Numbers (char *data);
static void menu_position
	(GtkMenu * menu, gint * x, gint * y, gboolean * push_in, gpointer data);
int all_ready_running (void);

// Global variables
char Line[MAX_LINE_LENGTH], data[MAX_LINE_LENGTH];
int depth, lineNum, menuX, menuY;
    int ox, oy, ax, ay, wx, wy;
FILE *pFile;
GtkWidget *menu[MAX_SUBMENU_DEPTH];
char *HelpMsg = "myGtkMenu version 1.3, Copyright (C) 2004"
	"-2011 John Vorthman.\n"
	"myGtkMenu comes with ABSOLUTELY NO WARRANTY - "
	"see license file.\n"
	"Purpose: read a description file and display a menu.\n"
	"Usage: myGtkMenu MenuDescriptionFilename\n"
	"TestMenu.txt is an example input file.\n"
	"See TestMenu.txt for help.\n\n";


// ----------------------------------------------------------------------
int main (int argc, char *argv[]) {
// ----------------------------------------------------------------------

	char *Filename;
	int Mode;		// What kind of input are we looking for?
	int Kind;		// Type of input actually read
	int curDepth;	// Root menu is depth = 0
	int curItem;	// Count number of menu entries
	gint w, h;		// Size of menu icon
	gboolean bSetMenuPos = FALSE;
	int i;
	char Item[MAX_LINE_LENGTH], Cmd[MAX_MENU_ENTRIES][MAX_LINE_LENGTH];
	GtkWidget *menuItem, *SubmenuItem = NULL;
	GtkWidget *image;
	GdkPixbuf *Pixbuf;
	GError *error = NULL;
	struct stat statbuf;

    // Some hot keys (keybindings) get carried away and start
    // many instances. Will try to use a lock file so that
    // only one instance of this program runs at a given time.
    // If this check causes you problems, comment it out.
    int i_all_ready_running = all_ready_running();
	if (i_all_ready_running == 1) {
        fprintf(stderr, "All ready running, will quit.\n");
        return EXIT_FAILURE;
    }
	else if (i_all_ready_running == 2) {
        fprintf(stderr, "%s: Error in routine all_ready_running(), "
                        "will quit.\n", argv[0]);
        return EXIT_FAILURE;
    }

	if (!gtk_init_check (&argc, &argv)) {
		g_print("Error, cannot initialize gtk.\n");
		exit (EXIT_FAILURE);
	}
	
	if ((argc > 1) && (argv[1][0] == '-')) {
		g_print (HelpMsg);
		exit (EXIT_SUCCESS);
	}

/*	if (argc < 2) {
		g_print (HelpMsg);
		g_print ("Missing the menu-description filename.\n");
		g_print ("Will try to open the default file.\n"); */
/*		memset (data, 0, sizeof (data));
		strncpy (data, argv[0], sizeof (data) - 1);	// Get myGtkMenu path
		i = strlen (data);
		while ((i > 0) && (data[i] != '/'))
			data[i--] = '\0';	// Remove filename
		if ((i > 0) && (i < sizeof (data) - 14)) {
			strcat (data, "TestMenu.txt");
			Filename = data;
		}
		else */
/*			Filename = "TestMenu.txt";
	}
	else
		Filename = (char *) argv[1]; */

			Filename = "TestMenu.txt";

	g_print ("Reading the file: %s\n", Filename);
	

 GError *errorx;
 gchar          *output;
 gchar			*tmpcmd;
gchar			**arcp;
errorx = NULL;

if (argc > 7) {
ox = atoi(argv[2]);
oy = atoi(argv[3]);
wx = atoi(argv[4]);
wy = atoi(argv[5]);
ax = atoi(argv[6]);
ay = atoi(argv[7]);
	g_print("get %i args %i %i %i %i %i %i\n", argc, ox, oy, wx, wy, ax, ay);
}


// tmpcmd = g_strdup_printf("/root/1/my/pass.sh \"%s\"", Filename);
if (argv > 0)
	 {tmpcmd = g_strdup(argv[1]);}
else {tmpcmd = g_strdup_printf("/etc/menu.sh");}
	g_print("goat %s\n", tmpcmd);

   g_spawn_command_line_sync( tmpcmd,
                               &output,
                               NULL,
                               NULL,
                               &errorx );
arcp = g_strsplit(output,"\n", -1);


 g_free(output); g_free(tmpcmd);
/*	pFile = fopen (Filename, "r");
	if (pFile == NULL) {
		g_print ("Cannot open the file.\n");
		exit (EXIT_FAILURE);
	} */

	menu[0] = gtk_menu_new ();
	if (!gtk_icon_size_lookup (GTK_ICON_SIZE_BUTTON, &w, &h)) {	// Default 
		w = 30;
		h = 30;
	};

	curItem = 0;
	Mode = 0;
	curDepth = 0;
	while ((Kind = ReadLine(arcp)) != 0) {	// Read next line and get 'Kind'
										// of keyword
		if (Kind == -1)
			Mode = -1;	// Error parsing file

		if (depth > curDepth) {
			g_print ("Keyword found at incorrect indentation.\n");
			Mode = -1;
		}
		else if (depth < curDepth) {	// Close submenu
			curDepth = depth;
		}

		if (Mode == 0) {	// Starting new sequence. Whats next?
			if (Kind == 1)
				Mode = 1;	// New item
			else if (Kind == 4)
				Mode = 4;	// New submenu
			else if (Kind == 5)
				Mode = 6;	// New separator
			else if (Kind == 6)
				Mode = 7;	// New icon size
			else if (Kind == 7)
				Mode = 8;	// Set menu position
			else {			// Problems
				g_print ("Keyword out of order.\n");
				Mode = -1;
			}
		}

		switch (Mode) {
		case 1:	// Starting new menu item
			if (curItem >= MAX_MENU_ENTRIES) {
				g_print ("Exceeded maximum number of menu items.\n");
				Mode = -1;
				break;
			}
			strcpy (Item, data);
			Mode = 2;
			break;
		case 2:	// Still making new menu item
			if (Kind != 2) {	// Problems if keyword 'cmd=' not found
				g_print ("Missing keyword 'cmd=' (after 'item=').\n");
				Mode = -1;
				break;
			}
			strcpy (Cmd[curItem], data);
			Mode = 3;
			break;
		case 3:	// Still making new menu item
			if (Kind != 3) {	// Problems if keyword 'icon=' not found
				g_print ("Missing keyword 'icon=' (after 'cmd=').\n");
				Mode = -1;
				break;
			}
			// Insert item into menu
			menuItem = gtk_image_menu_item_new_with_mnemonic (Item);
			gtk_menu_shell_append (GTK_MENU_SHELL (menu[curDepth]), menuItem);
			g_signal_connect_swapped (menuItem, "activate",
						  G_CALLBACK (RunItem), Cmd[curItem]);
			curItem++;
			Mode = 0;
			if (strncasecmp (data, "NULL", 4) == 0) break;	// No icon
			// If data is a dir name, program will hang.
			stat (data, &statbuf); 
			if (!S_ISREG (statbuf.st_mode)) {
				g_print ("Error at line # %d\n", lineNum);
				g_print ("Error, %s is not a icon file.\n",
					 data);
				break;
			}
			Pixbuf = gdk_pixbuf_new_from_file_at_size (data, w, h, &error);
			if (Pixbuf == NULL) {
				g_print ("Error at line # %d\n", lineNum);
				g_print ("%s\n", error->message);
				g_error_free (error);
				error = NULL;
			}
			image = gtk_image_new_from_pixbuf (Pixbuf);
			gtk_image_menu_item_set_image (
						       (menuItem), image);
			gtk_image_menu_item_set_always_show_image((menuItem),TRUE);
			break;
		case 4:	// Start submenu
			if (curDepth >= MAX_SUBMENU_DEPTH) {
				g_print ("Maximum submenu depth exceeded.\n");
				Mode = -1;
				break;
			}
			SubmenuItem = gtk_image_menu_item_new_with_mnemonic (data);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu[curDepth]), SubmenuItem);
			curDepth++;
			menu[curDepth] = gtk_menu_new ();
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (SubmenuItem),
						   				menu[curDepth]);
			Mode = 5;
			break;
		case 5:	// Adding image to new submenu
			if (Kind != 3) {	// Problems if keyword 'icon=' not found
				g_print ("Missing keyword 'icon=' (after 'submenu=').\n");
				Mode = -1;
				break;
			}
			Mode = 0;
			if (strncasecmp (data, "NULL", 4) == 0) break;	// No icon
			// If data is a dir name, program will hang.
			stat (data, &statbuf);	
			if (!S_ISREG (statbuf.st_mode)) {
				g_print ("Error at line # %d\n", lineNum);
				g_print ("Error, %s is not a icon file.\n",
					 data);
				break;
			}
			Pixbuf = gdk_pixbuf_new_from_file_at_size (data, w, h, &error);
			if (Pixbuf == NULL) {
				g_print ("Error at line # %d\n", lineNum);
				g_print ("%s\n", error->message);
				g_error_free (error);
				error = NULL;
			}
			image = gtk_image_new_from_pixbuf (Pixbuf);
			gtk_image_menu_item_set_image (
						       (SubmenuItem), image);
			gtk_image_menu_item_set_always_show_image((SubmenuItem),TRUE);
			break;
		case 6:	// Insert separator into menu
			menuItem = gtk_separator_menu_item_new ();
			gtk_menu_shell_append (GTK_MENU_SHELL (menu[curDepth]), menuItem);
			Mode = 0;
			break;
		case 7:	// Change menu icon size
			i = atoi (data);
			if ((i < MIN_ICON_SIZE) || (i > MAX_ICON_SIZE)) {
				g_print ("Illegal size for menu icon.\n");
				Mode = -1;
				break;
			}
			w = i;
			h = i;
			g_print ("New icon size = %d.\n", w);
			Mode = 0;
			break;
		case 8:	// Set menu position
			if (Get2Numbers (data)) {
				bSetMenuPos = TRUE;
				g_print ("Menu position = %d, %d.\n", menuX, menuY);
				Mode = 0;
			}
			else {
				g_print ("Error reading menu position.\n");
				Mode = -1;
			}
			break;
		default:
			Mode = -1;
		}	// switch

		if (Mode == -1) {	// Error parsing file 
			// Placed here so that ReadLine is not called again
			g_print ("Error at line # %d\n", lineNum);
			g_print (">>>%s\n", Line);
			break;	// Quit reading file
		}
	}	// while

/*	fclose (pFile); */
g_strfreev(arcp);

	gtk_widget_show_all (menu[0]);

	g_signal_connect_swapped (menu[0], "deactivate",
				  G_CALLBACK (QuitMenu), NULL);
	
	while(!gtk_widget_get_visible(menu[0])) { // Keep trying until startup 
//		usleep(50000); 						  // button (or key) is released
//		if (bSetMenuPos)
		if (argc > 7)
				gtk_menu_popup (GTK_MENU (menu[0]), NULL, NULL,
				(GtkMenuPositionFunc) menu_position,
				NULL, 0, gtk_get_current_event_time ());
		else
				gtk_menu_popup (GTK_MENU (menu[0]), NULL, NULL, NULL, NULL, 0,
				gtk_get_current_event_time ());
		gtk_main_iteration();
        }

	gtk_main ();

	return 0;
	
}	// int main

// ----------------------------------------------------------------------
void menu_position (GtkMenu * menu, gint * x, gint * y, gboolean * push_in,
	 				gpointer data) {
// ----------------------------------------------------------------------
	int             menu_x = 0;
	int             menu_y = 0;
	GtkRequisition  requisition;
	GtkRequisition  req;
	GdkScreen      *screen;
	gtk_widget_get_preferred_size (GTK_WIDGET (menu), &requisition, &req);
/*	*x = menuX;
	*y = menuY;
	*push_in = TRUE; */
/*    int w, h;
	GtkRequisition req;
	gtk_widget_get_requisition (GTK_WIDGET(menu), &req);
    w = req.width;
    h = req.height;
        *x = ox;
        if (*x + w > gdk_screen_width())
            *x = ox + ax - w;
        *y = oy - h;
        if (*y < 0)
            *y = oy + ay;

        if (*y + h > gdk_screen_height())
			*y = oy + ay - h;
            
    *y = *y + wy;
    *x = *x + wx; */
    	screen = gtk_widget_get_screen (GTK_WIDGET (menu));
menu_x = wx;
menu_y = wy;
		if (gtk_widget_get_direction (GTK_WIDGET (menu)) != GTK_TEXT_DIR_RTL) {
			if (ox < ax &&
			    requisition.width < ox)
				menu_x += MIN (ox,
					       ax - requisition.width);
		} else {
			menu_x += ax - requisition.width;
			if (ox > 0 && ox < ax &&
			    ox < ax - requisition.width) {
				menu_x -= MIN (ax - ox,
					       ax - requisition.width);
			}
		}
		menu_x = MIN (menu_x, gdk_screen_get_width (screen) - requisition.width);

menu_y -= req.height;

	*x = menu_x;
	*y = menu_y;

    *push_in = TRUE;
}	// void menu_position

// ----------------------------------------------------------------------
gboolean Get2Numbers (char *data) {
// ----------------------------------------------------------------------
	int n, i;

	n = strlen (data);
	if ((n == 0) | !isdigit (data[0]))
		return FALSE;
	menuX = atoi (data);
	i = 0;
	
	// Skip over first number
	while (isdigit (data[i])) {
		i++;
		if (i == n)
			return FALSE;
	}
	
	// Find start of the next number
	while (!isdigit (data[i])) {
		i++;
		if (i == n)	return FALSE;
	}
	
	menuY = atoi (&data[i]);
	return TRUE;
}	// gboolean Get2Numbers

// ----------------------------------------------------------------------
static void RunItem (char *Cmd) {
// ----------------------------------------------------------------------
	GError *error = NULL;

	g_print ("Run: %s\n", Cmd);
    if (!Cmd) return;

	if (!g_spawn_command_line_async(Cmd, &error)) {
		g_print ("Error running command.\n");
		g_print ("%s\n", error->message);
		g_error_free (error);
	}
	gtk_main_quit ();
}	// static void RunItem

// ----------------------------------------------------------------------
static void QuitMenu (char *Msg) {
// ----------------------------------------------------------------------
	g_print ("Menu was deactivated.\n");

	gtk_main_quit ();
}	// static void QuitMenu

// ----------------------------------------------------------------------
int ReadLine (gchar **arcp) {
// ----------------------------------------------------------------------
	// Return kind of line, menu depth, and stripped text
	// return(-1) = Error, return(0) = EOF, return(>0) = keyword
	char *chop;
	int i, len, count, Kind;
	char tmp[MAX_LINE_LENGTH];
	char *str1, *str2;

	len = 0;
	while (len == 0) {
		// Read one line.
/*		if (fgets (Line, MAX_LINE_LENGTH, pFile) == NULL)
			return (0); */
			
		if (arcp[lineNum] == NULL) {return (0);}
gint ia; gint jj;		
		ia = strlen(arcp[lineNum]);
    	for (jj=0;jj < ia;jj++) { tmp[jj] = arcp[lineNum][jj]; }
    	tmp[jj] = '\0';
		lineNum++;

		// Remove comments
		chop = strchr (tmp, '#');
		if (chop != 0)
			*chop = '\0';

		len = strlen (tmp);
		
		// Remove trailing spaces & CR
		if (len > 0) {
			chop = &tmp[len - 1];
			while ((chop >= tmp) && (isspace (*chop) != 0)) {
				*chop = '\0';
				chop--;
			}
			len = strlen (tmp);
		}
	};

	// Big error?
/*	if (len >= MAX_LINE_LENGTH) {
		strncpy (data, tmp, MAX_LINE_LENGTH);
		data[MAX_LINE_LENGTH] = '\0';
		return (-1);
	} */

	count = 0;
	
	// Calculate menu depth
	for (i = 0; i < len; i++) {
		if (tmp[i] == ' ')
			count += 1;
		else if (tmp[i] == '\t')	// Tab character = 4 spaces
			count += 4;
		else
			break;
	};
	depth = count / 4;

	// Remove leading white space
	if (count > 0) {
		str1 = tmp;
		str2 = tmp;
		while ((*str2 == ' ') || (*str2 == '\t')) {
			str2++;
			len--;
		}
		for (i = 0; i <= len; i++)
			*str1++ = *str2++;
	}

	if (strncasecmp (tmp, "separator", 9) == 0) {		// Found 'separator'
		strcpy (data, tmp);
		return (5);
	}
	else if (strchr (tmp, '=') == NULL)	{ 				// Its a bad line
		strcpy (data, tmp);
		return (-1);
	}
	else if (strncasecmp (tmp, "iconsize", 8) == 0) {	// Found 'iconsize'
		Kind = 6;
	}
	else if (strncasecmp (tmp, "item", 4) == 0)	{ 		// Found 'item'
		Kind = 1;
	}
	else if (strncasecmp (tmp, "cmd", 3) == 0) {		// Found 'cmd'
		Kind = 2;
	}
	else if (strncasecmp (tmp, "icon", 4) == 0) {		// Found 'icon'
		Kind = 3;
	}
	else if (strncasecmp (tmp, "submenu", 7) == 0) {	// Found 'submenu'
		Kind = 4;
	}
	else if (strncasecmp (tmp, "menupos", 7) == 0) {	// Found 'menupos'
		Kind = 7;
	}
	else {												// Its a bad line
		strcpy (data, tmp);
		return (-1);
	}

	// Remove keywords and white space 
	str2 = strchr (tmp, '=') + 1;
	while ((*str2 == ' ') || (*str2 == '\t'))
		str2++;
	strcpy (data, str2);


	return (Kind);
}	// int ReadLine

// ---------------------------------------------------------------------
int all_ready_running (void)
// ---------------------------------------------------------------------
{
	struct flock fl;
	int fd;
    char *home;
    int n;
    int ret;
    char *Lock_path;

    // Use lock file 'LOCK_NAME' to see if program is already running.
    // 0 = No, 1 = Yes, 2 = Error

    // Place lock in the home directory of user
    // If user messes with "HOME", will probably have problems
    home = (char *) getenv("HOME");

	Lock_path = malloc(MAX_PATH_LEN + 1);
    n = snprintf(Lock_path, MAX_PATH_LEN, "%s/.%s", home, LOCK_NAME);

	if (n > MAX_PATH_LEN || Lock_path == NULL) {
        fprintf(stderr, "Error, path name too long: %s.\n", Lock_path);
        ret = 2;
        goto Done;
     }

	fd = open(Lock_path, O_RDWR | O_CREAT, 0600);
	if (fd < 0) {
        fprintf(stderr, "Error opening %s.\n", Lock_path);
        ret = 2;
        goto Done;
     }

	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;

	if (fcntl(fd, F_SETLK, &fl) < 0)
        ret = 1;
    else
        ret = 0;

Done:
    free(Lock_path);

    return ret;
} // all_ready_running
