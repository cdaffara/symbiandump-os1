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
//

#ifndef __SSMSIMSTATUSOBSERVER_H__
#define __SSMSIMSTATUSOBSERVER_H__

#include <e32base.h>
#include <ssm/ssmutility.h>
#include <ssm/ssmadaptationcli.h>
#include <ssm/ssmadaptation.hrh>

#include <ssm/startupdomainpskeys.h>

/**
 * Observes the SIM status and publishes the current status to a property value. 
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CSsmSimStatusObserver) : public CActive, public MSsmUtility
{
public:
	static IMPORT_C MSsmUtility* NewL();
	
	CSsmSimStatusObserver();
	virtual ~CSsmSimStatusObserver();
	
	// From MSsmUtility
	virtual void InitializeL();
	virtual void StartL();
	virtual void Release();
		
protected:
	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:
	void RequestSimStatusChangeL(TPSSimStatus aSimStatus);
private:
	RProperty iSimStatusProperty;
	RSsmSimAdaptation iSimAdaptation;
	
	TPckgBuf<TSsmSimEventType> iSimEventPckg;
};

#endif // __SSMSIMSTATUSOBSERVER_H__
