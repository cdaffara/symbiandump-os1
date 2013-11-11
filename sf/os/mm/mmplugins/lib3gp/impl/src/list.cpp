// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "mp4list.h"
#include "mp4memwrap.h"



/*
 * Function:
 *
 *   list_s *listCreate()
 *
 * Description:
 *
 *   Create a list.
 *
 * Parameters:
 *
 *   None
 *
 * Return value:
 *
 *   Pointer to the allocated list.
 *
 */
list_s *listCreate()
{
  list_s *tmpList;

  if ((tmpList = (list_s *)mp4malloc(sizeof(list_s))) == NULL)
    return NULL;

  tmpList->elementsInList = 0;
  tmpList->bytesInList = 0;
  tmpList->cumulativeBytesInList = 0;
  tmpList->first = NULL;
  tmpList->last = NULL;

  return tmpList;
}


/*
 * Function:
 *
 *   void *listAppend(list_s *list,
 *                    void *data,
 *                    mp4_u32 dataSize)
 *
 * Description:
 *
 *   Append an element (void *) to the end of the list.
 *
 * Parameters:
 *
 *   list      Pointer to a list
 *   data      Pointer to add to the list
 *   dataSize  Size of data pointed to by data
 *
 * Return value:
 *
 *   NULL      Error
 *   Non-NULL  Success
 *
 */
void *listAppend(list_s *list, void *data, mp4_u32 dataSize)
{
  node_s *tmpNode;

  if ((tmpNode = (node_s *)mp4malloc(sizeof(node_s))) == NULL)
    return NULL;

  if (list->elementsInList == 0) /* List is empty */
    list->first = tmpNode;
  else
    list->last->next = tmpNode;

  list->last = tmpNode;
  list->elementsInList++;
  list->bytesInList += dataSize;
  list->cumulativeBytesInList += dataSize;

  tmpNode->next = NULL;
  tmpNode->data = data;
  tmpNode->dataSize = dataSize;

  return (void *)tmpNode;
}


/*
 * Function:
 *
 *   void listDeleteFirst(list_s *list)
 *
 * Description:
 *
 *   Delete the first element in the list.
 *
 * Parameters:
 *
 *   list   Pointer to a list
 *
 * Return value:
 *
 *   None
 *
 */
void listDeleteFirst(list_s *list)
{
  if (list->elementsInList == 1)
  {
	mp4free(list->first->data);
    mp4free(list->first);
    list->first = NULL;
    list->last = NULL;
    list->elementsInList = 0;
    list->bytesInList = 0;
  }

  if (list->elementsInList > 1)
  {
    node_s *tmpNode;

    tmpNode = list->first;
    list->bytesInList -= tmpNode->dataSize;
    list->first = tmpNode->next;
	mp4free(tmpNode->data);
    mp4free(tmpNode);
    list->elementsInList--;
  }
}


/*
 * Function:
 *
 *   void listDestroyList(list_s *list)
 *
 * Description:
 *
 *   Free list and all its elements from memory.
 *
 * Parameters:
 *
 *   list   Pointer to a list
 *
 * Return value:
 *
 *   None
 *
 */
void listDestroyList(list_s *list)
{
  while (list->elementsInList)
    listDeleteFirst(list);

  mp4free(list);
}


/*
 * Function:
 *
 *   mp4_u32 listElementsInList(list_s *list)
 *
 * Description:
 *
 *   Return the number of elements in the list.
 *
 * Parameters:
 *
 *   list   Pointer to a list
 *
 * Return value:
 *
 *   Number of elements in the list
 *
 */
mp4_u32 listElementsInList(list_s *list)
{
  return list->elementsInList;
}


/*
 * Function:
 *
 *   mp4_u32 listBytesInList(list_s *list)
 *
 * Description:
 *
 *   Return the number of bytes currently in list.
 *
 * Parameters:
 *
 *   list   Pointer to a list
 *
 * Return value:
 *
 *   Number of bytes in the list
 *
 */
mp4_u32 listBytesInList(list_s *list)
{
  return list->bytesInList;
}


/*
 * Function:
 *
 *   mp4_u32 listCumulativeBytesInList(list_s *list)
 *
 * Description:
 *
 *   Return the number of bytes that have been in the list since the list
 *   was created.
 *
 * Parameters:
 *
 *   list   Pointer to a list
 *
 * Return value:
 *
 *   Number of bytes in the list from the creation
 *
 */
mp4_u32 listCumulativeBytesInList(list_s *list)
{
  return list->cumulativeBytesInList;
}
// End of File
