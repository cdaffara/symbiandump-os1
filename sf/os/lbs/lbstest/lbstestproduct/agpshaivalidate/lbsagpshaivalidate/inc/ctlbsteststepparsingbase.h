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
// @file CTLbsTestStepParsingBase.h
// 
//

#ifndef __CTLBSTESTSTEPPARSINGBASE_H__
#define __CTLBSTESTSTEPPARSINGBASE_H__

#include <badesca.h>
#include <test/testexecutestepbase.h>
#include <lbs/lbsadmin.h>
#include <lbsposition.h>

class CT_LbsTestStepParsingBase : public CTestStep
	{
public:
	virtual ~CT_LbsTestStepParsingBase();
	virtual TBool GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint32& aResult);
	virtual TBool GetAGPSModeFromConfig(const TDesC& aSectName, const TDesC& aKeyName, CLbsAdmin::TGpsMode& aAGPSMode);
	virtual TBool GetReferencePositionFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TPosition& aPos);
protected:
	CT_LbsTestStepParsingBase();

	// helpers
	void SplitL(const TDesC& aDes, CDesCArray* aFields, TChar aDelim);
	void ExtractDataL(TPosition& aPos, CDesCArray* aFields);
	void DefaultData(TPosition& aPos);
	void ExtractValueL(const TDesC& aField, TInt& aValue);
	void ExtractValueL(const TDesC& aField, TReal32& aValue);
	void ExtractValueL(const TDesC& aField, TReal64& aValue);
	};

#endif // __CTLBSTESTSTEPPARSINGBASE_H__