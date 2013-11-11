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

#if !defined(__SS_RSLV_H__)
#define __SS_RSLV_H__

#define SYMBIAN_NETWORKING_UPS

#include <ss_std.h>
#include <comms-infras/ss_flowbinders.h>
#include "ss_flowrequest.h"

#include <comms-infras/ss_nodemessages_dataclient.h>

class CSockSession;
class CPlayer;

class CHostResolvProvdBase;
struct TServerProtocolDesc;

namespace ESock
{

class MFlowBinderControl;
class MLowerDataSender;
class CSubConnectionFlowBase;

NONSHARABLE_CLASS(CHostResolver) : public CSockSubSession, public Messages::ASimpleNodeIdBase, public AIPCFlowRequester,
								   public MResolverNotify,
								   protected MUpperControl //workaround until we have app layer protocols
#ifdef SYMBIAN_NETWORKING_UPS
								 , private ASockSubSessionPlatsecApiExt
#endif
/**
 @internalComponent
 */
	{
	friend class ::ProtocolManager;
	friend class ::CPlayer; //SetBlockedReq(..)
public:
	static CHostResolver* NewLC(CProtocolRef* aProtRef, CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
    virtual ~CHostResolver();

	virtual Den::TSubSessInfo Type() const
		{
		return TCFSubSessInfo(TCFSubSessInfo::EHostResolver);
		}

	virtual Messages::RNodeInterface* ServiceProvider() const
		{
		return NULL;
		}
	TBool GetFlowAndSCPR(Messages::TNodeId& aFlow, Messages::TNodeId& aSCPR) const;

	inline void SetResolver(CHostResolvProvdBase* aResolver);

    void GetByNameL();
	void Next();
	void GetByAddressL();
	void GetHostName();
	void SetHostNameL();
	void SetOptionL();

    void QueryL(void);
    void QueryGetNext(void);


	void Cancel();
	void QueryComplete(TInt anError);

	inline TUint RequiresOwnerInfo() const;

	virtual void ProcessMessageL();

	void InitiateDestruction();

protected:
	void LockToConnectionInfo();
	void LockToConnectionInfo(const TDesC8& aConnectionInfo);
    void SetBlockedReq(const RMessage2& aMsg)
        {
        iBlockedReq = aMsg;
        }
protected:

    //Messages::MNode

	virtual void ReceivedL(
		const Messages::TRuntimeCtxId& aSender,
		const Messages::TNodeId& aRecipient,
		Messages::TSignatureBase& aMessage
		);

#ifdef SYMBIAN_NETWORKING_UPS
	// from ASockSubSessionPlatSecApiExt
	TInt GetMessage(const RMessage2*& aMessage) const;
#endif

	//messages
	void BindToL(const TCFDataClient::TBindTo& aBindTo);

private:
	CHostResolver(CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	void CreateL(CProtocolRef* aProtRef);
	inline TUint RequiresConnectionStartup();
	void HandleConnectionSetupL();
	void FinalCompleteAllBlockedMessages(TInt aResult);
    HBufC8* GetBuffer(HBufC8* apBuf, TInt aBufLenRequired);
    TBool   GetQueryRespBuffer(void);
    TBool   GetQueryBuffer(void);

private:
    //MUpperControl
    virtual void StartSending();
	virtual void Error(TInt aError);

#ifdef SYMBIAN_NETWORKING_UPS
	void InitialiseFlow(CSubConnectionFlowBase* aFlow);
	TInt GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const;
#endif 
	void ReadAndSetOptionL();


private:
	CHostResolvProvdBase* iRSP;
	MLowerControl* iLowerControl;
	MLowerDataSender* iLowerDataSender;
	MFlowBinderControl* iFlowBinderControl;
	TBool iBusy : 1;
	TBool iAwaitingConnection : 1;
	RMessage2 iBlockedReq;
	TNameRecord iNameRec;
	TInt iCount;
	TInt iCurrentOp;
	TDblQueLink iInterfaceLink;			// for CInterface::iHostResolvers
	TProtocolDesc* iProtocolInfo;

    HBufC8* ipQryBuf;       //< pointer to the query data buffer heap descriptor
    HBufC8* ipQryRespBuf;   //< pointer to the query response buffer heap descriptor

    TPtr8   iPtrQryBuf;     //< Pointer descriptor, associated with ipQryBuf
    TPtr8   iPtrQryResBuf;  //< Pointer descriptor, associated with ipQryRespBuf

    };

inline void CHostResolver::SetResolver(CHostResolvProvdBase* aResolver)
	{iRSP=aResolver;}

inline TUint CHostResolver::RequiresConnectionStartup()
	{ return iProtocolInfo->iNamingServices & KNSRequiresConnectionStartup; };

inline TUint CHostResolver::RequiresOwnerInfo() const
	{
	return (iProtocolInfo->iServiceInfo & KSIRequiresOwnerInfo);
	}

// ================================================================================================

NONSHARABLE_CLASS(CServiceResolver) : public CSockSubSession, public MResolverNotify
/**
@internalComponent
*/
	{
	friend class ProtocolManager;
private:
	CServiceResolver(CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	void CreateL();
public:

	static CServiceResolver* NewLC(CProtocolRef* aProtRef, CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	virtual Den::TSubSessInfo Type() const
		{
		return TCFSubSessInfo(TCFSubSessInfo::EServiceResolver);
		}

	virtual Messages::RNodeInterface* ServiceProvider() const
		{
		return NULL;
		}

	inline void SetResolver(CServiceResolvProvdBase* aResolver);
	void GetByNameL();
	void GetByNumber();
	void RegisterServiceL();
	void RemoveServiceL();
	virtual ~CServiceResolver();
	void Cancel();
	void QueryComplete(TInt anError);

	virtual void ProcessMessageL();
private:
	void FinalCompleteAllBlockedMessages(TInt aResult);
private:
	CServiceResolvProvdBase* iRSP;
	RMessage2 iBlockedReq;
	TBool iBusy;
	TServiceName iNameBuf;
	TInt32 iPortNum;
	};

inline void CServiceResolver::SetResolver(CServiceResolvProvdBase* aResolver)
	{iRSP=aResolver;}

NONSHARABLE_CLASS(CNetDatabase) : public CSockSubSession, public MResolverNotify
/**
@internalComponent
*/
	{
	friend class ProtocolManager;
private:
	CNetDatabase(CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
public:
	static CNetDatabase* NewLC(CProtocolRef* aProtRef, CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	virtual Den::TSubSessInfo Type() const
		{
		return TCFSubSessInfo(TCFSubSessInfo::ENetDatabase);
		}

	virtual Messages::RNodeInterface* ServiceProvider() const
		{
		return NULL;
		}

	void RequestL();
	inline void SetProvd(CNetDBProvdBase* aResolver);
	virtual ~CNetDatabase();
	void QueryComplete(TInt anError);
	void Cancel();

	virtual void ProcessMessageL();

private:
	void FinalCompleteAllBlockedMessages(TInt aResult);
private:
	CNetDBProvdBase* iRSP;
	RMessage2 iBlockedReq;
	TBool iBusy;
	HBufC8* iBuf;
	TPtr8* iBufPtr;
	};

inline void CNetDatabase::SetProvd(CNetDBProvdBase* aResolver)
	{iRSP=aResolver;}

} //namespace ESock

#endif
// __SS_RSLV_H__
