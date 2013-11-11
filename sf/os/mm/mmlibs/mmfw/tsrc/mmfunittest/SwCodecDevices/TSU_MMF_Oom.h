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
//

#ifndef __TSU_MMF_OOM_H_
#define __TSU_MMF_OOM_H_

#include <testframework.h>
#include "TSU_MMF_ACOD.h"
#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodecBase.h"
#include "GSM610.H"

/**
*
* class CTestStepOomTest
* This class is responsible for testing the out of memory
* condition on all the codecs.
*
*/
template <class Codec>
class CTestStepOomTest : public CTestStep_MMF_SwCodecDevices
	{
public:
	CTestStepOomTest( TUint aTestIndex );
	virtual ~CTestStepOomTest();
	virtual enum TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
protected:
	virtual Codec* GetCodecL();
private:
	TInt Oom();
	};

class CTestStepGsm610ToPcm16OomTest: public CTestStepOomTest<CMMFGsm610ToPcm16Codec>
	{
public:
	CTestStepGsm610ToPcm16OomTest( TUint aTestIndex) 
		: CTestStepOomTest<CMMFGsm610ToPcm16Codec>(aTestIndex){};
private:
	virtual CMMFGsm610ToPcm16Codec* GetCodecL();
};

class CTestStepPcm16ToGsm610OomTest: public CTestStepOomTest<CMMFGsm610ToPcm16Codec>
	{
public:
	CTestStepPcm16ToGsm610OomTest( TUint aTestIndex) 
		: CTestStepOomTest<CMMFGsm610ToPcm16Codec>(aTestIndex){};
private:
	virtual CMMFGsm610ToPcm16Codec* GetCodecL();
};


#endif /* TSU_MMF_OOM_H_ */

