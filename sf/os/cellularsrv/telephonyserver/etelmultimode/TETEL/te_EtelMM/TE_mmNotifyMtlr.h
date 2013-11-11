// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_MMNOTIFYMTLR_H_
#define _TE_MMNOTIFYMTLR_H_

#include "TE_EtelMMTestStepBase.h"

	
class CTestNotifyMtlr : public CEtelMMLbsTestStepBase
	{
public:
	virtual enum TVerdict doTestStepL();
private:
	/**
		TNotifyMtlrV7 holds the details of the MTLR request being made by the client
	*/
	RMobileLocationServices::TNotifyMtlrV7   iNotifyMtlr;
 	/**
		iTestCount holds test count number
	*/
	TInt iTestCount;
	};

#endif
