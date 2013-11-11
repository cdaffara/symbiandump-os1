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

#ifndef CTESTSTEPBTROMCONFIGREMCON002_H
#define CTESTSTEPBTROMCONFIGREMCON002_H

#include "cteststepbtromconfigbase.h"
#include <remconcoreapitargetobserver.h>

class CTestStepBtRomConfigRemCon002 : public CTestStepBtRomConfigBase,
	public MRemConCoreApiTargetObserver
	{
public:
	static CTestStepBtRomConfigRemCon002* New(CTestServer& aParent);
	~CTestStepBtRomConfigRemCon002();
	TVerdict doTestStepL();

	// from MRemConCoreApiTargetObserver
	void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
		TRemConCoreApiButtonAction aButtonAct);
	void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, 
		TRemConCoreApiButtonAction aButtonAct);
	void MrccatoTuneFunction(TBool aTwoPart, 
		TUint aMajorChannel, 
		TUint aMinorChannel,
		TRemConCoreApiButtonAction aButtonAct);
	void MrccatoSelectDiskFunction(TUint aDisk,
		TRemConCoreApiButtonAction aButtonAct);
	void MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
		TRemConCoreApiButtonAction aButtonAct);
	void MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
		TRemConCoreApiButtonAction aButtonAct);
		
private:
	CTestStepBtRomConfigRemCon002(CTestServer& aParent);
	};

_LIT(KTestNameRemCon002, "BT_ROMCONFIG_REMCON_002"); 
#endif //  CTESTSTEPBTROMCONFIGREMCON002_H

// EOF
