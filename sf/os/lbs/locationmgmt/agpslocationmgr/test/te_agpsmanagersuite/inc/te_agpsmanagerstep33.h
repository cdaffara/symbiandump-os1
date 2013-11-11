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
 @file te_agpsmanagerstep33.h
*/
#if (!defined __TE_AGPSMANAGERSTEP33_H__)
#define __TE_AGPSMANAGERSTEP33_H__
#include <test/TestExecuteStepBase.h>
#include "te_earlyselflocpreventstepbase.h"

class CTe_AgpsManagerStep33 : public CTe_EarlySelfLocPreventStepBase
	{
public:
	CTe_AgpsManagerStep33();
	 		
private:
	void doTestStepImplL();
		
	};

_LIT(KAgpsManagerStep33,"Te_AgpsManagerStep33");

#endif //__TE_AGPSMANAGERSTEP33_H__
