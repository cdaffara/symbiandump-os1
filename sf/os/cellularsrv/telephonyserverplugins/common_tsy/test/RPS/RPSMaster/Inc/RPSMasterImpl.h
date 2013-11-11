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

#ifndef __RPSMasterImpl_H__
#define __RPSMasterImpl_H__

#include <e32base.h>
#include <etel.h>



#include "RPSMBase.h"
//
// CRPSMasterImpl.
// This class implements the MRPSMBase pure virtual class.
//
class CWriter;
class CRPSMasterImpl : public CBase, public MRPSMBase
	{
public:
	static CRPSMasterImpl* NewL();
	~CRPSMasterImpl();
	
	//from MRPSMBase
	
	// Requests
	virtual void doCallNumberL(const TDesC& aNumToDial, TEtelLine aLine);
	virtual void doAnswerNextCallL(TEtelLine aLine);
	virtual void doRejectNextCallL(TEtelLine aLine);
	virtual void doIgnoreNextCallL(TEtelLine aLine);
	
	virtual void doHangUpL(TEtelLine aLine, TInt aDelayInSec);
	virtual void doHoldL(TEtelLine aLine, TInt aDelayInSec);
	virtual void doResumeL(TEtelLine aLine, TInt aDelayInSec);
	
	virtual void doSendDataBackOnDataPortL(const TDesC& aDataToSend);
	
	// Ganged requests	
	virtual void doCallNumberHangUpL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec);
	virtual void doCallNumberHangUpBeforeConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec);
	virtual void doCallNumberHangUpAfterConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec);
	
	virtual void doAnswerNextCallHangUpL(TEtelLine aLine, TInt aDelayInSec);
	virtual void doAnswerNextCallHoldL(TEtelLine aLine, TInt aDelayInSec);
	virtual void doHangUpNextCallBeforeConnectL(TEtelLine aLine);

	virtual void doCleanupL();
	
	// Handshake requests
	virtual void doCheckRpsPresentL(TBool& aPresent);
	// Check presence of Slave
	virtual const TDesC16& doRpsSlaveTelNum()
							{ return iSlaveTelNumber; }
							
	virtual const TDesC16& doRpsSlaveTelNumData()
							{ return iSlaveTelNumberData; }
	
	virtual const TDesC16& doRpsSlaveTelNumFax()
							{ return iSlaveTelNumberFax; }
							
protected:
	void ReqstSlaveTelNumbersL();
private:
//	CRPSMasterImpl();
	void ConstructL();
	void HandShakeL();
	
private:
	CWriter* 		iWriter;		// Owned
	//CToAscii*		iToAscii;		// Owned. Static methods
	
	TBuf<100> 		iSlaveTelNumber;
	TBuf<100> 		iSlaveTelNumberData;
	TBuf<100> 		iSlaveTelNumberFax;
	};

#endif  // __RPSMasterImpl_H__
