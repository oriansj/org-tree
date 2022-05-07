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

FILE* output;

void print_leading_stars(uint64_t count, char* name)
{
	while(count > 0)
	{
		fputc('*', output);
		count = count - 1;
	}
	fputc(' ', output);
	fputs(name, output);
	fputc('\n', output);
}

void org_print_recursive(struct xfolder* head)
{
	struct xfile* hold;
	struct xfolder* sub;
	while(NULL != head)
	{
		print_leading_stars(head->depth+1, head->name);

		/* Get files first */
		hold = head->contents;
		while(NULL != hold)
		{
			fputs(hold->name, output);
			fputc('\n', output);
			hold = hold->next;
			if(NULL == hold) fputc('\n', output);
		}

		sub = head->child_folders;
		org_print_recursive(sub);

		head = head->next;
	}
}
