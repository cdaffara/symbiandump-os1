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
// Example CTestStep derived implementation
// 
//

/**
 @file ced2Step.cpp
 @internalTechnology
*/
#include "ced2Step.h"
#include "Te_cedSuiteDefs.h"

#include <e32base.h>
#include <metadatabase.h>
#include <commsdat.h>

#include <commsdattypesv1_1.h>
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#include <commsdattypeinfov1_1_internal.h>

using namespace CommsDat;
using namespace Meta;

Cced2Step::~Cced2Step()
/**
 * Destructor
 */
	{
	}

Cced2Step::Cced2Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced2Step);
	}

TVerdict Cced2Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced2Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    /**
     * Creating an IAP record in the clients' memory area try to find a given
     * record in the DB.
     */
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRecord);
    
    /**
     * Existing RecordIDs in the IAP table are 1, 2, 3.
     */
    iapRecord->SetRecordId(3);
    
    TRAPD(leavingError, iapRecord->LoadL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for IAPRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave, which means that we have a record with the recNum 3.
        {	
        if ( TPtrC(KCDTypeNameLANService) == iapRecord->iServiceType.GetL() )
		    if ( TPtrC(KCDTypeNameLANBearer) == iapRecord->iBearerType.GetL() )
		        if ( 3 == iapRecord->iNetworkWeighting )
		        /**
		         * So far so good. The inserted record is OK. Now let's check whether the 
		         * linking is working to the Location table...
		         */
		            {
		            TRAP(leavingError, iapRecord->iLocation.LoadL(*cmdbSession))
		            INFO_PRINTF2(_L("LoadL for linked LocationRecord returns %d"), leavingError);
		            
		            if (!leavingError)
		            //The LoadL didn't leave, which means that we have the linked Location record
		                {
		                CCDLocationRecord *locationRecord = static_cast<CCDLocationRecord *>((iapRecord->iLocation).iLinkedRecord);
		                
		                if ( TPtrC(_S("181")) == locationRecord->iAreaCode.GetL() )
		                    SetTestStepResult(EPass);
		                }
		            }
        }
    
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced2Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}



//---------- Cced3Step -----------
Cced3Step::~Cced3Step()
/**
 * Destructor
 */
	{
	}

Cced3Step::Cced3Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced3Step);
	}

TVerdict Cced3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced3Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    /**
     * Creating an IAP record in the clients' memory area try to find a given
     * record in the DB.
     */
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRecord);
    
    /**
     * Existing RecordIDs in the IAP table are 1, 2, 3, 4, 5.
     */
    iapRecord->SetRecordId(4);
    
    TRAPD(leavingError, iapRecord->LoadL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for IAPRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave, which means that we have a record with the recNum 4.
        {	
        if ( TPtrC(KCDTypeNameDialOutISP) == iapRecord->iServiceType.GetL() )
		    if ( TPtrC(KCDTypeNameModemBearer) == iapRecord->iBearerType.GetL() )
		        if ( 0 == iapRecord->iNetworkWeighting )
		        /**
		         * So far so good. The inserted record is OK. Now let's check whether the 
		         * linking is working to the Location table...
		         */
		            {
		            TRAP(leavingError, iapRecord->iLocation.LoadL(*cmdbSession))
		            INFO_PRINTF2(_L("LoadL for linked LocationRecord returns %d"), leavingError);
		            
		            if (!leavingError)
		            //The LoadL didn't leave, which means that we have the linked Location record
		                {
		                CCDLocationRecord *locationRecord = static_cast<CCDLocationRecord *>((iapRecord->iLocation).iLinkedRecord);
		                
		                if ( TPtrC(_S("93")) == locationRecord->iAreaCode.GetL() )
		                    SetTestStepResult(EPass);
		                }
		            }
        }
    
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}



//----------- Cced4Step -----------
Cced4Step::~Cced4Step()
/**
 * Destructor
 */
	{
	}

Cced4Step::Cced4Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced4Step);
	}

TVerdict Cced4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced4Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    /**
     * Creating an IAP record in the clients' memory area try to find a given
     * record in the DB.
     */
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(iapRecord);
    
    /**
     * Existing RecordIDs in the IAP table are 1, 2, 3, 4, 5.
     */
    iapRecord->SetRecordId(5);
    
    TRAPD(leavingError, iapRecord->LoadL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for IAPRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave, which means that we have a record with the recNum 5.
        {	
        if ( TPtrC(KCDTypeNameDialOutISP) == iapRecord->iServiceType.GetL() )
		    if ( TPtrC(KCDTypeNameModemBearer) == iapRecord->iBearerType.GetL() )
		        if ( 0 == iapRecord->iNetworkWeighting )
		        /**
		         * So far so good. The inserted record is OK. Now let's check whether the 
		         * linking is working to the Location table...
		         */
		            {
		            TRAP(leavingError, iapRecord->iLocation.LoadL(*cmdbSession))
		            INFO_PRINTF2(_L("LoadL for linked LocationRecord returns %d"), leavingError);
		            
		            if (!leavingError)
		            //The LoadL didn't leave, which means that we have the linked Location record
		                {
		                CCDLocationRecord *locationRecord = static_cast<CCDLocationRecord *>((iapRecord->iLocation).iLinkedRecord);
		                
		                if ( TPtrC(_S("171")) == locationRecord->iAreaCode.GetL() )
		                    SetTestStepResult(EPass);
		                }
		            }
        }
    
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
	
	
//----------- Cced5Step -----------------
Cced5Step::~Cced5Step()
/**
 * Destructor
 */
	{
	}

Cced5Step::Cced5Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced5Step);
	}

TVerdict Cced5Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced5Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    /**
     * Creating a GlobalSettings record in the clients' memory area try to find a given
     * record in the DB.
     */
    
    CCDGlobalSettingsRecord *globalSetRecord = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
    CleanupStack::PushL(globalSetRecord);
	
    globalSetRecord->iMaxRedialAttempts = 3;
    
    TRAPD(leavingError, globalSetRecord->FindL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for GlobalSettingsRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave
        {	
        if ( 1 == globalSetRecord->iGPRSAttachMode)
		    if ( 2 == globalSetRecord->iMaxConnectionAttempts )
		        /**
		         * So far so good. The inserted record is OK. Now let's check whether the 
		         * linking is working to the Location table...
		         */
		            {
		            TRAP(leavingError, globalSetRecord->iLocationForDataAndFax.LoadL(*cmdbSession))
		            INFO_PRINTF2(_L("LoadL for linked LocationRecord returns %d"), leavingError);
		            
		            if (!leavingError)
		            //The LoadL didn't leave, which means that we have the linked Location record
		                {
		                CCDLocationRecord *locationRecord = static_cast<CCDLocationRecord *>((globalSetRecord->iLocationForDataAndFax).iLinkedRecord);
		                
		                if ( TPtrC(_S("171")) == locationRecord->iAreaCode.GetL() )
		                    {
		                    TRAP(leavingError, globalSetRecord->iDefaultNetwork.LoadL(*cmdbSession))
        		            INFO_PRINTF2(_L("LoadL for linked NetworkRecord returns %d"), leavingError);
        		            
        		            if (!leavingError)
        		                {
		                        CCDNetworkRecord *networkRecord = static_cast<CCDNetworkRecord *>((globalSetRecord->iDefaultNetwork).iLinkedRecord);
		                        if ( TPtrC(_S("TestHostName1.intra.foo"))  == networkRecord->iHostName.GetL() )
		                            SetTestStepResult(EPass);
		                        }
		                    
		                    }
		                }
                    }
        }
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced5Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
	
	
//----------- Cced6Step -----------------
Cced6Step::~Cced6Step()
/**
 * Destructor
 */
	{
	}

Cced6Step::Cced6Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced6Step);
	}

TVerdict Cced6Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    CCDWLANServiceExtRecord *wlanServiceExtRecord = static_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
    CleanupStack::PushL(wlanServiceExtRecord);
	
    wlanServiceExtRecord->iWLANSSID.SetL(TPtrC(_S("BADC3344")));
    
    TRAPD(leavingError, wlanServiceExtRecord->FindL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for WlanServiceExtRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave
        {	
         TRAP(leavingError, wlanServiceExtRecord->iWLANSecData.LoadL(*cmdbSession))
		 INFO_PRINTF2(_L("LoadL for linked WlanSecurityData returns %d"), leavingError);
		            
        if (!leavingError)
        //The LoadL didn't leave, which means that we have the linked Location record
            {
            CCDEAPSecRecord *eapSecRecord = static_cast<CCDEAPSecRecord *>((wlanServiceExtRecord->iWLANSecData).iLinkedRecord);
            
            if ( TPtrC(_S("abcd0123")) == eapSecRecord->iEAPSecId.GetL() )
                SetTestStepResult(EPass);
            }
        }
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced6Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
	

//----------- Cced7Step -----------------
Cced7Step::~Cced7Step()
/**
 * Destructor
 */
	{
	}

Cced7Step::Cced7Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced7Step);
	}

TVerdict Cced7Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced7Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    CCDEAPSecRecord *eapSecRecord = static_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEAPSecRecord));
    CleanupStack::PushL(eapSecRecord);
	
    eapSecRecord->iEAPSecId.SetL(TPtrC(_S("abcd0123")));
    
    TRAPD(leavingError, eapSecRecord->FindL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for EAPSecRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave
        {	
         TRAP(leavingError, eapSecRecord->iEAPSecData.LoadL(*cmdbSession))
		 INFO_PRINTF2(_L("LoadL for linked TunEAPRecord returns %d"), leavingError);
		            
        if (!leavingError)
        //The LoadL didn't leave, which means that we have the linked Location record
            {
            CCDTunEAPRecord *tunEAPRecord = static_cast<CCDTunEAPRecord *>((eapSecRecord->iEAPSecData).iLinkedRecord);
            
            if ( ECommsDatTunEapMSCHAPv2 == tunEAPRecord->iTUNEAPInnerType )
                SetTestStepResult(EPass);
            }
        }
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced7Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}


//----------- Cced8Step -----------------
Cced8Step::~Cced8Step()
/**
 * Destructor
 */
	{
	}

Cced8Step::Cced8Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced8Step);
	}

TVerdict Cced8Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced8Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    CCDTunEAPRecord *tunEAPRecord = static_cast<CCDTunEAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTunEAPRecord));
    CleanupStack::PushL(tunEAPRecord);
	
    tunEAPRecord->iTUNEAPInnerType = ECommsDatTunEapMSCHAPv2;
    
    TRAPD(leavingError, tunEAPRecord->FindL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for TUNEAPRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave
        {	
         TRAP(leavingError, tunEAPRecord->iTUNEAPData.LoadL(*cmdbSession))
		 INFO_PRINTF2(_L("LoadL for linked EAPSecuritySettings returns %d"), leavingError);
		            
        if (!leavingError)
        //The LoadL didn't leave, which means that we have the linked Location record
            {
            CCDEAPSecRecord *eapSecRecord = static_cast<CCDEAPSecRecord *>((tunEAPRecord->iTUNEAPData).iLinkedRecord);
            
            if ( TPtrC(_S("abcd0123")) == eapSecRecord->iEAPSecId.GetL() )
                SetTestStepResult(EPass);
            }
        }
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced8Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
	
//----------- Cced9Step -----------------
Cced9Step::~Cced9Step()
/**
 * Destructor
 */
	{
	}

Cced9Step::Cced9Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kced9Step);
	}

TVerdict Cced9Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cced9Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
        SetTestStepResult(EFail);
    
    /**
     * Creating the session with the Central Repository.
     */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    CMDBSession* cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(cmdbSession);
    
    CCDEAPSecRecord *eapSecRecord = static_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEAPSecRecord));
    CleanupStack::PushL(eapSecRecord);
    
    eapSecRecord->iEAPSecId.SetL(TPtrC(_S("Hello")));
    
    TPtrC temName(eapSecRecord->iEAPSecId.GetL());
    
    TBool found = EFalse;
    
    TRAPD(leavingError, found = eapSecRecord->FindL(*cmdbSession));
    INFO_PRINTF2(_L("LoadL for EAPSecRecord returns %d"), leavingError);
    
    if (!leavingError)
    //The LoadL didn't leave
        {
        if (!found)
            {
            return TestStepResult();
            }
        // Set the record id of the linking field
        TInt recId =eapSecRecord->RecordId();
        
        eapSecRecord->iEAPSecData.SetRecordId(recId);
        
         TRAP(leavingError, eapSecRecord->iEAPSecData.LoadL(*cmdbSession))
		 INFO_PRINTF2(_L("LoadL for linked EAPTLSSettings returns %d"), leavingError);
		            
        if (!leavingError)
        //The LoadL didn't leave, which means that we have the linked Location record
            {
            CCDEAPTLSRecord *eapTSLRecord = static_cast<CCDEAPTLSRecord *>((eapSecRecord->iEAPSecData).iLinkedRecord);
            
            if ( TPtrC(_S("0123abcd")) == eapTSLRecord->iEAPTLSClientCertName.GetL() )
                SetTestStepResult(EPass);
            }
        }
    CleanupStack::PopAndDestroy(2, cmdbSession);
    
    return TestStepResult();

	}



TVerdict Cced9Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_cedSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
