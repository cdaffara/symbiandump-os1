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
 
#ifndef TZORDERMULTIPLESURFACES_H
#define TZORDERMULTIPLESURFACES_H


#include "te_graphicsperformanceSuiteStepBase.h"

#include <w32std.h>
#include <e32msgqueue.h> 


const TInt KNumberOfMultiSurfacesProcesses = 2;

_LIT(KTZOrderMultipleSurfaces, "tzordermultiplesurfaces");

class CTZOrderMultipleSurfaces : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTZOrderMultipleSurfaces();
	virtual ~CTZOrderMultipleSurfaces();
	
private:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual void zOrderMultipleSurfacesSwitchingL();

private:
	RMsgQueue<TInt> iChildMsgQueues[KNumberOfMultiSurfacesProcesses];
	RMsgQueue<TInt> iParentMsgQueue;
 	};

#endif
