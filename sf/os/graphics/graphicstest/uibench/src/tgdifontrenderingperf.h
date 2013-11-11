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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef __TGIFONTRENDERINGPERF_H__
#define __TGIFONTRENDERINGPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <gdi.h>
#include <shapeinfo.h>
#include <openfont.h>
#include <s32file.h>
#include <test/ttmsgraphicsstep.h>
#include <graphics/openfontrasterizer.h>
#include <graphics/openfontconstants.h>


//#include <e32base.h>

class CTGDIFontRenderingPerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTGDIFontRenderingPerf();
	~CTGDIFontRenderingPerf();
	
private:
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
		
	// GCP2 is an abbreviation used for GetCharacterPosition2()
	void GCP2IndicPerformanceTestL();
	void GCP2ReadingTextPerfL(TPtrC aPtr,const TDesC& aTestCaseName);
	void GCP2TypingTextPerfL(TPtrC aPtr,const TDesC& aTestCaseName);

	
	TVerdict ConfigSectionFailure(const TDesC16 &aErrorText);


private:

	CFont::TPositionParam 	iParam;

	//Values Read in from the ini file.
	TBuf<130> 			iTestText1;
	TBuf<30>			iTestText; 		//Unicode characters being read in.
	};
	
_LIT(KTGDIFontRenderingPerfName,"tgdifontrenderingperf");

#endif
