/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file LocIndicatorLib0Step.h
 @internalTechnology
*/
#if (!defined __LOCINDICATORLIB0_STEP_H__)
#define __LOCINDICATORLIB0_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LocIndicatorLibSuiteStepBase.h"
#include <lbs/epos_intgpshwstatus.h>
#include <lbs/locationfwdomainpskeys.h>

class CLocIndicatorLib0Step : public CTe_LocIndicatorLibSuiteStepBase, public MPosIntGpsHwStatusObserver
	{
public:
	CLocIndicatorLib0Step();
	~CLocIndicatorLib0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	

	// From MPosIntGpsHwStatusObserver
	virtual void OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt aError);

// Please add/modify your class members here:
private:
	TBool CompareEnums(CPosIntGpsHwStatus::TIntGpsHwStatus, TPosIntGpsHwState);
	
private:
	CPosIntGpsHwStatus::TIntGpsHwStatus iHwStatus;
	};

_LIT(KLocIndicatorLib0Step,"LocIndicatorLib0Step");

#endif
