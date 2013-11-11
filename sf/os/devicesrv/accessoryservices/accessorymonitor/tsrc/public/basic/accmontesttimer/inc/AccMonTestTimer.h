/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  headsetTestModule test module.
*
*/


#ifndef ACCMONTESTTIMER_H
#define ACCMONTESTTIMER_H 

#include <e32cons.h>

NONSHARABLE_CLASS( CAccMonTestTimer ) : public CActive
  	{
public:
		// Construction
	IMPORT_C static CAccMonTestTimer* NewLC();
	
		// From CActive
	void RunL();
	
	void DoCancel();
		// Destructor - virtual and class not intended
		// for derivation, so not exported
	IMPORT_C void WairForSecondsTest( TInt aSeconds );	
	
	~CAccMonTestTimer();
private:
		// C++ constructor - not exported;
		// implicitly called from NewLC()
	CAccMonTestTimer();
		// 2nd phase construction, called by NewLC()
	void ConstructL(); // second-phase constructor
private:
	RTimer iTimer;
	};


#endif	// ACCMONTESTTIMER_H