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

#ifndef __SSMPROPERTYMONITOR_H_
#define __SSMPROPERTYMONITOR_H_

#include <e32base.h>
#include <ssm/ssmsystemwideproperty.h>

/**
 @internalComponent
 @released 
 */
NONSHARABLE_CLASS(CSsmPropertyMonitor) : public CActive
	{
public:
	static CSsmPropertyMonitor* NewL(CSsmSystemWideProperty& aObserver, TUint aSwpKey, TUid aProcessSid);
	~CSsmPropertyMonitor();
	
	TInt GetValue(TSsmSwp& aSwp) const;
	
	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	CSsmPropertyMonitor(CSsmSystemWideProperty& aObserver);
	void ConstructL(TUint aSwpKey, TUid aProcessSid);
	
private:
	CSsmSystemWideProperty& iObserver;
	RSsmSystemWideProperty iSwpRemote;
	TSsmSwp iSwpLocal;
	TBool iDefined;
	};

#endif

