/*
    Sysmon.app - system monitoring dockapp for WindowMaker
    Copyright (C) 2018  David Slusky

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __LIST_H_
#define __LIST_H_

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
# define INLINE inline
#else
# define INLINE
#endif

typedef struct LinkedList {
  void *head;
  struct LinkedList *tail;
} LinkedList;

LinkedList* list_cons(void* head, LinkedList* tail);

int list_length(LinkedList* list);

void* list_nth(int index, LinkedList* list);

void list_remove_head(LinkedList** list);

LinkedList *list_remove_elem(LinkedList* list, void* elem);

void list_mapcar(LinkedList* list, void(*function)(void*));

LinkedList*list_find(LinkedList* list, void* elem);

void list_free(LinkedList* list);

#endif
