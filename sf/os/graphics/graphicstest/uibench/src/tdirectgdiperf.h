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

#ifndef TDIRECTGDIPERF_H_
#define TDIRECTGDIPERF_H_

#include "tdirectgditestbase.h"

/**
Test case derived from CTDirectGdiTestBase to perform the
non-drawing performance of DirectGDI.
*/
class CTDirectGdiPerf : public CTDirectGdiTestBase
	{
public:
	CTDirectGdiPerf();
	~CTDirectGdiPerf();
		
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
	
	void CreatingTargetsL();	
	void MakeCurrentOnDifferentFormatTargetL();
	void MakeCurrentOnSameFormatTargetL();
	
	// Helpers
	void DoMakeCurrentTestL(TPtrC aTestName, TBool aMatchingFormats);
	
private:
	TDisplayMode iDisplayMode;
	};

_LIT(KTDirectGdiPerfTest,"TDirectGdiPerfTest");

#endif /*TDIRECTGDIPERF_H_*/
