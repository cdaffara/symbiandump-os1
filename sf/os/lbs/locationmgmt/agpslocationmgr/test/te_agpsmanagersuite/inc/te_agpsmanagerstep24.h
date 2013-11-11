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
 @file te_agpsmanagerstep24.h
*/
#if (!defined __TE_AGPSMANAGERSTEP24_H__)
#define __TE_AGPSMANAGERSTEP24_H__
#include <test/testexecutestepbase.h>
#include "te_agpsmanagerstepbase.h"

class CTe_AgpsManagerStep24 : public CTe_AgpsManagerStepBase
	{
public:
	CTe_AgpsManagerStep24();
	~CTe_AgpsManagerStep24();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KAgpsManagerStep24,"Te_AgpsManagerStep24");

#endif
