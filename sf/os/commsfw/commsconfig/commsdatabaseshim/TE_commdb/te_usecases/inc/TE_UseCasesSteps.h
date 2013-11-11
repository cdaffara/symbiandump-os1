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
 * @file TE_RConnectionStep.h
 *
 * This defines the TE_RConnectionStep class which is the base class for all 
 * the RConnection multihoming test step classes
 *
 */
 
#if (!defined __TE_USECASESSTEPS_H__)
#define __TE_USECASESSTEPS_H__

#include <e32std.h>
#include <cdbstore.h>
#include "es_sock.h"
#include "in_sock.h"
#include <es_enum.h>
#include <commdbconnpref.h>

#include "TE_UseCasesStep.h"


class CTE_CreatingDatabaseAndView : public CTE_UseCasesStep
{
public:
	CTE_CreatingDatabaseAndView();
	virtual ~CTE_CreatingDatabaseAndView();
	virtual enum TVerdict doTestStepL();
};  


class CTE_UsecaseReading: public CTE_UseCasesStep
{
public:
	CTE_UsecaseReading();
	virtual ~CTE_UsecaseReading();
	virtual enum TVerdict doTestStepL();
};  


class CTE_UsecaseWriting: public CTE_UseCasesStep
{
public:
	CTE_UsecaseWriting();
	virtual ~CTE_UsecaseWriting();
	virtual enum TVerdict doTestStepL();
};  


class CTE_UsecaseOverridingATableEntry: public CTE_UseCasesStep
{
public:
	CTE_UsecaseOverridingATableEntry();
	virtual ~CTE_UsecaseOverridingATableEntry();
	virtual enum TVerdict doTestStepL();
};  


class CTE_UsecaseOverridingThePreferenceTable: public CTE_UseCasesStep
{
public:
	CTE_UsecaseOverridingThePreferenceTable();
	virtual ~CTE_UsecaseOverridingThePreferenceTable();
	virtual enum TVerdict doTestStepL();
};   


class CTE_UsecaseUsingATemplate: public CTE_UseCasesStep
{
public:
	CTE_UsecaseUsingATemplate();
	virtual ~CTE_UsecaseUsingATemplate();
	virtual enum TVerdict doTestStepL();
};   


#endif /* __TE_USECASESSTEPS_H__ */
