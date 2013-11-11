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

#ifndef _MP4MEMWRAP_H
#define _MP4MEMWRAP_H


#include <3gplibrary/mp4config.h>



#ifdef MP4_ARCH_WIN32 /* Windows compilation */

#include <stdlib.h>
#include <malloc.h>


void *mp4malloc(size_t size);
void *mp4realloc(void *memblock, size_t size, size_t oldsize);
void mp4free(void *mem);
void *mp4memcpy(void *dest, void *src, mp4_u32 count);


#else /* Symbian OS compilation */

#include <stddef.h>


void *mp4malloc(size_t size);
void *mp4realloc(void *memblock, size_t size, size_t oldsize);
void mp4free(void *mem);
void *mp4memcpy(void *dest, void *src, mp4_u32 count);


#endif

#endif
// End of File
