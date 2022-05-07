/* Copyright (C) 2022 Jeremiah Orians
 * This file is part of org-tree.
 *
 * org-tree is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * org-tree is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with org-tree.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "org-tree.h"

void prereads(int argc, char** argv)
{
	int i = 1;
	while(i <= argc)
	{
		if(NULL == argv[i])
		{
			i = i + 1;
		}
		else if(match("--show-hidden", argv[i]))
		{
			ignore_hidden = FALSE;
			i = i + 1;
		}
		else if(match("--ignore-case", argv[i]))
		{
			ignore_case = TRUE;
			i = i + 1;
		}
		else if(match("--reverse-order", argv[i]))
		{
			sort_order = FALSE;
			i = i + 1;
		}
		else
		{
			i = i + 1;
		}
	}
}

int main(int argc, char** argv)
{
	ignore_hidden = TRUE;
	ignore_case = FALSE;
	sort_order = TRUE;
	output = stdout;

	prereads(argc, argv);

	char* name;
	struct xfolder* head = NULL;
	struct xfolder* hold;

	int i = 1;
	while(i <= argc)
	{
		if(NULL == argv[i])
		{
			i = i + 1;
		}
		else if(match("--path", argv[i]))
		{
			name = argv[i + 1];
			hold = read_directory(NULL, name, 0);
			hold->name = name;
			if(NULL == head)
			{
				head = hold;
			}
			else
			{
				hold->next = head;
				head = hold;
			}

			i = i + 2;
		}
		else if(match("--output", argv[i]))
		{
			name = argv[i+1];
			output = fopen(name, "w");
			i = i + 2;
		}
		else if(match("--show-hidden", argv[i]))
		{
			i = i + 1;
		}
		else if(match("--ignore-case", argv[i]))
		{
			i = i + 1;
		}
		else if(match("--reverse-order", argv[i]))
		{
			i = i + 1;
		}
		else
		{
			fputs("UNKNOWN ARGUMENT\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	org_print_recursive(head);

	return EXIT_SUCCESS;
}
