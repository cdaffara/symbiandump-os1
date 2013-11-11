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
#ifndef TGRAPHICSRESOURCEINTERNALTESTSTEPBASE_H
#define TGRAPHICSRESOURCEINTERNALTESTSTEPBASE_H

#include <test/ttmsgraphicsstep.h>
#include <sgresource/sgresource.h>
#include "tgraphicsresourcemultiprocessthread.h"
#include "tgraphicsresourceinternalsecondprocessenums.h"

/**
Base Class for the Graphics Resource internal tests; contains helper functions
for creating second processes and retrieving test results or checking panics.
 */
class CTGraphicsResourceInternalBase : public CTTMSGraphicsStep
	{
public:
	CTGraphicsResourceInternalBase();
	~CTGraphicsResourceInternalBase();
	
	inline void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
	inline void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);

	TInt CreateSecondProcessAndDoTestL(const TDesC &aProcessName, TSgResIntTestInfo& aTestInfo);
	void CreateSecondProcessAndCheckAllocPanicL(const TDesC &aProcessName, TSgResIntTestInfo& aTestInfo, const TDesC &aAllocType);
	};

inline void CTGraphicsResourceInternalBase::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
	{
	MQCTest(aCondition, aFile, aLine);
	}

inline void CTGraphicsResourceInternalBase::testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	MQCTestWithErrorCode(aCondition, aErrorCode, aFile, aLine);
	}

#endif // TGRAPHICSRESOURCEINTERNALTESTSTEPBASE_H
