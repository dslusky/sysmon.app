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

#include "list.h"
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <stdlib.h>

/* Return a cons cell produced from (head . tail) */

INLINE LinkedList*
list_cons(void* head, LinkedList* tail)
{
  LinkedList* cell;

  cell = (LinkedList*)malloc(sizeof(LinkedList));
  cell->head = head;
  cell->tail = tail;
  return cell;
}

/* Return the length of a list, list_length(NULL) returns zero */

INLINE int
list_length(LinkedList* list)
{
  int i = 0;
  while(list)
    {
      i += 1;
      list = list->tail;
    }
  return i;
}

/* Return the Nth element of LIST, where N count from zero.  If N
   larger than the list length, NULL is returned  */

INLINE void*
list_nth(int index, LinkedList* list)
{
  while(index-- != 0)
    {
      if(list->tail)
	list = list->tail;
      else
	return 0;
    }
  return list->head;
}

/* Remove the element at the head by replacing it by its successor */

INLINE void
list_remove_head(LinkedList** list)
{
  if (!*list) return;
  if ((*list)->tail)
    {
      LinkedList* tail = (*list)->tail; /* fetch next */
      *(*list) = *tail;		/* copy next to list head */
      free(tail);			/* free next */
    }
  else				/* only one element in list */
    {
      free(*list);
      (*list) = 0;
    }
}


/* Remove the element with `car' set to ELEMENT */

INLINE LinkedList *
list_remove_elem(LinkedList* list, void* elem)
{
    LinkedList *tmp;

    if (list) {
	if (list->head == elem) {
	    tmp = list->tail;
	    free(list);
	    return tmp;
	}
	list->tail = list_remove_elem(list->tail, elem);
	return list;
    }
    return NULL;
}


/* Return element that has ELEM as car */

INLINE LinkedList*
list_find(LinkedList* list, void* elem)
{
  while(list)
    {
    if (list->head == elem)
      return list;
    list = list->tail;
    }
  return NULL;
}

/* Free list (backwards recursive) */

INLINE void
list_free(LinkedList* list)
{
  if(list)
    {
      list_free(list->tail);
      free(list);
    }
}

/* Map FUNCTION over all elements in LIST */

INLINE void
list_mapcar(LinkedList* list, void(*function)(void*))
{
  while(list)
    {
      (*function)(list->head);
      list = list->tail;
    }
}
