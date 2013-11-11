// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file with the declartions of the RConnection test step classes that basically 
// aren't proper tests at all - thet are just helper functions encapsulated as test steps so that they
// can be called from the scripts.
// 
//

/**
 @file
*/

#if (!defined __TE_RConnectionHELPERSTEPS_H__)
#define __TE_RConnectionHELPERTESTSTEPS_H__

#include "TE_RConnectionStep.h"

class TE_RConnectionForceCCoverWrite : public TE_RConnectionStep
{
public:
	TE_RConnectionForceCCoverWrite(TPtrC aName);
	virtual ~TE_RConnectionForceCCoverWrite();

	virtual enum TVerdict doTestStepL(void);
};

#endif

