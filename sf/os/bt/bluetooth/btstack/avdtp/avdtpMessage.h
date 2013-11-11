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
// Defines the avdtp message base class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPMESSAGE_H
#define AVDTPMESSAGE_H

#include <es_mbuf.h>

NONSHARABLE_CLASS(CAvdtpMessage) : public CBase
	{
public:
	/**
	@return KErrNone if successful and data as expected
			KErrNotReady if data not as expected (N.B. could indicate an abort message)
	*/
	virtual TInt NewData(TUint aCount/*RMBufChain& aFragment*/)=0;
	void AppendDataL(const TDesC8& aData);
	void AppendDataL(const TChar& aChar);
	
	inline RMBufChain& Data();
	virtual ~CAvdtpMessage();
	
protected:
	void AppendNewData(RMBufChain& aFragment);
	TPtrC8 DataPtr(TInt aFrom); // MBuf helper (!) which operates on the packet to the end
	TPtrC8 DataPtr(TInt aFrom, TInt aTo); // MBuf helper (!) which operates on the packet
	

private:
	RMBufChain	iPacket; // new packets might join with this to defrag
	};
	
RMBufChain& CAvdtpMessage::Data()
	{
	return iPacket;
	}

#endif //AVDTPMESSAGE_H
