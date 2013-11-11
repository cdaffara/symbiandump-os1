//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/** 
 * This defines the TS_RConnectionStep class which is the base class for all 
 * the RConnection multihoming test step classes
 *
 */
 
#if (!defined __TE_ENCRPTSTEPS_H__)
#define __TE_ENCRPTSTEPS_H__

#include <e32std.h>
#include <cdbstore.h>
#include "es_sock.h"
#include "in_sock.h"
#include <es_enum.h>
#include <commdbconnpref.h>

#include "TE_EncrptStep.h"


class CTE_CreateEntries : public CTE_EncrptStep
{
public:
	CTE_CreateEntries();
	virtual ~CTE_CreateEntries();
	virtual enum TVerdict doTestStepL();
};

class CTE_CheckPasswords: public CTE_EncrptStep
{
public:
	CTE_CheckPasswords();
	virtual ~CTE_CheckPasswords();
	virtual enum TVerdict doTestStepL();
};

class CTE_DeleteEntries: public CTE_EncrptStep
{
public:
	CTE_DeleteEntries();
	virtual ~CTE_DeleteEntries();
	virtual enum TVerdict doTestStepL();
};



#endif /* __TE_ENCRPTSTEPS_H__ */
