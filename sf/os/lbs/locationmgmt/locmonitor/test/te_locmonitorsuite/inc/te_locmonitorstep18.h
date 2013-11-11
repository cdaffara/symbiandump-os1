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
 @file te_locmonitorstep18h
*/
#ifndef TE_LOCMONITORSTEP18_H
#define TE_LOCMONITORSTEP18_H

#include <test/TestExecuteStepBase.h>
#include "te_locmonitorstepbase.h"

class CTe_LocMonitorStep18 : public CTe_LocMonitorStepBase
	{

public:

	CTe_LocMonitorStep18();
	~CTe_LocMonitorStep18();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:

	RPointerArray<TPositionSatelliteInfo> iPositions;
	RPointerArray<TPositionAreaExtendedInfo> iAreaInfo;
	RPointerArray<TPosAreaReqParams> iAreaReqParams;
	};

_LIT(KLocMonitorStep18,"te_locmonitorstep18");

#endif
