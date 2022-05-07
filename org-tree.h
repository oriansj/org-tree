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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "M2libc/bootstrappable.h"

struct xfile
{
	char* name;
	struct xfile* next;
};

struct xfolder
{
	uint64_t depth;
	char* name;
	struct xfolder* child_folders;
	struct xfile* contents;
	struct xfolder* next;
};

extern int ignore_hidden;
extern int ignore_case;
extern int sort_order;
extern FILE* output;

struct xfolder* read_directory(char* name, char* path, uint64_t depth);
void org_print_recursive(struct xfolder* head);
