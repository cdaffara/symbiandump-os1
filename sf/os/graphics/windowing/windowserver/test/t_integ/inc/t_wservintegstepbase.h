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
 @internalComponent
*/

#ifndef __T_WSERV_INTEG_STEP_H__
#define __T_WSERV_INTEG_STEP_H__

#include <e32std.h>
#include <test/testexecutestepbase.h>
#include "t_wservconfigdata.h"


_LIT(KT_WServIntegStep,	"WServInteg");

/**
WServ Integ test commands
*/
class CT_WServIntegStep : public CTestStep
	{
public:
	CT_WServIntegStep();

protected:
	virtual ~CT_WServIntegStep();
	virtual enum TVerdict	doTestStepPreambleL();
	virtual enum TVerdict	doTestStepL();
	virtual enum TVerdict	doTestStepPostambleL();

protected:
	void ClearConfigData();
	void CleanUp();
	void GetConfigDataL(const TDesC& aData, const TDesC& aName);
	void PrintConfigDataL(const TDesC& /*aFileName*/);
	TInt CreateTestProcessL(const TDesC& aApp, const TProcessPriority& aPriority);
	void WaitForTestL(const TDesC& aFileName, const TInt& aMaxTimeSeconds = 120);
	void CheckResultsL(const TDesC& aData, const TDesC& aName,const TDesC& aFileName);
	void PrintResultsL(const TDesC& aFileName);
	void PrintTestStepName();

private:
	CT_WServConfigData 		iWServConfigData;
	RArray<RProcess> 		iProcessList;
	RPointerArray<TDesC>    iFileList;
	RFs						iFs;
	};

#endif // __T_WSERV_INTEG_STEP_H__
