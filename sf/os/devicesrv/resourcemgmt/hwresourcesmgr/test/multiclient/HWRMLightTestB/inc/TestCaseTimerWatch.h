/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef TestCaseTimerWATCH_H
#define TestCaseTimerWATCH_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// CONSTANTS

const TUid KMyHWRMTestPpoperty={0x10005B94};
const TUint32 EHWRMNextTestToRun = 0x00001001;
const TUint32 EHWRMNextStepToRun = 0x00001002;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


// Active object that tracks changes to the test case property

class CTestCaseTimerWatch : public CActive
	{
	enum {EPriority=0};
public:
	static CTestCaseTimerWatch* NewL();
private:
	CTestCaseTimerWatch();
	void ConstructL();
	~CTestCaseTimerWatch();
	void RunL();
	void DoCancel();
private:
	RProperty iProperty;
	//RTimer iTimer;
	};

#endif
