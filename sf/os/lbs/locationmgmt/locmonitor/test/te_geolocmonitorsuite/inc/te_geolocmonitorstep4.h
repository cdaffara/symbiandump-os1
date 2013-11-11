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
 @file te_locmonitorstep4.h
*/
#ifndef TE_LOCMONITORSTEP4_H
#define TE_LOCMONITORSTEP4_H

#include <test/TestExecuteStepBase.h>
#include "te_geolocmonitorstepbase.h"

class CTe_LocMonitorStep4 : public CTe_LocMonitorStepBase
	{
	
public:

	CTe_LocMonitorStep4();
	~CTe_LocMonitorStep4();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	void CTe_LocMonitorStep4::ReadIniFileValues();

	TInt CTe_LocMonitorStep4::TestLocInfoAddWlanL();
	TInt CTe_LocMonitorStep4::TestLocInfoAddWcdmaCellL();
	TInt CTe_LocMonitorStep4::TestLocInfoAddGsmCellL();
	TInt CTe_LocMonitorStep4::TestValidateWlanInfoL();
	TInt CTe_LocMonitorStep4::TestWlanNewL();
	TInt CTe_LocMonitorStep4::TestValidateCoordInfoL();
	TInt CTe_LocMonitorStep4::TestCoordNewL();
	TInt CTe_LocMonitorStep4::TestValidateWCDMAInfoHierarchyL();
	TInt CTe_LocMonitorStep4::TestWcdmaAddNCellL();
	TInt CTe_LocMonitorStep4::TestWcdmaSetSC();
	TInt CTe_LocMonitorStep4::TestWcdmaNewL();
	TInt CTe_LocMonitorStep4::TestValidateGSMInfoHierarchyL();
	TInt CTe_LocMonitorStep4::TestGSMAddNCellL();
	TInt CTe_LocMonitorStep4::TestGSMSetRxLev();
	TInt CTe_LocMonitorStep4::TestGSMSetTA();
	TInt CTe_LocMonitorStep4::TestGSMNewL();
	TInt CTe_LocMonitorStep4::TestLocInfoAddCoordL();

	TInt iProcedure; 

private:

	TPositionInfo* iPosition;
	TPositionAreaInfo* iAreaInfo;
	TPosAreaReqParams* iAreaReqParams;
	
	};

_LIT(KLocMonitorStep4,"te_locmonitorstep4");

#endif
