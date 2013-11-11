// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the header file of the Sms socket Buffers.
// 
//

/**
 @file
*/


#if !defined (__SMSUSTRM_H__)
#define __SMSUSTRM_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__S32BUF_H__)
#include <s32buf.h>
#endif

#if !defined(__S32STRM_H__)
#include <s32strm.h>
#endif


class RSocket;

/**
 *  @publishedAll
 *  @released
 */
const TInt KSmsMaxSegmentLength=0x100;


/**
 *  An SMS stream buffer to read and write over a socket.
 *  @publishedAll
 *  @released 
 */
class RSmsSocketBuf : public TStreamBuf
	{
public:
	RSmsSocketBuf(RSocket& aSocket);
private:
// from TStreamBuf
	TInt UnderflowL(TInt aMaxLength);
	void OverflowL();
	void DoSynchL();
private:
	void SocketWriteL();
//
	inline TInt Lag(TRead) const;
	inline TInt Lag(TWrite) const;
private:
	RSocket& iSocket;
	TUint8 iBuffer[KSmsMaxSegmentLength];
	};


/**
 *  Stream that reads a CSmsMessage object across a socket.
 *  @publishedAll
 *  @released 
 */
class RSmsSocketReadStream : public RReadStream
	{
public:
	IMPORT_C RSmsSocketReadStream(RSocket& aSocket);
private:
	RSmsSocketBuf iBuf;
	};

/**
 *  Stream that writes a CSmsMessage object across a socket.
 *  @publishedAll
 *  @released 
 */
class RSmsSocketWriteStream : public RWriteStream
	{
public:
	IMPORT_C RSmsSocketWriteStream(RSocket& aSocket);
private:
	RSmsSocketBuf iBuf;
	};

#include "smsustrm.inl"

#endif // !defined __SMSUSTRM_H__
