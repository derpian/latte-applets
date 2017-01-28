#include <glib.h>
#include <stdio.h>

gint main(gint argc, gchar *argv[])
{
gboolean a;
a = true;
while (a)
	{
if (argc > 2){
	a = g_spawn_command_line_async(argv[1], NULL);
	a = g_spawn_command_line_sync(argv[2], NULL, NULL, NULL, NULL);
} else {
	if (argc > 1){
		a = g_spawn_command_line_sync(argv[1], NULL, NULL, NULL, NULL);
	} else { a = false; }
}
	}
	return 0;
}
