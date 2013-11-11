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

#ifndef __STARTUPADAPTATIONADAPTER_H__
#define __STARTUPADAPTATIONADAPTER_H__

#include <e32base.h>

#include <ssm/startupadaptationobserver.h>

class MStateAdaptation;
class MSimAdaptation;
class MRtcAdaptation;
class MEmergencyCallRfAdaptation;
class MMiscAdaptation;

class CAdaptationBase;

class CSaaStateAdaptation;
class CSaaSimAdaptation;
class CSaaRtcAdaptation;
class CSaaEmergencyCallRfAdaptation;
class CSaaMiscAdaptation;

class CStartupAdaptation;

/**
 * Defines the maximum number of adaptation classes that there are
 * 
 * @internalComponent
 */
const TInt KMaxAdaptationClasses = 5;

/**
 * Provides a class for managing the adaptation of the 5 adaptation interfaces
 * used by the adaptation server to the Core OS single interface adaptation
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CStartupAdaptationAdapter) : public CActive, MStartupAdaptationObserver
{
public:
	~CStartupAdaptationAdapter();

	static CStartupAdaptationAdapter* GetL();
	static void Destroy();
	
	IMPORT_C static MStateAdaptation* NewStateAdaptationL();
	IMPORT_C static MSimAdaptation* NewSimAdaptationL();
	IMPORT_C static MRtcAdaptation* NewRtcAdaptationL();
	IMPORT_C static MEmergencyCallRfAdaptation* NewEmergencyCallRfAdaptationL();
	IMPORT_C static MMiscAdaptation* NewMiscAdaptationL();
	
	void QueueDispatchL(CAdaptationBase* aAdaptation);
	void CancelDispatchedL(CAdaptationBase* aAdaptation);
	void RemoveFromDispatchQueue(CAdaptationBase* aAdaptation);

	// From MStartupAdaptationObserver
	virtual void ResponseL(const StartupAdaptation::TCommand aCommandId, TDesC8& aData);
	virtual void EventL(const StartupAdaptation::TEvent aEventId, TDesC8& aData);
	
protected:
	// From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
private:
	static CStartupAdaptationAdapter* NewLC();
	CStartupAdaptationAdapter();
	void ConstructL();
	
	CStartupAdaptation* GetAdaptationL();
	
	void DispatchQueued();
	
	void SelfComplete(TInt aError);
	
private:
	CSaaStateAdaptation* iStateAdaptation;
	CSaaSimAdaptation* iSimAdaptation;
	CSaaRtcAdaptation* iRtcAdaptation;
	CSaaEmergencyCallRfAdaptation* iEmergencyCallRfAdaptation;
	CSaaMiscAdaptation* iMiscAdaptation;
	
	CStartupAdaptation* iStartupAdaptation;
	
	CAdaptationBase* iDispatchedAdaptation;
	CAdaptationBase* iDispatchQueue[5];
	TInt iDispatchQueueSize;
};

#endif // __STARTUPADAPTATIONADAPTER_H__
