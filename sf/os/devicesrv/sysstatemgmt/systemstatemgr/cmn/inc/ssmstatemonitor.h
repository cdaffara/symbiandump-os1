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

#ifndef __SSMSTATEMONITOR_H_
#define __SSMSTATEMONITOR_H_

#include <e32base.h>
#include <domaindefs.h>

class CSsmStateAwareSession;

/**
 @internalComponent
 @released 
 */
NONSHARABLE_CLASS(CSsmStateMonitor) : public CActive
	{
public:
	static CSsmStateMonitor* NewL(CSsmStateAwareSession& aObserver, TDmDomainId aId);
	~CSsmStateMonitor();
	
	TSsmState State() const;
	
	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	CSsmStateMonitor(CSsmStateAwareSession& aObserver);
	void ConstructL(TDmDomainId aId);
	
private:
	CSsmStateAwareSession& iObserver;
	RSsmStateAwareSession iSas;
	TSsmState iState;
	};

#endif

