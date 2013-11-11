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



/**
 @file
 @publishedPartner
 @deprecated
*/

#ifndef _MP4CONFIG_H
#define _MP4CONFIG_H

#include <e32def.h>
#include <e32std.h>
#include <libc/stddef.h>

typedef TUint8                  mp4_u8;
typedef TUint16                 mp4_u16;
typedef TUint32                 mp4_u32;
typedef TInt64                  mp4_u64;
typedef TInt8                   mp4_i8;
typedef TInt16                  mp4_i16;
typedef TInt32                  mp4_i32;
typedef TInt64                  mp4_i64;

typedef TReal                   mp4_double;

typedef TBool                   mp4_bool;

typedef wchar_t *               MP4FileName;
typedef wchar_t *               MP4String;

typedef TInt32                  MP4Err;

/** Structure containing video frame properties */	
struct TFrameInfoParameters
{
	/** Frame size */
	mp4_u32 iSize;
	/** Frame start time */
	mp4_u32 iStartTime;
	/** Frame type */
	mp4_bool iType;
};

/** Setup info struct for video composition */
struct TVideoClipProperties
{
	/** Indicates the H263 level */
    mp4_u8 iH263Level;    
};

#endif
// End of File
