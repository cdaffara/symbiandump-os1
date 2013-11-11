/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_lbsbackuprestoreSuiteStepBase.h
 @internalTechnology
*/

#ifndef __TE_LBSBACKUPRESTORE_STEP_BASE__
#define __TE_LBSBACKUPRESTORE_STEP_BASE__
#include <test/testexecutestepbase.h>

#include <lbs.h>
#include <lbserrors.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <centralrepository.h>


class CTe_lbsbackuprestoreSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_lbsbackuprestoreSuiteStepBase();
	CTe_lbsbackuprestoreSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	CActiveScheduler* iScheduler;
	};

#endif
