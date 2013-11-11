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
 @file te_locmonitorstep5.h
*/
#ifndef TE_LOCMONITORSTEP5_H
#define TE_LOCMONITORSTEP5_H

#include <test/TestExecuteStepBase.h>
#include "te_locmonitorstepbase.h"

class CTe_LocMonitorStep5 : public CTe_LocMonitorStepBase
	{

public:

	CTe_LocMonitorStep5();
	~CTe_LocMonitorStep5();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:



	};

_LIT(KLocMonitorStep5,"te_locmonitorstep5");

#endif
