// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the CSmsProtocol and its helper classes.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsprotTraces.h"
#endif

#include <commsdattypesv1_1.h>
#include <logwrap.h>
#include "Gsmumsg.h"
#include "gsmubuf.h"
#include "Gsmuelem.h"
#include "gsmunonieoperations.h"

#include "smsprot.h"
#include "smspfacadestor.h"
#include "smspmodm.h"
#include "smspenum.h"
#include "smspread.h"
#include "smspdel.h"
#include "smspproc.h"
#include "smspmondsk.h"
#include "smspmain.h"
#include "smspver.h"
#include "smsppara.h"
#include "SmsuBackup.h"
#include <es_prot_internal.h>

using namespace CommsDat;

//
// Class implementations
//

// application port range according to 3GPP TS 23.040 v4.4.0
// chapters 9.2.3.24.3 and 9.2.3.24.4

const TInt KMin8BitPortNumber = 240;
const TInt KMax8BitPortNumber = 255;

const TInt KMin16BitPortNumber = 16000;
const TInt KMax16BitPortNumber = 16999;


/**
 *  2 phase constructor
 *  
 */
CSmsProtocolFamily * CSmsProtocolFamily::NewL()
	{
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLFAMILY_NEWL_1, "CSmsProtocolFamily::NewL [version %u.%u.%u]",KSmsPrtMajorVersionNumber,KSmsPrtMinorVersionNumber,KSmsPrtBuildVersionNumber );
	return new (ELeave)CSmsProtocolFamily;
	} // CSmsProtocolFamily::NewL


/**
 *  Implementation of the pure virtual CProtocolFamilyBase::Install().
 *  Called by the socket server after creation of this object.
 *  Does nothing.
 *  
 */
TInt CSmsProtocolFamily::Install()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLFAMILY_INSTALL_1, "CSmsProtocolFamily::Install");
	return KErrNone;
	} // CProtocolFamilyBase::Install


/**
 *  Implementation of the pure virtual CProtocolFamilyBase::Remove().
 *  Called by the socket server before unloading the library for the SMS
 *  protocol family.
 *  Does nothing.
 *  
 */
TInt CSmsProtocolFamily::Remove()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLFAMILY_REMOVE_1, "CSmsProtocolFamily::Remove");
	return KErrNone;
	} // CProtocolFamilyBase::Remove


/**
 *  Implementation of the pure virtual CProtocolFamilyBase::NewProtocolL().
 *  Called by the socket server to create the CSmsProtocol object.
 *  
 *  @param aSockType not used, assumes datagram.
 *  @param aProtocol not used, assumes KSmsDatagram, the only protocol provided.
 *  @return a new instance of the CSmsProtocol class.
 *  
 */
CProtocolBase * CSmsProtocolFamily::NewProtocolL(TUint /*aSockType*/,TUint /*aProtocol*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLFAMILY_NEWPROTOCOLL_1	        , "CSmsProtocolFamily::NewProtocolL");
	return CSmsProtocol::NewL();
	} // CProtocolFamilyBase::NewProtocolL


/**
 *  Implementation of the pure virtual CProtocolFamilyBase::ProtocolList().
 *  Called by the socket server during initialisation to retrieve a list
 *  of the protocols we support.
 *  Only KSmsDatagram is supported.
 *  
 *  @return aProtocolList a pointer to an array of protocol description objects
 *  that this function creates and fills in.
 *  @return the number of protocols supported (1).
 *  
 */
TUint CSmsProtocolFamily::ProtocolList(TServerProtocolDesc *& aProtocolList)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLFAMILY_PROTOCOLLIST_1, "CSmsProtocolFamily::ProtocolList");

	TRAPD(ret, (aProtocolList=new(ELeave) TServerProtocolDesc[1]));
	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLFAMILY_PROTOCOLLIST_2, "WARNING! new TServerProtocolDesc left with %d", ret);
		return 0;
		}

	// Datagram protocol
	aProtocolList[0].iName=KSmsDatagram;
	aProtocolList[0].iAddrFamily=KSMSAddrFamily;
	aProtocolList[0].iSockType=KSockDatagram;
	aProtocolList[0].iProtocol=KSMSDatagramProtocol;
	aProtocolList[0].iVersion=TVersion(KSmsPrtMajorVersionNumber,KSmsPrtMinorVersionNumber,KSmsPrtBuildVersionNumber);
	aProtocolList[0].iByteOrder=ELittleEndian;
	aProtocolList[0].iServiceInfo=KSMSDatagramServiceInfo;
	aProtocolList[0].iNamingServices=0;
	aProtocolList[0].iSecurity=KSocketNoSecurity;
	aProtocolList[0].iMessageSize=KSMSMaxDatagramSize;
	aProtocolList[0].iServiceTypeInfo=ESocketSupport;
	aProtocolList[0].iNumSockets=KSMSNumberSockets;

	return 1;
	} // CProtocolFamilyBase::ProtocolList


/**
 *  Constructor (empty)
 */
CSmsProtocolFamily::CSmsProtocolFamily()
	{
	} // CSmsProtocolFamily::CSmsProtocolFamily


/**
 *  The single exported function, called by the socket server to create
 *  an instance of our CProtocolFamilyBase derived class.
 *  
 */
EXPORT_C CProtocolFamilyBase* InstallSMS()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, INSTALLSMS_1, "CSmsProtocolFamily::CSmsProtocolFamily()");

	CSmsProtocolFamily*  smsProtocolFamily(NULL);
	
	TRAP_IGNORE(smsProtocolFamily = CSmsProtocolFamily::NewL());

	return smsProtocolFamily;
	} // CSmsProtocolFamily::CSmsProtocolFamily


//
// implementation of CSmsProtocol
//


/**
 *  2 phase constructor.
 *  
 */
CSmsProtocol* CSmsProtocol::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NEWL_1, "CSmsProtocol::NewL()");

	return new (ELeave) CSmsProtocol();
	} // CSmsProtocol::NewL


/**
 *  Destructor, ensures all outstanding queues are empty,
 *  all resource handles are closed, and all allocated memory freed.
 *  
 */
CSmsProtocol::~CSmsProtocol()
    {
    delete iSmsPhoneInitialization;
    delete iSmsModemNotification;

    delete iSendQueue;
	delete iSetBearer;	// referenced by iSendQueue

    delete iSmsPDURead;
	delete iReceiveMode;	// referenced by iSmsPDURead
    delete iSmsPhoneEnumeration;
    delete iWriteQueue;
    delete iDeleteQueue;


    delete iSmsReadParams;
    delete iSmsWriteParams;
    delete iSmsMonitorDiskSpace;

    delete iReassemblyStore;

    if(iSegmentationStore)
        {
        iSegmentationStore->Close();
        delete iSegmentationStore;
        }
    iFs.Close();

    iSmsMessaging.Close();
    iGsmPhone.Close();
    iEnumerationPhone.Close();
    iWritePhone.Close();
    iTelServer.Close();
	delete iBackupRestoreSession;
	delete iBootTimer;
    } // CSmsProtocol::~CSmsProtocol



/**
 *  Override of CProtocolBase::NewSAPL().
 *  Called by the server to create a new SMS service access point object.
 *  
 *  @param aSocketType the socket type
 *  @leave Leaves if aSocketType is not KSockDatagram.
 *  @leave Leaves if not enough memory is available.
 *  @return a new CSmsProvider object.
 *  
 */
CServProviderBase *CSmsProtocol::NewSAPL(TUint aSocketType)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NEWSAPL_1, "*CSmsProtocol::NewSAPL [sockettype=%d]", aSocketType);
	if (aSocketType!=KSockDatagram)
		User::Leave(KErrNotSupported);
	return CSmsProvider::NewL(*this);
	} // CProtocolBase::NewSAPL


/**
 *  Override of CProtocolBase::InitL().
 *  Called by the socket server to allow any resource initialisation
 *  by the protocol.
 *  
 *  The following tasks are performed:
 *  - All member objects constructed (deferred from ConstructL() for efficiency reasons; see comment there)
 *  - TSY Name read from Comms Database
 *  - Event notification state machines started
 *  - Modem checked to see if it is already connected, and if so, a number of
 *  state machines started (otherwise wait until we receive notification of connection).
 *  
 */
void CSmsProtocol::InitL(TDesC& /*aTag*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_INITL_1, "CSmsProtocol::InitL");

	User::LeaveIfError(iFs.Connect());

	iReassemblyStore=CFacadeSmsReassemblyStore::NewL(iFs, *this);
	iReassemblyStore->InitL();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_INITL_2, "CSmsProtocol::InitL Constructing members");

	ReadConfigurableSettingsL();

	iBootTimer = CSmsProtocolBootTimer::NewL(*this);
	iBootTimer->Start(iSmsSettings.BootTimerTimeout().Int());

	iSegmentationStore=CSmsSegmentationStore::NewL(iFs);

	iSetBearer=CSmspSetBearer::NewL(iSmsSettings, iSmsMessaging, KSmsSessionPriority);

	iReceiveMode = CSmspReceiveMode::NewL(iSmsSettings, iSmsMessaging, iMobileSmsCaps, KSmsSessionPriority);

	iSmsModemNotification=CSmsModemNotification::NewL(*this);

	iSendQueue = CSmspSendQueue::NewL(*this, *iSegmentationStore, iSmsSettings, iMobileSmsCaps, iSmsMessaging, KSmsSessionPriority, *iSetBearer);

	iSmsMonitorDiskSpace=CSmsMonitorDiskSpace::NewL(*this, iSmsMessaging,iFs);

	iSmsPDURead=CSmsPDURead::NewL(*this, iSmsSettings, iSmsMessaging,*iReassemblyStore,*iSegmentationStore, iMobileSmsCaps, KSmsSessionPriority, *iReceiveMode, *iSmsMonitorDiskSpace);

	iSmsPhoneInitialization = new (ELeave) CSmsPhoneInitialization(iSmsMessaging, iGsmPhone, *iSmsPDURead, iMobileSmsCaps, KSmsSessionPriority, iSetBearer);

	iBackupRestoreSession = CBackupAndRestore::NewL(*this);

/* THIS CODE SHOULD NEVER BE NECESSARY - who could have called CProtocolBase::InitL() if not ESOCK, which means
 * a running C32 already. However, if this is a wrong analysis then reinstate the code but with an explanatory comment
#if defined (__WINS__)
	// Make sure C32 is started under WINS
	TInt ret=StartC32();
	if (ret!=KErrAlreadyExists)
		User::LeaveIfError(ret);
#endif
*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_INITL_3, "CSmsProtocol::InitL Querying CommDb");

	// Read the global modem ID setting from  Cooms Database
	TUint32 modemId = 0;

	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* sess = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* sess = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(sess);

	CMDBField<TUint32>* globalSettingField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
	CleanupStack::PushL(globalSettingField);
	globalSettingField->SetRecordId(1);
	globalSettingField->LoadL(*sess);
	modemId = *globalSettingField;
	CleanupStack::PopAndDestroy(globalSettingField);

	CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
	CleanupStack::PushL(tsyField);
	tsyField->SetRecordId(modemId);
	tsyField->LoadL(*sess);
	iGsmTsyName = *tsyField;
	CleanupStack::PopAndDestroy(tsyField);

	CleanupStack::PopAndDestroy(sess);

    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_INITL_4, "CSmsProtocol::InitL [modemId=%d tsy=%S]",(TInt)modemId, iGsmTsyName );

	// Start event notification state machines
	iSmsModemNotification->Start();

	// Intialise the SmsStack to the state that the phone has been found in
	iModemDetection=iSmsModemNotification->ModemState();

	if (iModemDetection == RPhone::EDetectedPresent && !iBackupRestoreSession->IsBackupOrRestoreInProgress())
		{
		DoPowerUpL(); //Call the leaving version because powering up must work here
		}
	else
		{
		PowerDown();
		}

	//
	// Define the Disk Space Monitor P&S variable...
	//
	TInt  ret;

	TSecurityPolicy  readPolicy(ECapabilityReadDeviceData);
	TSecurityPolicy  writePolicy(ECapabilityWriteDeviceData);
	
	ret = RProperty::Define(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey,
							KUidPSSMSStackDiskSpaceMonitorKeyType, readPolicy, writePolicy);
	if (ret != KErrNone  &&  ret != KErrAlreadyExists)
		{
		User::Leave(ret);
		}

	User::LeaveIfError(RProperty::Set(KUidPSSMSStackCategory,
									  KUidPSSMSStackDiskSpaceMonitorKey, ESmsDiskSpaceAvailable));
	} // CProtocolBase::InitL


/**
 *  Override of CProtocolBase::StartL().
 *  Called by the socket server to indicate all bindings are complete
 *  and that datagram processing can begin.
 *  
 *  Binding is not supported so this indication is not important to us.
 *  
 */
void CSmsProtocol::StartL(void)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_STARTL_1, "CSmsProtocol::StartL");
	} // CSmsProtocol::StartL


/**
 *  Override of CProtocolBase::Identify().
 *  Called by the socket server to obtain a description of the SMS protocol.
 *  
 */
void CSmsProtocol::Identify(TServerProtocolDesc *aDesc)const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_IDENTIFY_1, "CSmsProtocol::Identify");

	aDesc->iName=KSmsDatagram;
	aDesc->iAddrFamily=KSMSAddrFamily;
	aDesc->iSockType=KSockDatagram;
	aDesc->iProtocol=KSMSDatagramProtocol;
	aDesc->iVersion=TVersion(KSmsPrtMajorVersionNumber,KSmsPrtMinorVersionNumber,KSmsPrtBuildVersionNumber);
	aDesc->iByteOrder=ELittleEndian;
	aDesc->iServiceInfo=KSMSDatagramServiceInfo;
	aDesc->iNamingServices=0;
	aDesc->iSecurity=KSocketNoSecurity;
	aDesc->iMessageSize=KSMSMaxDatagramSize;
	aDesc->iServiceTypeInfo=0;
	aDesc->iNumSockets=KSMSNumberSockets;
	} // CProtocolBase::Identify


/**
 *  Override of CProtocolBase::NewHostResolverL().
 *  Called by socket server to create a host resolver.
 *  No host resolver service is provided by this SMS protocol.
 *  
 *  @leave Panics if called.
 *  
 */
CHostResolvProvdBase *CSmsProtocol::NewHostResolverL()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NEWHOSTRESOLVERL_1, "CSmsProtocol::NewHostResolverL");
    SmspPanic(ESmspCantCreateHostResolver);
    return NULL;
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::NewServiceResolverL().
 *  Called by socket server to create a new service resolver.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
CServiceResolvProvdBase *CSmsProtocol::NewServiceResolverL()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NEWSERVICERESOLVERL_1, "*CSmsProtocol::NewServiceResolverL");
    SmspPanic(ESmspCantCreateServiceResolver);
    return NULL;
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::NewNetDatabaseL().
 *  Called by socket server to create a new network database.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
CNetDBProvdBase* CSmsProtocol::NewNetDatabaseL()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NEWNETDATABASEL_1, "CSmsProtocol::NewNetDatabaseL");
    SmspPanic(ESmspCantCreateNetDatabase);
    return NULL;
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::BindL().
 *  Called by next protocol above wishing to bind to the SMS protocol.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
void CSmsProtocol::BindL(CProtocolBase* /*aProtocol*/,TUint /*aId*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_BINDL_1, "CSmsProtocol::BindL");
    SmspPanic(ESmspCantBind);
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::BindToL().
 *  Called by socket server when a lower protocol must bind to the
 *  SMS protocol.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
void CSmsProtocol::BindToL(CProtocolBase* /*aProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_BINDTOL_1, "CSmsProtocol::BindToL");
    SmspPanic(ESmspCantBindTo);
    BULLSEYE_RESTORE
    }

/**
 *  Reads timeout value for send operation from configuration file
 *  
 *  @leave Leaves if configuration file cannot be read
 *  @leave Leaves if value not greater than zero
 */
void CSmsProtocol::ReadConfigurableSettingsL()
	{
	CESockIniData* ini = NULL;
	TRAPD(ret, ini=CESockIniData::NewL(_L("smswap.sms.esk")));
	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_READCONFIGURABLESETTINGSL_1, "esk read failed, error code = [%d]", ret);
		User::Leave(ret);
		}

	CleanupStack::PushL(ini);

	TInt var(0);
	if(ini->FindVar(_L("customTimeoutSettings"),_L("sendTryTimeout"),var))
		{
		if (var > 0)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_READCONFIGURABLESETTINGSL_2, "sendTryTimeout [%d]", var);
			iSmsSettings.SetSendTryTimeout(var);
			}
		else
			{
			User::Leave(KErrArgument);
			}
		}

	if(ini->FindVar(_L("customTimeoutSettings"),_L("bootTimerTimeout"),var))
		{
		if (var > 0)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_READCONFIGURABLESETTINGSL_3, "bootTimerTimeout [%d]", var);
			iSmsSettings.SetBootTimerTimeout(var);
			}
		else
			{
			User::Leave(KErrArgument);
			}
		}

	CleanupStack::PopAndDestroy(ini);
	} // CSmsProtocol::ReadConfigurableSettingsL


/**
 *  Override of CProtocolBase::Send().
 *  A down call from a protocol bound above to send a datagram.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
TInt CSmsProtocol::Send(TDes8& /*aDes*/,TSockAddr* /*aTo*/,TSockAddr* /*aFrom*/,CProtocolBase* /*aSourceProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_SEND_1, "CSmsProtocol::Send");
    SmspPanic(ESmspCantSend);
    return KErrNotSupported;
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::Process().
 *  An up call from a protocol bound below to process a datagram.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
void CSmsProtocol::Process(TDes8& /*aDes*/,TSockAddr* /*aFrom*/,TSockAddr* /*aTo*/,CProtocolBase* /*aSourceProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_PROCESS_1, "CSmsProtocol::Process");
    SmspPanic(ESmspCantProcess);
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::GetOption().
 *  A down call from a protocol bound above to get a protocol option.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
TInt CSmsProtocol::GetOption(TUint /*aLevel*/,TUint /*aName*/,TDes8 & /*aOption*/,CProtocolBase* /*aSourceProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_GETOPTION_1, "CSmsProtocol::GetOption");
    // SmspPanic(ESmspCantGetOption);
    return KErrNotSupported;
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::SetOption().
 *  A down call from a protocol bound above to set a protocol option.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
TInt CSmsProtocol::SetOption(TUint /*aLevel*/,TUint /*aName*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_SETOPTION_1, "CSmsProtocol::SetOption");
    //SmspPanic(ESmspCantSetOption);
    return KErrNotSupported;
    BULLSEYE_RESTORE
    }

/**
 *  Override of CProtocolBase::Error().
 *  An up call from a protocol bound below to indicate an error.
 *  Not supported by this protocol.
 *  
 *  @leave Panics if called.
 *  
 */
void CSmsProtocol::Error(TInt /*aError*/,CProtocolBase* /*aSourceProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ERROR_1, "CSmsProtocol::Error");
    SmspPanic(ESmspCantError);
    BULLSEYE_RESTORE
    }

/**
 *  Adds an observer to the protocol's observer list.
 *  Used by CSmsProvider and CWapSmsProtocol.
 *  
 *  @leave Leaves if not enough memory is available.
 *  
 */
void CSmsProtocol::AddSmsMessageObserverL(MSmsMessageObserver& aObserver)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ADDSMSMESSAGEOBSERVERL_1, "CSmsProtocol::AddSmsMessageObserverL [aObserver=0x%08x]", &aObserver);
    iSmsMessageObserverList.AppendL(&aObserver);
    SetClosingDown(EFalse);
    }

/**
 *  Binds an SMS observer to the protocol.
 *  The protocol ensures there are no providers with duplicate addresses.
 *  Used by CSmsProvider::SetLocalName(), and SMS WAP protocol.
 *  
 *  @param aObserver the observer.
 *  @param aSmsAddr the local address of the observer.
 *  @return KErrNone on success, or KErrAlreadyExists if a duplicate exists.
 *  
 */
TInt CSmsProtocol::BindSmsMessageObserver(MSmsMessageObserver& aObserver,const TSmsAddr& aSmsAddr)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_BINDSMSMESSAGEOBSERVER_1, "CSmsProtocol::BindSmsMessageObserver 0x%08x", &aObserver);
	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));
	TInt ret=KErrNone;

	if (!SmsAddrIsAlreadyUsed(&aObserver,aSmsAddr))
		{
		aObserver.SetLocalAddress(aSmsAddr);
		OrderSmsMessageObserver(aObserver);

		// If not in the powered-up state then the SAR store is closed. When the phone does power-up
		// then anything waiting in the store will get processed
		if(CheckPoweredUp() == KErrNone)
			{
			//
			// check the SAR store for any complete messages.
			// if there are any, send them to the observer
			//
			TRAP(ret, ProcessCompleteSmsMessagesL());
			if(ret != KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_BINDSMSMESSAGEOBSERVER_2, "WARNING! CSmsProtocol::ProcessCompleteSmsMessagesL left with %d", ret);
				}
			}
		}
	else
		{
		ret=KErrAlreadyExists;
		}
	return ret;
	} // CSmsProvider::SetLocalName


/**
 *  Removes an observer from the observer list.
 *  Observers should at least call this method in their destructors.
 *  
 */
void CSmsProtocol::RemoveSmsMessageObserver(const MSmsMessageObserver& aObserver)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_REMOVESMSMESSAGEOBSERVER_1, "CSmsProtocol::RemoveSmsMessageObserver 0x%08x", &aObserver);
    __ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));
    TInt index=ObserverIndex(aObserver);
    iSmsMessageObserverList.Delete(index);
    }

/**
 *  Handles a request from a SAP to send a SMS message.
 *  Ensures there is a current connection to the modem and queues the message.
 *  Completes with an error immediately if CheckPoweredUp() returns an error code
 */
void CSmsProtocol::SendSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver,TUint aOptions)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_SENDSMSMESSAGE_1, "CSmsProtocol::SendSmsMessage [aObserver=0x%08X]", (TUint) &aObserver);
	__ASSERT_DEBUG(ObserverIsPresent(aObserver), SmspPanic(ESmspMessageObserverNotFound));

	// Ensure the modem is connected and initialized
	const TInt err = CheckPoweredUp();

	if (err != KErrNone)
		{
		// Nope, complete immediately
		aObserver.MessageSendCompleted(err);
		delete aSmsMessage;
		}
	else
		{
		iSendQueue->Queue(aSmsMessage, aObserver, aOptions);
		}
	}

TInt CSmsProtocol::CheckPoweredUp() const
    {
    TInt err = KErrNone;
    
    if( iModemDetection == RPhone::EDetectedNotPresent || iState == EPoweredDown )
        {
        err = KErrDisconnected;
        }
    else if( iBackupRestoreSession->IsBackupOrRestoreInProgress() )
        {
        err = KErrAccessDenied;
        }
    else
        {
        err = iSmsPhoneInitialization->Initialized();
        }
    if( err != KErrNone )
        {
        OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CHECKPOWEREDUP_1, "CSmsProtocol::CheckPoweredUp [err=%d, iModemDetection=%d, IsBackupOrRestoreInProgress=%d]", err, iModemDetection, iBackupRestoreSession->IsBackupOrRestoreInProgress());
        }    
    return err;
    }

/**
 *  Cancels a previous request to send an SMS message.
 *  Searches the send queue for the message.  If it is found at the
 *  head of the queue the sending state machine is cancelled, which
 *  in turn will callback to the protocol and delete the message.
 *  If the message is elsewhere in the queue, it is simply removed
 *  from the queue and the observer notified.
 *  
 *  @leave Panics in DEBUG if the message is not found in the queue.
 *  
 */
void CSmsProtocol::CancelSendSmsMessage(MSmsMessageObserver& aObserver,TBool)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CANCELSENDSMSMESSAGE_1, "CSmsProtocol::CancelSendSmsMessage 0x%08x", &aObserver);

	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));

	iSendQueue->CancelObserver(aObserver);
	} // CSmsProtocol::CancelSendSmsMessage


/**
 *  Handles a request from a SAP to enumerate the SMS messages stored on the phone.
 */
void CSmsProtocol::EnumeratePhone(MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ENUMERATEPHONE_1, "CSmsProtocol::EnumeratePhone");
	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));

	// Ensure the modem is connected and initialized
	const TInt err = CheckPoweredUp();

	if (err != KErrNone)
		aObserver.EnumeratePhoneCompleted(err);
	else
		{
		if (iPhoneEnumerationObserver!=NULL)
			{
			aObserver.EnumeratePhoneCompleted(KErrInUse);
			}
		else
			{
			iPhoneEnumerationObserver=&aObserver;
			iSmsPhoneEnumeration->Start();
			}
		}
	} // CSmsProtocol::EnumeratePhone


/**
 *  Cancels a previous request to enumerate the message on the phone memory.
 *  
 *  @leave Panics in DEBUG if the SAP is invalid.
 *  
 */
void CSmsProtocol::CancelEnumeratePhone(MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CANCELENUMERATEPHONE_1, "CSmsProtocol::CancelEnumeratePhone");

	__ASSERT_DEBUG(ObserverIsPresent(aObserver), SmspPanic(ESmspMessageObserverNotFound));
	__ASSERT_DEBUG(iPhoneEnumerationObserver==NULL || &aObserver==iPhoneEnumerationObserver, SmspPanic(ESmspMessageWrongObserver));

	if (iSmsPhoneEnumeration != NULL)
		{
		iSmsPhoneEnumeration->Cancel();
		}

	iReassemblyStore->DeleteEnumeratedSIMEntries();

    (void) aObserver;
	} // CSmsProtocol::CancelEnumeratePhone


TInt CSmsProtocol::ExternalizeEnumeratedMessagesL(CSmsProvider& aProvider,TInt& aCount)
	{
	return iReassemblyStore->ExternalizeEnumeratedMessagesL(aProvider, aCount);
	} // CSmsProtocol::ExternalizeEnumeratedMessagesL


/**
 *  Handles a request to write an SMS message to the phone's memory.
 *  Completes with an error immediately if CheckPoweredUp() returns an error code
 *  Otherwise the message is added to the tail of the write queue.
 */
void CSmsProtocol::WriteSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_WRITESMSMESSAGE_1, "CSmsProtocol::WriteSmsMessage");
	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));

	const TInt err = CheckPoweredUp();

	if (err != KErrNone)
		{
		aObserver.MessageWriteCompleted(err);
		delete aSmsMessage;
		}
	else
		{
		iWriteQueue->Queue(aSmsMessage, aObserver, NULL);
		}
	} // CSmsProtocol::WriteSmsMessage


void CSmsProtocol::CancelWriteSmsMessage(MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CANCELWRITESMSMESSAGE_1, "CSmsProtocol::CancelWriteSmsMessage()");

	if (iWriteQueue != NULL)
		iWriteQueue->CancelObserver(aObserver);
	} // CSmsProtocol::CancelWriteSmsMessage


/**
 *  Handles a request from a SAP to delete a message from the phone memory.
 *  Completes with an error immediately if CheckPoweredUp() returns an error code
 *  If the cached image of the phone's memory is not yet initialised, complete the
 *  request with KErrNotReady.
 *  Otherwise match the message in the phone image and queue the message for deletion.
 *  
 *  @note aSmsMessage is destroyed from memory on completion.
 *  
 */
void CSmsProtocol::DeleteSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETESMSMESSAGE_1, "CSmsProtocol::DeleteSmsMessage");
	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));

	// Ensure the modem is connected and initialized
	const TInt err = CheckPoweredUp();

	if (err != KErrNone)
		{
		// Not connected or backup running
		aObserver.MessageDeleteCompleted(err);
		}
	else if (aSmsMessage->iSlotArray.Count()==0)
		aObserver.MessageDeleteCompleted(KErrArgument);
	else
		DeletePDUs(aSmsMessage->iSlotArray,&aObserver);

	delete aSmsMessage;
	} // CSmsProtocol::DeleteSmsMessage


/**
 *  Cancels a previous request to delete a message from the phone memory.
 *  
 *  @leave Panics in DEBUG if the SAP is invalid.
 *  
 */
void CSmsProtocol::CancelDeleteSmsMessage(MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CANCELDELETESMSMESSAGE_1, "CSmsProtocol::CancelDeleteSmsMessage");

	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));

	iDeleteQueue->CancelObserver(aObserver);
	} // CSmsProtocol::CancelDeleteSmsMessage


/**
 *  Handles a request from a SAP to read the SMS parameters stored on the phone.
 *  Completes with an error immediately if CheckPoweredUp() returns an error code
 *  If TSY doesn't support retrieving of parameters, the request is completed
 *  immediately with KErrNotSupported.
 *  The request is completed with KErrNoMemory if creation of CSmsReadParams
 *  object fails.
 *  The request is completed with KErrInUse if another SAP is reading.
 *  Otherwise, the reading process is started.
 *  
 */
void CSmsProtocol::ReadSmsParameters(MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_READSMSPARAMETERS_1, "CSmsProtocol::ReadSmsParameters");
	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));
	TInt ret = CheckPoweredUp();

	if (ret == KErrNone)
		{
		if(iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsGetSmspList)
			{
			if (iSmsReadParams==NULL)
				{
				TRAP(ret,(iSmsReadParams=CSmsReadParams::NewL(*this,iSmsSettings,iSmsMessaging)));
				}
			else if (iSmsReadParams->IsActive())
				{
				ret = KErrInUse;
				}
			}
		else
			ret = KErrNotSupported;
		}

	if(ret != KErrNone)
		aObserver.ReadSmsParamsCompleted(ret, NULL);
	else
		iSmsReadParams->Start(aObserver);
	} // CSmsProtocol::ReadSmsParameters


/**
 *  Cancels a previous request to read SMS parameters.
 *  
 */
void CSmsProtocol::CancelReadSmsParams()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CANCELREADSMSPARAMS_1, "CSmsProtocol::CancelReadSmsParams");

	if(iSmsReadParams != NULL)
		iSmsReadParams->Cancel();
	} // CSmsProtocol::CancelReadSmsParams


/**
 *  Handles a request to write an SMS parameters to the phone's memory.
 *  Completes with an error immediately if CheckPoweredUp() returns an error code
 *  If TSY doesn't support retrieving and writing of parameters, the request is completed
 *  immediately with KErrNotSupported.
 *  The request is completed with KErrInUse if another SAP is reading or writing.
 *  The request is completed with KErrNoMemory if creation of CSmsReadParams or
 *  CSmsWriteParams object fails.
 *  Otherwise, the writing process is started.
 *  
 */
void CSmsProtocol::WriteSmsParameters(CMobilePhoneSmspList* aMobilePhoneSmspList,MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_WRITESMSPARAMETERS_1, "CSmsProtocol::WriteSmsParameters");
	__ASSERT_DEBUG(ObserverIsPresent(aObserver),SmspPanic(ESmspMessageObserverNotFound));

	TInt ret = CheckPoweredUp();

	if (ret == KErrNone)
		{
		if(iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsGetSmspList && iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSetSmspList)
			{
			if (iSmsReadParams==NULL)
				{
				TRAP(ret,(iSmsReadParams=CSmsReadParams::NewL(*this,iSmsSettings,iSmsMessaging)));
				}
			else if (iSmsReadParams->IsActive())
				ret=KErrInUse;
			}
		else
			ret=KErrNotSupported;
		}

	if (ret==KErrNone)
		{
		if (iSmsWriteParams==NULL)
			{
			TRAP(ret,(iSmsWriteParams=CSmsWriteParams::NewL(*this,iSmsSettings,iSmsMessaging,*iSmsReadParams)));
			}
		else if (iSmsWriteParams->IsActive())
			ret=KErrInUse;
		}

	if (ret==KErrNone)
		iSmsWriteParams->Start(aObserver,aMobilePhoneSmspList);
	else
		{
		aObserver.WriteSmsParamsCompleted(ret);
		delete aMobilePhoneSmspList;
		}
	} // CSmsProtocol::WriteSmsParameters


/**
 *  Cancels a previous request to write SMS parameters.
 *  
 */
void CSmsProtocol::CancelWriteSmsParams()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CANCELWRITESMSPARAMS_1, "CSmsProtocol::CancelWriteSmsParams");

	if(iSmsWriteParams != NULL)
		iSmsWriteParams->Cancel();
	} // CSmsProtocol::CancelWriteSmsParams


/**
 *  Processes a received SMS message by matching it to an observer and
 *  notifying the observer of the new message.
 *  
 *  If the message has been matched to a message previously sent by the protocol
 *  (i.e. a status report from a previous submit), then the original message and address
 *  are supplied.  Matching priority in this case is given to the original sender.
 *  
 *  If the message remains in phone or SIM memory, the message is sent to the "receive any"
 *  observer, otherwise the match based on the message and address of the observer.
 *  
 *  @param aSmsMessage the received message
 *  @param aOriginalSmsAddr pointer to the address of the sender of a previously sent
 *  message matched to the received one (e.g. status report). Null if not matched.
 *  @param aOriginalSmsMessage pointer to a message previously sent matched to the
 *  received one (e.g. status report).	Null if not matched.
 *  @param aDes user data for the deliver report acknowledging this message to the SC.
 *  Filled in by the observer.
 *  @return KErrNone if the observer handled the message successfully, otherwise an error.
 *  
 */
TInt CSmsProtocol::ProcessMessageL(const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,
		const CSmsMessage* /*aOriginalSmsMessage*/,TDes& aDes)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_PROCESSMESSAGEL_1, "CSmsProtocol::ProcessMessage");

	MSmsMessageObserver* observer=NULL;
	if (aOriginalSmsAddr!=NULL)
		{
		// Status report and original message matched - send to original sender as priority
		observer=MatchSmsAddressToObserver(*aOriginalSmsAddr);
		if (observer==NULL)
			{
			// Sender no longer present or is read only - match on received message as usual
			observer=MatchSmsMessageToObserver(aSmsMessage);
			}
		}
	else if (iPhoneEnumerationObserver==NULL  ||  IsAppPortSMS(aSmsMessage ))
		{
		// Only match if not currently enumerating or if it is meant for an app port.
		observer = MatchSmsMessageToObserver(aSmsMessage);
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_PROCESSMESSAGEL_2, "CSmsProtocol::ProcessMessage [observer=0x%08x]",observer);
	TInt ret=KErrNone;

	if (observer!=NULL)
		{
		TBool isAppPortSms = IsAppPortSMS(aSmsMessage );

		if(isAppPortSms && iBootTimer !=NULL && iBootTimer->IsActive() && observer->GetLocalAddress().SmsAddrFamily() == ESmsAddrRecvAny)
			{
			ret = KErrNotReady;
			}
		else
			{
			ret=observer->MessageReceived(aSmsMessage,aDes);
			}

		if(ret == KErrNone && (observer->ClientConfirmsMessage() == EFalse)  && observer->GetLocalAddress().SmsAddrFamily() != ESmsAddrRecvAny)
		{
			iReassemblyStore->DeleteMessageL(aSmsMessage, EFalse);
			ret=KErrInUse;                
			// used to signal to client that there is nothing to be done on reassembly store
			// and code different from KErrNone will do - won't be propagated
		}
		else if(ret == KErrNotFound && observer->GetLocalAddress().SmsAddrFamily() != ESmsAddrRecvAny)
			{
			if ( iBootTimer && !iBootTimer->IsActive() && (aOriginalSmsAddr==NULL || aOriginalSmsAddr->SmsAddrFamily() != ESmsAddrRecvAny))
 				{
				TSmsAddr addr;
				addr.SetSmsAddrFamily(ESmsAddrRecvAny);
				ret = ProcessMessageL(aSmsMessage, &addr, NULL, aDes);
				}
			}
		}
	else
		{
        ret=KErrNotReady; // Observers are not ready
		}
	return ret;
	} // CSmsProtocol::ProcessMessageL


/**
 *  Deletes one or more PDUs from phone or SIM memory.
 *  The PDUs are added to the deletion queue.  If this queue is empty, the deletion
 *  state machine is started.
 *  
 *  @param aSlotArray
 */
void CSmsProtocol::DeletePDUs(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray, MSmsMessageObserver* aObserver)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETEPDUS_1, "CSmsProtocol::DeletePDUs [count=%d aObserver=0x%08X", aSlotArray.Count(), (TUint)aObserver);
    __ASSERT_DEBUG(aSlotArray.Count() != 0, SmspPanic(KSmspSlotArrayEmpty));

    for(TInt i=0; i< aSlotArray.Count() ;i++)
        {
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETEPDUS_2, "CSmsProtocol::DeletePDUs index: %d store %S", aSlotArray[i].iIndex, aSlotArray[i].iStore);
        }

	if (iDeleteQueue != NULL && aSlotArray.Count() != 0)
		{
		iDeleteQueue->Queue(aSlotArray, aObserver);
		}
	} // CSmsProtocol::DeletePDUs

/**
 *  Called when the modem detection state has changed.
 *  If a modem has connection has just been made, a number of event monitoring
 *  state machines are restarted, and as is the bearer setting state machine.
 *  If the the modem connection was lost, outstanding operations are cancelled.
 *  
 */
void CSmsProtocol::ModemNotificationCompleted(TInt aStatus,
											  RPhone::TModemDetection aNewState)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MODEMNOTIFICATIONCOMPLETED_1, "CSmsProtocol::ModemNotificationCompleted(): aStatus=%d, aNewState=%d",aStatus, aNewState);

	TBool stateChanged = EFalse;

	if (aStatus==KErrNone)
		{
		switch (iModemDetection)
			{
			//
			// it goes from OFF to ON
			//
			case RPhone::EDetectedNotPresent:
			case RPhone::EDetectedUnknown:
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MODEMNOTIFICATIONCOMPLETED_2, "RPhone::EDetectedNotPresent: [OFF -> ON]");
				if (aNewState==RPhone::EDetectedPresent)
					{
					// There is a new modem connection
					iModemDetection=aNewState;
					stateChanged = ETrue;
					}
				break;
				}
			//
			// it goes from ON to OFF
			//
			case RPhone::EDetectedPresent:
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MODEMNOTIFICATIONCOMPLETED_3, "RPhone::EDetectedPresent: [ON -> OFF]");
				if (aNewState!=RPhone::EDetectedPresent)
					{
					// Ah, lost our modem - cancel outstanding operations
					iModemDetection=aNewState;
					stateChanged = ETrue;
					}
				break;
				}
			default:
				{
				__ASSERT_DEBUG(EFalse,SmspPanic(KSmspPanicUnknownModemDetection));
				}
			}

		if (iModemDetection == RPhone::EDetectedPresent && !iBackupRestoreSession->IsBackupOrRestoreInProgress())
			{
			if (stateChanged || iState == EPoweredDown)
				{
				PowerUp();
				}
			}
		else if (stateChanged || iState != EPoweredDown)
			{
			PowerDown();
			}
		}
	} // CSmsProtocol::ModemNotificationCompleted


void CSmsProtocol::DiskSpaceMonitorStateChange(TSmsDiskSpaceMonitorStatus aStatus)
/**
 * Called when the Disk Space Monitor state has changed.
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DISKSPACEMONITORSTATECHANGE_1, "CSmsProtocol::DiskSpaceMonitorStateChange(): aStatus=%d", aStatus);

	RProperty::Set(KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey, aStatus);
	} // CSmsProtocol::DiskSpaceMonitorStateChange


/**
 *  Called when all message send requests have completed.
 *  Increments the number of segmentation store accesses and purges the store
 *  if it exceeds CSmsProtocol::KNumSARStoreAccessesBeforePurging.
 */
void CSmsProtocol::MessageSendCompleted(TInt aStatus)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGESENDCOMPLETED_1, "*** CSmsProtocol::MessageSendCompleted [aStatus=%d iNumSegmentationStoreAccesses=%d]", aStatus, iNumSegmentationStoreAccesses);
    (void) aStatus;
	iNumSegmentationStoreAccesses++;
	if (iNumSegmentationStoreAccesses>=KNumSARStoreAccessesBeforePurging)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGESENDCOMPLETED_2, "iSegmentationStore->PurgeL Start");
		TRAPD(ret, iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse));
		if(ret!=KErrNone)
			{
			// we need to close the file because the function
			// left with the file opened
			// iSegmentationStore->CloseFile();
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGESENDCOMPLETED_3, "WARNING! iSegmentationStore->PurgeL left with %d", ret);
			}
		iNumSegmentationStoreAccesses=0;
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGESENDCOMPLETED_4, "iSegmentationStore->PurgeL End [ret=%d]", ret);
		}
	} // CSmsProtocol::KNumSARStoreAccessesBeforePurging


/**
 *  Called when the state machine for enumerating messages on the
 *  phone has completed.  Notifies the SAP that made the request.
 *  
 */
void CSmsProtocol::PhoneEnumerationCompleted(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_PHONEENUMERATIONCOMPLETED_1, "CSmsProtocol::PhoneEnumerationCompleted");
	iPhoneEnumerationObserver->EnumeratePhoneCompleted(aStatus);
	} // CSmsProtocol::PhoneEnumerationCompleted


/**
 *  Called by CProtocolBase::Close() to handle any delayed shutdown.
 *  If there are messages in the deletion queue then the shut down flag
 *  is set, otherwise CProtocolBase::CanClose() is called to finish closing.
 *  
 */
void CSmsProtocol::CloseNow()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CLOSENOW_1, "CSmsProtocol::CloseNow");
	if (iDeleteQueue != NULL && iDeleteQueue->IsActive())
		SetClosingDown(ETrue);
	else
		CanClose();
	} // CSmsProtocol::CloseNow


/**
 *  Private constructor.
 *  
 */
CSmsProtocol::CSmsProtocol()
	:iSmsMessageObserverList(8)
	,iModemDetection(RPhone::EDetectedUnknown)
	{
	iSmsSettings.SetDeletePDUsFromSIM(EFalse);
	iSmsSettings.SetDeletePDUsFromPhoneStores(EFalse);
	iSmsSettings.SetDeletePDUsFromCombinedStores(EFalse);
	iNext8BitPort=KMin8BitPortNumber;
	iNext16BitPort=KMin16BitPortNumber;
	} // CSmsProtocol::CSmsProtocol


/**
 *  Returns the index of an observer in the observer list.
 *  
 */
TInt CSmsProtocol::ObserverIndex(const MSmsMessageObserver& aObserver) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_OBSERVERINDEX_1, "CSmsProtocol::ObserverIndex()");

	TInt count=iSmsMessageObserverList.Count();
	TInt index=0;
	for (; index<count; index++)
		if (iSmsMessageObserverList[index]==&aObserver)
			break;
	return index;
	} // CSmsProtocol::ObserverIndex


/**
 *  Checks if an SMS address type is a duplicate of an existing SAP / observer.
 *  
 */
TBool CSmsProtocol::SmsAddrIsAlreadyUsed(const MSmsMessageObserver* aObserver,const TSmsAddr& aSmsAddr)const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_SMSADDRISALREADYUSED_1, "CSmsProtocol::SmsAddrIsAlreadyUsed()");

	TBool isduplicate=EFalse;
	TInt count=iSmsMessageObserverList.Count();
	for (TInt i=0; (i<count) && (!isduplicate); i++)
		isduplicate=(iSmsMessageObserverList[i]->SmsAddrIsDuplicate(aObserver,aSmsAddr));
	return isduplicate;
	} // CSmsProtocol::SmsAddrIsAlreadyUsed


/**
 *  A utility class for prioritizing the SMS protocol observers based
 *  on their SMS address type.
 *  A higher priority observer will be given the opportunity to
 *  accept a received messages before one of a lower priority.
 */
class TKeySmsObserver : public TKeyArrayFix
	{
public:
	TKeySmsObserver();
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
	static TInt SmsAddressPriority(const TSmsAddr& aAddr);
	};


/**
 *  Constructor
 *  
 */
TKeySmsObserver::TKeySmsObserver()
	:TKeyArrayFix(0,ECmpTInt)
	{
	iKeyOffset = 0;
	} // TKeySmsObserver::TKeySmsObserver


/**
 *  Static method that returns the priority of a given SMS address.
 *  
 *  @leave Panics on an unknown address type.
 *  
 */
TInt TKeySmsObserver::SmsAddressPriority(const TSmsAddr& aAddr)
	{
	TSmsAddrFamily fam = aAddr.SmsAddrFamily();

	switch (fam)
		{
		case ESmsAddrEmail:
			return 11;
		case ESmsAddrApplication8BitPort:
			return 10;
		case ESmsAddrApplication16BitPort:
			return 9;
		case ESmsAddrMessageIndication:
			return 8;
		case ESmsAddrStatusReport:
			return 7;
		case ESmsAddrMatchIEI:
			return 6;
		case ESmsAddrMatchText:
			return 5;
		case ESmsAddrRecvAny:
			return 4;
		case ESmsAddrSendOnly:
			return 3;
		case ESmsAddrLocalOperation:
			return 2;
		case ESmsAddrUnbound:
			return 1;
		default:
			SmspPanic(ESmspUnknownSmsAddressFamily);
		};
	return KErrNotFound;
	} // TKeySmsObserver::SmsAddressPriority


/**
 *  Compares two observers.
 *  Override of TKeyArrayFix::Compare() called on to help sort
 *  the observer list based on their address types.
 *  
 */
TInt TKeySmsObserver::Compare(TInt aLeft, TInt aRight) const
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TKEYSMSOBSERVER_COMPARE_1, "TKeySmsObserver::Compare [left=%d, right=%d]", aLeft, aRight);

	const TInt lhptr = -1; // Left higher priority than right
	const TInt rhptl = 1; // Right higher priority than left

	MSmsMessageObserver* left = *(MSmsMessageObserver**)At(aLeft);
	MSmsMessageObserver* right = *(MSmsMessageObserver**)At(aRight);

	const TSmsAddr& leftAddr = left->GetLocalAddress();
	const TSmsAddr& rightAddr = right->GetLocalAddress();

	TSmsAddrFamily leftFamily = leftAddr.SmsAddrFamily();


	TInt leftPriority = SmsAddressPriority(leftAddr);
	TInt rightPriority = SmsAddressPriority(rightAddr);

	if (leftPriority > rightPriority)
		return lhptr;
	if (rightPriority > leftPriority)
		return rhptl;

	if (leftFamily == ESmsAddrMatchText)
		{
		// Longer strings are higher priority than shorter ones
		TInt rightLen = rightAddr.TextMatch().Length();
		TInt leftLen = leftAddr.TextMatch().Length();
		if (leftLen > rightLen)
			return lhptr;
		if (rightLen > leftLen)
			return rhptl;
		}

	return 0;
	} // TKeyArrayFix::Compare


/**
 *  Re-orders the observer list using TKeySmsObserver to determine priorities.
 *  
 */
void CSmsProtocol::OrderSmsMessageObserver(const MSmsMessageObserver& /*aObserver*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ORDERSMSMESSAGEOBSERVER_1, "CSmsProtocol::OrderSmsMessageObserver()");

	TKeySmsObserver smsObsKey;
#ifdef _DEBUG
	TInt ret=iSmsMessageObserverList.Sort(smsObsKey);
	__ASSERT_DEBUG(ret==KErrNone,SmspPanic(ESmspCorruptObserverList));
#else
    iSmsMessageObserverList.Sort(smsObsKey);
#endif
	} // CSmsProtocol::OrderSmsMessageObserver


/**
 *  Attempts to match an incoming message to a SAP / observer.  Starts with the
 *  highest priority observer.
 *  
 */
MSmsMessageObserver* CSmsProtocol::MatchSmsMessageToObserver(const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MATCHSMSMESSAGETOOBSERVER_1, "CSmsProtocol::MatchSmsMessageToObserver()");

	TInt count=iSmsMessageObserverList.Count();
	for (TInt i=0;i<count;i++)
		{
		MSmsMessageObserver* obs = iSmsMessageObserverList[i];
		if (IsMatch(obs->GetLocalAddress(),aSmsMessage))
			return obs;
		}
	return NULL;
	} // CSmsProtocol::MatchSmsMessageToObserver


/**
 *  Tries to match an SMS address to a SAP / observer.  Starts with the highest
 *  priority observer.
 *  
 */
MSmsMessageObserver* CSmsProtocol::MatchSmsAddressToObserver(const TSmsAddr& aAddr)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MATCHSMSADDRESSTOOBSERVER_1, "CSmsProtocol::MatchSmsAddressToObserver()");

	TInt count=iSmsMessageObserverList.Count();
	for (TInt i=0;i<count;i++)
		{
		MSmsMessageObserver* obs = iSmsMessageObserverList[i];
		if (obs->GetLocalAddress()==aAddr)
			return obs;
		}
	return NULL;
	} // CSmsProtocol::MatchSmsAddressToObserver


/**
 *  Static function checks if a message is matched to a given SMS address type.	Used by
 *  MatchSmsMessageToObserver() to find a matching observer for the message.
 *  
 */
TBool CSmsProtocol::IsMatch(const TSmsAddr& aSmsAddr, const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ISMATCH_1, "CSmsProtocol::IsMatch()");

	TSmsAddrFamily family = aSmsAddr.SmsAddrFamily();

	switch(family)
	{
		case (ESmsAddrUnbound):
		case (ESmsAddrSendOnly):
		case(ESmsAddrLocalOperation):
			return EFalse;

		case(ESmsAddrApplication8BitPort ):
			{
			return (MatchApplicationPort(aSmsMessage,aSmsAddr.Port(),EFalse));
			}

		case (ESmsAddrApplication16BitPort ):
			{
			return(MatchApplicationPort(aSmsMessage,aSmsAddr.Port(),ETrue));
			}

		case (ESmsAddrMessageIndication):
			{
			if (MatchInformationElement(aSmsMessage,CSmsInformationElement::ESmsIEISpecialSMSMessageIndication))
				return ETrue;
			if (aSmsMessage.SmsPDU().DataCodingSchemePresent())
				{
				TSmsDataCodingScheme::TSmsDCSBits7To4 bits7to4 = aSmsMessage.SmsPDU().Bits7To4();
				switch (bits7to4)
					{
					case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationDiscardMessage:
					case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndication7Bit:
					case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2:
						return ETrue;
					default:
						break;
					}
				}
			// CPHS Implementation of message waiting indicator (CPHS Phase 2)
			TGsmSmsTelNumber address;
			aSmsMessage.ParsedToFromAddress(address);

			//as far as I can see you can't get the npi from the address. (==0?)
			//might want to compare the content of the tel number with the spec to make sure
			//that we are dealing with a cphs message !

			if((address.iTypeOfAddress.TON() == EGsmSmsTONAlphaNumeric)  &&
				(address.iTelNumber.Length() == 4) &&
					((address.iTelNumber[2] & 0x7E) == 0x10)  &&// x001 000x constant value
						((address.iTelNumber[3] & 0x7E) == 0x00) )// x000 000x constant value
						return ETrue; // This means that the SMS received is a Message Waiting indication
			return EFalse;
			}

		case (ESmsAddrStatusReport):
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ISMATCH_2, "TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationDiscardMessage:");
			if (aSmsMessage.Type() == CSmsPDU::ESmsStatusReport)
				return ETrue;
			}
			break;
		case (ESmsAddrMatchIEI):
			{
			return (MatchInformationElement(aSmsMessage,CSmsInformationElement::TSmsInformationElementIdentifier(aSmsAddr.IdentifierMatch())));
			}
		case (ESmsAddrMatchText):
			{
			TBuf<TSmsAddr::EMaxTextMatchLength> textMatch;
			textMatch.Copy(aSmsAddr.TextMatch());
			TBuf<TSmsAddr::EMaxTextMatchLength> smsMsgBuf;
			TInt bufferLen = aSmsMessage.Buffer().Length();
			bufferLen = Min(bufferLen,textMatch.Length());
			aSmsMessage.Buffer().Extract(smsMsgBuf,0,bufferLen);
			if (textMatch.CompareF(smsMsgBuf)==KErrNone)
				return ETrue;
			return EFalse;
			}
		case (ESmsAddrEmail):
			{
			if(aSmsMessage.IsEmailHeader())
	 			return ETrue;
			break;
			}
		case (ESmsAddrRecvAny):
			return ETrue;
		default:
			return EFalse;

		}
	return EFalse;
	} // CSmsProtocol::IsMatch

/**
 *  Static function checks if a message contains a given port.  Used by IsMatch()
 *  to determine if an SMS address type matches an application port.
 *  
 */
TBool CSmsProtocol::MatchApplicationPort(const CSmsMessage& aSmsMessage,TUint aPort,TBool a16Bit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MATCHAPPLICATIONPORT_1, "CSmsProtocol::MatchApplicationPort");

	if (!aSmsMessage.SmsPDU().UserDataPresent())
		return EFalse;

	const CSmsPDU& Pdu = aSmsMessage.SmsPDU();
	TInt ToPort = 0;
	TInt FromPort = 0;
	TBool Is16BitPorts = 0;

    Pdu.ApplicationPortAddressing(ToPort,FromPort,&Is16BitPorts);
	if((TInt)aPort == ToPort && Is16BitPorts == a16Bit) return ETrue;

	return EFalse;
	} // CSmsProtocol::MatchApplicationPort


/**
 *  Static function checks if a message contains a given information element.  Used by IsMatch()
 *  to determine if an SMS address type matches a message.
 *  
 */
TBool CSmsProtocol::MatchInformationElement(const CSmsMessage& aSmsMessage, CSmsInformationElement::TSmsInformationElementIdentifier aIeVal)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MATCHINFORMATIONELEMENT_1, "CSmsProtocol::MatchInformationElement");

	if (!aSmsMessage.SmsPDU().UserDataPresent())
		return EFalse;

	TInt count=aSmsMessage.SmsPDU().UserData().NumInformationElements();
	for (TInt i=0; i<count; i++)
		{
		TInt identifier=aSmsMessage.SmsPDU().UserData().InformationElement(i).Identifier();
		if (identifier==aIeVal)
			return ETrue;
		}
	return EFalse;
	} // CSmsProtocol::MatchInformationElement


/**
 *  Searches the reassembly store for complete messages and tries to match
 *  them to an observer.  If a match is found the message is passed to
 *  the observer and deleted from the store, otherwise it's left in store.
 *  Called when a new observer is added or a PDU has been received and
 *  successfully processed.
 *  
 */
void CSmsProtocol::ProcessCompleteSmsMessagesL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_PROCESSCOMPLETESMSMESSAGESL_1, "CSmsProtocol::ProcessCompleteSmsMessagesL");

	if(iPhoneEnumerationObserver) return;

	iReassemblyStore->ProcessCompleteSmsMessagesL(*this, iSmsPDURead->CurrentMessage());
	} // CSmsProtocol::ProcessCompleteSmsMessagesL


/**
 *  Called when the state machine for processing a received PDU has completed.
 *  The reassembly store access count is incremented and purged if it exceeds
 *  CSmsProtocol::KNumSARStoreAccessesBeforePurging.
 *  The completed entry is removed from the processing queue and if the queue is
 *  not empty, and the processing state machinery is started for the next entry.
 *  
 */
void CSmsProtocol::MessageReadedSuccessfully()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGEREADEDSUCCESSFULLY_1, "CSmsProtocol::MessageReadedSuccessfully");
	TRAPD(ret,ProcessCompleteSmsMessagesL());
	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGEREADEDSUCCESSFULLY_2, "WARNING! CSmsProtocol::ProcessCompleteSmsMessagesL left with %d", ret);
		}

	iNumReassemblyStoreAccesses++;
	if(iNumReassemblyStoreAccesses>=KNumSARStoreAccessesBeforePurging)
		{
		TRAP(ret, iReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(),EFalse));

		if(ret!=KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_MESSAGEREADEDSUCCESSFULLY_3, "WARNING! iReassemblyStore->PurgeL left with %d", ret);
			}

		iNumReassemblyStoreAccesses=0;
		}
	if(CheckPoweredUp() == KErrNone )
		iSmsPDURead->ResumeSmsReception();
	} // CSmsProtocol::MessageReadedSuccessfully


/**
 *  method to delete sms from the reastore
 */
void CSmsProtocol::DeleteSMSFromReaStoreL(const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETESMSFROMREASTOREL_1, "CSmsProtocol::DeleteSMSFromReaStoreL entry");
	if (aSmsMessage.Type() == CSmsPDU::ESmsStatusReport)
	{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETESMSFROMREASTOREL_2, "CSmsProtocol::DeleteSMSFromReaStoreL it's SR");
		return;
	}

	TSmsDataCodingScheme::TSmsClass  msgClass;

	if (aSmsMessage.SmsPDU().DataCodingSchemePresent()	&&	aSmsMessage.SmsPDU().Class(msgClass))
		{
		if (msgClass == TSmsDataCodingScheme::ESmsClass0)
			{
			if (!aSmsMessage.IsComplete())
				{
				CIncompleteClass0MessageInfo& incompleteClass0MsgInfo = (CIncompleteClass0MessageInfo &) aSmsMessage.GetOperationsForNonIEL(ESmsIncompleteClass0MessageParameter);
				// Retrieve incomplete class 0 message information & process
				TInt startPos, endPos;
				TBool isLastMessage;
				incompleteClass0MsgInfo.GetIncompleteMessageInfoL(startPos, endPos, isLastMessage);
				if (!isLastMessage)
					{
					/*
					Only in this condition set incomplete message as forwarded
					which internally will remove the PDUs from pre-allocated file.
					*/
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETESMSFROMREASTOREL_3, "CSmsProtocol::DeleteSMSFromReaStoreL Incomplete Message Not last segment");
					iReassemblyStore->SetIncompleteMessageForwardedToClientL(aSmsMessage);
					return;
					}
				}
			}
		}
	iReassemblyStore->DeleteMessageL(aSmsMessage, ETrue);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DELETESMSFROMREASTOREL_4, "CSmsProtocol::DeleteSMSFromReaStoreL exit");
	} // CSmsProtocol::DeleteSMSFromReaStoreL


/**
 *  Used to notify observer of the change in state of the modem connection.
 *  Send only observers are not notified (why?).
 */
void CSmsProtocol::NotifyMessageObservers(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NOTIFYMESSAGEOBSERVERS_1, "CSmsProtocol::NotifyMessageObservers");

	TInt count=iSmsMessageObserverList.Count();
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NOTIFYMESSAGEOBSERVERS_2, "CSmsProtocol::NotifyMessageObservers [count=%d, aStatus=%d]",count, aStatus);
	for (TInt index=0; index<count; index++)
		{
		MSmsMessageObserver* observer=iSmsMessageObserverList[index];
		TSmsAddrFamily fam = observer->GetLocalAddress().SmsAddrFamily();
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NOTIFYMESSAGEOBSERVERS_3, "CSmsProtocol::NotifyMessageObservers [family=%d]", fam);
		switch (fam)
			{
			case ESmsAddrMessageIndication:
			case ESmsAddrStatusReport:
			case ESmsAddrMatchIEI:
			case ESmsAddrMatchText:
			case ESmsAddrEmail:
			case ESmsAddrRecvAny:
			case ESmsAddrLocalOperation:
			case ESmsAddrApplication8BitPort:
	        case ESmsAddrApplication16BitPort:
				{
				observer->ModemNotificationCompleted(aStatus);
				break;
				}
			case ESmsAddrSendOnly:
			case ESmsAddrUnbound:
				break;
			default:
				SmspPanic(ESmspPanicAddrFamilyNotAllowed);
			}
		}
	} // CSmsProtocol::NotifyMessageObservers


void CSmsProtocol::PowerUp()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERUP_1, "CSmsProtocol::PowerUp [iState=%d]", iState);
	__ASSERT_DEBUG(iState == EPoweredDown, SmspPanic(KSmspPanicUnexpectedState));

	if (iState == EPoweredDown)
		{
		TRAPD(err, DoPowerUpL());

		if (err != KErrNone)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERUP_2, "WARNING: CSmsProtocol::DoPowerUpL left [err=%d iState=%d]", err, iState);
			__ASSERT_DEBUG(iState == EPoweredDown, SmspPanic(KSmspPanicUnexpectedState));
			PowerDown();
			}
		}
	} // CSmsProtocol::PowerUp


/**
 *  Handles the event of the modem powering on (when it had been powered off).
 *  Does the following...
 *  - The TSY loaded
 *  - ETEL resource handles initialised
 *  - ETEL parameters set and retrieved
 *  - A number of state machines started
 *  
 */
void CSmsProtocol::DoPowerUpL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_1, "CSmsProtocol::DoPowerUpL");
	__ASSERT_DEBUG(iModemDetection==RPhone::EDetectedPresent,SmspPanic(KSmspPhoneHasNotTurnedOn));

	//Open the segmentation and reassembly stores

	iReassemblyStore->OpenStoreL();
	iSegmentationStore->OpenStoreL();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_2, "CSmsProtocol::DoPowerUpL->PurgeL Start");
	iReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_3, "CSmsProtocol::DoPowerUpL->PurgeL End");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_4, "CSmsProtocol::DoPowerUpL->PurgeL Start");
	iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_5, "CSmsProtocol::DoPowerUpL->PurgeL End");

	// Connect to ETEL and load the TSY
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.LoadPhoneModule(iGsmTsyName));
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_6, "CSmsProtocol::DoPowerUpL  Connected to Etel");

	// Find the phone corresponding to this TSY and open a number of handles on it
	TInt numPhones;
	User::LeaveIfError(iTelServer.EnumeratePhones(numPhones));
	TInt i=0;
	for (; i<numPhones; i++)
		{
		RTelServer::TPhoneInfo info;
		User::LeaveIfError(iTelServer.GetPhoneInfo(i,info));
		TName tsyName;
		User::LeaveIfError(iTelServer.GetTsyName(i,tsyName));
		if (tsyName.CompareF(iGsmTsyName)==KErrNone)
			{
			User::LeaveIfError(iGsmPhone.Open(iTelServer,info.iName));
			User::LeaveIfError(iEnumerationPhone.Open(iTelServer,info.iName));
			User::LeaveIfError(iWritePhone.Open(iTelServer,info.iName));
			break;
			}
		}
	__ASSERT_DEBUG(i<numPhones,SmspPanic(ESmspPhoneNotFound));
	if (iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended)!=KErrNone)
		User::LeaveIfError(iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorBasic));
	User::LeaveIfError(iSmsMessaging.Open(iGsmPhone));

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_7, "CSmsProtocol::DoPowerUpL  Opened TSY handles");

	if (iSmsPhoneEnumeration == NULL)
		iSmsPhoneEnumeration=CSmsPhoneEnumeration::NewL(*this, iSmsSettings, *iReassemblyStore, *iSegmentationStore, iEnumerationPhone, KSmsSessionPriority, *iSmsMonitorDiskSpace);

	if (iWriteQueue == NULL)
		iWriteQueue = CSmspWriteQueue::NewL(*this, iSmsSettings, iWritePhone,*iSegmentationStore, KSmsSessionPriority);

	if (iDeleteQueue == NULL)
		iDeleteQueue = CSmspDeleteQueue::NewL(*this,iSmsSettings,iSmsMessaging, KSmsSessionPriority);

	// Start state machines
	iSmsPhoneInitialization->Start();

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_8, "CSmsProtocol::DoPowerUpL  Started state machines");

	NotifyMessageObservers(KIoctlSelectModemPresent);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_DOPOWERUPL_9, "CSmsProtocol::DoPowerUpL  Notified message observers");


	// Process any waiting messages
	ProcessCompleteSmsMessagesL();

	iState = EPoweredUp;
	} // CSmsProtocol::DoPowerUpL


/**
 *  Handles the event of the modem powering off (when it had been powered on).
 *  Does the following...
 *  - State mechines cancelled
 *  - Close TSY handles
 *  - Disconnect from Etel
 *  
 *  This function will also be called if PowerUpL() leaves
 */
void CSmsProtocol::PowerDown()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERDOWN_1, "CSmsProtocol::PowerDown");
    
    iSetBearer->Cancel();
    iReceiveMode->Cancel();
    
    iSendQueue->Cancel();
    
    iSmsPDURead->Cancel();
    
    iSmsMonitorDiskSpace->Cancel();
    
    delete iSmsPhoneEnumeration; //must be deleted because uses iSmsMessaging which is soon closed
    iSmsPhoneEnumeration = NULL;
    
    iSmsPhoneInitialization->Cancel();
    
    if( iSmsReadParams != NULL )
        {
        iSmsReadParams->Cancel();
        }
    if( iSmsWriteParams != NULL )
        {
        iSmsWriteParams->Cancel();
        }
    
    delete iWriteQueue; //must be deleted because uses iSmsMessaging which is soon closed
    iWriteQueue = NULL;
    
    delete iDeleteQueue; //must be deleted because uses iSmsMessaging which is soon closed
    iDeleteQueue = NULL;
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERDOWN_2, "CSmsProtocol::PowerDown  Cancelled state machines");
    
    NotifyMessageObservers(KIoctlSelectModemNotPresent);
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERDOWN_3, "CSmsProtocol::PowerDown  Notified message observers");
    
    // Close TSY handles
    iSmsMessaging.Close();
    iGsmPhone.Close();
    iEnumerationPhone.Close();
    iWritePhone.Close();
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERDOWN_4, "CSmsProtocol::PowerDown  Closed TSY handles");
    
    // Disconnect from Etel
    iTelServer.Close();
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_POWERDOWN_5, "CSmsProtocol::PowerDown  Disconnected from Etel");
    
    iReassemblyStore->Close();
    iSegmentationStore->Close();
    
    iState = EPoweredDown;
    }

void CSmsProtocol::CloseNowWrap()
    {
    // Ignore in code coverage - called when PDU delete queue is not 
    // empty and SMS stack is closing down - can only get this situation
    // when the PDU delete has been initiated by the SMS stack itself 
    // (rather than the client) and the PDUs are still being deleted 
    // when last client disconnects.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_CLOSENOWWRAP_1, "CSmsProtocol::CloseNowWrap()");
    
    if( iDeleteQueue == NULL || !iDeleteQueue->IsActive() )
        {
        CloseNow();
        }
    BULLSEYE_RESTORE
    }

void CSmsProtocol::HandleBackupOrRestoreStartingL()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_HANDLEBACKUPORRESTORESTARTINGL_1, "CSmsProtocol::HandleBackupOrRestoreStartingL [ModemState=%d]",  iSmsModemNotification->ModemState());
    PowerDown();
    }

void CSmsProtocol::HandleBackupOrRestoreCompleteL()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_HANDLEBACKUPORRESTORECOMPLETEL_1, "CSmsProtocol::HandleBackupOrRestoreCompleteL [ModemState=%d]", iSmsModemNotification->ModemState());
    
    if (iModemDetection == RPhone::EDetectedPresent)
        {
        PowerUp();
        }
    }

/**
 *  Set the sap port number
 */
TBool CSmsProtocol::AllocateLocalAddress(TSmsAddr& aAddr)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ALLOCATELOCALADDRESS_1, "CSmsProtocol::AllocateLocalAddressL");

	TBool found=EFalse;
	TUint count=0,attempts=0;
	TSmsAddr locAddr=aAddr;

	if(locAddr.SmsAddrFamily() == ESmsAddrApplication8BitPort )
		{
		count =KMax8BitPortNumber-KMin8BitPortNumber+1;
		}
	else if(locAddr.SmsAddrFamily() == ESmsAddrApplication16BitPort )
		{
		count =KMax16BitPortNumber-KMin16BitPortNumber+1;
		}

	for(;!found && attempts < count; ++attempts)
		{
		if(locAddr.SmsAddrFamily() == ESmsAddrApplication8BitPort )
			{
			locAddr.SetPort(iNext8BitPort++);
			if(iNext8BitPort > KMax8BitPortNumber)iNext8BitPort=KMin8BitPortNumber;
			}
		else if(locAddr.SmsAddrFamily() == ESmsAddrApplication16BitPort )
			{
			locAddr.SetPort(iNext16BitPort++);
			if(iNext16BitPort > KMax16BitPortNumber)iNext8BitPort=KMin16BitPortNumber;
			}
		if(!SmsAddrIsAlreadyUsed(NULL,locAddr))found=ETrue;
		}

	if(found)
		{
		aAddr=locAddr;
		}
	return found;
	} // CSmsProtocol::AllocateLocalAddress

/**
 *  Static method that returns the boolean value indicating whether the given
 *  CSmsMessage is for application port.
 *  This function is an utility function which determines the passed sms message
 *  is for application port or not. This function is also used in CFacadeSmsReassemblyStore class.
 *
 *  @param aSmsMessage reference to CSmsMessage object.
 *  
 */
TBool CSmsProtocol::IsAppPortSMS(const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_ISAPPPORTSMS_1, "CSmsProtocol::IsAppPortSMS()");

	TSmsAddr addr;
	addr.SetSmsAddrFamily(ESmsAddrMatchIEI);
	addr.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit);
	if (IsMatch(addr,aSmsMessage))
		{
		return ETrue;
		}

	addr.SetSmsAddrFamily(ESmsAddrMatchIEI);
	addr.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit);
	if (IsMatch(addr,aSmsMessage))
		{
		return ETrue;
		}

	addr.SetSmsAddrFamily(ESmsAddrMatchText);
	addr.SetTextMatch(_L8("//SCK"));
	if (IsMatch(addr,aSmsMessage))
		{
		return ETrue;
		}

	return EFalse;
	} // CSmsProtocol::IsAppPortSMS


const RMobilePhone::TMobilePhoneNetworkInfoV1& CSmsProtocol::NetworkInfo() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NETWORKINFO_1, "CSmsProtocol::NetworkInfo()");

	return iSmsPhoneInitialization->NetworkInfo();
	} // CSmsProtocol::NetworkInfo


TBool CSmsProtocol::NetworkInfoAvailable() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOL_NETWORKINFOAVAILABLE_1, "CSmsProtocol::NetworkInfoAvailable()");

	return iSmsPhoneInitialization->NetworkInfoAvailable();
	} // CSmsProtocol::NetworkInfoAvailable


// implementation of CSmsProtocolBootTimer
//

/**
 *  2 phase contructor
 *  
 *  @param aActive Reference to an CSmsProtocol object
 */
CSmsProtocolBootTimer* CSmsProtocolBootTimer::NewL(CSmsProtocol& aSmsProtocol)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLBOOTTIMER_NEWL_1, "CSmsProtocolBootTimer::NewL");
    
    CSmsProtocolBootTimer* self = new(ELeave) CSmsProtocolBootTimer(aSmsProtocol);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 *  D'tor
 */
CSmsProtocolBootTimer::~CSmsProtocolBootTimer()
    {
    Cancel();
    }

/**
 *  Start a timeout specified in aTimeIntervalMicroSeconds32
 */
void CSmsProtocolBootTimer::Start(const TTimeIntervalMicroSeconds32& aTimeIntervalMicroSeconds32)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLBOOTTIMER_START_1, "CSmsProtocolBootTimer::Start");
	After(aTimeIntervalMicroSeconds32);
	}

/**
 *  C'tor
 */
CSmsProtocolBootTimer::CSmsProtocolBootTimer(CSmsProtocol& aSmsProtocol)
: CTimer(KSmsSessionPriority), iSmsProtocol(aSmsProtocol)
    {
    CActiveScheduler::Add(this);
    }

/**
 *  Timer completed - cancel the observer
 */
void CSmsProtocolBootTimer::RunL()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPROTOCOLBOOTTIMER_RUNL_1, "CSmsProtocolBootTimer::RunL [iStatus=%d]", iStatus.Int() );
    iSmsProtocol.MessageReadedSuccessfully();
    }
