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
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef TZORDER_H
#define TZORDER_H


#include "te_graphicsperformanceSuiteStepBase.h"

#include <w32std.h>
#include <e32msgqueue.h> 


_LIT( KTZOrder, "tzorder");
const TInt KNumberOfProcesses = 3;

class CTZOrder : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTZOrder();
	virtual ~CTZOrder();
	
private:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual void zOrderSwitchingL();

private:    
	RMsgQueue<TInt> iChildMsgQueues[KNumberOfProcesses];
	RMsgQueue<TInt> iParentMsgQueue;
 	};

#endif
