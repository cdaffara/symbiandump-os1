// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMSTATEMANAGER_H__
#define __SSMSTATEMANAGER_H__

#include <e32std.h>

#include <ssm/ssmstatetransition.h>
#include <ssm/ssmswp.h>

/**
 RSsmaStateManager is provided as an API used to request changes to the 
 system state or to request changes to the value of a system-wide property.
 The policy plug-in associated with the current system state or the requested 
 system wide property implements the required security policy in 
 MSsmStatePolicy::TransitionAllowed() or MSsmSwpPolicy::TransitionAllowed().
 
 @see MSsmStatePolicy
 @see MSsmSwpPolicy
 @publishedPartner
 @released
 */
NONSHARABLE_CLASS(RSsmStateManager) : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(TInt aAsyncMessageSlotCount);
	IMPORT_C void Close();
	IMPORT_C TInt RequestStateTransition(TSsmStateTransition aRequest);
	IMPORT_C void RequestStateTransition(TSsmStateTransition aRequest, TRequestStatus& aStatus);
	IMPORT_C void RequestStateTransitionCancel();
	IMPORT_C TInt RequestSwpChange(TSsmSwp aSwp);
	IMPORT_C void RequestSwpChange(TSsmSwp aSwp, TRequestStatus& aStatus);
	IMPORT_C void RequestSwpChangeCancel();
	IMPORT_C TInt RegisterSwpMapping(TUint aSwpKey, const TDesC& aFilename);

protected:
	TInt DoConnect(const TDesC& aServerName, TVersion aVersion, TInt aAsyncMessageSlots);
private:
	TVersion Version() const;
	};

#endif



