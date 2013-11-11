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
 @file te_agpsmanagerstep32.h
*/
#if (!defined __TE_AGPSMANAGERSTEP32_H__)
#define __TE_AGPSMANAGERSTEP32_H__
#include <test/TestExecuteStepBase.h>
#include "te_earlyselflocpreventstepbase.h"

class CTe_AgpsManagerStep32 : public CTe_EarlySelfLocPreventStepBase
	{
public:
	CTe_AgpsManagerStep32();
	 		
private:
	virtual void doTestStepImplL();
	
	};

_LIT(KAgpsManagerStep32,"Te_AgpsManagerStep32");

#endif //__TE_AGPSMANAGERSTEP32_H__
