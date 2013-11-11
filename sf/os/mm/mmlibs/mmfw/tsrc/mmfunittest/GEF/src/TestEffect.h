
// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTEFFECT_H
#define TESTEFFECT_H

#include <mmf/common/mmfglblaudioeffect.h>

class CTestGlobalEffect : public CMmfGlobalAudioEffect
	{
public:
	/**
	Factory function
	@param Observer to use - can be NULL
	@return Freshly created object
	*/
	static CTestGlobalEffect* NewL(MMmfGlobalAudioEffectObserver* aObserver);
	
	/**
	Struct to form basis of package buffer to ExtractValuesL() and SetByValuesL()
	*/
	class TValues
		{
	public:
		TInt  iValue1;
		TInt  iValue2;
		};
		
	void SetActive(TBool aValue);
	
	void GenCallbackL();

private:
	CTestGlobalEffect();
	void ConstructL(MMmfGlobalAudioEffectObserver* aObserver);
	};


#endif // TESTEFFECT_H
