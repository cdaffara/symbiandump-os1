/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TD_PRIVFW_H
#define TD_PRIVFW_H

//  INCLUDES
//#include <utfw.h>
#include <lbs.h>
#include <badesca.h>
#include <EPos_MPosVerificationObserver.h>
#include <EPos_CPosNetworkPrivacy.h>

#include "ctlbsprivfwstep.h"

// CONSTANTS

// FORWARD DECLARATIONS

class CPosReqLogEntry;
class CPosRequestLog;
class CPosRequestor;
class CLcfNotifierManager;
class CPosGSMPrivacyRequestInfo;
class CPosCDMAPrivacyRequestInfo;
class CPosSUPLPrivacyRequestInfo;
class CTcTimer;

_LIT(KLbsPrivFwStep, "LbsPrivFwStep");


// CLASS DECLARATION

/**
*  Example test module
*
*/
class CTdPrivFW : public CT_LbsPrivFwStep, public MPosVerificationObserver
{

public:  // Constructors and destructor

	virtual TVerdict doTestStepL();
		
    /**
    * Two-phased constructor.
    */
    static CTdPrivFW* NewL(CT_LbsPrivFwServer& aParent);
    
    /**
    * Destructor.
    */
    ~CTdPrivFW();
    
    /**
    * Retrieves the test module name
    * @param aName The name of the test module.
    */
    //void GetName(TDes& aName) const;
    
    /**
    * Retrieves nr of tests in the module
    * @return Nr of tests in the module.
    */
    //TInt Count() const;
    
    /**
    * Retrieves if the test is eikon dependent
    * @return Nr of tests in the module.
    */
    //TBool IsEikonDependent() const;
    
    /**
    * Runs the test in the module
    */
    //void StartL(TInt aIndex);
    
    /**
    * Gets the name of an individual test index
    * @param aIndex Indicates which test index name that is asked for.
    * @param aName The name of the test index.
    */
    //void GetIndexName(TInt aIndex,
    //  TDes& aName) const;

    // call back from network privacy
    void HandleVerifyComplete( 
        /* IN  */       TInt aRequestId,
        /* IN  */       TInt aCompletionCode
        );

	TInt iMilliseconds;
	
protected:

private:
    
/**
* C++ default constructor.
    */
    CTdPrivFW(CT_LbsPrivFwServer& aParent);
    /**
    * By default EPOC constructor is private.
    */
    void ConstructL();
    
    void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);
    
	// test case functions
	void TCPFW001L();
	void TCPFW002L();
	void TCPFW003L();
    void TCPFW004L();
	void TCPFW005L();
	//void TCPFW006L();
	//void TCPFW007L();
	//void TCPFW008L();
	void TCPFW009L();
	void TCPFW010L();
	void TCPFW011L();
	void TCPFW012L();
	void TCPFW013L();
	//void TCPFW014L();
	//void TCPFW015L();
	void TCPFW017L();
	void TCPFW018L();
	void TCPFW020L();
	//void TCPFW021L();
	void TCPFW022L();
	void TCPFW023L();
	void TCPFW024L();
	void TCPFW025L();
	void TCPFW026L();
	void TCPFW027L();
	void TCPFW028L();
	void TCPFW029L();

    // subfunctions of different test cases
    CPosNetworkPrivacy* ConnectToNetworkPrivacyAPIL();

    CPosGSMPrivacyRequestInfo* CreateGSMRequestInfoL();
    CPosCDMAPrivacyRequestInfo* CreateCDMARequestInfoL();
    CPosSUPLPrivacyRequestInfo* CreateSUPLRequestInfoL();
    
   	void SetRequestorNameL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aName);
   	void SetRequestorEmailL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aEmail);
   	void SetRequestorURLL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aUrl);
   	void SetRequestorPhoneL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);
    
    
    
       
    void SetRequestorSIPURLL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aSipUrl);   					
   	void SetRequestorIMSL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity);
   						
   	
   	void SetRequestorMINL(CPosCDMAPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);   						
   	void SetRequestorMDNL(CPosCDMAPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);																					
    
    void SetRequestorNameL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aName);
   	void SetRequestorEmailL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aEmail);
   	void SetRequestorURLL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aUrl);
   	void SetRequestorPhoneL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);
   	void SetRequestorSIPURLL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aSipUrl);   					
   	void SetRequestorIMSL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity);
   	void SetRequestorMINL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);   						
   	void SetRequestorMDNL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);										
    
    
    
    
   	void SetServiceNameL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aName);
   	void SetServiceEmailL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aEmail);
   	void SetServiceURLL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aUrl);
   	void SetServicePhoneL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);
   						
   
   	
   	void SetServiceSIPURLL(CPosGSMPrivacyRequestInfo& aInfo,
   	         			const TDesC& aSipUrl);   		
   	void SetServiceIMSL(CPosGSMPrivacyRequestInfo& aInfo,
     					const TDesC& aIdentity);
      																			
    void SetServiceMINL(CPosCDMAPrivacyRequestInfo& aInfo,
   	         			const TDesC& aSipUrl);   		
   	void SetServiceMDNL(CPosCDMAPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity);
    
    void SetServiceNameL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aName);
   	void SetServiceEmailL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aEmail);
   	void SetServiceURLL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aUrl);
   	void SetServicePhoneL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber);
   	void SetServiceSIPURLL(CPosSUPLPrivacyRequestInfo& aInfo,
   	         			const TDesC& aSipUrl);   		
   	void SetServiceIMSL(CPosSUPLPrivacyRequestInfo& aInfo,
     					const TDesC& aIdentity);      																			
    void SetServiceMINL(CPosSUPLPrivacyRequestInfo& aInfo,
   	         			const TDesC& aSipUrl);   		
   	void SetServiceMDNL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity);					
    
    void MakeVerificationL(	CPosNetworkPrivacy& aApi,
						const CPosNetworkPrivacyRequestInfo& aReq,
						TInt& aId,
						const CPosNetworkPrivacy::TRequestDecision& aTimeoutStrategy = CPosNetworkPrivacy::EDecisionRejected);

	void MakeNotificationL(CPosNetworkPrivacy& aApi,
							 const CPosNetworkPrivacyRequestInfo& aReq,
							 TInt& aId);
	static void ResetAndDestroy(TAny* aPointerArray);
	
    TBool CheckQNAmounts( CLcfNotifierManager* pluginManager, TInt aVerifies, TInt aNotifies );
    
    // observer need these functions to get Active Scheduler events
    
    void WaitForModuleEventsL();
    TBool AnyCompletionWaiting();
    
    void WaitForTimeL(const TTimeIntervalMicroSeconds32& aTime);
    static TInt IdleCallbackTime(TAny *aThisObj);
    
    // Special helpers to deal with Active scheduler
    static TInt IdleCallback(TAny *aThisObj);
    void StopWaiting();
  
  //	void LogEnvironment();
  	  	
private:
    //CDesCArrayFlat*			iRequestorNameArray;
    RArray<TInt>            iArray;
    RArray<TInt>            iResultArray;
    CActiveScheduler*		iScheduler;
    CActiveSchedulerWait    iWait;
    TBuf<80> 				errorMessage;
    CLcfNotifierManager* 	iPluginManager;
    TInt 					iResult;
    
    friend class CTcTimer;
    TTimeIntervalMicroSeconds32	iWaitTime;
	TBool					    iLogged;   
};

#endif      // TD_PRIVFW_H  

// End of File
