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

#ifndef _TE_MMSETPRIORITYCLIENT_H_
#define _TE_MMSETPRIORITYCLIENT_H_

#include "TE_EtelMMTestStepBase.h"

	
class CTestMMSetPrClientPreallocHeap : public CEtelMMLbsTestStepBase
/** To check even under low memory condition of default Etel process heap ,the APIs using 
	reserved heap doesn't fail
*/	
	{
public:
	CTestMMSetPrClientPreallocHeap();
	virtual enum TVerdict doTestStepL();
	};

class CTestMMSetPrClientAndSetEmgClient : public CEtelMMLbsTestStepBase
/** To test a combination of SetPriorityClient API and SetEmergencyClient API.
*/
	{
public:
	CTestMMSetPrClientAndSetEmgClient();
	virtual enum TVerdict doTestStepL();
	};
	
class CTestMMSetPrClientV2AndSetEmgClientLcsReq : public CEtelMMLbsTestStepBase
/** To check even under low memory condition of default Etel process heap ,the APIs using 
    reserved heap doesn't fail
*/	
	{
public:
	CTestMMSetPrClientV2AndSetEmgClientLcsReq();
	virtual enum TVerdict doTestStepL();
	};
	
class CTestMMSetPrClientV2AndSetEmgClientVoiceReq : public CEtelMMLbsTestStepBase
/** To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest) API.
*/	
	{
public:
	CTestMMSetPrClientV2AndSetEmgClientVoiceReq();
	virtual enum TVerdict doTestStepL();
	};
		
class CTestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq : public CEtelMMLbsTestStepBase
/** To test a combination of SetPriorityClient API and SetEmergencyClient(LCSRequest or Voice request) API.
*/	
	{
public:
	CTestMMSetPrClientV2AndSetEmgClientVoiceOrLbsReq();
	virtual enum TVerdict doTestStepL();
	};
		
#endif
