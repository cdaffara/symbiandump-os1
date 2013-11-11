// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#if !defined(__SS_THREAD_H__)
#define __SS_THREAD_H__

#include <elements/sd_thread.h>

#include <es_sock.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_threadtransport.h>
#include <comms-infras/api_ext_msg.h>
#include <ss_fact.h>

class CSockManData;
class CSockManData;
class CPlayer;
class CPitBoss;
class CSockSession;

namespace Den
	{
	class TWorkerTransportMsg;
	}

NONSHARABLE_CLASS(TCFWorkerThreadPublicInfo) : public Den::TWorkerThreadPublicInfo
	{
public:
    enum TType
        {
        //see Den::TWorkerThreadPublicInfo
		EBtPlayerThread = 2,	//< BT Group Player
		EIrPlayerThread = 3,	//< IR Group Player
		ESmsWapPlayerThread	= 4,//< SMS + WAP Group Player
		EIpPlayerThread = 5		//< IP Group Player
		};
	};

/**
Granularity and default number of values in the list of EskMasks.
@internalComponent
*/
const TInt KEskMaskListGranularity=2;

NONSHARABLE_CLASS(CWorkerThread) : public Den::CCommonWorkerThread
/**
@internalComponent
*/
	{
public:
	static CWorkerThread* NewL(CommsFW::TCFModuleInfo* aModuleInfo);
	~CWorkerThread();

	const CSockManData* SockManGlobals() const
		{
		return iSockManGlobals;
		}

	CSockManData* SockManGlobals()
		{
		return iSockManGlobals;
		}

	CPlayer* Player() const;
	CPitBoss& PitBoss() const;
	Den::CCommonWorkerDealer* WorkerDealer() const;

	Den::CCommonPitBoss* DoCreatePitBossL(Den::CCommonWorkerThread* aWorkerThread);
	Den::CCommonPlayer* DoCreatePlayerL(Den::CCommonWorkerThread* aWorkerThread, Den::TPlayerRole aPlayerRole);
	Den::CCommonDealer* DoCreateDealerL(Den::CCommonWorkerThread* aWorkerThread, Den::TPlayerRole aPlayerRole);

	void CFBindMessageReceived(const CommsFW::TCFBindMsg& aMsg);
	void CFMessageForward(const CommsFW::TCFForwardMsg& aMessage);

	// From MLegacyMessageReciever
	TBool DoDispatchL(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId);
	void OnDispatchLeave(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId, TInt aError);
	// For external use
	IMPORT_C static TInt CookieForProtocol(TUint aAddrFamily, TUint aSockType, TUint aProtocol, Messages::TRuntimeCtxId& aCookie);
	IMPORT_C static TInt CookieForProtocolByName(const TProtocolName& aName, Messages::TRuntimeCtxId& aCookie);
	IMPORT_C static TBool ResolveWorkerNameToId(const TDesC8& aWorkerName, CommsFW::TWorkerId& aWorkerId);
	IMPORT_C static TInt CookieForWorkerId(CommsFW::TWorkerId aWorkerId, Messages::TRuntimeCtxId& aCookie);
	IMPORT_C static TInt CurrentWorkerId(CommsFW::TWorkerId& aWorkerId);

	/**
	This is the main thread function used by the RootServer when creating a new ESock thread.
	The RootServer will know the DLL ordinal for it, as specified in the .CMI file, and this is where
	an ESock instance starts and ends operation (unless it PANICs).
	@param aArg Will be the module info structure from the RootServer.
	@see CommsFW::TCFModuleInfo
	*/
	IMPORT_C static TInt ThreadEntrypoint(TAny* aArg);

protected:
	void ConstructL(CommsFW::TCFModuleInfo* aModuleInfo);
	CWorkerThread();
	void CreateTransportUserL(TTransportUser& aTemplateUser);
	void DoProcessWorkerIntroductionL(const Den::TWorkerIntroductionMsg& aMessage);
	void DoFillIntroMessage(CommsFW::TWorkerId aPeerId, Den::TWorkerIntroductionMsg& aIntroMsg);
	void DoCompleteUnbinding(CommsFW::TWorkerId aWorker);
	void DoDeterminePlayerRoleL(HBufC8* aIniData, Den::TPlayerRole &aPlayerRole);
	void DoSetShuttingDown();
	void DoPostMortemCleanup();
	void DoSendOtherIntroductionMessagesL(const CommsFW::TWorkerId& aPeerId);

private:
	void MaybeIncorporateFCL(const Den::TPlayerRole& aPlane, const Messages::TNodeId& aPeerId);
	void ProcessMatchingEskFilesL(const CommsFW::COwnEntryList* aEskFiles);
	void ProcessIniDataL();
	TInt TransportMessageReceived(const Den::TWorkerTransportMsg& aMessage, TRequestWrapper& aRequestWrapper, CommsFW::TWorkerId aWorkerId);

	void SendObjectBrokerIntroductionMessage(const CommsFW::TWorkerId& aPeerId);

private:
	CSockManData* iSockManGlobals;					//< Cache pointer to global variables to avoid TLS lookups

	enum
		{
		/** Max length of the EskMask value */
		KMaxEskMaskLength = 16,
		/** Max number of optional EskMasks, e.g. EskMask0, EskMask1, etc... */
		KEskMaskLabelMax = 8,
		/** Max length of label searched for, e.g. length of "EskMask0" */
		KEskMaskLabelLength
		};

	/** Buffer type holding the ESK mask defined in the .CMI files, e.g. "IP.*.ESK". */
	typedef TBuf<KMaxEskMaskLength> TEskMask;

	/**
	Array of ESK masks as defined in the .CMI file loading this instance of ESock.
	The Player will load all .ESK files matching these masks.
	*/
	RArray<TEskMask> iEskMaskList;

	/**
	A holder of plug-ins capable of consuming transport messages not internal to ESock and
	some methods to manage them.
	*/
	class RTransportUsers : public RArray<TTransportUser>
		{
	public:
		void Shutdown(TBool aAlreadyDead);
		void NotifyPeerDeath(CommsFW::TWorkerId aPeer);
		};

	/**
	The array of transport users for this instance of ESock. Will be populated when
	the appropriate ECOM plug-ins are loaded on demand (as messages with unknown message ID arrives
    and causes the appropriate ECOM plugins to be loaded and inserted).
	@see TTransportUser
	*/
	RTransportUsers iTransportUsers;

//Keep conditional data members at the end, even if this class is not supposed to be derived from
#ifdef ESOCK_HOME_THREAD_CHECK_ENABLED
public:
	TThreadId OwnThread() const
		{
		return iOwnThread;
		}
private:
	TThreadId iOwnThread;
#endif
	};

#endif	// __SS_THREAD_H__

