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

#ifndef __RPSMBASE_H__
#define __RPSMBASE_H__

#include <e32base.h>
#include "rpsasciirqstdefs.h"


//
// MRPSMBase.
// This is the abstract base class offering Remote Party Simulator functionality.
// It is designed to be inherited by concrete classes that implement functionality
// required by the TRP/TSY Test Execute test-steps.
//

class MRPSMBase
	{
public:

	virtual void doCallNumberL(const TDesC& aNumToDial, TEtelLine aLine) =0;
	
	virtual void doAnswerNextCallL(TEtelLine aLine) =0;
	virtual void doRejectNextCallL(TEtelLine aLine) =0;
	virtual void doIgnoreNextCallL(TEtelLine aLine) =0;
	
	virtual void doHangUpL(TEtelLine aLine, TInt aDelayInSec) =0;
	virtual void doHoldL(TEtelLine aLine, TInt aDelayInSec) =0;
	virtual void doResumeL(TEtelLine aLine, TInt aDelayInSec) =0;
	
	virtual void doSendDataBackOnDataPortL(const TDesC& aDataToSend) =0;
	
	virtual void doCallNumberHangUpL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec) =0;
	virtual void doCallNumberHangUpBeforeConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec) =0;
	virtual void doCallNumberHangUpAfterConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec) =0;
	
	virtual void doAnswerNextCallHangUpL(TEtelLine aLine, TInt aDelayInSec) =0;
	virtual void doHangUpNextCallBeforeConnectL(TEtelLine aLine) =0;
	virtual void doAnswerNextCallHoldL(TEtelLine aLine, TInt aDelayInSec) = 0;

	virtual void doCleanupL() = 0;
	
	virtual void doCheckRpsPresentL(TBool& aPresent) =0;
	
	virtual const TDesC16& doRpsSlaveTelNum() =0;
	virtual const TDesC16& doRpsSlaveTelNumData() =0;
	virtual const TDesC16& doRpsSlaveTelNumFax() =0;
	
	};

#endif  // __RPSMBASE_H__
