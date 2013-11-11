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
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ws_prtclTraces.h"
#endif

#include <e32base.h>
#include <f32file.h>
#include "smsprot.h"
#include "WAPDGRM.H"
#include "wappstor.h"
#include "ws_main.h"
#include "es_wsms.h"
#include "ws_obsvr.h"
#include "SmsuBackup.h"

//
// Sets the values used for the local wap ports
// Limits the number of simultaneously open SAPs to 254
//
const TInt KMaxWapPortNumber = 255;
const TInt KMinWapPortNumber = 1;

const TInt KMax8BitPortNumber = 255;
const TInt KMax16BitPortNumber = 65535;


/**
 *  Constructor
 */
CWapSmsProtocol::CWapSmsProtocol()
    {
	iSAPList.SetOffset(CWapSmsProvider::LinkOffset());
    } // CWapSmsProtocol::CWapSmsProtocol


//
// Factory
//
CWapSmsProtocol* CWapSmsProtocol::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_NEWL_1, "CWapSmsProtocol::NewL()");

	CWapSmsProtocol* p=new(ELeave) CWapSmsProtocol;
	CleanupStack::PushL(p);
	User::LeaveIfError(p->iFs.Connect());
	p->iObserver[0]=CWapProtocolObserver::NewL(p);
	p->iObserver[1]=CWapProtocolObserver::NewL(p);
	p->iObserver[2]=CWapProtocolObserver::NewL(p);
	p->iWapStore=CWapReassemblyStore::NewL(p->iFs);

	p->iBackupRestoreSession=CBackupAndRestore::NewL(*p);
	CleanupStack::Pop();
	return p;
	} // CWapSmsProtocol::NewL


void CWapSmsProtocol::HandleBackupOrRestoreStartingL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_HANDLEBACKUPORRESTORESTARTINGL_1, "CWapSmsProtocol::HandleBackupOrRestoreStartingL");
	iWapStore->Close();
	} // CWapSmsProtocol::HandleBackupOrRestoreStartingL


void CWapSmsProtocol::HandleBackupOrRestoreCompleteL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_HANDLEBACKUPORRESTORECOMPLETEL_1, "CWapSmsProtocol::HandleBackupOrRestoreCompleteL ");
	iWapStore->DoOpenL();
	} // CWapSmsProtocol::HandleBackupOrRestoreCompleteL


/**
 *  Destructor
 */
CWapSmsProtocol::~CWapSmsProtocol()
    {
    delete iBackupRestoreSession;

    RemoveObserversFromSmsProtocol();
    delete iObserver[0];
    delete iObserver[1];
    delete iObserver[2];
    delete iWapStore;

    if (iSmsProtocol)
        {
        iSmsProtocol->Close();
        }

    iFs.Close();
    } // CWapSmsProtocol::~CWapSmsProtocol


//
// Socket server asking for a new sap
//
CServProviderBase *CWapSmsProtocol::NewSAPL(TUint aSocketType)
	{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_NEWSAPL_1, "*CWapSmsProtocol::NewSAPL");

	if (aSocketType!=KSockDatagram)
		User::Leave(KErrNotSupported);
	CWapSmsProvider *pSAP = CWapSmsProvider::NewL(this);
	AddSAP(pSAP, aSocketType);
	return pSAP;
	} // CWapSmsProtocol::NewSAPL


//
// InitL call from socket server.
//
void CWapSmsProtocol::InitL(TDesC& /*aTag*/)
	{
	}


//
// Called by another protocol that runs on top of WAP
//
void CWapSmsProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*aId*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_BINDL_1, "CWapSmsProtocol::BindL");
    Panic(EWapSmsCantBind);
    BULLSEYE_RESTORE
    }

//
// StartL call from socket server.
// Open lower layers
//
void CWapSmsProtocol::StartL()
	{
	}


//
// Identify request from SOCKET server
//
void CWapSmsProtocol::Identify(TServerProtocolDesc *aDes) const
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_IDENTIFY_1, "CWapSmsProtocol::Identify");

	aDes->iName=KWAPSMSProtocolId;
	aDes->iAddrFamily=KWAPSMSAddrFamily;
	aDes->iSockType=KSockDatagram;
	aDes->iProtocol=KWAPSMSDatagramProtocol;

	aDes->iVersion=TVersion(KWapSmsMajorVersionNumber,KWapSmsMinorVersionNumber,KWapSmsBuildVersionNumber);
	aDes->iByteOrder=ELittleEndian;
	aDes->iServiceInfo=KWAPSMSDatagramServiceInfo;
	aDes->iNamingServices=0;
	aDes->iSecurity=KSocketNoSecurity;
	aDes->iMessageSize=KWAPSMSMaxDatagramSize;
	aDes->iServiceTypeInfo=0;
	aDes->iNumSockets=KWAPSMSNumberSockets;
	} // CWapSmsProtocol::Identify


//
// Called by socket server to initiate bind to SMS protocol
//
void CWapSmsProtocol::BindToL(CProtocolBase* aProtocol)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_BINDTOL_1, "CWapSmsProtocol::BindToL");

	TServerProtocolDesc info;
	aProtocol->Identify(&info);
	TUint theirId = info.iProtocol;
	Identify(&info);
	TUint ourId = info.iProtocol;

	if (theirId==ourId)
		User::Leave(KErrGeneral); // Nutter!

	if (theirId!=KSMSDatagramProtocol)
		User::Leave(KErrGeneral);

	iSmsProtocol = (CSmsProtocol*)aProtocol;
	iSmsProtocol->Open();
	iSmsProtocol->StartL();
	BindObserversToSmsL();
	} // CWapSmsProtocol::BindToL


//
// Register the observers with sms.prt
//
void CWapSmsProtocol::BindObserversToSmsL()
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_BINDOBSERVERSTOSMSL_1, "CWapSmsProtocol::BindObserversToSmsL");

	iNextSapPort=KMinWapPortNumber;
	TSmsAddr addr0;
	TSmsAddr addr1;
	TSmsAddr addr2;

	addr0.SetSmsAddrFamily(ESmsAddrMatchIEI);
	addr0.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit);
	addr1.SetSmsAddrFamily(ESmsAddrMatchIEI);
	addr1.SetIdentifierMatch(CSmsInformationElement::ESmsIEIApplicationPortAddressing16Bit);
	addr2.SetSmsAddrFamily(ESmsAddrMatchText);
	addr2.SetTextMatch(_L8("//SCK"));

	iSmsProtocol->AddSmsMessageObserverL(*iObserver[0]);
	TInt ret=iSmsProtocol->BindSmsMessageObserver(*iObserver[0],addr0);
    User::LeaveIfError(ret);

	iSmsProtocol->AddSmsMessageObserverL(*iObserver[1]);
	ret=iSmsProtocol->BindSmsMessageObserver(*iObserver[1],addr1);
    User::LeaveIfError(ret);

	iSmsProtocol->AddSmsMessageObserverL(*iObserver[2]);
	ret=iSmsProtocol->BindSmsMessageObserver(*iObserver[2],addr2);
    User::LeaveIfError(ret);
	} // CWapSmsProtocol::BindObserversToSmsL


//
// Deregister the observers
//
void CWapSmsProtocol::RemoveObserversFromSmsProtocol()
	{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_REMOVEOBSERVERSFROMSMSPROTOCOL_1, "CWapSmsProtocol::RemoveObserversFromSmsProtocol");

	if (iSmsProtocol==NULL)
		return;
	iSmsProtocol->RemoveSmsMessageObserver(*iObserver[0]);
	iSmsProtocol->RemoveSmsMessageObserver(*iObserver[1]);
	iSmsProtocol->RemoveSmsMessageObserver(*iObserver[2]);
	} // CWapSmsProtocol::RemoveObserversFromSmsProtocol


//
// Send a datagram originating from a higher protocol
// Can never be called as we don't implement BindL
//
//
TInt CWapSmsProtocol::Send(TDes8 &, TSockAddr* /*to*/,TSockAddr* /*from*/,CProtocolBase* /*aSourceProtocol*/)
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_SEND_1, "CWapSmsProtocol::Send");
    Panic(EWapSmsSendCallCantBind);
    return KErrNone;
    BULLSEYE_RESTORE
    }

//
// Receive an SMS
//
void CWapSmsProtocol::ProcessSmsL(const CSmsMessage& aSmsMessage)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_PROCESSSMSL_1, "CWapSmsProtocol::ProcessSmsL");
	TInt index=0;
	TBool storeDatagramComplete = EFalse;
	TBool isNewStyleClient = EFalse;

	__ASSERT_DEBUG(aSmsMessage.IsComplete(),Panic(EWapSmsIncompleteSms));

	CWapDatagram* wapDatagram = CWapDatagram::NewL(aSmsMessage);

	CleanupStack::PushL(wapDatagram);
	TBool isCompleteDatagram = wapDatagram->IsComplete();
	if (!isCompleteDatagram)
		{

		storeDatagramComplete = iWapStore->AddMessageL(index,*wapDatagram);
		if (!storeDatagramComplete)
			{
			CleanupStack::PopAndDestroy(wapDatagram);
			return;
			}
		iWapStore->GetDatagramL(index,*wapDatagram);
		}

	CWapSmsProvider* wapsmsProvider = LookupSAP(wapDatagram);

	if (wapsmsProvider)
		{
		isNewStyleClient= wapsmsProvider->IsNewStyleClient();
		if(isCompleteDatagram && isNewStyleClient)//8 bit datagram or complete messages, Need to store it for new clients
			{
			storeDatagramComplete = iWapStore->AddMessageL(index,*wapDatagram);
			if (!storeDatagramComplete)
				{
				CleanupStack::PopAndDestroy(wapDatagram);
				return;
				}
			}
		if(!isNewStyleClient && !isCompleteDatagram)
			{
			iWapStore->BeginTransactionLC();
			iWapStore->DeleteEntryL(index);
			iWapStore->CommitTransactionL();
			}
		CleanupStack::Pop(wapDatagram);
		wapsmsProvider->AddToQueue(wapDatagram);
		return;
		}
	else if(!isCompleteDatagram)
		{
		iWapStore->BeginTransactionLC();
		iWapStore->DeleteEntryL(index);
		iWapStore->CommitTransactionL();
		}

	CleanupStack::PopAndDestroy(wapDatagram);
	User::Leave(KErrNotFound);
	} // CWapSmsProtocol::ProcessSmsL


//
// Get Wap Protocol options
// If none match the level/name pass the query on to SMS
//
TInt CWapSmsProtocol::GetOption(TUint aLevel, TUint aName, TDes8& aOption, CProtocolBase* /*aSourceProtocol*/)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_GETOPTION_1, "CWapSmsProtocol::GetOption");

	TInt ret = iSmsProtocol->GetOption(aLevel, aName, aOption,this);
	return ret;
	} // CWapSmsProtocol::GetOption


//
// Set Wap Protocol options
// If none match the level/name pass the query on to SMS
//
TInt CWapSmsProtocol::SetOption(TUint aLevel, TUint aName, const TDesC8& aOption, CProtocolBase* /*aSourceProtocol*/)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_SETOPTION_1, "CWapSmsProtocol::SetOption");
    
    TInt ret= iSmsProtocol->SetOption(aLevel,aName,aOption,this);
    return ret;
    } // CWapSmsProtocol::SetOption


//
// Inform all SAPs of error.
//
void CWapSmsProtocol::Error(TInt aError, CProtocolBase* /*aSourceProtocol*/)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_ERROR_1, "CWapSmsProtocol::Error");
    
    TDblQueIter<CWapSmsProvider> iter(iSAPList);
    CWapSmsProvider* sap;
    while (sap = iter++, sap!=NULL)
        sap->Error(aError,MSocketNotify::EErrorAllOperations);
    }

//
// Socket server asking for a host resolver
//
CHostResolvProvdBase *CWapSmsProtocol::NewHostResolverL()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_NEWHOSTRESOLVERL_1, "*CWapSmsProtocol::NewHostResolverL");
    Panic(EWapSmsCantCreateHostResolver);
    return NULL;
    BULLSEYE_RESTORE
    }

//
// Socket server asking for a service resolver
//
CServiceResolvProvdBase *CWapSmsProtocol::NewServiceResolverL()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_NEWSERVICERESOLVERL_1, "*CWapSmsProtocol::NewServiceResolverL");
    Panic(EWapSmsCantCreateServiceResolver);
    return NULL;
    BULLSEYE_RESTORE
    }

//
// Socket server asking for a net data base
//
CNetDBProvdBase* CWapSmsProtocol::NewNetDatabaseL()
    {
    // Ignore in code coverage - not intended to be used
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_NEWNETDATABASEL_1, "CWapSmsProtocol::NewNetDatabaseL");
    Panic(EWapSmsCantCreateNetDatabase);
    return NULL;
    BULLSEYE_RESTORE
    }

//
// Add a SAP to the SAP list and checks the SAR store for this SAP's entries
//
void CWapSmsProtocol::AddSAP(CWapSmsProvider* aSAP, TUint /*aSockType*/)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_ADDSAP_1, "CWapSmsProtocol::AddSAP");

	iSAPList.AddLast(*aSAP);
	} // CWapSmsProtocol::AddSAP


//
// Set the sap port number
//
TBool CWapSmsProtocol::AllocateLocalAddress(TWapAddr& aAddr)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_ALLOCATELOCALADDRESS_1, "CWapSmsProtocol::AllocateLocalAddressL");
	//
	TBool found=EFalse;
	TUint count=0,attempts=0;
	count =KMaxWapPortNumber-KMinWapPortNumber+1;

	TSmsAddr addr8;
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);

	for(;!found && attempts < count;attempts++)
		{
			addr8.SetPort(iNextSapPort++);
			if(iNextSapPort > KMaxWapPortNumber)iNextSapPort=KMinWapPortNumber;
			if(!iSmsProtocol->SmsAddrIsAlreadyUsed(NULL,addr8))found=ETrue;
		}
	if(found)
		aAddr.SetWapPort(static_cast<TWapPortNumber>(addr8.Port()));

	return found;
	} // CWapSmsProtocol::AllocateLocalAddress


//
// Find the provider who wants the message
//
CWapSmsProvider* CWapSmsProtocol::LookupSAP(CWapDatagram* aMsg)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_LOOKUPSAP_1, "CWapSmsProtocol::LookupSAP");

	TBuf8<KMaxSockAddrSize> addrBuf;
	addrBuf.Copy(aMsg->FromAddress());
	TInt toPort=0;
	TInt fromPort=0;
	TInt Is16BitPorts = ETrue;
	aMsg->Ports(fromPort,toPort,&Is16BitPorts);

	// Modification to relax port checking to allow 16 bit port number < KMax8BitPortNumber
	if(toPort < 0 || (!Is16BitPorts && toPort > KMax8BitPortNumber) || (Is16BitPorts && toPort > KMax16BitPortNumber) )
		{
		return NULL;
		}

	TWapAddr addr;
	addr.SetWapAddress(addrBuf);
	addr.SetWapPort(static_cast<TWapPortNumber>(toPort));

	TDblQueIter<CWapSmsProvider> iter(iSAPList);
	CWapSmsProvider* sap;
	while (sap = iter++, sap!=NULL)
		{
		if (sap->MatchesLocalAddress(addr))
			return sap;
		}

	return NULL;
	} // CWapSmsProtocol::LookupSAP


//
// Check for duplicate address
//
TInt CWapSmsProtocol::AddrAlreadyUsedByWAP(const TWapAddr &aAddr, const CWapSmsProvider* aSap)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_ADDRALREADYUSEDBYWAP_1, "CWapSmsProtocol::AddrAlreadyUsedByWAP");

	TDblQueIter<CWapSmsProvider> iter(iSAPList);
	CWapSmsProvider* sap;

	while ((sap = iter++)!=NULL)
		{
		if (sap->MatchesLocalAddress(aAddr))
			{
			if(sap==aSap)
				return KErrAlreadyExists;
			else
				return KErrInUse;
			}
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_ADDRALREADYUSEDBYWAP_2, "CWapSmsProtocol::AddrAlreadyUsedByWAP not used by WAP");
	return KErrNone;
	} // CWapSmsProtocol::AddrAlreadyUsedByWAP


//
// Return a pointer to the sms protocol
//
CSmsProtocol* CWapSmsProtocol::SmsProtocol()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_SMSPROTOCOL_1, "CWapSmsProtocol::SmsProtocol()");

    return iSmsProtocol;
    } // CWapSmsProtocol::SmsProtocol


//
//	Search the store for particular datagram, if found the entry is deleted
//
TBool CWapSmsProtocol::FindAndDeleteMsg(CWapDatagram& aDatagram)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_FINDANDDELETEMSG_1, "CWapSmsProtocol::FindAndDeleteMsg()");

	TInt err;
	TInt ret = EFalse;
	TRAP(err,ret=iWapStore->FindAndDeleteDatagramL(aDatagram));
	__ASSERT_DEBUG(!err,Panic(EWapSmsNotFoundInStore));
	if(!err && ret)
		return ETrue;
	else
		return EFalse;
	} // CWapSmsProtocol::FindAndDeleteMsg


//
//	Search for SAR for this SAP entries. If found any, adds it to message queue
//	Note:	This retrieves stored but not acked messages for this SAP
//			Is called when client's socket binds to address
//
TInt CWapSmsProtocol::CheckSarL(const TWapAddr& aAddr,CWapSmsProvider* aSap)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOL_CHECKSARL_1, "CWapSmsProtocol::CheckSarL()");

	TInt count=0;
	TInt err = KErrNone;
	count = iWapStore->Entries().Count();
	TWapReassemblyEntry entry;


	TWapPortNumber portNumber = aAddr.WapPort() ;
	for(TInt index=0;index< count; index++)
		{
		entry = (TWapReassemblyEntry&)iWapStore->Entries()[index];
		if(entry.ToPort()==portNumber)
			{
			CWapDatagram* wapDatagram = NULL;
			wapDatagram = CWapDatagram::NewL(KNullDesC8);
			CleanupStack::PushL(wapDatagram);
			TRAP(err,iWapStore->GetDatagramL( index,*wapDatagram));
			__ASSERT_DEBUG(!err,Panic(EWapSmsBadGetDataCall));
			CleanupStack::Pop(wapDatagram);
			aSap->AddToQueue(wapDatagram);
			}
		}
	return err;
	} // CWapSmsProtocol::CheckSarL
