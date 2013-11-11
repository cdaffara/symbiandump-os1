// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the avdtp message base class
// 
//

/**
 @file
 @internalComponent
*/

#include <nifmbuf.h>
#include "avdtpMessage.h"

void CAvdtpMessage::AppendDataL(const TDesC8& aData)
	{
	TInt tmp = iPacket.Length();
	iPacket.AppendL(aData.Length());
	iPacket.CopyIn(aData,tmp);
	}

void CAvdtpMessage::AppendDataL(const TChar& aChar)
	{
	TBuf8<1> buf;
	buf.Append(aChar);
	AppendDataL(buf);
	}

CAvdtpMessage::~CAvdtpMessage()
	{
	iPacket.Free();
	}
		
