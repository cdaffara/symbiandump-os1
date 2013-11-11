// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements phone based(multimode) functionality such as getting 
// signal, battery, network and network registration status inforamtion(and more).
// 
//

#include "mSLOGGER.H"
#include "mPHONE.H"
#include "mSMSMESS.H"
#include "mPHBOOK.H"
#include "Mphbkcom.h"
#include "Mbattery.h"
#include "Msignal.h"
#include "mnetwork.h"
#include "NOTIFY.H"
#include <etslotnum.h>
#include "mimsi.h"
#include "Mownnum.h"
#include <etelmmcs.h>
#include "ATIO.H"
#include "monstore.h"
#include "gprs.h"
#include "TSYCONFG.H"
#include "ATINIT.H"
#include "etsvr_slots.h"

// This class is used when retreiving lists from the phone. 
CListReadAllAttempt* CListReadAllAttempt::NewL(RMobilePhone::TClientId* aId, TTsyReqHandle aReqHandle)
	{
	CListReadAllAttempt* read=new(ELeave) CListReadAllAttempt(aId, aReqHandle);
	CleanupStack::PushL(read);
	read->ConstructL();
	CleanupStack::Pop();
	return read;
	}

CListReadAllAttempt::CListReadAllAttempt(RMobilePhone::TClientId* aId, TTsyReqHandle aReqHandle)
	:CBase()
	{
	iClient.iSessionHandle=aId->iSessionHandle;
	iClient.iSubSessionHandle=aId->iSubSessionHandle;
	iReqHandle=aReqHandle;
	iListBuf = NULL;
	}

void CListReadAllAttempt::ConstructL()
	{
	}

CListReadAllAttempt::~CListReadAllAttempt()
	{
	delete iListBuf;
	}


void CPhoneMobile::ClosePhone(TAny* aObj)
//
// Utility func for cleanup stack
//
	{
	((CObject*)aObj)->Close();
	}

//
// CPhoneMobile
//
CPhoneMobile* CPhoneMobile::NewL()
	{
	CPhoneMobile* phone=new(ELeave) CPhoneMobile();
	TCleanupItem newPhoneClose(ClosePhone,phone);
	CleanupStack::PushL(newPhoneClose);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

void CPhoneMobile::ConstructL()
//
// Creation of Global Params
//
	{
	CPhoneHayes::ConstructL();
	}

CPhoneMobile::~CPhoneMobile()
//
	{
	if(iGetDetectedNetworks)		// This prevents an Access Violation when CommDB is missing
		iGetDetectedNetworks->ResetAndDestroy();	
	delete iGetDetectedNetworks;
	delete iPhoneBookInfo;
	delete iATBattery;
	delete iATSignal;
	delete iATNetworkInfo;
	delete iATSubscriberId;
	delete iATOwnNumbers;
	delete iATDetectNetwork;
	}

TInt CPhoneMobile::MultimodeInitL(TBool aExplicit)

	{	
	if(aExplicit)
		{
		if( !iPhoneGlobals )
			{
			iPhoneGlobals = CPhoneGlobals::NewL(aExplicit);
			iPhoneGlobals->iConfiguration->SetTableViewsL(iTableSettings);
			}
		}

	CPhoneHayes::MultimodeInitL(aExplicit);

	if(!iPhoneBookInfo)
		iPhoneBookInfo=CATPhoneBookInfo::NewL(iIo,this,iInit,iPhoneGlobals);
	
	if(!iATBattery)
		iATBattery=CATGetBattery::NewL(iIo,this,iInit,iPhoneGlobals);

	if(!iATSignal)
		iATSignal=CATGetSignal::NewL(iIo,this,iInit,iPhoneGlobals);
	
	if(!iATNetworkInfo)
		iATNetworkInfo=CATNetworkInfo::NewL(iIo,this,iInit,iPhoneGlobals);
	
	iPhoneGlobals->SetChangeOfNetworkPtr(iATNetworkInfo);
	
	if(!iATSubscriberId)
		iATSubscriberId=CATSubscriberId::NewL(iIo,this,iInit,iPhoneGlobals);
	
	if(!iATOwnNumbers)
		iATOwnNumbers=CATOwnNumbers::NewL(iIo,this,iInit,iPhoneGlobals);
	
	if(!iATDetectNetwork)
		iATDetectNetwork=CATDetectNetwork::NewL(iIo,this,iInit,iPhoneGlobals);
	
	if(!iGetDetectedNetworks )
		iGetDetectedNetworks = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1); 
	
	return KErrNone;
	}

TInt CPhoneMobile::ExplicitInitialisationL(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMMTableSettings* aSettings)
	{
	//  If we haven't initialised iPhoneGlobals, do so here.
	//  Either way, call CPhoneHayes::ControlledInitialisation().
	if (iPhoneGlobals == NULL)
		{
		iTableSettings=*aSettings;
		MultimodeInitL(ETrue);
		iPhoneGlobals->iConfiguration->GetRequestedTableViewsL();
		CPhoneHayes::ControlledInitialisation(aTsyReqHandle);
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		CPhoneHayes::ControlledInitialisation(aTsyReqHandle);
		}
		
	return KErrNone;
	}


TInt CPhoneMobile::ExplicitInitialisationCancel(const TTsyReqHandle aTsyReqHandle)
/** Cancel explicit initialisation
 * 
 * @param aTsyReqHandle const pointer to the request ID 
 * @return error code. KErrCancel
 */
	{
	iInit->StopInit(aTsyReqHandle);
	return KErrCancel;
	}

TInt CPhoneMobile::ControlledInitialisation(const TTsyReqHandle aTsyReqHandle)
	{
	TInt  ret = KErrNone;

	if (iPhoneGlobals == NULL)
		{
		TRAP(ret, MultimodeInitL(EFalse));
		}

	if (ret == KErrNone)
		{
		CPhoneHayes::ControlledInitialisation(aTsyReqHandle);
		}

	return ret;
	}
//
// Gsm extended functions
//

CTelObject::TReqMode CPhoneMobile::ReqModeL(const TInt aIpc)
	{
//
// What's the request mode
//
	CTelObject::TReqMode ret=0;

	// we need to delay initialising the phone as we have not yet been given the IAP to use
	if( iPhoneGlobals == NULL )
		{
		if(aIpc!=EMobilePhoneMMInitialise)
			{
			//  original initialise()
			MultimodeInitL(EFalse);
			}
		else
			{
			//  initialiseMM() - temporarily hardwire IAP value (DEF036487)
			iTableSettings.iLocId = KInternetAccessPoint;
			MultimodeInitL(ETrue);
			}
		}	

	switch (aIpc)
		{

	case EMobilePhoneMMInitialise:	
		ret=KReqModeFlowControlObeyed | KReqModeMultipleCompletionEnabled;
		break;
	
	case EMobilePhoneNotifyCurrentNetworkChange:  
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;

		break;

	case EMobilePhoneGetPhoneStoreInfo:
	case EMobilePhoneGetSignalStrength:
	case EMobilePhoneGetBatteryInfo: 
	case EMobilePhoneGetNetworkRegistrationStatus: 
	case EMobilePhoneGetSubscriberId:
	case EMobilePhoneGetCurrentNetwork:
	case EMobilePhoneGetDetectedNetworksPhase1:
		ret=KReqModeFlowControlObeyed;
		break;

	case EMobilePhoneGetCurrentMode:
	case EMobilePhoneGetPhoneId:
	case EMobilePhoneGetBatteryCaps:
	case EMobilePhoneGetSignalCaps:
	case EMobilePhoneGetIdentityCaps:
	case EMobilePhoneGetMultimodeCaps:
	case EMobilePhoneGetNetworkCaps:		
	case EMobilePhoneGetDetectedNetworksPhase2:
		ret=0;
		break;

	default:
		ret=CPhoneBase::ReqModeL(aIpc);
		break;
		}
	
	// Check if the data port is currently loaned. If it is and the requested IPC
	// is flow controlled then block Etel calling the IPC by leaving with KErrInUse
	if(iPhoneGlobals && (ret&KReqModeFlowControlObeyed) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}

	return ret;
	}

TInt CPhoneMobile::NumberOfSlotsL(const TInt aIpc)
	{
	// Return the number of slots (buffered in server)
	// for any KReqRepostImmediately ipc calls in the above ReqModeL function
	switch (aIpc)
		{
	case EMobilePhoneNotifyCurrentNetworkChange:  
		return (KSlotNotifyCurrentNetworkChange<4 ? 4 : KSlotNotifyCurrentNetworkChange);

	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		return KSlotNotifyNetworkRegistrationStatusChange;

	default:
		return CPhoneHayes::NumberOfSlotsL(aIpc);
		}
	}

TInt CPhoneMobile::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CPhoneMobile::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

TInt CPhoneMobile::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	TInt ret=KErrNone;
	// If an AT command class has been used to service a request, then try to cancel 
	// the sending of the AT command. If the AT command class is in an idle state
	// the CancelCommand() function should do nothing.
	switch (aIpc)
		{
	case EMobilePhoneMMInitialiseCancel:
		ret= ExplicitInitialisationCancel(aTsyReqHandle);
		break;

	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		ret=NotifyNetworkRegistrationStatusChangeCancel(aTsyReqHandle);
		break;

	case EMobilePhoneNotifyCurrentNetworkChange:  
	 	ret=NotifyCurrentNetworkChangeCancel(aTsyReqHandle);
		break;

	case EMobilePhoneGetSignalStrength: 
		iATSignal->CancelCommand(aTsyReqHandle);
		break;

	case EMobilePhoneGetBatteryInfo: 
		iATBattery->CancelCommand(aTsyReqHandle);
		break;

	case EMobilePhoneGetCurrentNetwork:
		iATNetworkInfo->CancelCommand(aTsyReqHandle);
		break;

	case EMobilePhoneGetDetectedNetworksPhase1:
		iATDetectNetwork->CancelCommand(aTsyReqHandle);
		break;

	case EMobilePhoneGetPhoneStoreInfo:
		{
		// Cancel both - as could be trying to get phonebook or own number store info
		iATOwnNumbers->CancelCommand(aTsyReqHandle);
		iPhoneBookInfo->CancelCommand(aTsyReqHandle);
		}
		break;

	case EMobilePhoneGetSubscriberId: 
		iATSubscriberId->CancelCommand(aTsyReqHandle);
		break;

	case EMobilePhoneGetNetworkRegistrationStatus: 
	case EMobilePhoneGetCurrentMode:
	case EMobilePhoneGetPhoneId:
		// Can not cancel these requests because they are completed synchronously
		break;

	default:
		ret=CPhoneBase::CancelService(aIpc,aTsyReqHandle);
		}
	
	return ret;
	}


TInt CPhoneMobile::ExtFunc(const TTsyReqHandle aTsyReqHandle, const TInt aIpc, const TDataPackage& aPackage)
//
// Dispatch Function
//
	{
	// Prior to dispatch check that we're not setting up or in the middle of a data or fax call
	if((iPhoneGlobals && iPhoneGlobals->iPhoneStatus.iPortAccess==EPortAccessDenied) || (iPhoneGlobals && iPhoneGlobals->iPhoneStatus.iMode == RPhone::EModeOnlineData))		
		{
		LOGTEXT2(_L8("CPhoneMobile::ExtFunc (aIpc=%d)"),aIpc);
		LOGTEXT(_L8("CPhoneMobile::ExtFunc\tPort Access Denied/Mode Online flag detected"));

		switch(aIpc)
			{
			// These may interfere with the Fax, so error the request now...
		case EMobilePhoneGetSignalStrength:
		case EMobilePhoneGetBatteryInfo: 
		case EMobilePhoneGetCurrentNetwork:
		case EMobilePhoneGetPhoneStoreInfo:
		case EMobilePhoneGetNetworkRegistrationStatus: 
		case EMobilePhoneGetSubscriberId:
		case EMobilePhoneGetDetectedNetworksPhase1:
			LOGTEXT(_L8("CPhoneMobile::ExtFunc\tReturning KErrAccessDenied error"));
			ReqCompleted(aTsyReqHandle, KErrAccessDenied);
			return KErrNone;		// Real error value returned in completion of clients request
		default:
			break;
			}
		}

	//
	// Asynchrous Requests
	//
	TAny* dataPtr1=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();
	TInt ret = KErrNone;

	switch (aIpc)
		{
	case EMobilePhoneMMInitialise:
		{
			TRAP(ret,ExplicitInitialisationL(aTsyReqHandle, REINTERPRET_CAST(RMobilePhone::TMMTableSettings*, dataPtr1)));
			return ret;
		}

	case EMobilePhoneGetBatteryCaps:
		return GetBatteryCaps(aTsyReqHandle, REINTERPRET_CAST(TUint32*, dataPtr1));

	case EMobilePhoneGetBatteryInfo: 
		return GetBatteryInfo(aTsyReqHandle, REINTERPRET_CAST(RMobilePhone::TMobilePhoneBatteryInfoV1*, dataPtr1));

	case EMobilePhoneGetSignalStrength: 
		return GetSignalStrength(aTsyReqHandle, REINTERPRET_CAST(TInt32*,dataPtr1), REINTERPRET_CAST(TInt8*, dataPtr2));

	case EMobilePhoneGetSignalCaps:
		return GetSignalCaps(aTsyReqHandle, REINTERPRET_CAST(TUint32*, dataPtr1));

	case EMobilePhoneGetPhoneId: 
		return GetPhoneId(aTsyReqHandle,REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdentityV1*, dataPtr1));

	case EMobilePhoneGetCurrentNetwork: 
		return GetCurrentNetwork(aTsyReqHandle, aPackage.Des1n(), REINTERPRET_CAST(RMobilePhone::TMobilePhoneLocationAreaV1* ,dataPtr2));

	case EMobilePhoneNotifyCurrentNetworkChange:
		return NotifyCurrentNetworkChange(aTsyReqHandle, aPackage.Des1n(), REINTERPRET_CAST(RMobilePhone::TMobilePhoneLocationAreaV1* ,dataPtr2));

	case EMobilePhoneGetDetectedNetworksPhase1: 
		return GetDetectedNetworksPhase1(aTsyReqHandle, REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr1), REINTERPRET_CAST(TInt*, dataPtr2)); // Last argument is TDes8* aBufSiz

	case EMobilePhoneGetDetectedNetworksPhase2:
		return GetDetectedNetworksPhase2(aTsyReqHandle, REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr1), aPackage.Des2n());

	case EMobilePhoneGetPhoneStoreInfo: 
		switch (aPackage.Type())
			{
		case TDataPackage::EPackage1n2u:
			// Client has called the version of RMobilePhone::GetPhoneStoreInfo 
			// with 3 arguments.
			return GetPhoneStoreInfo(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2u());// Last parameter is "TDesC* aName"
		case TDataPackage::EPackage1n2n:
			// Client has called the version of RMobilePhone::GetPhoneStoreInfo
			// with 4 arguments.
			return GetPhoneStoreInfo(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());// Last parameter is RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg
		default:
			// No valid argument from RMobilePhone::GetPhoneStoreInfo found.
			Panic(EUnexpectedState);
			break;
			} // switch (aPackage.Type())



	case EMobilePhoneGetIdentityCaps:
		return GetIdentityCaps(aTsyReqHandle, REINTERPRET_CAST(TUint32* ,dataPtr1));
				
	case EMobilePhoneGetNetworkRegistrationStatus: 
		return GetNetworkRegistrationStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneRegistrationStatus*,dataPtr1));	

	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		return NotifyNetworkRegistrationStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneRegistrationStatus*,dataPtr1));

	case EMobilePhoneGetNetworkCaps:
		return GetNetworkCaps(aTsyReqHandle, REINTERPRET_CAST(TUint32*, dataPtr1)); 

	case EMobilePhoneGetCurrentMode:
		return GetCurrentMode(aTsyReqHandle,REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkMode*,dataPtr1));

	case EMobilePhoneGetMultimodeCaps:
		return GetMultimodeCaps(aTsyReqHandle, REINTERPRET_CAST(TUint32*,dataPtr1));

	case EMobilePhoneGetSubscriberId: 
		return GetSubscriberId(aTsyReqHandle,
			static_cast<RMobilePhone::TMobilePhoneSubscriberId*>(dataPtr1));

	default:
		LOGTEXT2(_L8("CPhoneMobile::ExtFunc\tunsupported request %d"), aIpc);
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
		return KErrNone;		// Real error value returned in completion of clients request	
		}
	}


//
// Implemented Phone Functions
//

CTelObject* CPhoneMobile::OpenNewObjectL(TDes& /*aName*/)
	{

	User::Leave(KErrNotSupported);
	return NULL;
	}	


CTelObject* CPhoneMobile::OpenNewObjectByNameL(const TDesC& aName)
/**
 *	Open a new line. Opens fax line even if phone does not support it, as that information
 *	may not be available (init sequence may not have reached that far.)
 */
	{

	if(!iPhoneGlobals)
		{
		MultimodeInitL(EFalse);
		}

	if (aName.Compare(KETelSmsMessaging)==KErrNone)
		{
		iSmsMessaging = CMobileSmsMessaging::NewL(iIo,iInit,iPhoneGlobals);
		return iSmsMessaging;
		}
	
	if (!aName.CompareF(KPacketName)) 
		{
		iGprsService = CGprs::NewL(iIo, iInit, iPhoneGlobals);
		return iGprsService;		
		}

	// Check if the name specified is the own number store. If it is, create the store.
	if (aName.Compare(KETelOwnNumberStore)==KErrNone)
		{
		CMobileONStore* ownNumberStore=CMobileONStore::NewL(iIo,iInit,iPhoneGlobals);
		return ownNumberStore;
		}

	TStorageType storageType;
	TBool supported;
	// If the name is in the list of known phone book names check to see 
	// if it supported by the current modem/phone
	if (IsListedPhoneBook(aName,storageType,supported))
		{
		if (supported && storageType.Length()>0)
			{
			CMobilePhonebookStore* phoneBook=CMobilePhonebookStore::NewL(iIo,iInit,iPhoneGlobals,storageType);
			return phoneBook;
			}
		else
			{
			if (!supported)
				User::Leave(KErrNotSupported);
			else
				User::Leave(KErrUnknown);
			return NULL;
			}
		}

	// Use base class to open a line object
	return CPhoneHayes::OpenNewObjectByNameL(aName);;
	}


TBool CPhoneMobile::IsListedPhoneBook(const TDesC& aName,TDes8& aStorageType,TBool& aSupported) const 
/*
 * Check whether aName is a valid phone book name.  If it is then if supported by 07.07
 * return the memory storage type (used by AT commands) in aStorageType and whether it
 * is supported by the current ME in aSupported.
 */
	{

	aStorageType.SetLength(0);
	const TUint flag=iPhoneGlobals->iPhoneStatus.iSupportedPhoneBookStorageFlag;

	if (aName.CompareF(KETelMeAdnPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KMEStorage);
		aSupported=((flag&KPhoneBookMEStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelMeDialledPhoneBook)==KErrNone) //LAST_DIALLED_PHONEBOOK
		{
		aStorageType.Copy(KDCStorage);
		aSupported=((flag&KPhoneBookLastDialledStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelMeMissedPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KMCStorage);
		aSupported=((flag&KPhoneBookMissedCallsStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelMeReceivedPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KRCStorage);
		aSupported=((flag&KPhoneBookReceivedCallsStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelCombinedAdnPhoneBook)==KErrNone) 
		{
		aStorageType.Copy(KMTStorage);
		aSupported=((flag&KPhoneBookMTStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelTaAdnPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KTAStorage);
		aSupported=((flag&KPhoneBookTAStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelIccAdnPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KSMStorage);
		aSupported=((flag&KPhoneBookSMStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelIccFdnPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KFDStorage);
		aSupported=((flag&KPhoneBookFDStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelIccLndPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KLDStorage);
		aSupported=((flag&KPhoneBookLDStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelOwnNumberStore)==KErrNone)
		{
		aStorageType.Copy(KONStorage);
		aSupported=((flag&KPhoneBookOwnNumbersStorage)!=0);
		return ETrue;
		}
	if (aName.CompareF(KETelEmergencyNumberStore)==KErrNone)
		{
		aStorageType.Copy(KENStorage);
		aSupported=((flag&KPhoneBookEmergencyStorage)!=0);
		return ETrue;
		}	
	if (aName.CompareF(KETelIccSdnPhoneBook)==KErrNone)
		{
		aStorageType.Copy(KSNStorage);
		aSupported=((flag&KPhoneBookServiceNumberStorage)!=0);
		return ETrue;
		}
	return EFalse;
	}

//
// Extended functions for Multimode

TInt CPhoneMobile::GetPhoneId(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneIdentityV1* aId)
/** Get Phone Identification 
 *
 * This method gets information about the mobilephone using cashed information 
 * from the initialisation of the phone.  
 * @param aTsyReqHandle the request ID 
 * @param aId contains information of manufacturer, model, revision and serialnumber.
 * @return error code. KErrNone 
 */

	{
	aId->iManufacturer = iPhoneGlobals->iPhoneId.iManufacturer;
	aId->iModel = iPhoneGlobals->iPhoneId.iModel;
	aId->iRevision = iPhoneGlobals->iPhoneId.iRevision;
	aId->iSerialNumber = iPhoneGlobals->iPhoneId.iSerialNumber;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CPhoneMobile::GetIdentityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aIdentityCaps)
/** Get identity capabilities
 *
 * This method gets information about the capabilities to retrieve information about 
 * the phone and subscriber. Information regarding manufacturer, phone model, revision 
 * of the phone, serialnumber of the phone and subscriber ID is retrieved during initialisation.
 * Cashed information from the initialisation is used.
 * @param aTsyReqHandle the request ID 
 * @param aIdentityCaps contains the capability information(capabilities to get manufacturer, phone model, revision of the phone, serialnumber of the phone and subscriber id) 
 * @return error code. KErrNone 
 */
	{
	
	*aIdentityCaps = iPhoneGlobals->iPhoneIdCaps;
	
	if (*aIdentityCaps == 0)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		ReqCompleted(aTsyReqHandle, KErrNone);

	return KErrNone;
	}


TInt CPhoneMobile::GetSignalStrength(const TTsyReqHandle aTsyReqHandle, 
								  TInt32* aSignalStrength, TInt8* aBar) 
/** Signal Strength
 *
 * This method receivs a request from the server to get the signal strength. 
 * The the request is passed on to the CATGetSignal which gets the information 
 * from the MS using the AT command AT+CSQ.
 * @param aTsyReqHandle the request ID 
 * @param aSignalStrength pointer to the signalstrength. Can take the values -113dB to -53dB and 99, where 99 means signal is "not known or not detectable".
 * @param aBar pointer to the number of signal bars that are to be shown by the client. Can take the values (-1) to 5.
 * @return error code. KErrNone 
 */

	{
	LOGTEXT(_L8("MMTsy:\tCPhoneMobile::GetSignalStrength"));
	if (!iPhoneGlobals->iSignalCaps)
		// No signal strength capabilites - so error request
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		{
		iSignalPtrs.iSignalStrength = aSignalStrength;
		iSignalPtrs.iBar = aBar;
		iATSignal->ExecuteCommand(aTsyReqHandle, &iSignalPtrs);			
		}

	return KErrNone;
	}// end of CPhoneMobile::GetSignalStrength


TInt CPhoneMobile::GetSignalCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aSignalCaps)
/** Signal capabilities 
 * 
 * This method passes back the signal capabilities to the client. The information is 
 * extracted during the initialistaion and than cashed in the TSY for later use.
 * @param aTsyReqHandle the request ID 
 * @param aCaps, pointer to the capabilities  
 * @return error code.  
 */

	{
	//The signal capabilities are extracted during the initialisationsequence and saved in iPhoneGlobals->iSignalCaps.
	*aSignalCaps=iPhoneGlobals->iSignalCaps;

	if (*aSignalCaps == 0)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		ReqCompleted(aTsyReqHandle, KErrNone);

	return KErrNone;
	} 


TInt CPhoneMobile::GetMultimodeCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aMultimodeCaps) 
/** Get Multimode Capabilities 
 *
 * This method returnes the multimode capabilities for the phone. 
 * This TSY always supports GSM (which is hard coded) and may support
 * GPRS if the attached phone supports it.
 * @param aTsyReqHandle the request ID 
 * @param aMultimodeCaps, Pointer to the multimode capability to return.
 * @return error code. KErrNone 
 */
	{
	// This TSY always supports GSM 
	*aMultimodeCaps = 0;

	*aMultimodeCaps |= RMobilePhone::KCapsGsmSupported;

	// The GPRS capability of the phone is determined by the class 
	// CATInit during the initialisation dialogue.
	//
	if (iPhoneGlobals->iGprsMaxNumContexts != 0)
		{
		*aMultimodeCaps |= RMobilePhone::KCapsGprsSupported;
		}

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


TInt CPhoneMobile::GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo,TDesC* aName)
/** Get phonestore information 
 *
 * This method returns information about a store specified by aName in aInfo. 
 * This method only supports phonebooks and own number store. Other stores are 
 * either not supported by this TSY or opned from other subsessions. 
 * In the case when informtion about a phonebook the request is passed on to the 
 * CATPhopnebookInfo class which executes the AT command 

 * @param aTsyReqHandle const pointer to the request ID 
 * @param aInfo Pointer to the struct to contain the information about the specified store
 * @param aName The name of the store to get information about.
 * @return error code. 
 */

	{
	LOGTEXT2(_L8("MMTsy:\tCPhoneMobile::GetPhoneStoreInfo aTsyReqHandle = %d"), aTsyReqHandle);
	TStorageType storageType;
	TBool supported;
	
	TBool ret = IsListedPhoneBook(*aName, storageType, supported);
	if (aName->CompareF(KETelOwnNumberStore)==KErrNone) 
		{
		// Getting own number store inormation.
		iATOwnNumbers->GetInfo(aTsyReqHandle,aInfo);
		}
	else if (ret && supported)
		{
		iPhoneBookInfo->SetStorageType(storageType);
		iPhoneBookInfo->ExecuteCommand(aTsyReqHandle,aInfo);
		}
	else 
		{
		LOGTEXT2(_L8("MMTsy:\tCPhoneMobile::GetPhoneStoreInfo aTsyReqHandle = %d ReqCompleted(KErrNotSupported)"), aTsyReqHandle);
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
		}

	return KErrNone;
	}
	
TInt CPhoneMobile::GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aStoreNameIccType,TDes8* aInfo)
/** Get phonestore information 
 *
 * This method returns information about a store specified by aName in aInfo. 
 * This method only supports phonebooks and own number store. Other stores are 
 * either not supported by this TSY or opened from other subsessions. 
 * In the case when informtion about a phonebook the request is passed on to the 
 * CATPhopnebookInfo class which executes the AT command 

 * @param aTsyReqHandle const pointer to the request ID 
 * @param aStoreNameIccType contains a RMobilePhone::TPhoneStoreNameAndIccType including the store name and Icc type.
 * @param aInfo Pointer to the struct to contain the information about the specified store 
 * @return error code. 
 */

	{
	LOGTEXT2(_L8("MMTsy:\tCPhoneMobile::GetPhoneStoreInfo aTsyReqHandle = %d"), aTsyReqHandle);
	
	RMobilePhone::TPhoneStoreNameAndIccType* phoneStoreNameAndIccType = 
				reinterpret_cast<RMobilePhone::TPhoneStoreNameAndIccType*>
				(const_cast<TUint8*>(aStoreNameIccType->Ptr()));
				
	TDesC* mode = reinterpret_cast<TDesC*>(&phoneStoreNameAndIccType->iMode);
	if (*mode != KEtelGsmPhoneBookType)
		{
			LOGTEXT2(_L8("MMTsy:\tCPhoneMobile::GetPhoneStoreInfo aTsyReqHandle = %d ReqCompleted(KErrNotSupported)"), aTsyReqHandle);
			ReqCompleted(aTsyReqHandle, KErrNotSupported);
			return KErrNone;
		}
	TDesC* storeName = reinterpret_cast<TDesC*>(&phoneStoreNameAndIccType->iStoreName);
	
	TStorageType storageType;
	TBool supported;
	
	TBool ret = IsListedPhoneBook(*storeName, storageType, supported);
	if (storeName->CompareF(KETelOwnNumberStore)==KErrNone) 
		{
		// Getting own number store information.
		iATOwnNumbers->GetInfo(aTsyReqHandle,aInfo);
		}
	else if (ret && supported)
		{
		iPhoneBookInfo->SetStorageType(storageType);
		iPhoneBookInfo->ExecuteCommand(aTsyReqHandle,aInfo);
		}
	else 
		{
		LOGTEXT2(_L8("MMTsy:\tCPhoneMobile::GetPhoneStoreInfo aTsyReqHandle = %d ReqCompleted(KErrNotSupported)"), aTsyReqHandle);
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
		}
		return KErrNone;
	}

void CPhoneMobile::CurrentNetworkInfo(CATNetworkInfo::TTsyNetworkInfo* aNetworkInfo) const
	{
	iATNetworkInfo->GetCurrentNetworkInfo(aNetworkInfo);
	}


TInt CPhoneMobile::GetCurrentNetwork(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg,
									 RMobilePhone::TMobilePhoneLocationAreaV1* aArea)
/** Get Current Network Info 
 *
 * This method gets information about the current state of the network, 
 * such as network name(short and long), status and mode of the network etc.
 * @param aTsyReqHandle the request ID 
 * @param aNetworkInfoPckg  pointer to a packet of TMobilePhoneNetworkInfoV1Pckg type.
 * @param aArea Pointer to a struct of the type TMobilePhoneLocationAreaV1.
 * @return error code. 
 */
	{
	LOGTEXT(_L8("MMTsy:\tCPhoneMobile::GetCurrentNetwork"));

	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkPckg = static_cast<RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*>(aNetworkInfoPckg);
	// The networkPckg is finally unpacked to be set in CATNetworkInfo::GetCurrentNetworkInfo

	// Check if retrieval of network information is supported.
	if (iPhoneGlobals->iNetworkCaps & RMobilePhone::KCapsGetCurrentNetwork) 
		{
		iPhoneGlobals->iNetworkMode = GetMode();	//Updating the network mode. 
													//This is then used in CATNetworkInfo::GetCurrentNetworkInfo.
		iNetworkInfoPtrs.iNetworkInfoPckg = networkPckg;
		iNetworkInfoPtrs.iArea = aArea;

		// Need to read the network information
		iATNetworkInfo->ExecuteCommand(aTsyReqHandle, &iNetworkInfoPtrs); 
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		}

	return KErrNone;
	}


TInt CPhoneMobile::NotifyCurrentNetworkChange(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg,
												RMobilePhone::TMobilePhoneLocationAreaV1* aArea)
/** Notification of Current Network change 
 *
 * This method gets information about the current state of the network, such as network name(short and long), 
 * status and mode of the network etc, when the network changes.
 * @param aTsyReqHandle the request ID 
 * @param aNetworkInfoPckg  pointer to a packet of the type TMobilePhoneNetworkInfoV1Pckg.
 * @param aArea Pointer to a struct of the type TMobilePhoneLocationAreaV1.
 * @return error code. 
 */
	{
	if (iPhoneGlobals->iNetworkCaps & RMobilePhone::KCapsNotifyCurrentNetwork) // Check if notification is supported.
		{
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkPckg = static_cast<RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*>(aNetworkInfoPckg);
		CATNetworkInfo::TTsyNetworkInfo networkInfo; // Have to put the inforamtion in a struct to be able to pass it on in the TSY.
		networkInfo.iNetworkInfoPckg =  networkPckg; 
		networkInfo.iArea = aArea;

		iPhoneGlobals->iNotificationStore->RegisterNotification(ECurrentNetwork,aTsyReqHandle,this, &networkInfo); // posting the notification request
		}
	else
		ReqCompleted(aTsyReqHandle,KErrNotSupported);	
	
	return KErrNone;
	}


TInt CPhoneMobile::NotifyCurrentNetworkChangeCancel(const TTsyReqHandle aTsyReqHandle)
/** Notification of Current Network change Cancel
 *
 * This method cancels an outstanding NotifyCurrentNetworkChange request.
 * @param aTsyReqHandle the request ID that belong to the request to be cancelled
 * @return error code. 
 */
	{
	if (iPhoneGlobals->iNetworkCaps & RMobilePhone::KCapsNotifyCurrentNetwork) // Check if notification is supported.
		{
		LOGTEXT(_L8("PhoneMM:\tCurrent Network Change Notification cancelled"));
		iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
		}	
	// else - Request not posted in the first place, so do nothing

	return KErrNone;
	}


TInt CPhoneMobile::GetNetworkCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aNetworkCaps)
/** Get Multimode Capabilities 
 *
 * This method returnes the network capabilities for the phone. The capabilities are 
 * extracted during the initialisation(ATINIT.cpp) using AT+CREG and AT+COPS.
 * @param aTsyReqHandle the request ID 
 * @param aNetworkCaps, Pointer to the network capabilities.
 * @return error code. KErrNone 
 */
	{
	*aNetworkCaps = iPhoneGlobals->iNetworkCaps;

	if (*aNetworkCaps == 0)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


RMobilePhone::TMobilePhoneRegistrationStatus CPhoneMobile::RegistrationStatus() const
	{
	return iPhoneGlobals->iPhoneStatus.iRegistrationStatus;
	}

TInt CPhoneMobile::GetNetworkRegistrationStatus(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TMobilePhoneRegistrationStatus* aRegistrationStatus)
/** Network Registration Status 
 *
 * Gives information about the network registration status
 *(ENotRegisterdNotSearching, ERegisterdOnHomeNetwork, ENotRegisterdSearching, ERegistrationDenied, 
 * EUnknown or ERegisteredRoaming). The AT command +CREG is used to get this information. 
 * @param aTsyReqHandle the request ID 
 * @param aRegistrationStatus, pointer to be set to point to the registration status.
 * @return error code. KErrNone 
 */
	{
	*aRegistrationStatus = RegistrationStatus();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}	
	

TInt CPhoneMobile::NotifyNetworkRegistrationStatusChange(const TTsyReqHandle aTsyReqHandle, 
														RMobilePhone::TMobilePhoneRegistrationStatus*  aRegistrationStatus)
/** Notify change of Network Registration Status 
 * 
 * @param aTsyReqHandle const pointer to the request ID 
 * @param aRegistrationStatus, pointer to be set to point to the registartion status.
 * @return error code. KErrNone 
 */
	{
	if (iPhoneGlobals->iNetworkCaps & RMobilePhone::KCapsNotifyRegistrationStatus) // Check if notification is supported.
		{
		LOGTEXT(_L8("CPhoneMobile:\tNetwork Registration Status Change Notification loged"));
		iPhoneGlobals->iNotificationStore->RegisterNotification(ERegistrationStatus,aTsyReqHandle,this,aRegistrationStatus);
		return KErrNone;
		}

	ReqCompleted(aTsyReqHandle,KErrNotSupported);	
	return KErrNone;
	}


TInt CPhoneMobile::NotifyNetworkRegistrationStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
/** Notify change of Network Registration Status Cancel
 * 
 * @param aTsyReqHandle const pointer to the request ID 
 * @return error code. KErrNone 
 */

	{
	if (iPhoneGlobals->iNetworkCaps & RMobilePhone::KCapsNotifyRegistrationStatus) // Check if notification is supported.
		{
		LOGTEXT(_L8("CPhoneMobile:\tNetwork Registration Status Change Notification cancelled"));
		iPhoneGlobals->iNotificationStore->RemoveNotification(aTsyReqHandle);
		}
	// else - Request not posted in the first place, so do nothing

	return KErrNone;
	}


TInt CPhoneMobile::GetCurrentMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aNetworkMode)
/** Get Network Mode 
 *
 * Gives information about the current network mode, such as ENetworkModeUnknown,ENetworkModeUnregistered or ENetworkModeGsm.
 * TDM, AMPS, UMTS, WCDMA is not supported by this TSY  
 * @param aTsyReqHandle the request ID 
 * @param aNetworkMode The networkmode to be returned(as above)
 * @return error code. KErrNone 
 */
	{	
	if (iPhoneGlobals->iNetworkCaps & RMobilePhone::KCapsGetCurrentMode)
		{
		*aNetworkMode = GetMode(); //Calling local method to get the networkmode
		ReqCompleted(aTsyReqHandle, KErrNone);
		return KErrNone;
		}

	ReqCompleted(aTsyReqHandle,KErrNotSupported);	
	return KErrNone;
	}

TInt CPhoneMobile::GetBatteryCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aBatteryCaps)
/** Battery Capabilities 
 *
 * This method passes information about the battery capabilities back to the client.
 * @param aTsyReqHandle the request ID. 
 * @param aBatteryCaps, passes back the battery capabilities.
 * @return error code.KErrNone 
 */
	{
	// The battery capabilities are extracted during the initialisation(ATINIT.CPP) 
	// and stored in iPhoneGlobals->iBatteryCaps to be retrieved when needed.
	*aBatteryCaps=iPhoneGlobals->iBatteryCaps;

	if (*aBatteryCaps == 0)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		ReqCompleted(aTsyReqHandle, KErrNone);
	
	return KErrNone;
	}


TInt CPhoneMobile::GetBatteryInfo(const TTsyReqHandle aTsyReqHandle,
								RMobilePhone::TMobilePhoneBatteryInfoV1* aBatteryInfo) 
/** Battery information 
 * 
 * This method passes a request from the etel server to get battery information from 
 * the GSM phone on to the CATGetBattery class. CATGetBattery executes the AT command
 * AT+CBC to get battery information. The information reurned is status and precent of
 * capacity remaining.
 * @param aTsyReqHandle the request ID. 
 * @param aBatteryInfo, pointer to a struct for infortmation about battery status and charge level.
 * @return error code.KErrNone 
 */

	{
	if (!iPhoneGlobals->iBatteryCaps) 
		// There are no battery capabilities - so error request
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
	else
		iATBattery->ExecuteCommand(aTsyReqHandle, aBatteryInfo);
	
	return KErrNone;
	}


TInt CPhoneMobile::GetSubscriberId(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSubscriberId* aId)
/** Get Subscriber Id
 *  
 * This method passes the request for the IMSI number on to CATSubscriberId, if the capabilities
 * are right. The AT command AT+CIMI is used to retrieve the information from the phone.
 * @param aTsyReqHandle the request ID 
 * @param aId,a pointer to the memory where the subscriber id should be stored. 
 * @return error code. 
 */

	{
	// Intialise the clients data in case they ignore our completion code 
	// and use the ID we supplied them.
	aId->Zero();

	if ((iPhoneGlobals->iPhoneIdCaps & RMobilePhone::KCapsGetSubscriberId) && 
		(iATSubscriberId->CachedValue(*aId)))
		{
		// The IMSI has already been retrieved and cached within iATSubscriberId
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else if (iPhoneGlobals->iPhoneIdCaps & RMobilePhone::KCapsGetSubscriberId)
		{
		// Pass the request in to the CATSubscriberId class which executes AT+CIMI.
		iATSubscriberId->ExecuteCommand(aTsyReqHandle,aId);
		}
	else
		ReqCompleted(aTsyReqHandle,KErrNotSupported);

	return KErrNone;
	}


//*******************************************
//*		Get Detected Networks 
//*******************************************
TInt CPhoneMobile::GetDetectedNetworksPhase1(const TTsyReqHandle aTsyReqHandle, 
											 RMobilePhone::TClientId* aClient, 
											 TInt* aBufSize)
/** Get Detected Networks Phase 1 
 *
 * If the GetDetectedNetworksPhase1L should leave this method takes care of that and 
 * makes a premature ReqCompleted to the client.
 *
 * @param aTsyReqHandle the request ID 
 * @param aClient The client sends down a handle that is saved together with the 
 *				  list so the list can be returned to the right client in phase 2.
 * @param aBufSiz The size of the retrieved network lidt. The size is set in 
 * @return error code. 
 */

	{
	LOGTEXT(_L8("MMTsy::GetDetectedNetworksPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=GetDetectedNetworksPhase1L(aTsyReqHandle, aClient, aBufSize););
 	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

																		
TInt CPhoneMobile::GetDetectedNetworksPhase1L(TTsyReqHandle aTsyReqHandle, 
											 RMobilePhone::TClientId* aClient, 
											 TInt* aBufSize)
/** Get Detected Networks Phase 1 
 *
 * Phase 1 retrieves the network list with help of AT+COPS=?, 
 * the memory space needed to store the list is then calculated 
 * and returned to the client.The returned list is saved to be 
 * copied to the client in phase 2, when the client has alocated memory for it.	
 * The actuall execution of the AT command (AT+COPS=?) is done in the CATDetectNetwork class.
 *
 * @param aTsyReqHandle const pointer to the request ID 
 * @param aClient		The client sends down a handle that is saved together with the 
 *						list so the list can be returned to the right client in phase 2.
 * @param aBufSize		The size of the retrieved network. The size is set in CATDetectNetwork::ParseResponseL().
 * @return error code. 
 */
	{

	LOGTEXT(_L8("CPhoneMobile::GetDetectedNetworksPhase1 called"));
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClient,aTsyReqHandle);
	CleanupStack::PushL(read);

	iNetworkListInfoPtrs.iBufSize = aBufSize;
	iNetworkListInfoPtrs.iBufPtr = &(read->iListBuf);
	iGetDetectedNetworks->AppendL(read);

	iATDetectNetwork->ExecuteCommand(aTsyReqHandle, &iNetworkListInfoPtrs); 

	// pop the CListReadAllAttempt (read)
	CleanupStack::Pop();
	return KErrNone;
	}


TInt CPhoneMobile::GetDetectedNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
/** Get detected networks phase 2 
 *
 * In this metod the list which was retrieved during phase 1 is copied to 
 * the memory which the client has allocated for this purose.
 * @param aTsyReqHandle		Const pointer to the request ID 
 * @param aClient			Handle to the client which list we are looking for.
 * @param aBuf				Pointer to the memory that the etelmm has allocated. 
 * @return error code. 
 */
	{	

	LOGTEXT(_L8("CPhoneMobile::GetDetectedNetworksPhase2 called"));
	CListReadAllAttempt* read=NULL;
	TInt numberOfLists = iGetDetectedNetworks->Count();

	// Find the get detected network attempt from this client
	for (TInt i = 0; i < numberOfLists; ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			
			aBuf->Copy(bufPtr);						// Copy the streamed list to the client
			delete read;
			iGetDetectedNetworks->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);	// Completes the retrieval of a network list succesfully.
			return KErrNone;
			}
		}

	Panic(EIllegalEvent); 
	return(KErrNotFound);
	}


void CPhoneMobile::CompleteDetectedNetworksCancel(const TTsyReqHandle aTsyReqHandle)
/** CompleteDetectedNetworksCancel
 *
 * This method is called from the CATDetectNetworks class if a cancel is succesful
 * It cleans-up the client's stored request data
 * @param aTsyReqHandle		Const pointer to the request ID 
 * @return error code. 
 */
	{
	LOGTEXT(_L8("CPhoneMobile::GetDetectedNetworksCancel called"));
	// Remove the read all attempt from iGetDetectedNetworks
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetDetectedNetworks->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	}

RMobilePhone::TMobilePhoneNetworkMode CPhoneMobile::GetMode()
/** 
* This method returns the current networkmode. 
* GSM is the only type of network supported in this TSY. 
*/

	{
	RMobilePhone::TMobilePhoneRegistrationStatus registrationstatus;	
	registrationstatus = iPhoneGlobals->iPhoneStatus.iRegistrationStatus;

	switch (registrationstatus)
		{
	case RMobilePhone::ERegistrationUnknown:
		return RMobilePhone::ENetworkModeUnknown;
	case RMobilePhone::ERegistrationDenied:
	case RMobilePhone::ENotRegisteredNoService:
	case RMobilePhone::ENotRegisteredEmergencyOnly:
	case RMobilePhone::ENotRegisteredSearching:
		return RMobilePhone::ENetworkModeUnregistered;
	case RMobilePhone::ERegisteredOnHomeNetwork:
	case RMobilePhone::ERegisteredRoaming:
		return RMobilePhone::ENetworkModeGsm;
	default:
		return RMobilePhone::ENetworkModeUnknown;
		}
	
	}
