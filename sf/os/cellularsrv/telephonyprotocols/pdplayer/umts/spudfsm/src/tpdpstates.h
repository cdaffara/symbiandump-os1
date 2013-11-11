// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// PDP FSM states header
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef TPDPSTATES_H
#define TPDPSTATES_H

#include "tpdpstate.h"

class CPdpFsm;

#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
#define DEFINE_TPDPSTATE_CLASS(state) _LIT(KName##state, #state); \
NONSHARABLE_CLASS(TPdpState##state) : public TPdpState \
{ \
public: \
	TPdpState##state (CPdpFsmFactory * aPdpFsmFactory) \
	{ \
		iName = KName##state; \
		iPdpFsmFactory = aPdpFsmFactory; \
	} \
	TInt Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aParam); \
private: \
	TInt iErrorCode; \
};	
#else
#define DEFINE_TPDPSTATE_CLASS(state) NONSHARABLE_CLASS(TPdpState##state) : public TPdpState \
{ \
public: \
	TPdpState##state (CPdpFsmFactory * aPdpFsmFactory) \
	{ \
		iPdpFsmFactory = aPdpFsmFactory; \
	} \
	TInt Input (CPdpFsm& aFsm, TInt aOperation, TInt aErrorCode); \
private: \
	TInt iErrorCode; \
};	
#endif

DEFINE_TPDPSTATE_CLASS(Initialised)
DEFINE_TPDPSTATE_CLASS(OpeningPhone)
DEFINE_TPDPSTATE_CLASS(CreatingPrimary)
DEFINE_TPDPSTATE_CLASS(ActivatingPrimary)
DEFINE_TPDPSTATE_CLASS(CreatingSecondary)
DEFINE_TPDPSTATE_CLASS(CreatedSecondary)
DEFINE_TPDPSTATE_CLASS(SettingQoS)
DEFINE_TPDPSTATE_CLASS(SettingTFT)
DEFINE_TPDPSTATE_CLASS(ActivatingSecondary)
DEFINE_TPDPSTATE_CLASS(Open)
DEFINE_TPDPSTATE_CLASS(ChangingQoS)
DEFINE_TPDPSTATE_CLASS(ChangingTFT)
DEFINE_TPDPSTATE_CLASS(ModifyingActive)
DEFINE_TPDPSTATE_CLASS(Suspended)
DEFINE_TPDPSTATE_CLASS(Closing)
DEFINE_TPDPSTATE_CLASS(Stopping)
DEFINE_TPDPSTATE_CLASS(GettingNegQoS)
DEFINE_TPDPSTATE_CLASS(CreatingMbms)
DEFINE_TPDPSTATE_CLASS(CreatedMbms)
DEFINE_TPDPSTATE_CLASS(ActivatingMbms)
#endif // TPDPSTATES_H
