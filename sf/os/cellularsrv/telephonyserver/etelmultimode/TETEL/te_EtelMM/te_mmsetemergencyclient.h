// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef _TE_MMSETEMERGENCYCLIENT_H_
#define _TE_MMSETEMERGENCYCLIENT_H_

#include "TE_EtelMMTestStepBase.h"

	
class CTestMMSetEmgClientLcsReqPreallocHeap : public CEtelMMLbsTestStepBase
/** To check that SetEmergencyClient API preallocate the heap memory for handling Emergency Mobile
	Terminated Location service request.
*/
	{
public:
	CTestMMSetEmgClientLcsReqPreallocHeap();
	virtual enum TVerdict doTestStepL();
	};

class CTestMMSetEmgClientLcsReqAndSetPrClient : public CEtelMMLbsTestStepBase
/** To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest) API.
*/
	{
public:
	CTestMMSetEmgClientLcsReqAndSetPrClient();
	virtual enum TVerdict doTestStepL();
	};

class CTestMMSetEmgClientVoiceReqAndSetPrClient : public CEtelMMLbsTestStepBase
/** To test a combination of SetEmergencyClient(VoiceCallRequest) API and SetPriorityClient API.
*/	
	{
public:
	CTestMMSetEmgClientVoiceReqAndSetPrClient();
	virtual enum TVerdict doTestStepL();
	};

class CTestMMSetEmgClientHeapSuffForLBS : public CEtelMMLbsTestStepBase
/** To check preallocated heap is sufficient to handle all Location Based Services.
*/	
	{
public:
	CTestMMSetEmgClientHeapSuffForLBS();
	virtual enum TVerdict doTestStepL();
	};
	
class CTestMMSetEmgClientVoiceReqPreallocHeap : public CEtelMMLbsTestStepBase
/** Ensure SetEmergencyClient API(With Voice request) preallocates heap.
*/	
	{
public:
	CTestMMSetEmgClientVoiceReqPreallocHeap();
	virtual enum TVerdict doTestStepL();
	};

#endif
