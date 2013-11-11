// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestSuite_MMF_SwCodecDevices 
// 
//

#ifndef __TSU_MMF_SWCODECDEVICES_SUITE_H_
#define __TSU_MMF_SWCODECDEVICES_SUITE_H_

#include <testframework.h>

/*
 TMmfAudioControllerPanics is an enumeration with the following entries:
 EBadArgument indicates a bad argument
 EBadState indicates a state viaolation
 EBadInvariant indicates an invariant violation
 EBadReset indicates failed reset
 EPostConditionViolation indicates a post condition violation
*/
enum TMmfCodecTestPanics
	{
	EBadArgument,
	EBadState,
	EBadInvariant,
	EBadReset,
	EPostConditionViolation
	};

/**
* Panic
* This method generates a panic
* @param "TInt"
* @xxxx
*/
inline void Panic(TInt aPanicCode)
	{
	_LIT(KMMFTestSwHwDevicesPanicCategory, "MMFTestSwHwDevices");
	User::Panic(KMMFTestSwHwDevicesPanicCategory, aPanicCode);
	}

class CTestStep_MMF_SwCodecDevices;

class  CTestSuite_MMF_SwCodecDevices : public CTestSuite
	{
public:
	
	void InitialiseL( void );
	virtual ~CTestSuite_MMF_SwCodecDevices();
	void AddTestStepL( CTestStep_MMF_SwCodecDevices *ptrTestStep );
	TPtrC GetVersion( void );
	void InitSystemPath();
	const TDesC& DefaultPath() const;
	
private:
	TFileName iDefaultPath;
	};


#endif /* TSU_MMF_SWCODECDEVICES_SUITE_H_ */

