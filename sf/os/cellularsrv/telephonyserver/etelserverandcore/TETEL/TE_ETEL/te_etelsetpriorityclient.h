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

#ifndef _TE_ETELSETPRIORITYCLIENT_H_
#define _TE_ETELSETPRIORITYCLIENT_H_

#include <etel.h>


class CTestSetPrClientCapNsidPosCheck : public CEtelTestStep
/** This class Test connecting Positive check of Capability and SID check for SetPriorityClient API
*/
	{
public:
	CTestSetPrClientCapNsidPosCheck() ;
	~CTestSetPrClientCapNsidPosCheck(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetPrClientCapNsidNegCheck : public CEtelTestStep
/** This class Test connecting Negative check of Capability and SID check for SetPriorityClient API
*/
	{
public:
	CTestSetPrClientCapNsidNegCheck() ;
	~CTestSetPrClientCapNsidNegCheck(){} ;
	virtual enum TVerdict doTestStepL();
	};	
	
	
class CTestSetPrClientMulCallFrSameSes : public CEtelTestStep
/** This class Test connecting Multiple call of setpriorityclient API from the same session
*/	
	{
public:
	CTestSetPrClientMulCallFrSameSes() ;
	~CTestSetPrClientMulCallFrSameSes(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSetPrClientMulCallFrDiffSubSes : public CEtelTestStep
/** This class Test connecting Multiple call of setpriorityclient API from the different sub session
*/	
	{
public:
	CTestSetPrClientMulCallFrDiffSubSes() ;
	~CTestSetPrClientMulCallFrDiffSubSes(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetPrClientMulCallFrDiffClt : public CEtelTestStep
/** This class Test connecting Multiple call of setpriorityclient API from the different threads
*/
	{
public:
	CTestSetPrClientMulCallFrDiffClt() ;
	~CTestSetPrClientMulCallFrDiffClt(){} ;
	virtual enum TVerdict doTestStepL();
private:
	static TInt	FirstThreadFunctiona(TAny *aParam);
	static TInt	SecondThreadFunctiona(TAny *aParam);
	};
	
class CTestSetPrClientV2CapNsidPosCheck : public CEtelTestStep
/** This class Test connecting Positive check of Capability and SID check for SetPriorityClientV2 API
*/
	{
public:
	CTestSetPrClientV2CapNsidPosCheck() ;
	~CTestSetPrClientV2CapNsidPosCheck(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetPrClientV2CapNsidNegCheck : public CEtelTestStep
/** This class Test connecting  Negative check of Capability and SID check for SetPriorityClientV2 API
*/
	{
public:
	CTestSetPrClientV2CapNsidNegCheck() ;
	~CTestSetPrClientV2CapNsidNegCheck(){} ;
	virtual enum TVerdict doTestStepL();
	};	
	
class CTestSetPrClientV2MulCallFrSameSes : public CEtelTestStep
/** This class Test connecting Multiple call of setpriorityclientV2 API from the same session
*/	
	{
public:
	CTestSetPrClientV2MulCallFrSameSes() ;
	~CTestSetPrClientV2MulCallFrSameSes(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestSetPrClientV2MulCallFrDiffSubSes : public CEtelTestStep
/** This class Test connecting Multiple call of setpriorityclientV2 API from the different sub session
*/
	{
public:
	CTestSetPrClientV2MulCallFrDiffSubSes() ;
	~CTestSetPrClientV2MulCallFrDiffSubSes(){} ;
	virtual enum TVerdict doTestStepL();
	};
	
class CTestSetPrClientV2MulCallFrDiffClt : public CEtelTestStep
/** This class Test connecting Multiple call of setpriorityclientV2 API from the different threads
*/	
	{
public:
	CTestSetPrClientV2MulCallFrDiffClt() ;
	~CTestSetPrClientV2MulCallFrDiffClt(){} ;
	virtual enum TVerdict doTestStepL();
private:
	static TInt	FirstThreadFunctiona(TAny *aParam);
	static TInt	SecondThreadFunctiona(TAny *aParam);
	};
	
#endif
