// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _TE_MMCONFERENCE_H_
#define _TE_MMCONFERENCE_H_

#include "TE_EtelMMTestStepBase.h"

class CTestConferenceCall : public CEtelMMTestStepBase
	{
public:

	TInt iTestCount;
	CTestConferenceCall() ;
	~CTestConferenceCall(){} ;
	virtual enum TVerdict doTestStepL();

	};

#endif
