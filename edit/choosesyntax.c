/* User interface for syntax selection.

   Copyright (C) 2005 Leonard den Ottolander <leonard den ottolander nl>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>
#include "edit.h"
#include "../src/global.h"
#include "../src/wtools.h"

#define MAX_ENTRY_LEN 40
#define LIST_LINES 14
#define N_DFLT_ENTRIES 1

int
exec_edit_syntax_dialog (const char **names) {
    int i;

    Listbox *syntaxlist = create_listbox_window (MAX_ENTRY_LEN, LIST_LINES,
	N_(" Choose syntax highlighting "), NULL);
    LISTBOX_APPEND_TEXT (syntaxlist, 'A', N_("< Auto >"), NULL);

    for (i = 0; names[i]; i++) {
	LISTBOX_APPEND_TEXT (syntaxlist, 0, names[i], NULL);
	if (! option_auto_syntax && option_syntax_type &&
	    (strcmp (names[i], option_syntax_type) == 0))
    	    listbox_select_by_number (syntaxlist->list, i + N_DFLT_ENTRIES);
    }

    return run_listbox (syntaxlist);
}

void
edit_syntax_dialog (void) {
    char *old_syntax_type;
    int old_auto_syntax, syntax;
    char **names;
    int i;

    names = (char**) g_malloc (sizeof (char*));
    names[0] = NULL;
    /* We fill the list of syntax files every time the editor is invoked.
       Instead we could save the list to a file and update it once the syntax
       file gets updated (either by testing or by explicit user command). */
    edit_load_syntax (NULL, &names, NULL);

    if ((syntax = exec_edit_syntax_dialog ((const char**) names)) < 0) {
	for (i = 0; names[i]; i++) {
	    g_free (names[i]);
	}
	g_free (names);
	return;
    }

    old_auto_syntax = option_auto_syntax;
    old_syntax_type = g_strdup (option_syntax_type);

    /* Using a switch as we might want to define more specific commands, f.e.
       "Refill syntax list" (compare N_DFLT_ENTRIES). */
    switch (syntax) {
	case 0: /* auto syntax */
	    option_auto_syntax = 1;
	    break;
	default:
	    option_auto_syntax = 0;
	    g_free (option_syntax_type);
	    option_syntax_type = g_strdup (names[syntax - N_DFLT_ENTRIES]);
    }

    /* Load or unload syntax rules if the option has changed */
    if (option_auto_syntax && !old_auto_syntax || old_auto_syntax ||
	old_syntax_type && option_syntax_type &&
	(strcmp (old_syntax_type, option_syntax_type) != 0))
	edit_load_syntax (wedit, NULL, option_syntax_type);

    for (i = 0; names[i]; i++) {
	g_free (names[i]);
    }
    g_free (names);
    g_free (old_syntax_type);
}