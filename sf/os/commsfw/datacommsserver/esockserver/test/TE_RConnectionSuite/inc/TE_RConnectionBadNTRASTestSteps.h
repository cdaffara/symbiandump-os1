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
// This is the header file with the declartions of the RConnection test step classes that predominately 
// require that NT RAS is the default interface specified in commdb. These test TCP sockets and 
// host resolvers. Each class encapsulates a complete test case and is derived from TS_RConnectionStep
// 
//

/**
 @file
*/

#if (!defined __TE_RCONNECTIONBADNTRASTESTSTEPS_H__)
#define __TE_RCONNECTIONBADNTRASTESTSTEPS_H__

#include "TE_RConnectionStep.h"

//class Te_RConnectionStep;

class TE_RConnectionTest500 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest500();

	virtual enum TVerdict doTestStepL(void);
};

class TE_RConnectionTest501 : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionTest501();

	virtual enum TVerdict doTestStepL(void);
};

#endif

