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

#ifndef _TE_ETELSETEMERGENCYCLIENT_H_
#define _TE_ETELSETEMERGENCYCLIENT_H_

#include <etel.h>

class CTestSetEmgClientLCSReqMulCallFrSameSes : public CEtelTestStep
/** Test connecting Multiple call of SetEmergencyClientLCSRequet API on the same session
*/	
	{
public:
	CTestSetEmgClientLCSReqMulCallFrSameSes() ;
	~CTestSetEmgClientLCSReqMulCallFrSameSes(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSetEmgClientLCSReqMulCallFrDiffSubSes : public CEtelTestStep
/** Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different session
*/
	{
public:
	CTestSetEmgClientLCSReqMulCallFrDiffSubSes() ;
	~CTestSetEmgClientLCSReqMulCallFrDiffSubSes(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetEmgClientLCSReqMulCallFrDiffClt : public CEtelTestStep
/** Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different thread
*/
	{
public:
	CTestSetEmgClientLCSReqMulCallFrDiffClt() ;
	~CTestSetEmgClientLCSReqMulCallFrDiffClt(){} ;
	virtual enum TVerdict doTestStepL();
private:
	static TInt	FirstThreadFunctiona(TAny *aParam);
	static TInt	SecondThreadFunctiona(TAny *aParam);
	};
	
class CTestSetEmgClientVoiceReqMulCallFrSameSes : public CEtelTestStep
/** Test connecting Multiple call of SetEmergencyClientVoiceRequet API from the same session
*/
	{
public:
	CTestSetEmgClientVoiceReqMulCallFrSameSes() ;
	~CTestSetEmgClientVoiceReqMulCallFrSameSes(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetEmgClientVoiceReqMulCallFrDiffSubSes : public CEtelTestStep
/** Test connecting Capability check of SetEmergencyClientVoiceRequet API from the different sub session
*/	
	{
public:
	CTestSetEmgClientVoiceReqMulCallFrDiffSubSes() ;
	~CTestSetEmgClientVoiceReqMulCallFrDiffSubSes(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetEmgClientVoiceReqMulCallFrDiffClt : public CEtelTestStep
/** Test connecting Capability check of SetEmergencyClientVoiceRequet API from different client
*/	
	{
public:
	CTestSetEmgClientVoiceReqMulCallFrDiffClt() ;
	~CTestSetEmgClientVoiceReqMulCallFrDiffClt(){} ;
	virtual enum TVerdict doTestStepL();
private:
	static TInt	FirstThreadFunctiona(TAny *aParam);
	static TInt	SecondThreadFunctiona(TAny *aParam);
	};

class CTestSetEmergencyClientCapabilityPosCheck : public CEtelTestStep
/** Test calling SetEmergencyClient API with the right capability
*/
	{
public:
	CTestSetEmergencyClientCapabilityPosCheck() ;
	~CTestSetEmergencyClientCapabilityPosCheck(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSetEmergencyClientCapabilityNegCheck : public CEtelTestStep
/** Test calling SetEmergencyClient API with the different capability
*/
	{
public:
	CTestSetEmergencyClientCapabilityNegCheck() ;
	~CTestSetEmergencyClientCapabilityNegCheck(){} ;
	virtual enum TVerdict doTestStepL();
	};

	
#endif

