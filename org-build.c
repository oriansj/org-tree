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
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int ignore_hidden;
int ignore_case;
int sort_order;

struct gnode
{
	char* name;
	char* fullname;
	struct gnode* next;
};

void* xalloc(uint64_t count, uint64_t size)
{
	require(count > 0, "WTF?");
	require(size > 0, "WTF2?");
	void* r = calloc(count, size);
	require(NULL != r, "ran out of memory\n");
	return r;
}

char* xcopy(char* s)
{
	if(NULL == s) return NULL;
	char* r = xalloc(strlen(s) + 4, sizeof(char));
	strcat(r, s);
	return r;
}

int next(char* a, char* b)
{
	if(ignore_case)
	{
		return strcasecmp(a, b);
	}
	else
	{
		return strcmp(a, b);
	}
}

struct gnode* insert_gnode(struct gnode* head, struct gnode* n)
{
	if(NULL == head) return n;
	if(NULL == n) return head;
	if(sort_order)
	{
		if(next(head->name, n->name) <= 0)
		{
			n->next = head;
			return n;
		}
		else
		{
			head->next = insert_gnode(head->next, n);
			return head;
		}
	}
	else
	{
		if(next(head->name, n->name) >= 0)
		{
			n->next = head;
			return n;
		}
		else
		{
			head->next = insert_gnode(head->next, n);
			return head;
		}
	}
}

char* combine_strings(char* path, char* name)
{
	if(NULL == path) return xcopy(name);
	if(NULL == name) return xcopy(path);
	uint64_t size = strlen(path) + strlen(name) + 4;
	char* r = xalloc(size, sizeof(char));
	strcat(r, path);
	if(path[strlen(path)-1] != '/') strcat(r, "/");
	strcat(r,name);
	return r;
}

struct stat *getinfo(char *name)
{
	struct stat* lst = xalloc(1, sizeof(struct stat));
	int status = lstat(name, lst);
	if (status < 0) return NULL;
	return lst;
}

struct gnode* list_of_names(char* foldername)
{
	DIR* dir = opendir(foldername);
	if(NULL == dir) return NULL;

	struct dirent *ent;
	struct gnode* list = NULL;
	struct gnode* hold;
	do
	{
		ent = readdir (dir);
		if(NULL == ent) break;

		char* name = xcopy(ent->d_name);
		char* fullname = combine_strings(foldername, name);
		hold = xalloc(1, sizeof(struct gnode));
		hold->name = xcopy(ent->d_name);
		hold->fullname = fullname;
		list = insert_gnode(list, hold);
	} while (TRUE);
	closedir (dir);
	return list;
}

struct xfolder* read_directory(char* name, char* path, uint64_t depth)
{
	char* foldername;
	if(NULL == name)
	{
		foldername = path;
	}
	else if(match("..", name)) return NULL;
	else if(match(".", name)) return NULL;
	else if(ignore_hidden && ('.' == name[0])) return NULL;
	else foldername = combine_strings(path, name);

	struct stat* info;
	struct xfolder* r = xalloc(1, sizeof(struct xfolder));
	r->name = xcopy(name);
	r->depth = depth;

	struct gnode* contents = list_of_names(foldername);

	while(NULL != contents)
	{
		info = getinfo(contents->fullname);
		int mode = info->st_mode & S_IFMT;
		if(mode == S_IFDIR)
		{
			/* Is a folder */
			struct xfolder* hold = read_directory(contents->name, foldername, depth + 1);
			if(NULL != hold)
			{
				hold->next = r->child_folders;
				r->child_folders = hold;
			}
		}
		else if(mode == S_IFREG)
		{
			/* Is a file */
			struct xfile* hold = xalloc(1, sizeof(struct xfile));
			char* name = contents->name;
			require(NULL != name, "DAFAQ??");

			if(!ignore_hidden || ('.' != name[0]))
			{
				hold->name = xcopy(contents->name);
				hold->next = r->contents;
				r->contents = hold;
			}
		}
		else
		{
			fputs("don't support this yet", stderr);
		}
		contents = contents->next;
	}
	return r;
}
