// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of ResourceCondition class
//



#include "cndlogical.h"
#include "ssmpanic.h"

CLogicalNot::CLogicalNot(CConditionEvaluateBase* aExpr) : iExpr(aExpr)
    {
    }

CLogicalNot::~CLogicalNot()
    {
    delete iExpr;
    }

TBool CLogicalNot::EvaluateL() const
    {
    __ASSERT_DEBUG(NULL != iExpr, PanicNow(KPanicCLogicalNot, ENullPtr));
    TBool retVal = !(iExpr->EvaluateL());
    return retVal;
    }

CLogicalAnd::CLogicalAnd(CConditionEvaluateBase* aLhs, CConditionEvaluateBase* aRhs)
  : iLhs(aLhs),
    iRhs(aRhs)
    {
    }

CLogicalAnd::~CLogicalAnd()
    {
    delete iLhs;
    delete iRhs;
    }

TBool CLogicalAnd::EvaluateL() const
    {
    __ASSERT_DEBUG(NULL != iLhs, PanicNow(KPanicCLogicalAnd, ENullPtr));
    __ASSERT_DEBUG(NULL != iRhs, PanicNow(KPanicCLogicalAnd, ENullPtr));

    TBool retVal = (iLhs->EvaluateL() && iRhs->EvaluateL());
    return retVal;
    }

CLogicalOr::CLogicalOr(CConditionEvaluateBase* aLhs, CConditionEvaluateBase* aRhs)
  : iLhs(aLhs),
    iRhs(aRhs)
    {
    }

CLogicalOr::~CLogicalOr()
    {
    delete iLhs;
    delete iRhs;
    }

TBool CLogicalOr::EvaluateL() const
    {
    __ASSERT_DEBUG(NULL != iLhs, PanicNow(KPanicCLogicalOr, ENullPtr));
    __ASSERT_DEBUG(NULL != iRhs, PanicNow(KPanicCLogicalOr, ENullPtr));

    TBool retVal = (iLhs->EvaluateL() || iRhs->EvaluateL());
    return retVal;
    }
