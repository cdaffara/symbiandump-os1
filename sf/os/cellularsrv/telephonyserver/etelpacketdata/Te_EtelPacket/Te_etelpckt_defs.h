// Te_etelpckt_defs.h

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
// This file contains the macro and helper definitions that are used in the 
// Packet data suite test steps.
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __TE_ETELPCKT_DEFS_H__
#define __TE_ETELPCKT_DEFS_H__


// Macros to be used in Test Steps
#define CHP_TEL_CONNECT _L("Failed connect phone")
#define CHP_OPEN_PHONE _L("Failed open phone")

#define CHECKPOINT(code,expected,msg){\
	TEST((code)==(expected));\
	if (code != expected ) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_EX(boolexpr,msg) {\
	TEST((boolexpr));\
	if (EFalse == (boolexpr)) INFO_PRINTF1(msg);\
	}

#define CHECKPOINT_COMP_STR(value,expected,msg) CHECKPOINT_EX((value)==(expected),msg)

// Helpers to get the referencies to the  specification 
#if defined(_UNICODE)
#define	CHP_SRVS_CASE(the_case) (TPtrC((const TText *)(L"Case: " L##the_case)))
#define	CHP_CNTXT_CASE(the_case) (TPtrC((const TText *)(L"Case: " L##the_case)))
#define	CHP_QOS_CASE(the_case) (TPtrC((const TText *)(L"Case: " L##the_case)))
#else
#define	CHP_SRVS_CASE(the_case) (TPtrC((const TText *)("Case: " the_case)))
#define	CHP_CNTXT_CASE(the_case) (TPtrC((const TText *)("Case: " the_case)))
#define	CHP_QOS_CASE(the_case) (TPtrC((const TText *)("Case: " the_case)))
#endif


#endif //__TE_ETELPCKT_DEFS_H__
