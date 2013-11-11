// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the class that completes sessions with
// LBS asynchronously.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLSESSIONCOMPLETER_H__
#define __SUPLSESSIONCOMPLETER_H__
#include <e32base.h>
#include <lbs/lbsnetcommon.h>

class MSuplProtocolManagerObserver;


/** Session Completer class

This class is used by the SUPL Procol Manager for completing sessions 
requested by LBS for procedures that are not supported by SUPL (like Transmit
to Third Party) and for procedures that do not require the use of SUPL (like
a self-locate request in Autonomous mode).

*/
NONSHARABLE_CLASS(CSuplSessionCompleter) : public CActive
	{

public:

	typedef struct {TLbsNetSessionId iSessionId;
					TInt iError;} 
				   TSuplCompleteDetails;

	static CSuplSessionCompleter* NewL(MSuplProtocolManagerObserver& aGateway);
	~CSuplSessionCompleter();

	void CompleteSession(const TLbsNetSessionId& aSessionId, TInt aError);
	
	// From CActive
	void RunL();
	void RunError();
	void DoCancel();

private:

	CSuplSessionCompleter(MSuplProtocolManagerObserver& aGateway);
	void ConstructL();
	
	void ActivateCompleter();

private:

	/** Protocol Manager Observer
	Reference to the Gateway Interface for 
	sending session completion messages
	to LBS	*/
	MSuplProtocolManagerObserver& iGateway;

	/** Queue of session IDs plus associated errors to be used when
	completing sessions
	*/
	RArray<TSuplCompleteDetails> iSessionInfo;
	};

#endif // __SUPLSESSIONCOMPLETER_H__
