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

#ifndef _MP4FILE_H
#define _MP4FILE_H


#include <3gplibrary/mp4config.h>
#include <3gplibrary/mp4lib.h>
#include "mp4atom.h"



mp4_i32 saveFileName(MP4FileName filename, MP4HandleImp handle);
mp4_i32 initFileRead(MP4FileName filename, MP4HandleImp handle);
mp4_i32 initFileWrite(MP4FileName filename, MP4HandleImp handle);
mp4_i32 initTmpFileWrite(MP4FileName filename, MP4HandleImp handle);
mp4_i32 closeFile(MP4HandleImp handle);
mp4_i32 closeTmpFile(MP4HandleImp handle);
mp4_i32 deleteTmpFile(MP4HandleImp handle);
mp4_i32 readFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread);
mp4_i32 readTmpFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread);
mp4_i32 peekFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread);
mp4_i32 seekFile(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 seekFileAbs(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 seekFileWrite(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 seekFileAbsWrite(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 seekTmpFileAbs(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 writeFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite);
mp4_i32 writeFileUnbuffered(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite);
mp4_i32 writeTmpFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite);
mp4_i32 createTmpFileName(MP4FileName filename, MP4FileName *tmpfilename);
mp4_i32 freeTmpFileName(MP4FileName filename);
void    closeStdlib();
mp4_i32 bufferedRead(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread);
mp4_i32 bufferedSeek(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 bufferedSeekAbs(MP4HandleImp handle, mp4_i64 amount);
mp4_i32 bufferedWrite(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite);

mp4_i32 initMetaDataFiles(MP4HandleImp handle);
mp4_i32 writeMetaDataFileNum(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite, mp4_u32 filenumber);
mp4_i32 seekMetaDataFileNumAbs(MP4HandleImp handle, mp4_i64 amount, mp4_u32 filenumber);
mp4_i32 readMetaDataFileNum(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread, mp4_u32 filenumber);
mp4_i32 deleteMetaDataFiles(MP4HandleImp handle);
mp4_i32 closeMetaDataFiles(MP4HandleImp handle);
mp4_i32	openStdlib();

TInt RecommendedBufferSize(MP4HandleImp aHandle);

#endif
// End of File
