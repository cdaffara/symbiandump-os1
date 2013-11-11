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
 @publishedPartner
 @released
*/

#ifndef DELAY_H
#define DELAY_H

#include <e32base.h>

/**
Concrete Active Object to call back after a specified time delay.
This class is not intended for derivation.
*/
NONSHARABLE_CLASS(CDelay) : public CTimer
	{
public:
	IMPORT_C static CDelay* NewL(TCallBack& aCallback, TPriority aPriority);
	IMPORT_C ~CDelay();

private:
	CDelay(TCallBack& aCallback, TPriority aPriority);

	// From CTimer
	virtual void RunL();

private: 
	TCallBack iCallback;
	};

#endif // DELAY_H
