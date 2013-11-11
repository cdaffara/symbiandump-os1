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
 @file te_locmonitorstep28.h
*/
#ifndef TE_LOCMONITORSTEP28_H
#define TE_LOCMONITORSTEP28_H

#include <test/TestExecuteStepBase.h>
#include "te_locmonitorstepbase.h"

class CTe_LocMonitorStep28 : public CTe_LocMonitorStepBase
	{
public:
	CTe_LocMonitorStep28();
	~CTe_LocMonitorStep28();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:

	TPositionSatelliteInfo* iPosition;
	TPositionAreaExtendedInfo* iAreaInfo;
	TPosAreaReqParams* iAreaReqParams;
	};

_LIT(KLocMonitorStep28,"te_locmonitorstep28");

#endif
