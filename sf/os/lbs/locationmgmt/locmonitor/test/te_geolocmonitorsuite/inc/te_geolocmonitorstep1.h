/*
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
 @file te_locmonitorstep1.h
*/
#ifndef TE_LOCMONITORSTEP1_H
#define TE_LOCMONITORSTEP1_H

#include <test/TestExecuteStepBase.h>
#include "te_geolocmonitorstepbase.h"

class CTe_LocMonitorStep1 : public CTe_LocMonitorStepBase
	{
public:
	CTe_LocMonitorStep1();
	~CTe_LocMonitorStep1();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	TInt TestValidateGSMInfoL(TInt iMCC, TInt iMNC, TInt iLAC, TInt iCI, TInt iTA);
	void CTe_LocMonitorStep1::ReadIniFileValues();
	
	TInt iMCC; 
	TInt iMNC; 
	TInt iLAC; 
	TInt iCID; 
	TInt iTA;
	TInt iExpectedError;
private:
	};

_LIT(KLocMonitorStep1,"te_locmonitorstep1");

#endif
