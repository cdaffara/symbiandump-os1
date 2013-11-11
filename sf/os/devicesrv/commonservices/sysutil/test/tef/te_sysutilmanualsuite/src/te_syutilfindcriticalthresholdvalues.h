/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file 
 @internalTechnology
*/

#if (!defined __TE_SYSUTILFINDCRITICALTHRESHOLDVALUES_H__)
#define __TE_SYSUTILFINDCRITICALTHRESHOLDVALUES_H__
#include "te_sysutilsmanualtestepbase.h"
#include <bafl/sysutil.h>
#include <centralrepository.h>
class CSysUtilsFindCriticalThresholdValuesStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsFindCriticalThresholdValuesStep();
	~CSysUtilsFindCriticalThresholdValuesStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	void processResult(int err, TBool aStatus);
	TInt testStep1(TInt aDrive);
	TInt testStep2(TInt aDrive);
	TInt testStep3(TInt aDrive);
	TInt CreateRepositoryValues(CRepository *aRepository, const TUint32 aKey, TInt aValue );
	TInt CreateRepositoryEntries(CRepository *aRepository, TInt aDrive);
	TInt CreateValuesToWrite(TInt aDrive, TInt aRamThreshold, TInt aOtherThreshold);
	TInt CallDiskThresholdCheck(TInt aDrive, TInt aDiskAbove, TInt aDiskBelow);
	TInt CallFFSThresholdCheck(TInt aFFSAbove, TInt aFFSBelow);
	TInt CreatePSValues(const TUid aUid, const TUint32 aKey, TInt aValue);
	
	};

_LIT(KSysUtilsFindCriticalThresholdValuesStep,"SysUtilsFindCriticalThresholdValues");

#endif
