/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declaration of CBoolExpr, CLogicalNot, CLogicalAnd and
* CLogicalOr classes.
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CNDLOGICAL_H__
#define __CNDLOGICAL_H__

#include "conditionevaluatebase.h"

NONSHARABLE_CLASS(CLogicalNot) : public CConditionEvaluateBase
    {
public:
	CLogicalNot(CConditionEvaluateBase* aExpr);
    virtual ~CLogicalNot();
    virtual TBool EvaluateL() const;

private:
	CConditionEvaluateBase* iExpr;
    };

NONSHARABLE_CLASS(CLogicalAnd) : public CConditionEvaluateBase
    {
public:
    CLogicalAnd(CConditionEvaluateBase* aLhs, CConditionEvaluateBase* aRhs);
    virtual ~CLogicalAnd();
    virtual TBool EvaluateL() const;

private:
    //The left-hand-side of the logical AND expression. Not NULL. Owned.
	CConditionEvaluateBase* iLhs;

	//The right-hand-side of the logical AND expression. Not NULL. Owned.
	CConditionEvaluateBase* iRhs;

    };
NONSHARABLE_CLASS(CLogicalOr) : public CConditionEvaluateBase
    {

public:
    CLogicalOr(CConditionEvaluateBase* aLhs, CConditionEvaluateBase* aRhs);
    virtual ~CLogicalOr();
    virtual TBool EvaluateL() const;

private:
	//The left-hand-side of the logical OR expression. Not NULL. Owned.
	CConditionEvaluateBase* iLhs;

	//The right-hand-side of the logical OR expression. Not NULL. Owned.
	CConditionEvaluateBase* iRhs;

    };

#endif // __CNDLOGICAL_H__
