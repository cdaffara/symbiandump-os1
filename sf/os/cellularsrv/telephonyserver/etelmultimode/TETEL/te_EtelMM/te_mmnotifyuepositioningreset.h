// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/

#ifndef TE_MMNOTIFYUEPOSITIONINGRESET_H
#define TE_MMNOTIFYUEPOSITIONINGRESET_H

#include "TE_EtelMMTestStepBase.h"

	
class CTestNotifyUePositioningReset : public CEtelMMLbsTestStepBase
	{
public:
	virtual enum TVerdict doTestStepL();
private:
	/**
		iTestCount holds test count number
	*/	
	TInt iTestCount;		
	};
	
#endif
