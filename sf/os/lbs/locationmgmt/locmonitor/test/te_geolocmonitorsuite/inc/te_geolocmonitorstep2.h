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
 @file te_locmonitorstep2.h
*/
#ifndef TE_LOCMONITORSTEP2_H
#define TE_LOCMONITORSTEP2_H

#include <test/TestExecuteStepBase.h>
#include "te_geolocmonitorstepbase.h"

class CTe_LocMonitorStep2 : public CTe_LocMonitorStepBase
	{
public:
	CTe_LocMonitorStep2();
	~CTe_LocMonitorStep2();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	TInt TestValidateWCDMAInfoL(TInt iMCC, TInt iMNC, TInt iUCID, TInt iSC);
	void ReadIniFileValues();
	
	TInt iMCC; 
	TInt iMNC; 
	TInt iLAC; 
	TInt iUCID; 
	TInt iSC; 
	TInt iExpectedError;
	
private:

	TPositionSatelliteInfo* iPosition;
	TPositionAreaExtendedInfo* iAreaInfo;
	TPosAreaReqParams* iAreaReqParams;
	};

_LIT(KLocMonitorStep2,"te_locmonitorstep2");

#endif
