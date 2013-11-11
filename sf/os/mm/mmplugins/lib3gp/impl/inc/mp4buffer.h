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

#ifndef _MP4BUFFER_H
#define _MP4BUFFER_H

#include <3gplibrary/mp4config.h>
#include <3gplibrary/mp4lib.h>

mp4_i32 addData(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite);
mp4_u32 getBufferedBytes(MP4HandleImp handle);
mp4_u32 getCumulativeBufferedBytes(MP4HandleImp handle);
mp4_i32 readData(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread);
mp4_i32 peekData(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread);
mp4_i32 discardData(MP4HandleImp handle, mp4_i32 bytesToDiscard);

#endif
// End of File
