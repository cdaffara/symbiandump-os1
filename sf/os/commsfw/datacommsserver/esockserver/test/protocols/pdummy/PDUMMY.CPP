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
//

#include <es_prot.h>
#include <e32test.h>
#include "ES_DUMMY.H"
#include <comms-infras/nifif.h>
#include <es_mbuf.h>
#include <ss_std.h>
#include <ss_pman.h>
#include <ss_glob.h>
#include <e32math.h>
#include <dns_qry.h>
#include <agenterrors.h>
#include <e32property.h>
#include <nifman_internal.h>
#include "pdummy.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestPDUMMY, "ESockTestPDUMMY.");
#endif

const TInt KPDummyDefaultGranularity=0xA; // Default number of incoming messages the protocol can queue up

//---------------------------------------------------------------------------------------------------------


CAsyncSocketErrorer* CAsyncSocketErrorer::NewL(MSocketNotify& aSocket,TInt aErrorCode,TUint aOpMask,
                                  CDummyProtocol * apProtocols, CDummyProvd* aOwner,const TDesC8* aListenData )
	{
	return new(ELeave) CAsyncSocketErrorer(aSocket, aErrorCode, aOpMask,apProtocols,aOwner,aListenData);
	}

CAsyncSocketErrorer::CAsyncSocketErrorer(MSocketNotify& aSocket, TInt aErrorCode, TUint aOpMask,
		CDummyProtocol* apProtocols, CDummyProvd* aOwner, const TDesC8* aListenData):
    CActive(KMaxTInt),
	iSocket(aSocket),
	iErrorCode(aErrorCode),
	iOpMask(aOpMask),
	ipProtocols(apProtocols),
	iOwner(aOwner),
	iListenData(aListenData)
	{
	CActiveScheduler::Add(this);
	SetActive();
	TRequestStatus* p = &iStatus;
	User::RequestComplete(p, KErrNone);
	}

void CAsyncSocketErrorer::RunL()
	{
	CDummyProvd* p = NULL;
    if ( iErrorCode == KErrNone )
        {
	    TRAP(iErrorCode,p = CDummyProvd::NewL(*ipProtocols););
	    if (iErrorCode)
	    	{
	    	iOpMask = MSocketNotify::EErrorFatal;
	    	}
        }
    if ( iErrorCode == KErrNone )
        {
	    if(iListenData)
	    	{
	    	iSocket.ConnectComplete(*p, *iListenData);
	    	}
	    else
	    	{
	    	iSocket.ConnectComplete(*p);
	    	}
        }
    else
        {
	    iSocket.Error(iErrorCode,iOpMask);
        }
	iOwner->DestroyAsyncErrorer();	// ie "delete this"
	}

CAsyncSocketErrorer::~CAsyncSocketErrorer()
	{
	Cancel();
	}

void CAsyncSocketErrorer::DoCancel()
	{
	}

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDummyProvd::CFlowOnTimer): public CTimer
	{
public:
	static CFlowOnTimer* NewL(CDummyProvd& aProvd, MSocketNotify& aSocketNotify);
	virtual ~CFlowOnTimer();

	virtual void RunL();

private:
	CFlowOnTimer(CDummyProvd& aProvd, MSocketNotify& aSocket);

	CDummyProvd& iProvd;
	MSocketNotify& iSocketNotify;
	};


CDummyProvd::CFlowOnTimer* CDummyProvd::CFlowOnTimer::NewL(CDummyProvd& aProvd, MSocketNotify& aSocketNotify)
	{
	CFlowOnTimer* self = new(ELeave) CFlowOnTimer(aProvd, aSocketNotify);
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop(self);
	return self;
	}

CDummyProvd::CFlowOnTimer::CFlowOnTimer(CDummyProvd& aProvd, MSocketNotify& aSocketNotify)
	: CTimer(CActive::EPriorityStandard), iProvd(aProvd), iSocketNotify(aSocketNotify)
	{
	}


CDummyProvd::CFlowOnTimer::~CFlowOnTimer()
	{
	Cancel();
	}

void CDummyProvd::CFlowOnTimer::RunL()
	{
	iProvd.iFlowOffWriteTimeout = 0;
	iSocketNotify.CanSend();
	}

enum TDPanic
	{
	EBadProtocol,
	EBadCall,
	ENotBound,
	ENotStarted,
	EInterfaceNotDeleted,
	EIdAndNoHolder,
	EIfAndHolder,
	ETestPanic
	};

// RTest test(_L("Dummy protocol"));

LOCAL_C void Panic(TDPanic aPanic)
//
// Panic the Protocol
//
	{

	User::Panic(_L("Dummy Prot"),aPanic);
	}

extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallDummy(void);	// Force export

	EXPORT_C CProtocolFamilyBase * InstallDummy(void)
	//
	// Create a new protocol family
	//
		{
		return CDProtocolFamily::NewL();
		}
	}

CDProtocolFamily* CDProtocolFamily::NewL()
//
//
//
	{
	CDProtocolFamily* pf = new (ELeave) CDProtocolFamily();
	CleanupStack::PushL(pf);
	pf->ConstructL();
	CleanupStack::Pop();
	return pf;
	}

void CDProtocolFamily::ConstructL()
//
//
//
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;

	}

CDProtocolFamily::CDProtocolFamily() : CProtocolFamilyBase()
	{
	iProtocolDescs[0].iAddrFamily=KDummyAddrFamily;
	iProtocolDescs[0].iSockType=KSockDatagram;
	iProtocolDescs[0].iProtocol=KDummyOne;
	iProtocolDescs[0].iVersion=TVersion(KDummyMajor,KDummyMinor,KDummyBuild);
	iProtocolDescs[0].iByteOrder=EBigEndian;
	// KSIRequiresOwnerInfo is required to ensure that SetOption(KSoSetPlatSecApi) is issued to
	// TransportFlowShim for NoBearer() testing.
	iProtocolDescs[0].iServiceInfo=kDDatagramServiceInfo|KSIPeekData|KSIRequiresOwnerInfo;
	iProtocolDescs[0].iSecurity=KSocketNoSecurity;
	iProtocolDescs[0].iMessageSize=0x300;
	iProtocolDescs[0].iName=KDummyOneName;
	iProtocolDescs[0].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[0].iNamingServices=KNSNameResolution|KNSServiceResolution|KNSInfoDatabase;
	iProtocolDescs[0].iNumSockets=100;

	iProtocolDescs[1].iAddrFamily=KDummyAddrFamily;
	iProtocolDescs[1].iSockType=KSockStream;
	iProtocolDescs[1].iProtocol=KDummyTwo;
	iProtocolDescs[1].iVersion=TVersion(KDummyMajor,KDummyMinor,KDummyBuild);
	iProtocolDescs[1].iByteOrder=EBigEndian;
	iProtocolDescs[1].iServiceInfo=KDStreamServiceInfo;
	iProtocolDescs[1].iSecurity=KSocketNoSecurity;
	iProtocolDescs[1].iMessageSize=KSocketMessageSizeIsStream;
	iProtocolDescs[1].iName=KDummyTwoName;
	iProtocolDescs[1].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[1].iNamingServices=0;
	iProtocolDescs[1].iNumSockets=100;

	iProtocolDescs[2].iAddrFamily=KDummyAddrFamily;
	iProtocolDescs[2].iSockType=KSockSeqPacket;
	iProtocolDescs[2].iProtocol=KDummyThree;
	iProtocolDescs[2].iVersion=TVersion(KDummyMajor,KDummyMinor,KDummyBuild);
	iProtocolDescs[2].iByteOrder=EBigEndian;
	iProtocolDescs[2].iServiceInfo=KSIReliable|KSIInOrder|KSIDatagram|KSIGracefulClose|KSIBroadcast|KSIQOS|KSICanReconnect|KSIConnectData|KSIDisconnectData;
	iProtocolDescs[2].iSecurity=KSocketNoSecurity;
	iProtocolDescs[2].iMessageSize=KSocketMessageSizeNoLimit;
	iProtocolDescs[2].iName=KDummyThreeName;
	iProtocolDescs[2].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains|ETransport|EPreferDescriptors|EUseCanSend;
	iProtocolDescs[2].iNamingServices=0;
	iProtocolDescs[2].iNumSockets=100;

	iProtocolDescs[3].iAddrFamily=KDummyAddrFamily;
	iProtocolDescs[3].iSockType=KSockStream;
	iProtocolDescs[3].iProtocol=KDummyFour;
	iProtocolDescs[3].iVersion=TVersion(KDummyMajor,KDummyMinor,KDummyBuild);
	iProtocolDescs[3].iByteOrder=EBigEndian;
	iProtocolDescs[3].iServiceInfo=KDStreamServiceInfo;
	iProtocolDescs[3].iSecurity=KSocketNoSecurity;
	iProtocolDescs[3].iMessageSize=KSocketMessageSizeIsStream;
	iProtocolDescs[3].iName=KDummyFourName;
	iProtocolDescs[3].iServiceTypeInfo=ESocketSupport|ENeedMBufs;
	iProtocolDescs[3].iNamingServices=0;
	iProtocolDescs[3].iNumSockets=100;

	iProtocolDescs[4].iAddrFamily=KDummyAddrFamily;
	iProtocolDescs[4].iSockType=KSockDatagram;
	iProtocolDescs[4].iProtocol=KDummyFive;
	iProtocolDescs[4].iVersion=TVersion(KDummyMajor,KDummyMinor,KDummyBuild);
	iProtocolDescs[4].iByteOrder=EBigEndian;
	iProtocolDescs[4].iServiceInfo=kDDatagramServiceInfo|KSIPeekData;
	iProtocolDescs[4].iSecurity=KSocketNoSecurity;
	iProtocolDescs[4].iMessageSize=KSocketMessageSizeNoLimit;
	iProtocolDescs[4].iName=KDummyFiveName;
	iProtocolDescs[4].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[4].iNamingServices=KNSNameResolution|KNSServiceResolution|KNSInfoDatabase;
	iProtocolDescs[4].iNumSockets=100;

	__DECLARE_NAME(_S("CDProtocolFamily"));
	}

#pragma warning( disable : 4100 )

TInt CDProtocolFamily::Install()
	{
	// Force a fail on Memory tests
	char* ptr=new char;
	if (!ptr)
		return KErrNoMemory;
	delete ptr;
	return KErrNone;
	}

TInt CDProtocolFamily::Remove()
	{
	return KErrNone;
	}

TUint CDProtocolFamily::ProtocolList(TServerProtocolDesc *& aProtocolDescPointer)
	{

  	aProtocolDescPointer=new TServerProtocolDesc[KPDummyNumProtocols];
  	if (!aProtocolDescPointer)
  		return 0;

	Mem::Copy(aProtocolDescPointer, iProtocolDescs, sizeof(TServerProtocolDesc)*KPDummyNumProtocols);

	TRAP_IGNORE(Nif::CheckInstalledMBufManagerL();)

	return KPDummyNumProtocols;
	};

CProtocolBase * CDProtocolFamily::NewProtocolL(TUint /* aSockType */,TUint aProtocol)
	{

	CDummyProtocol* p=CDummyProtocol::NewL(aProtocol, &iProtocolDescs[aProtocol-1]);
	return p;
	}


//---------------------------------------------------------------------------------------------------------


CServProviderBase * CDummyProtocol::NewSAPL(TUint /*aProtocol*/)
	{
	return CDummyProvd::NewL(*this);
	}


CHostResolvProvdBase* CDummyProtocol::NewHostResolverL()
	{
	return CDatagramHostResolver::NewL();
	}

CServiceResolvProvdBase* CDummyProtocol::NewServiceResolverL()
	{
	return CDatagramServResolver::NewL();
	}

CNetDBProvdBase* CDummyProtocol::NewNetDatabaseL()
	{
	return CDatagramNetDataBase::NewL();
	}

CDummyProtocol::~CDummyProtocol()
	{
	for (TInt i=0;i<iProtocols.Count();i++)
		{
		iProtocols[i]->Close();
		}
	}


void CDummyProtocol::InitL(TDesC& /*aTag*/)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	}

void CDummyProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*anId*/)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	}

void CDummyProtocol::BindToL(CProtocolBase* aProtocol)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	iProtocols.AppendL(aProtocol);
	aProtocol->Open();
	}


void CDummyProtocol::StartL(void)
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;
	iIsStarted=ETrue;
	}

TInt CDummyProtocol::Send(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	return 1;
	}

TInt CDummyProtocol::Send(TDes8 &, TSockAddr* /*to*/,TSockAddr* /*from*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return 1;
	}

void CDummyProtocol::Process(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	}

void CDummyProtocol::Process(TDes8 & ,TSockAddr* /*from*/,TSockAddr* /*to*/,CProtocolBase* /*aSourceProtocol*/)
	{
	}

void CDummyProtocol::Identify(TServerProtocolDesc *aDesc) const
	{
	Mem::Copy(aDesc, iProtoDesc, sizeof(TServerProtocolDesc));
	}

TInt CDummyProtocol::GetOption(TUint level,TUint,TDes8&,CProtocolBase* /*aSourceProtocol*/)
	{

	if(level==KNifOptLevel)
		{
		return KErrNotSupported;
		}
	return KErrNone;
	}

TInt CDummyProtocol::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return KErrNone;
	}

void CDummyProtocol::Error(TInt /*anerror*/,CProtocolBase* /*aSourceProtocol*/)
	{
	}


EXPORT_C CDummyProtocol *CDummyProtocol::NewL(TInt aType, TServerProtocolDesc* aProtoDesc)
	{

	CDummyProtocol* p=0;
	switch (aType)
		{
	case KDummyOne:
	case KDummyTwo:
	case KDummyFive:
		p=new (ELeave) CDummyProtocol(aProtoDesc);
		break;

	case KDummyThree:
	case KDummyFour:
		p=new (ELeave) CInterfaceProtocol(aProtoDesc);
		break;

	default:
		p=(CDummyProtocol*)0xABCD; // keep lint happy
		Panic(EBadProtocol);
		}
	p->iType=aType;
	return p;
	}


EXPORT_C CDummyProtocol::CDummyProtocol(TServerProtocolDesc* aProtoDesc)

	:CProtocolBase(),iProtoDesc(aProtoDesc),iProtocols(0x16)
	{
	__DECLARE_NAME(_S("CDummyProtocol"));
	iIsStarted=EFalse;
	}


/* exported functions that are used to exercise ProtocolBase functionality
*/
EXPORT_C TInt CDummyProtocol::ProtocolBaseBind(CProtocolBase* aProt, TUint aId)
	// expected to Panic(EDoesNotBindBelow);
	{
	TRAPD(ret, CProtocolBase::BindL(aProt, aId));
	return ret;
	}


EXPORT_C TInt CDummyProtocol::ProtocolBaseBindTo(CProtocolBase* aProt)
	// expected to Panic(EDoesNotBindAbove);
	{
	TRAPD(ret, CProtocolBase::BindToL(aProt));
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseError(CProtocolBase* aProt)
	// expected to Panic(EErrorCallNotHandled)
	{
	CProtocolBase::Error(KErrNone, aProt);
	return KErrNone;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseGetOption(TUint aLevel, TUint aName, TDes8& aBuf, CProtocolBase* aProt)
	{
	// expected to return KErrNotSupported
	TInt ret = CProtocolBase::GetOption(aLevel,aName,aBuf,aProt);
	__ASSERT_DEBUG(ret == KErrNotSupported, User::Panic(KSpecAssert_ESockTestPDUMMY, 1));
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseNewHostResolver()
	// expected to panic with Fault(EBadHostResolver);
	{
	TRAPD(ret, CProtocolBase::NewHostResolverL());
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseNewNetDatabase()
	// expected to panic with Fault(EBadNetDBRequest)
	{
	TRAPD(ret, CProtocolBase::NewNetDatabaseL());
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseNewSAP(TUint aProt)
	// expected to panic with Fault(EOddSock)
	{
	TRAPD(ret, CProtocolBase::NewSAPL(aProt));
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseNewServiceResolver()
	// expected to panic with Fault(EBadServiceResolver)
	{
	TRAPD(ret, CProtocolBase::NewServiceResolverL());
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseProcess(RMBufChain &aChain, CProtocolBase* aProt)
	// expected to panic with Panic(ECantProcessMbufs)
	{
	CProtocolBase::Process(aChain, aProt);
	return KErrNone;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseSend(RMBufChain &aChain, CProtocolBase* aProt)
	// expected to panic with Panic(ECantSendMBufs);
	{
	TInt ret = CProtocolBase::Send(aChain, aProt);
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseSetOption(TUint aLevel, TUint aName, TDes8& aBuf, CProtocolBase* aProt)
	// expected to return KErrNotSupported
	{
	TInt ret = CProtocolBase::SetOption(aLevel, aName, aBuf, aProt);
	__ASSERT_DEBUG(ret == KErrNotSupported, User::Panic(KSpecAssert_ESockTestPDUMMY, 2));
	return ret;
	}

EXPORT_C TInt CDummyProtocol::ProtocolBaseSendWithAddress(TDes8& aBuf,TSockAddr* aTo ,TSockAddr* aFrom, CProtocolBase* aProt)
	// expected to panic with Panic(ECantSendMBufs);
	{
	TInt ret = CProtocolBase::Send(aBuf, aTo, aFrom, aProt);
	return ret;
	}


EXPORT_C TInt CDummyProtocol::ProtocolBaseProcessWithAddress(TDes8& aBuf, TSockAddr* aFrom, TSockAddr* aTo, CProtocolBase* aProt)
	// expected to panic with Panic(ECantProcessMbufs)
	{
	CProtocolBase::Process(aBuf, aFrom, aTo, aProt);
	return KErrNone;
	}

//---------------------------------------------------------------------------------------------------------

CDummyProvd *CDummyProvd::NewL(CDummyProtocol &aProtocol)
	{
	CDummyProvd *sp=new(ELeave) CDummyProvd(aProtocol);
	CleanupStack::PushL(sp);
	sp->ConstructL();
	CleanupStack::Pop(sp);
	return sp;

	}

void CDummyProvd::ConstructL()
	{
	iDataArray = new (ELeave) CArrayFixFlat<RMBufChain>(KPDummyDefaultGranularity);
	iChain.AllocL(128);
	}

TInt CDummyProvd::NoBearerCB(TAny* aArg)
/**
Issue a NoBearer() upcall to ESock.
 */
    {
    CDummyProvd* This = static_cast<CDummyProvd*>(aArg);
    // Scoped NoBearer() prevents a connection from being started by ESock - a side-effect we aren't interested in.
    _LIT8(KScoped, "scoped");
    This->iSocket->NoBearer(KScoped()); 
    return KErrNone;
    }

CDummyProvd::CDummyProvd(CDummyProtocol &aProtocol) :
	  iCancelIoctl(EFalse),
	  iFlowOffWriteTimeout(0),
	  iErrNoMBufs(EFalse),
	  iNoBearerCB(TCallBack(NoBearerCB, this), EPriorityNormal)
	{
	__DECLARE_NAME(_S("CDummyProvd"));
	iIsBound=EFalse;
	iProtocol=&aProtocol;
	iCompleteIoctl = FALSE;
	iConnectCompleteState = ETrue;
	iConnectPending = EFalse;
	}

CDummyProvd::~CDummyProvd()
//

	{
	if (iDataArray)
		{
		TInt c = iDataArray->Count();
		TInt i;
		for (i=0; i < c; ++i)
			{
			iDataArray->At(i).Free();
			}
		delete iDataArray;
		}

	iListenData.Close();
	iChain.Free();
	delete iFlowOnTimer;
	}

void CDummyProvd::DestroyAsyncErrorer()
	{
	delete iAsyncErrorer;
	iAsyncErrorer = NULL;
	}


void CDummyProvd::LocalName(TSockAddr& anAddr) const
	{
	anAddr=iAddr;
	}

TInt CDummyProvd::SetLocalName(TSockAddr& anAddr)
	{
	iAddr=anAddr;
	iIsBound=ETrue;
	return KErrNone;
	}

void CDummyProvd::RemName(TSockAddr& /*anAddr*/)const
	{
	}

TInt CDummyProvd::SetRemName(TSockAddr& /*anAddr*/)
	{
	return KErrNone;
	}

TInt CDummyProvd::GetOption(TUint aLevel, TUint aName, TDes8& anOption) const
	{
	if(aLevel==KNifOptLevel)
		{
		return iProtocol->GetOption(aLevel, aName, anOption, 0);
		}
	else
		{
		switch(aName)
			{
			case KDummyOptionSetBlockConnect:
				{
				TBool blockedConnect = !iConnectCompleteState;
				const TUint8* opt = reinterpret_cast<const TUint8*>(&blockedConnect);
				anOption.Copy(opt,sizeof(blockedConnect));
				return KErrNone;
				}
			case KDummyOptionSetErrorNextWrite:
				{
				anOption.Copy(reinterpret_cast<const TUint8*>(&iErrorForNextWrite), sizeof(iErrorForNextWrite));
				return KErrNone;
				}
			case KDummyOptionSetErrorNextShutdown:
				{
				anOption.Copy(reinterpret_cast<const TUint8*>(&iErrorForNextShutdown), sizeof(iErrorForNextShutdown));
				return KErrNone;
				}
			case KDummyOptionSetErrorNextConnect:
				{
				anOption.Copy(reinterpret_cast<const TUint8*>(&iErrorForNextConnect), sizeof(iErrorForNextConnect));
				return KErrNone;
				}
			case KDummyOptionGetMBufFreeSpace:
				{
				RMBufAllocator mBufAllocator;
				TInt availableBytes = mBufAllocator.BytesAvailable();
				anOption.Copy(reinterpret_cast<const TUint8*>(&availableBytes), sizeof(availableBytes));
				return KErrNone;
				}
			default:
				return KErrNotSupported;
			}
		}
	}

TInt CDummyProvd::SetOption(TUint level,TUint name,const TDesC8& anOption)
	{
	switch(name)
		{
		case KDummyOptionSetErrorNextListen:
			iListenErrorCode=*(TInt*)&anOption[0];
			break;
		case KDummyOptionSetConnectComplete:
			__ASSERT_DEBUG(!iAsyncErrorer, User::Panic(KSpecAssert_ESockTestPDUMMY, 3));	// to support multiple outstanding would need a list
				{
				TInt ret = KErrNone;
				if (iListenData.Length())
					{
					TRAP(ret,iAsyncErrorer = CAsyncSocketErrorer::NewL(*iSocket, KErrNone,0, iProtocol, this, &iListenData));
					}
			    else
			    	{
			    	TRAP(ret,iAsyncErrorer = CAsyncSocketErrorer::NewL(*iSocket,KErrNone,0,iProtocol,this,NULL));
			    	}
	            return ret;
				}
		case KDummyOptionSetBlockConnect:
			return SetBlockedConnect(anOption);
		case KDummyOptionSetBlockConnectData:
			{
			TBool opt = *reinterpret_cast<const TBool*>(anOption.Ptr());
			if (!opt)	
				{
				iListenData.Create(KConnectCompleteData());
				}
			else
				{
				iConnectPending = ETrue;	
				}
			return SetBlockedConnect(anOption);
			}			
		case KDummyOptionSetIocltComplete:
			if (iCompleteIoctl)
				{
				iSocket->IoctlComplete(NULL);
				iCompleteIoctl = EFalse;
				}
			break;
		case KDummyOptionSetErrorNextWrite:
			return OptToErr(anOption,iErrorForNextWrite);
		case KDummyOptionSetErrorNextShutdown:
			return OptToErr(anOption,iErrorForNextShutdown);
		case KDummyOptionSetErrorNextConnect:
			return OptToErr(anOption,iErrorForNextConnect);

	case KDummyOptionSetFlowOffWrite:
		{
		if (anOption.Length() != sizeof(TBool))
			return KErrArgument;

		iFlowOffWrite = *reinterpret_cast<const TBool*>(anOption.Ptr());
		if (!iFlowOffWrite)
			iSocket->CanSend();
		}
		break;


	case KDummyOptionSetTimedFlowOffWrite:
		{
		if (anOption.Length() != sizeof(TInt))
			return KErrArgument;

		iFlowOffWriteTimeout = *reinterpret_cast<const TInt*>(anOption.Ptr());
		if (iFlowOffWriteTimeout > 0)
			{
			if (!iFlowOnTimer)
				{
				TRAPD(ret, iFlowOnTimer = CFlowOnTimer::NewL(*this, *iSocket));
				if (ret)
					return ret;
				}
			}
		else if (iFlowOffWriteTimeout == 0)
			{
			delete iFlowOnTimer;
			iFlowOnTimer = NULL;
			}
		else
			return KErrArgument;
		}
		break;

	case KDummyOptionSetGobbleMBufs:
		{
		iErrNoMBufs = ETrue;
		return GobbleMBufs();
		}

	case KDummyOptionSetFreeMBufs:
		{
		iErrNoMBufs = EFalse;
		FreeMBufs();
		break;
		}

	case KDummyOptionSetFreeSomeMBufs:
		{
		iErrNoMBufs = EFalse;
		FreeMBufs(level);
		break;
		}


	case KDummyOptionLeakMemory:
		{
		for(TUint i = 0; i < level; ++i)
			{
			(void) new TInt;	// deliberately leak the allocs
			}
		return KErrNone;
		}
		
	case KDummyOptionIssueNoBearer:
	    {
	    // Issue a NoBearer() upcall.  We are testing the behaviour whereby NoBearer() is called
	    // outside the context of any pending IPC.  We use an async callback to ensure that the current
	    // SetOption() IPC is completed by ESock before the NoBearer() upcall.
	    iNoBearerCB.Call();
	    return KErrNone;
	    }

	default:
		return KErrNotSupported;
		}
	return KErrNone;
	}

TInt CDummyProvd::GobbleMBufs()
// Eat all the available mBufs in the mBuf pool
	{
	RMBufAllocator allocator;
	RMBufChain aChain;
	TInt size = allocator.NextMBufSize(0);
	while (size != KErrNotFound)
		{
		TInt ret = KErrNone;
		while (ret == KErrNone)
			{
			TRAP(ret, aChain.AllocL(size));
			iChain.Append(aChain);
			}
		size = allocator.NextMBufSize(size);
		}
		
	TInt length = iChain.Length();
	RDebug::Print(_L("Out of MBuf Memory... Total MBuf memory in use %d"), length);
	TInt numBufs = iChain.NumBufs();
	RDebug::Print(_L("Out of MBuf Memory... Total MBufs in use %d"), numBufs);
	return numBufs;
	}

void CDummyProvd::FreeMBufs()
// Free All the MBufs that were allocated by GobbleMBufs
	{
	if(iChain.IsEmpty())
		{
		return;
		}
	iChain.Free();
	TInt length = iChain.Length();
	RDebug::Print(_L("MBufMemory De-Allocated... Total MBuf memory in use %d"), length);
	}

void CDummyProvd::FreeMBufs(TUint aNumber)
// Free aNumber of mBufs that were allocated by GobbleMBufs
	{
	if(iChain.IsEmpty())
		{
		return;
		}
	TInt length = 0;
	while (aNumber-- > 0)
		{
		TInt length = iChain.Length();
		if (length == 0)
			{
			break;
			}
		TInt trimOffset = length - iChain.Last()->Size();
		iChain.TrimEnd(trimOffset);
		}
	length = iChain.Length();
	RDebug::Print(_L("MBufMemory De-Allocated... Total MBuf memory in use %d"), length);
	}


TInt CDummyProvd::OptToErr(const TDesC8 &aOption, TInt &aRes)
	{
	if (aOption.Length() != sizeof(TInt))
		{
		return KErrCorrupt;
		}
	const TInt &opt = *reinterpret_cast<const TInt*>(aOption.Ptr());
	if (opt > KErrNone)
		{
		return KErrCorrupt;
		}
	aRes = opt;
	return KErrNone;
	}

TInt CDummyProvd::SetBlockedConnect(const TDesC8 &anOption)
	{
	if (anOption.Length() != sizeof(TBool))
		{
		return KErrCorrupt;
		}
	TBool opt = *reinterpret_cast<const TBool*>(anOption.Ptr());
	if (opt != TRUE && opt != FALSE)
		{
		return KErrCorrupt;
		}
	iConnectCompleteState = !opt;
	if (iConnectCompleteState && iConnectPending)
		{
			if (iErrorForNextConnect != KErrNone)
			{
			iSocket->Error(iErrorForNextConnect, MSocketNotify::EErrorConnect);
			iErrorForNextConnect = KErrNone;
			return KErrNone;
			}
		if (iListenData.Length())
			{
			iSocket->ConnectComplete(iListenData);
			}
		else
			{
			iSocket->ConnectComplete();
			}
		iConnectPending = EFalse;
		}
	return KErrNone;
	}

void CDummyProvd::Ioctl(TUint /*level*/,TUint name,TDes8* anOption)
	{
	switch (name)
		{
		case KDummyIoctlCheckBound:
			if (!iIsBound)
				Panic(ENotBound);
			iSocket->IoctlComplete(NULL);
			break;
		case KDummyIoctlCheckStarted:
			if (!iProtocol->IsStarted())
				Panic(ENotStarted);
			iSocket->IoctlComplete(NULL);
			break;
		case KDummyIoctlIgnore:
			iCancelIoctl=EFalse;
			break;
		case KDummyIoctlCancelled:
			if(iCancelIoctl)
				{
				iSocket->IoctlComplete(NULL);
				}
			else
				{
				iSocket->Error(KErrNotFound,MSocketNotify::EErrorIoctl);
				}
			break;
		case KDummyIoctlPanicProtocolModule:
			Panic(ETestPanic);
			iSocket->IoctlComplete(NULL); //Should never get here
			break;
		case KDummyIocltNonCompleting: //Ioclt never send back a complete message
			iCompleteIoctl = TRUE;
			break;
		case KDummyIoctBlockHardOnClose:
			iBlockOnClose = TRUE;
			iSocket->IoctlComplete(NULL);
			break;

		case KDummyIoctlSlowIoctlReturn:
			iSocket->IoctlComplete(NULL);
			User::After(100000);
			break;
		case KDummyIoctlHangIoctlReturn:
			iSocket->IoctlComplete(NULL);
			HangModule();
			break;
		case KDummyIocltJustComplete:
			iSocket->IoctlComplete(NULL);
			break;
		case KDummyIocltRemoteDisconnect:
			if (anOption)
				{
				TBuf8<40> tmp(*anOption);
				iSocket->Disconnect(tmp);
				}
			else
				{
				iSocket->Disconnect();
				}
			iSocket->IoctlComplete(NULL);
			break;
		
		case KDummyIoctlCompleteWithData:
			{
			TBuf8<64> ioctlBuffer(KIoctlData());
			iSocket->IoctlComplete(	&ioctlBuffer );	
			}
			
			break;
		default:
			iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
		}
	}

void CDummyProvd::CancelIoctl(TUint /*aLevel*/,TUint /*aName*/)
	{
	iCancelIoctl=ETrue;
	}

TInt CDummyProvd::Write(RMBufChain& aData,TUint aOptions, TSockAddr*)
	{
	if (iFlowOffWriteTimeout > 0)
		{
		__ASSERT_DEBUG(iFlowOffWriteTimeout, User::Panic(KSpecAssert_ESockTestPDUMMY, 4));
		iFlowOnTimer->After(iFlowOffWriteTimeout);
		return 0;
		}

	if (iFlowOffWrite)
		return 0;

	if (aOptions==KSockWriteUrgent)
		{
		iSocket->Error(KErrNotSupported);
		return 0;
		}
	if (iErrorForNextWrite != KErrNone)
		{
		iSocket->Error(iErrorForNextWrite, MSocketNotify::EErrorSend);
		iErrorForNextWrite = KErrNone;
		return 0;
		}

	// add a new chain for storing data
	if(iDataArray->Count()==0 ||( iDataArray->Count()>0 && !iDataArray->At(iDataArray->Count()-1).IsEmpty()))
		{
		RMBufChain chain;
		TRAPD(ret, iDataArray->AppendL(chain));
		if(ret!=KErrNone)
			return ret;
		}
	
	iDataArray->At(iDataArray->Count()-1).Append(aData);

	// Gotta save the length of the chain before calling
	// NewData, as it could be changed after the call
	// (We need to be re-entrant).
	TInt consumed;
	if(IsTransportType(KSockStream))
		{
		consumed = iDataArray->At(iDataArray->Count()-1).Length();
		}
	else
		{
		consumed = 1;
		}

	if(!iInputStopped)
		{
		if(IsTransportType(KSockDatagram))
			iSocket->NewData(1);
		else
			iSocket->NewData(consumed);
		}

	return consumed;
	}

TUint CDummyProvd::Write(const TDesC8& aDesc, TUint options, TSockAddr* anAddr)
	{
	if (iFlowOffWriteTimeout > 0)
		{
		__ASSERT_DEBUG(iFlowOffWriteTimeout, User::Panic(KSpecAssert_ESockTestPDUMMY, 5));
		iFlowOnTimer->After(iFlowOffWriteTimeout);
		return 0;
		}

	if (iFlowOffWrite)
		return 0;

	RMBufChain chain;
	TRAPD(r,chain.CreateL(aDesc));
	if (r != KErrNone)
		{
		return r;
		}

	TInt ret = Write(chain, options, anAddr);
	if (ret<0)
		{
		return 0;
		}
	return ret;
	}

TBool CDummyProvd::IsTransportType(TUint aType)
	{
	TServerProtocolDesc info;
	iProtocol->Identify(&info);
	return (info.iSockType==aType);
	}

void CDummyProvd::GetData(TDes8& aDesc,TUint aOptions,TSockAddr* aAddr)
	{
	RMBufChain chain;
	GetData(chain,aDesc.MaxLength(),aOptions, aAddr);
	aDesc.SetMax();
	chain.CopyOut(aDesc);
	chain.Free();
	}

TInt CDummyProvd::GetData(RMBufChain& aData,TUint aLength, TUint aOptions,TSockAddr* aAddr)
	{
	__ASSERT_DEBUG(iDataArray->Count()>0, User::Panic(KSpecAssert_ESockTestPDUMMY, 6));
	TInt res = KErrNone;

	// If KSockReadPeek we copy the whole bally lot instead of moving it out.
	if((aOptions&KSockReadPeek)==KSockReadPeek)
		{
		TInt err;
		// For datagram, copy all datagram else copy just aLength
		if(!IsTransportType(KSockDatagram))
			{
			TRAP(err, iDataArray->At(0).CopyL(aData, 0, aLength));
//			iDataArray->At(0).TrimStart(aLength);
			}
		else
			{
			TRAP(err, iDataArray->At(0).CopyL(aData));
//			iDataArray->At(0).TrimStart(aData.Length());
			}
		if(res==KErrNone)
			{
			iSocket->NewData(1);
			}
		else
			{
			iDataArray->At(0).Free();
			}
		}
	else // Not KSockReadPeek
		{
		// Move datablock to receiving chain
		aData.Assign(iDataArray->At(0));

		// For non-datagram types exact only amount of bytes
		// if they have more than asked for
		// so we move some of the data back to our array
		if( !IsTransportType(KSockDatagram) &&
			(((TUint)aData.Length())>aLength) )
			{
			// Put the tail back into our array for another good time
			TRAP(res, aData.SplitL(aLength, iDataArray->At(0)));
			// If it didnt work, save data internally again and return error
			if(res!=KErrNone)
				{
				iDataArray->At(0).Assign(aData);
				}
			}
		}

	// No errors? Return number of bytes copied (stream) or 1 (dgram)
	if(res==KErrNone)
		{
		if(IsTransportType(KSockStream))
			{
			res = aData.Length();
			}
		else
			{
			res = 1;
			}
		}
	else // else report mbuf shortage
		{
		res=KErrNoMBufs;
		}

	// Cleanup possibly unused chain
	if(iDataArray->At(0).Length()==0)
		iDataArray->Delete(0);
	
	// Copy the TSockAddr if requested
	if ( aAddr )
		{
		const TUint KProxyAddrFamily=0x20000;		
		TSockAddr addr(KProxyAddrFamily);
		aAddr->Copy(addr);
		}
	return res;
	}


void CDummyProvd::ActiveOpen(void)
	{
	if (iErrorForNextConnect != KErrNone)
		{
		iSocket->Error(iErrorForNextConnect, MSocketNotify::EErrorConnect);
		iErrorForNextConnect = KErrNone;
		return;
		}
	if (iConnectCompleteState)
		{
		iSocket->ConnectComplete();
		}
	else
		{
		if(iListenData.Length())
			{
			iListenData.Close();
			}
		iConnectPending = ETrue;
		}
	}

void CDummyProvd::ActiveOpen(const TDesC8& aConnectionData)
	{
		if (iErrorForNextConnect != KErrNone)
		{
		iSocket->Error(iErrorForNextConnect, MSocketNotify::EErrorConnect);
		iErrorForNextConnect = KErrNone;
		return;
		}
	if (iConnectCompleteState)
		{
		iSocket->ConnectComplete(aConnectionData);
		}
	else
		{
		if(iListenData.Length())
			{
			iListenData.Close();
			}
		iListenData.Create(aConnectionData);
		iConnectPending = ETrue;
		}
	}

TInt CDummyProvd::PassiveOpen(TUint /*aQue*/)
	{
//	test.Printf(_L("CDummyProvd::Open\n"));
	if(iListenErrorCode)
		{// We're going to error this call, but can't do it synchonously.
		// Do it with an AO of max prio, so we know it will happen next
//		iSocket->Error(iListenErrorCode);// Error all operations
		__ASSERT_DEBUG(!iAsyncErrorer, User::Panic(KSpecAssert_ESockTestPDUMMY, 7));	// to support multiple outstanding would need a list
		TRAPD(err, iAsyncErrorer = CAsyncSocketErrorer::NewL(*iSocket,iListenErrorCode,MSocketNotify::EErrorConnect,0,this,NULL));
		iListenErrorCode=KErrNone;
		return err;
		}
	return KErrNone;
	}

TInt CDummyProvd::PassiveOpen(TUint /*aQue*/, const TDesC8& aConnectionData)
	{
	iListenData.Close();
	TInt ret = iListenData.Create(aConnectionData);
	if (ret != KErrNone)
		{
		return ret;
		}
		if(iListenErrorCode)
		{// We're going to error this call, but can't do it synchonously.
		// Do it with an AO of max prio, so we know it will happen next
//		iSocket->Error(iListenErrorCode);// Error all operations
		__ASSERT_DEBUG(!iAsyncErrorer, User::Panic(KSpecAssert_ESockTestPDUMMY, 8));	// to support multiple outstanding would need a list
		TRAPD(err, iAsyncErrorer = CAsyncSocketErrorer::NewL(*iSocket, iListenErrorCode, MSocketNotify::EErrorConnect, 0, this, &aConnectionData));
		iListenErrorCode=KErrNone;
		return err;
		}
	return KErrNone;
	}


void CDummyProvd::Shutdown(TCloseType anOption)
	{
	if (iErrorForNextShutdown != KErrNone)
		{
		iSocket->Error(iErrorForNextShutdown, MSocketNotify::EErrorClose);
		iErrorForNextShutdown = KErrNone;
		return;
		}
	if (iBlockOnClose)
		{
		HangModule();
		}
	DestroyAsyncErrorer();

	if(anOption==EStopInput)
		iInputStopped=ETrue;
	if(anOption==EStopOutput)
		{

		TInt c = iDataArray->Count();
		TInt i;
		for (i=0; i < c; ++i)
			{
			iDataArray->At(i).Free();
			}
		iDataArray->Reset();


		_LIT8(KEndOfData, "End of Data");
		RMBufChain datachain;
		TRAPD(res, iDataArray->AppendL(datachain));

		if(res!=KErrNone)
			iSocket->Error(KErrNoMemory);
		else
			{
			TRAPD(res, iDataArray->At(0).CreateL(KEndOfData));

			if(res!=KErrNone)
				iSocket->Error(KErrNoMemory);
			else
				{
				if(IsTransportType(KSockDatagram))
					{
					iSocket->NewData(1);
					}
				else
					{
					iSocket->NewData(iDataArray->At(0).Length());
					}
				iSocket->NewData(KNewDataEndofData);
				}
			}
		}
	if (anOption==ENormal)
		iSocket->CanClose();
	}

void CDummyProvd::Start()
//
//
//
	{
	}

void CDummyProvd::Shutdown(TCloseType /*anOption*/,const TDesC8 &/*aDisconnectData*/)
	{
	Panic(EBadCall);
	}

void CDummyProvd::AutoBind( void )
	{
//	test.Printf(_L("CDummyProvd::AutoBind\n"));
	iIsBound=ETrue;

	}
void CDummyProvd::HangModule(void)
	{
	TInt val;
	TInt shutdownVal;
	RProperty hangProp;
	TRequestStatus hangStat;

	hangProp.Attach(KDummyUid,KDummyTerminationProperty);
	hangProp.Subscribe(hangStat);
	hangProp.Get(shutdownVal);
	val = shutdownVal + 1;
	hangProp.Set(val);

	while (val > shutdownVal)
		{
		User::WaitForRequest(hangStat);
		hangProp.Subscribe(hangStat);
		hangProp.Get(val);
		}
	hangProp.Set(shutdownVal - 1);
	hangProp.Cancel();
	User::WaitForRequest(hangStat);
	hangProp.Close();
	}

//

TInt CDummyProvd::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
/**
Perform a security policy check on the client process (default implementation).
*/
	{
	return KErrNone;
	}


//---------------------------------------------------------------------------------------------------------

#pragma warning( default : 4100)

CDatagramHostResolver::CDatagramHostResolver()
//
//
//
	{
	__DECLARE_NAME(_S("CDatagramHostResolver"));
	}

CDatagramHostResolver::~CDatagramHostResolver()
{
    delete ipDNSQueryProcessor;
}

CDatagramHostResolver* CDatagramHostResolver::NewL()
//
// Make a new resolver
//
	{

      CDatagramHostResolver* self = new(ELeave) CDatagramHostResolver();

      CleanupStack::PushL(self);
      self->ConstructL();
      CleanupStack::Pop();

      return  self;
	}

void CDatagramHostResolver::ConstructL()
{
    //-- construct DNS Query processor
    ipDNSQueryProcessor = CDNSQueryProcessor::NewL();
}

void CDatagramHostResolver::CancelCurrentOperation()
//
//
//
	{
	    //-- cancel possible pending DNS Query processor request
        if(ipDNSQueryProcessor)
            ipDNSQueryProcessor->Cancel();
	}


void CDatagramHostResolver::GetByName(TNameRecord &aName)
//
//
//
	{

	if(aName.iName==_L("DontComplete"))
		return;

	aName.iAddr.SetFamily(KDummyAddrFamily);
	aName.iAddr.SetPort(0);
	if(aName.iFlags==0)
		{
		aName.iName=_L("Name One");
		aName.iFlags=0;
		iNotify->QueryComplete(KErrNone);
		}
	else if(aName.iFlags==1)
		{
		aName.iName=_L("Name Two");
		aName.iFlags=0;
		iNotify->QueryComplete(KErrNone);
		}
	else if(aName.iFlags==4)
		{
		aName.iName=_L("Name Five");
		aName.iFlags=0;
		iNotify->QueryComplete(KErrNone);
		}
	else
		iNotify->QueryComplete(KErrEof);
	}

void CDatagramHostResolver::GetByAddress(TNameRecord &aName)
//
//
//
	{

	aName.iName=_L("");
	if(aName.iFlags==0)
		{
		aName.iAddr.SetPort(10);
		aName.iFlags=0;
		iNotify->QueryComplete(KErrNone);
		}
	else if(aName.iFlags==1)
		{
		aName.iAddr.SetPort(11);
		aName.iFlags=0;
		iNotify->QueryComplete(KErrNone);
		}
	else
		iNotify->QueryComplete(KErrEof);
	}

void CDatagramHostResolver::SetHostName(TDes & aNameBuf)
//
//
//
	{
	if(aNameBuf!=_L("Tara"))
		iNotify->QueryComplete(KErrNotSupported);
	else
		iNotify->QueryComplete(KErrNone);
	}

void CDatagramHostResolver::GetHostName(TDes &aNameBuf)
//
//
//
	{
	aNameBuf=_S("PDummyHostName");
	iNotify->QueryComplete(KErrNone);
	}

/**
*   Implementation "Query" function for CHostResolver. Simulates Query(...) behaviour, checks query data validity
*   and forges query response that shall be checked at client side.
*
* @param    aQryBuf  descriptor representing query data.
* @param    aResBuf  descriptor representing query response data.
* @param    aCounter query sequential number counter. From the client's point of view it will be 0 for "Query" call
*           and increased by 1 for each "QueryGetNext" call.
* @note     for aCounter > 0 data in aQryBuf may be invalid.
*/
void CDatagramHostResolver::Query(const TDesC8& aQryBuf, TDes8& aResBuf, TInt aCounter)
{

    //-- convert 1st parameter to the reference to TDnsQuery.
    //-- Actually, for pdummy.prt and tcpip6.prt aQryBuf is a reference to the TDnsQueryBuf indeed,
    //-- but for the RHostresolver and CHostResolver protocol independency all the data passes like references to
    //-- the raw buffers.
    const TDnsQuery &dnsQry = * (reinterpret_cast<const TDnsQuery*> (aQryBuf.Ptr()));

    //-- set Query proccessor notifier.
    //-- On request completion Query processor will call iNotify->QueryComplete();
    ipDNSQueryProcessor->SetNotifier(iNotify);

    //-- process and complete query, calling iNotify->QueryComplete() later
    ipDNSQueryProcessor->ProcessQuery(dnsQry, aResBuf, aCounter);
}


TInt CDatagramHostResolver::SetOption(TUint /*aLevel*/, TUint /*aName*/, const TDesC8 &/*anOption*/)
//
//
//
	{
	return (KErrNotSupported);
	}



TInt CDatagramHostResolver::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
/**
Perform a security policy check on the client process (default implementation).
*/
	{
	return KErrNone;
	}

CDatagramServResolver::CDatagramServResolver()
//
//
//
	{
	__DECLARE_NAME(_S("CDatagramServResolver"));
	}

CDatagramServResolver* CDatagramServResolver::NewL()
//
//
//
	{
	return new(ELeave) CDatagramServResolver;
	}

void CDatagramServResolver::CancelCurrentOperation()
//
//
//
	{
	// were always synchronous so don't actually do anything
	}

void CDatagramServResolver::GetByName(const TDesC & aNameBuf,TInt32 & aPortNum)

//
//
//
	{

	if(aNameBuf!=_L("DummyName"))
		{
		iNotify->QueryComplete(KErrNotFound);
		return;
		}
	aPortNum=64;
	iNotify->QueryComplete(KErrNone);
	}

void CDatagramServResolver::GetByNumber(TDes & aNameBuf,TInt32 aPortNum)
//
//
//
	{

	if(aPortNum==66) // for testing cancel
		{
		return;
		}

	if(aPortNum!=21)
		{
		iNotify->QueryComplete(KErrNotFound);
		return;
		}
	aNameBuf=_S("DummyService");
	iNotify->QueryComplete(KErrNone);
	}

void CDatagramServResolver::RegisterService(const TDesC & aNameBuf,TInt32 aPortNum)
//
// Register a new service with the net database
//
	{

	if(aNameBuf!=_L("Simpson") || aPortNum!=500)
		{
		iNotify->QueryComplete(KErrNotFound);
		return;
		}
	iNotify->QueryComplete(KErrNone);
	}

void CDatagramServResolver::RemoveService(const TDesC & aNameBuf,TInt32 aPortNum)
//
// remove a service registered with the database
//
	{

	if(aNameBuf!=_L("Colt") || aPortNum!=45)
		{
		iNotify->QueryComplete(KErrNotFound);
		return;
		}
	iNotify->QueryComplete(KErrNone);
	}


TInt CDatagramServResolver::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
/**
Perform a security policy check on the client process (default implementation).
*/
	{
	return KErrNone;
	}
//---------------------------------------------------------------------------------------------------------

CDNSQueryProcessor::CDNSQueryProcessor()
                   :CActive(0)
{
    iDnsNotifier    = NULL;
    iCompleteResult = KErrNotSupported;

    //-- initialize seed for random delay generator
    TTime currTime;
    currTime.UniversalTime();
    iRndSeed = currTime.Int64();
}

CDNSQueryProcessor::~CDNSQueryProcessor()
{
    Cancel();
    iDelayTimer.Close();
}

CDNSQueryProcessor* CDNSQueryProcessor::NewL()
{
    CDNSQueryProcessor* self = new(ELeave) CDNSQueryProcessor;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

void CDNSQueryProcessor::ConstructL(void)
{
    User::LeaveIfError(iDelayTimer.CreateLocal());
    CActiveScheduler::Add(this);
}

void CDNSQueryProcessor::DoCancel()
{
    iDelayTimer.Cancel();
}

void CDNSQueryProcessor::RunL()
{
  //-- complete esock's request
  if(iDnsNotifier)
    iDnsNotifier->QueryComplete(iCompleteResult);
}

/**
* cancels pending request and completes with aResultCode
*
* @param    aResultCode completion code, which will be passed to the Esock
*/
void  CDNSQueryProcessor::CompleteImmediately(TInt aResultCode)
{
    iCompleteResult = aResultCode;
    CompleteImmediately();
}

/**
* cancels pending request and completes with code iCompleteResult
*/
void  CDNSQueryProcessor::CompleteImmediately()
{
    Cancel();

    SetActive();

    TRequestStatus* pStat = &iStatus;
    User::RequestComplete(pStat, iCompleteResult);
}

/**
* set up timer for a random delay between KMinDelay and KMaxDelay
* Used to similate queries processing
*/
void CDNSQueryProcessor::SetQryProcessDelay()
{
    const TInt KMinDelay = 200000; //-- minimal delay, 200 ms
    const TInt KMaxDelay = 600000; //-- maximal delay, 600 ms

    iDelayTimer.Cancel();
    iDelayTimer.After(iStatus, KMinDelay+(Math::Rand(iRndSeed) % (KMaxDelay-KMinDelay)));
}


/**
* process query. Checks query type and calls appropriate query processor function.
* @param    aQry       ref. to the query data
* @param    aResBuf    descriptor representing query response data. Will be initialized here.
* @param    aCounter   queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void CDNSQueryProcessor::ProcessQuery(const TDnsQuery& aQry, TDes8& aResBuf, TInt aCounter)
{

    if(aCounter < 0)
    {//-- invalid parameter
     CompleteImmediately(KErrCorrupt);
     return;
    }

    if(aCounter == 0)
    {//-- this is the "Query" call, not "QueryGetNext()", store query type for future use
     iCurrQryType = aQry.Type();
    }

    switch(iCurrQryType)
    {

        case KDnsRRTypeA:
            //-- process query of A type, treating aResBuf as a reference to TDnsRespABuf
             if((TUint)aResBuf.MaxSize() < sizeof(TDnsRespABuf))
             {  //-- there is no room in aResBuf to place there TDnsRespSRV structure
                CompleteImmediately(KErrNoMemory);
             }
             else
             {
                aResBuf.SetLength(sizeof(TDnsRespA));
                //-- use placement new operator to construct object of our class in the chunk of memory
                //-- in aResBuf (which is already allocated by server)
                TDnsRespA *pRespA = new((TAny*)aResBuf.Ptr())TDnsRespA;
                QryProcessA(aQry, *pRespA, aCounter);   //-- process query and complete request.
             }
        break;

        case KDnsRRTypeSRV:
             //-- process query of SRV type, treating aResBuf as a reference to TDnsRespSRVBuf
             if((TUint)aResBuf.MaxSize() < sizeof(TDnsRespSRVBuf))
             {  //-- there is no room in aResBuf to place there TDnsRespSRV structure
                CompleteImmediately(KErrNoMemory);
             }
             else
             {
                aResBuf.SetLength(sizeof(TDnsRespSRV));
                //-- use placement new operator to construct object of our class in the chunk of memory
                //-- in aResBuf (which is already allocated by server)
                //-- aResBuf shall be considered as a TPckgBuf<TDnsRespSRV>, we will also need to set a length of the aResBuf descriptor
				TDnsRespSRV    *pRespSRV = new((TAny*)aResBuf.Ptr())TDnsRespSRV;
                QryProcessSRV(aQry, *pRespSRV, aCounter);//-- process query and complete request.


             }

        break;


        case KDnsRRTypePTR:
            //-- process query of PTR type, treating aResBuf as a reference to TDnsRespPTRBuf
             if((TUint)aResBuf.MaxSize() < sizeof(TDnsRespPTRBuf))
             {  //-- there is no room in aResBuf to place there TDnsRespPTRBuf structure
                CompleteImmediately(KErrNoMemory);
             }
             else
             {
                aResBuf.SetLength(sizeof(TDnsRespPTR));

                TDnsRespPTR    *pRespPTR = new((TAny*)aResBuf.Ptr())TDnsRespPTR;
                QryProcessPTR(aQry, *pRespPTR, aCounter);//-- process query and complete request.

             }
        break;

        case KDnsRRTypeNAPTR:
            //-- process query of NAPTR type, treating aResBuf as a reference to TDnsRespNAPTRBuf
             if((TUint)aResBuf.MaxSize() < sizeof(TDnsRespNAPTRBuf))
             {  //-- there is no room in aResBuf to place there TDnsRespNAPTRBuf structure
                CompleteImmediately(KErrNoMemory);
             }
             else
             {
                aResBuf.SetLength(sizeof(TDnsRespNAPTR));

                TDnsRespNAPTR    *pRespNAPTR = new((TAny*)aResBuf.Ptr())TDnsRespNAPTR;
                QryProcessNAPTR(aQry, *pRespNAPTR, aCounter);//-- process query and complete request.
             }
        break;

        case KDnsRRTypeMX:
            //-- process query of MX type, treating aResBuf as a reference to TDnsRespMXBuf
             if((TUint)aResBuf.MaxSize() < sizeof(TDnsRespMXBuf))
             {  //-- there is no room in aResBuf to place there TDnsRespMXBuf structure
                CompleteImmediately(KErrNoMemory);
             }
             else
             {
                aResBuf.SetLength(sizeof(TDnsRespMX));

                TDnsRespMX    *pRespMX  = new((TAny*)aResBuf.Ptr())TDnsRespMX;
                QryProcessMX(aQry, *pRespMX, aCounter);//-- process query and complete request.
             }
        break;


        case KDnsRRTypeAAAA:
            //-- process query of AAAA type, treating aResBuf as a reference to TDnsRespAAAABuf
             if((TUint)aResBuf.MaxSize() < sizeof(TDnsRespAAAABuf))
             {  //-- there is no room in aResBuf to place there TDnsRespMXBuf structure
                CompleteImmediately(KErrNoMemory);
             }
             else
             {
                aResBuf.SetLength(sizeof(TDnsRespAAAA));

                TDnsRespAAAA    *pRespAAAA = new((TAny*)aResBuf.Ptr())TDnsRespAAAA;
                QryProcessAAAA(aQry, *pRespAAAA, aCounter);//-- process query and complete request.
             }
        break;


        default:
            //-- Query of unknown type
            CompleteImmediately(KErrNotSupported);
        break;
    } // switch

}

/**
* process A query. Checks query data and forges query result.
* @param aQry       ref. to the A query data
* @param aQryResult ref. to the destination query result buffer.
* @param aCounter   - queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void CDNSQueryProcessor::QryProcessA(const TDnsQuery& aQry, TDnsRespA& aQryResult, TInt aCounter)
{
    TInetAddr   addr;

    switch(aCounter)
    {
        //-- query counter is 0, so it is "Query" call
        case 0:

            //-- check query data, it shall be set properly in the test case that calls RHostResolver::Query(...)
            if( aQry.Class() != KDnsRRClassIN  ||  //-- Query class shall be IN
                aQry.Type()  != KDnsRRTypeA    ||  //-- Query type shall be KDnsRRTypeA
                aQry.Data().CompareF(_L8("http://www.sample.net/")) != 0
              )
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }

            //-- resolve domain name, forge "A" query result that will be checked on the client side.
            addr.Input(_L("192.168.40.4"));

            aQryResult.SetHostAddress(addr);
            aQryResult.SetRRTtl(0x121212);

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();

        break;

        //-- query counter is 1, so it is the first "QueryGetNext" call.
        case 1:

            //-- resolve domain name, forge "A" query result that will be checked on the client side.
            addr.Input(_L("177.123.221.251"));

            aQryResult.SetHostAddress(addr);
            aQryResult.SetRRTtl(0x112233);

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();

        break;


        default:
            //-- query counter is greater than 0, let's not support more than one "QueryGetNext" call
            CompleteImmediately(KErrNotFound);
        break;

    }

}

/**
* process SRV query. Checks query data and forges query result.
* @param aQry       ref. to the SRV query data
* @param aQryResult ref. to the destination query result buffer.
* @param aCounter   - queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void  CDNSQueryProcessor::QryProcessSRV   (const TDnsQuery& aQry, TDnsRespSRV& aQryResult, TInt aCounter)
{

    switch(aCounter)
    {
        case 0: //-- query counter is 0, so it is "Query" call

            //-- check query data, it shall be set properly in the test case that calls RHostResolver::Query(...)
            //-- see also RFC 2782

            if(aQry.Class() != KDnsRRClassIN || //-- Query class shall be IN
               aQry.Type() != KDnsRRTypeSRV  || //-- Query type shall be KDnsRRTypeSRV
               aQry.Data().CompareF(_L8("_ldap._tcp.example.com")) != 0)
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }

            //-- forge a SRV query result that will be checked on the client side.
            aQryResult.SetRRTtl(0x123456);

            aQryResult.SetPriority(384);              //-- priority
            aQryResult.SetWeight(784);                //-- weight
            aQryResult.SetPort(123);                  //-- port
            aQryResult.SetTarget(_L8("old-slow-box"));//-- target

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();
        break;

        case 1:
            //-- query counter is 1, so it is the first "QueryGetNext" call.
            //-- Forge a "SRV" query next result that will be checked on the client side.
            aQryResult.SetRRTtl(0x123765);

            aQryResult.SetPriority(236);              //-- priority
            aQryResult.SetWeight(962);                //-- weight
            aQryResult.SetPort(125);                  //-- port
            aQryResult.SetTarget(_L8("new-fast-box"));//-- target

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();
       break;

        default:
            //-- query counter is greater than 0, let's not support more than one "QueryGetNext" call
            CompleteImmediately(KErrNotFound);
        break;
    }// switch
}

/**
* process PTR query. Checks query data and forges query result.
* @param aQry       ref. to the PTR query data
* @param aQryResult ref. to the destination query result buffer.
* @param aCounter   - queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void  CDNSQueryProcessor::QryProcessPTR   (const TDnsQuery& aQry, TDnsRespPTR& aQryResult, TInt aCounter)
{
    switch(aCounter)
    {
        case 0: //-- query counter is 0, so it is "Query" call
           {

            //-- check query data, it shall be set properly in the test case that calls RHostResolver::Query(...)
            if(aQry.Class() != KDnsRRClassIN || //-- Query class shall be IN
               aQry.Type() != KDnsRRTypePTR)    //-- Query type shall be KDnsRRTypePTR
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }

            //-- check  Inet Address passes by test from client side
            const TInetAddr& inetAddr = (const TInetAddr&)aQry.Data();

            TInetAddr expInetAddr;
            expInetAddr.Input(_L("192.111.22.77"));

            if(! inetAddr.CmpAddr(expInetAddr))
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }

            //-- resolve address, forge "PTR" query result that will be checked on the client side.
            aQryResult.SetHostName(_L8("http://www.CDatagramHostResolver_QryProcessPTR.response/"));
            aQryResult.SetRRTtl(0x223441);

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();
           }
        break;

        default:
            //-- query counter is greater than 0, let's not support more than one "QueryGetNext" call
            CompleteImmediately(KErrNotFound);
        break;

    }

}

/**
* process NAPTR query. Checks query data and forges query result.
* @param aQry       ref. to the NAPTR query data
* @param aQryResult ref. to the destination query result buffer.
* @param aCounter   - queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void  CDNSQueryProcessor::QryProcessNAPTR (const TDnsQuery& aQry, TDnsRespNAPTR& aQryResult, TInt aCounter)
{
    switch(aCounter)
    {
        case 0: //-- query counter is 0, so it is "Query" call
           {

            //-- check query data, it shall be set properly in the test case that calls RHostResolver::Query(...)
            if(aQry.Class() != KDnsRRClassIN    ||  //-- Query class shall be IN
               aQry.Type() != KDnsRRTypeNAPTR   ||  //-- Query type shall be KDnsRRTypeNAPTR
               aQry.Data().CompareF(_L8("http://www.foo_bar.ru/")) != 0)
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }


            //-- forge "NAPTR" query result that will be checked on the client side.
            aQryResult.SetRRTtl(0x2673411);
            aQryResult.SetOrder(123);
            aQryResult.SetPref(99);

            aQryResult.SetFlags(_L8("SAUP"));
            aQryResult.SetService(_L8("http+I2R"));
            aQryResult.SetRegexp(_L8("!£%^^&($%£$~~## !!!!!"));
            aQryResult.SetReplacement(_L8("www.next-name.it"));

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();
           }
        break;

        default:
            //-- query counter is greater than 0, let's not support more than one "QueryGetNext" call
            CompleteImmediately(KErrNotFound);
        break;

    }

}

/**
* process MX query. Checks query data and forges query result.
* @param aQry       ref. to the MX query data
* @param aQryResult ref. to the destination query result buffer.
* @param aCounter   - queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void  CDNSQueryProcessor::QryProcessMX    (const TDnsQuery& aQry, TDnsRespMX& aQryResult, TInt aCounter)
{
    switch(aCounter)
    {
        case 0: //-- query counter is 0, so it is "Query" call
           {
            //-- check query data, it shall be set properly in the test case that calls RHostResolver::Query(...)

            if(aQry.Class() != KDnsRRClassIN || //-- Query class shall be IN
               aQry.Type() != KDnsRRTypeMX   || //-- Query type shall be KDnsRRTypeMX
               aQry.Data().CompareF(_L8("http://www.gooooooogle.ru/")) != 0)
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }


            //-- forge "MX" query result that will be checked on the client side.
            aQryResult.SetRRTtl(0xdead);

            aQryResult.SetPref(345);
            aQryResult.SetHostName(_L8("c.example.org"));

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();
           }
        break;

        default:
            //-- query counter is greater than 0, let's not support more than one "QueryGetNext" call
            CompleteImmediately(KErrNotFound);
        break;

    }

}

/**
* process AAAA query. Checks query data and forges query result.
* @param aQry       ref. to the AAAA query data
* @param aQryResult ref. to the destination query result buffer.
* @param aCounter   - queries counter. It is 0 for "Query" call and increased each "QueryGetNext" call
*/
void  CDNSQueryProcessor::QryProcessAAAA(const TDnsQuery& aQry, TDnsRespAAAA& aQryResult, TInt aCounter)
{
    TInetAddr   addr;

    switch(aCounter)
    {
        case 0: //-- query counter is 0, so it is "Query" call
           {

            //-- check query data, it shall be set properly in the test case that calls RHostResolver::Query(...)
            if(aQry.Class() != KDnsRRClassIN ||
               aQry.Type() != KDnsRRTypeAAAA ||
               aQry.Data().CompareF(_L8("http://www.sample_AAAA.net/")) != 0)
            {
                CompleteImmediately(KErrCorrupt);
                return;
            }

            //-- forge "AAAA" query result that will be checked on the client side.

            //-- resolve domain name, forge "A" query result that will be checked on the client side.
            addr.Input(_L("2001:618:400:6a:0:0:0:abc"));
            aQryResult.SetHostAddress(addr);
            aQryResult.SetRRTtl(0xbeef);

            SetCompletionCode(KErrNone);

            //-- simulate processing random delay and complete the request later
            SetQryProcessDelay();
            SetActive();
           }
        break;

        default:
            //-- query counter is greater than 0, let's not support more than one "QueryGetNext" call
            CompleteImmediately(KErrNotFound);
        break;

    }

}


//---------------------------------------------------------------------------------------------------------

void CDatagramNetDataBase::Query(TDes8& aBuffer)
//
// Ctor
//
	{
	aBuffer.Capitalize();
	iNotify->QueryComplete(KErrNone);
	}

void CDatagramNetDataBase::Add(TDes8& aBuffer)
//
// Ctor
//
	{

	// Query not completed because its cancelled by the test code
	if(aBuffer==_L8("Rabbit"))
		iCount=10;
	else
		iNotify->QueryComplete(KErrNotSupported);
	}

void CDatagramNetDataBase::Remove(TDes8& aBuffer)
//
// Ctor
//
	{


	if(aBuffer==_L8("Rabbit") && iCount==20)
		iNotify->QueryComplete(KErrNone);
	else
		iNotify->QueryComplete(KErrNotFound);
	}

void CDatagramNetDataBase::CancelCurrentOperation()
//
//
//
	{

	if(iCount==10)
		iCount=20;
	}

CDatagramNetDataBase* CDatagramNetDataBase::NewL()
//
// Ctor
//
	{
	return new(ELeave)CDatagramNetDataBase;
	}

CDatagramNetDataBase::CDatagramNetDataBase()
//
// Ctor
//
	{
	__DECLARE_NAME(_S("CDatagramNetDataBase"));
	}


TInt CDatagramNetDataBase::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
/**
Perform a security policy check on the client process (default implementation).
*/
	{
	return KErrNone;
	}

//---------------------------------------------------------------------------------------------------------

CInterfaceProtocol::CInterfaceProtocol(TServerProtocolDesc* aProtoDesc):CDummyProtocol(aProtoDesc)
	{
	iInterfaces.SetOffset(_FOFF(CIfHolder, iLink));
	}

CInterfaceProtocol::~CInterfaceProtocol()
//
// Dtor
//
	{

	__ASSERT_DEBUG(iInterfaces.IsEmpty(), Panic(EInterfaceNotDeleted));
	}

TInt CInterfaceProtocol::GetOption(TUint level,TUint name,TDes8 &anOption,CProtocolBase* /*aSourceProtocol*/)
//
// Extra options
//
	{

	if(level==KNifOptLevel)
		{
		TInt ret;

		if(name==KNifOptGetNifIfUser)
			{
			TNifIfUser ifuser;
			ifuser() = this;
			anOption.Copy(ifuser);
			return KErrNone;
			}
		else if(name==1)
			{
			TRAP(ret, StartAutoInterfaceL();)
			return ret;
			}
		else if(name==2)
			{
			if(iInterfaceName.Length())
				Nif::Stop(iInterfaceName);
			return KErrNone;
			}
		else if(name==3)
			{
			TRAP(ret, Nif::StartL(iInterfaceName);)
			return ret;
			}
		else if(name==4)
			{
			TNifProgress* p = (TNifProgress*)anOption.Ptr();
			anOption.SetLength(sizeof(TNifProgress));
		    TRAP(ret, Nif::ProgressL(*p, iInterfaceName);)
			return ret;
			}
		else if(name==5)
			{
			if(iInterfaces.IsEmpty())
				return KErrNotFound;
		    Nif::Stop(iInterfaces.First(), iInterfaces.First()->iIf);
			return KErrNone;
			}
		else if(name==6)
			{
			if(iInterfaces.IsEmpty())
				return KErrNotFound;
			TNifProgress* p = (TNifProgress*)anOption.Ptr();
			anOption.SetLength(sizeof(TNifProgress));
		    TRAP(ret, Nif::ProgressL(*p, iInterfaces.First(), iInterfaces.First()->iIf);)
			return ret;
			}
		}
	return KErrNotSupported;
	}

void CInterfaceProtocol::StartAutoInterfaceL()
	{

	// Create holder
	CIfHolder* h;
	h = new (ELeave) CIfHolder(*this);
	CleanupStack::PushL(h);
	Nif::BindL(*this, h, iInterfaceName);
	CleanupStack::Pop();
	}

void CInterfaceProtocol::IfUserBindFailure(TInt, TAny* aId)
	{

	// Find the interface which went down
	TDblQueIter<CIfHolder> iter(iInterfaces);
	CIfHolder* h = 0;
	while((h=iter++)!=0)
		{
		if(aId==h)
			{
			delete h;
			break;
			}
		}
	__ASSERT_ALWAYS(h, Panic(EIdAndNoHolder));
	}

void CInterfaceProtocol::IfUserNewInterfaceL(CNifIfBase* aIf, TAny* aId)
	{

	// If Id try to find it
	CIfHolder* h;
	if(aId)
		{
		TDblQueIter<CIfHolder> iter(iInterfaces);
		while((h=iter++)!=0)
			{
			if(aId==h)
				break;
			}
		__ASSERT_ALWAYS(h, Panic(EIdAndNoHolder));
		if (h==0)
			{ // will never get in here as will have panic-ed above
			h = (CIfHolder*)0xABCD; // keep lint happy
			}
		}
	else // create a new holder
		h = new (ELeave) CIfHolder(*this);

    h->iIf=aIf;
	CleanupStack::PushL(h);
	aIf->BindL(this);
	CleanupStack::Pop();
	}

void CInterfaceProtocol::IfUserInterfaceDown(TInt aError, CNifIfBase* aIf)
	{

	if (aError == KErrLinkConfigChanged)
		{
		return;
		}

	// Interface has gone down so delete it from our records
	TDblQueIter<CIfHolder> iter(iInterfaces);
	CIfHolder* h = 0;
	while((h=iter++)!=0)
		{
		if(aIf==h->iIf)
			{
			delete h;
			break;
			}
		}
	__ASSERT_ALWAYS(h, Panic(EIdAndNoHolder));
	}

CProtocolBase* CInterfaceProtocol::IfUserProtocol()
	{
	return this;
	}

void CInterfaceProtocol::IfUserOpenNetworkLayer()
	{
	iInterfaceCount++;
	Open();
	}

void CInterfaceProtocol::IfUserCloseNetworkLayer()
	{
	iInterfaceCount--;
	Close();
	}

TBool CInterfaceProtocol::IfUserIsNetworkLayerActive()
	{
	return (RefCount()-iInterfaceCount>0);
	}

void CInterfaceProtocol::Close()
	{
	if(RefCount()-iInterfaceCount<=0)
      {
		Nif::NetworkLayerClosed(*this);
      }
	CProtocolBase::Close();
	}

CIfHolder::CIfHolder(CInterfaceProtocol& aProt)
	{
	aProt.iInterfaces.AddLast(*this);
	}

CIfHolder::~CIfHolder()
	{
	iLink.Deque();
	}


