// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__CTELBEARER_H__)
#define __CTELBEARER_H__

#include <comms-infras/cbearerbase.h>

class CTelBearer : public CBearerBase
	{
public:
	virtual ~CTelBearer();
	static CTelBearer* NewLC(MBearerObserver* aObserver);
	static CTelBearer* NewL(MBearerObserver* aObserver);

	// implementation of base class pure virtuals
	virtual TBool StartChecking();
	virtual TBool StartChecking(TBool aIsReconnect);
	virtual TBool StopChecking();
	virtual void Disconnect();

	// override of base-class function
	TInt SecondPhaseAvailability();

protected:
	CTelBearer(MBearerObserver* aObserver);
	void ConstructL();

	// implementation of base class pure virtual
	virtual void AssignSupportedBearerSet();
	};

#endif //__CTELBEARER_H__

