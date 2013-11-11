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

#ifndef _MP4LIST_H
#define _MP4LIST_H


#include <3gplibrary/mp4config.h>



typedef struct node_s
{
  struct node_s   *next;
  void            *data;
  mp4_u32         dataSize;
} node_s;


typedef struct list_s
{
  mp4_u32   elementsInList;
  mp4_u32   bytesInList;
  mp4_u32   cumulativeBytesInList;
  node_s    *first;
  node_s    *last;
} list_s;


list_s    *listCreate();
void      *listAppend(list_s *list, void *data, mp4_u32 datasize);
void      listDeleteFirst(list_s *list);
void      listDestroyList(list_s *list);
mp4_u32   listElementsInList(list_s *list);
mp4_u32   listBytesInList(list_s *list);
mp4_u32   listCumulativeBytesInList(list_s *list);


#endif
// End of File
