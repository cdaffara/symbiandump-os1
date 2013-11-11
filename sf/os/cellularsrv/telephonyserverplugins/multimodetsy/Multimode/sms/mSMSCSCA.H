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
//

/**
 @file
 @internalAll 
*/

#ifndef __MSMSCSCA_H__
#define __MSMSCSCA_H__

#include "smsbase.H"
#include "mmlist.h"

class CATSmsGetSCAddress : public CATSmsCommands
	{
public:
	struct TTsySmsp
		{
		TInt* iBufSize;		// Size of the streamed SMSP list, in bytes.
		CBufBase** iBufPtr; // Streamd SMSP list
		};

	static CATSmsGetSCAddress* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals);

protected:
	// Framework stuff from CATBase...
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void Stop(TTsyReqHandle aTsyReqHandle);
	void Complete(TInt aError);
	virtual void Complete(TInt aError, TEventSource aSource);
	
private:
	// Constructor
	CATSmsGetSCAddress(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aGlobals);

	// Utility functions
	void CreateSmspListL();

private:
	// Pointer to data passed down by client (this is how we report the back SCA to the client)
	// We do not own this data.
	RMobilePhone::TMobileAddress iSCAddress;	
	TTsySmsp iSmspInfo;
	};

#endif
