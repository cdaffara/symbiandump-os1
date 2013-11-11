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
 @internalComponent
*/

#ifndef __WRITER_H__
#define __WRITER_H__

#include <e32base.h>
#include <badesca.h>
#include "rpscommon.h"

//#include <RPSAsciiRqstDefs.h>

class CSender;


//
// CWriter.
// Offers clients the ability to send data.
// Packages data components into a message before sending to the Sender.
// 
//
class CWriter : public CBase
	{
public:
	static CWriter* NewL();
	~CWriter();
	
	void IssueRequestL(const TDesC8& aOpCode, CDesCArrayFlat& aArgArray);
	void IssueRequestL(const TDesC8& aOpCode, CDesCArrayFlat& aArgArray, CRpsMsg& aRespMsg);
	
private:
	void ConstructL();
	TUint NextMsgNum();

private:
	CSender*		iSender;  		// Owned
	TUint 			iMsgNumCntr;
	};

	
//
// CToAscii.
// This class translates requests into ASCII string
//
class CToAscii : public CBase
	{
public:
	static const TDesC8& CallNumber();
	static const TDesC8& AnswerNextCall();
	static const TDesC8& RejectNextCall();
	static const TDesC8& IgnoreNextCall();
	static const TDesC8& HangUp();
	static const TDesC8& Hold();
	static const TDesC8& Resume();
	static const TDesC8& SendDataBackOnDataPort();
	static const TDesC8& CallNumberHangUp();
	static const TDesC8& CallNumberHangUpBeforeConnect();
	static const TDesC8& CallNumberHangUpAfterConnect();
	static const TDesC8& AnswerNextCallHangUp();
	static const TDesC8& ReqstSlaveTelNumbers();
	static const TDesC8& AnswerNextCallHold();
	static const TDesC8& CheckRpsPresent();
	static const TDesC8& HangUpNextCallBeforeConnect();
	static const TDesC8& Cleanup();
	};

#endif  // __WRITER_H__
