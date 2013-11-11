/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file Te_EsockStepBase.h
*/

#if (!defined TE_ESOCKSTEP_BASE)
#define TE_ESOCKSTEP_BASE

#include <test/testexecutestepbase.h>
#include "EsockTestBase.h"
#include "Te_EsockStepBaseDefs.h"
#include "ErrorLogStrings.h"

class CTe_EsockStepBase : public CTestStep
	{
public:
	CTe_EsockStepBase(CCEsockTestBase*& aEsockTest);
	virtual ~CTe_EsockStepBase();
	virtual TVerdict doTestStepPreambleL(); //by default stop if test does not exist
	virtual TVerdict doTestStepL();

	//utility functions
	TBool GetIpAddressFromConfig(const TDesC &aSectName,const TDesC &aKeyName,TInetAddr &anAddr);
	void CopyFileL (const TDesC& anOld,const TDesC& aNew);
	void DeleteFileL (const TDesC& aFileName);

protected:
	virtual TVerdict doSingleTestStep() = 0;
	virtual TInt ConfigureFromIni() = 0;
	TInt GetCurrentIdx();
	CCEsockTestBase*& iEsockTest;
	TBuf<KMaxTestExecuteNameLength> iSection; //currect section of the ini file
	TInt iExpectedError; //expected return error
	TBool iExpectedCompareResult; //expected results from compare
    TBool iFoundUniqueSection;

private:
    TInt iIdx; //index of the current single step
    
    RSocketServ iSocketServer;
    RSocket     iSocketNapt;
    
	};

/**
Class containing IP R5 Qos parameters

@internalComponent
*/
class TExtensionQosParamsSetIPLinkR5
    {
public:
    TBool aSignallingIndicator;
    RPacketQoS::TSourceStatisticsDescriptor iSrcStatisticsDescriptor;
	};
	
	
/**
Class containing IMS Extension parameters

@internalComponent
*/
class TExtensionIMSParamsSet
    {
public:
    CSubConParameterFamily::TParameterSetType iType;
	TBool									  iIMSSignallingIndicator;	// Signalling Indicator for IMS
    };	
// SYMBIAN_NETWORKING_UMTSR5 

#endif //TE_ESOCKSTEP_BASE


