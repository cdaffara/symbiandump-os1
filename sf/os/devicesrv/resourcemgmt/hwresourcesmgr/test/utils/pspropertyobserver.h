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

#ifndef PSPOPERTYOBSERVER_H
#define PSPOPERTYOBSERVER_H

#include <e32base.h>
#include <e32property.h>

class MPsPropertyObserver
	{
public:
	virtual void PsPropertyChanged(TInt aKey, TInt aValue) =0;
	};
	
class CPsPropertyObserver : public CActive
	{
public:
	static CPsPropertyObserver* NewL(const TUid& aGroup, TInt aKey, MPsPropertyObserver& aObserver);
	void Start();
	~CPsPropertyObserver();
		
protected:
	CPsPropertyObserver(TInt aKey, MPsPropertyObserver& aObserver);
	void ConstructL(const TUid& aGroup);
	
protected:
	void RunL();
	void DoCancel();
	
private:
	RProperty iProperty;
	TInt iKey;
	TInt iValue;
	MPsPropertyObserver& iObserver;
	};

#endif
