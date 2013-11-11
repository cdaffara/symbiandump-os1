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
 @file unbanhwaddress.h
 @internalTechnology
*/
#ifndef __UNBANHWADDR_STEP_H__
#define __UNBANHWADDR_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"
#include <lbs.h>
#include <lbsCommon.h>



class CUnBanHwAddrStep : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CUnBanHwAddrStep();
	~CUnBanHwAddrStep();
	virtual TVerdict doTestStepL();


// Please add/modify your class members here:
private:
	};

_LIT(KUnBanHwAddrStep,"UnBanHWAddrStep");

#endif
