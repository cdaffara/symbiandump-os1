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

#if !defined(__PDUMMY_H__)
#define __PDUMMY_H__

#include <comms-infras/nifif_internal.h>

#include "ES_DUMMY.H"


//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDProtocolFamily): public CProtocolFamilyBase
	{
public:
	static CDProtocolFamily * NewL();
	void ConstructL();
	TInt Install();
	TInt Remove();
	CProtocolBase * NewProtocolL(TUint aSockType,TUint aProtocol);
	TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
	CDProtocolFamily();
		
private:
	void Construct();

private:
	TServerProtocolDesc iProtocolDescs[5];
	};

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDummyProtocol): public CProtocolBase
	{
public:
	IMPORT_C static CDummyProtocol *NewL(TInt aProtocol, TServerProtocolDesc* aProtoDesc);
	IMPORT_C TInt ProtocolBaseBind(CProtocolBase* aProt, TUint aId);
	IMPORT_C TInt ProtocolBaseBindTo(CProtocolBase* aProt);
	IMPORT_C TInt ProtocolBaseError(CProtocolBase* aProt);
	IMPORT_C TInt ProtocolBaseGetOption(TUint aLevel, TUint aName, TDes8& aBuf, CProtocolBase* aProt);
	IMPORT_C TInt ProtocolBaseNewHostResolver();
	IMPORT_C TInt ProtocolBaseNewNetDatabase();
	IMPORT_C TInt ProtocolBaseNewSAP(TUint aProt);
	IMPORT_C TInt ProtocolBaseNewServiceResolver();
	IMPORT_C TInt ProtocolBaseProcess(RMBufChain &aChain, CProtocolBase* aProt);		
	IMPORT_C TInt ProtocolBaseSend(RMBufChain &aChain, CProtocolBase* aProt);
	IMPORT_C TInt ProtocolBaseSetOption(TUint aLevel, TUint aName, TDes8& aBuf, CProtocolBase* aProt);
	IMPORT_C TInt ProtocolBaseSendWithAddress(TDes8& aBuf,TSockAddr* aTo,TSockAddr* aFrom, CProtocolBase* aProt);
	IMPORT_C TInt ProtocolBaseProcessWithAddress(TDes8& aPDU, TSockAddr* aFrom, TSockAddr* aTo, CProtocolBase* aProt);
	IMPORT_C CDummyProtocol(TServerProtocolDesc* aProtoDesc);
	IMPORT_C ~CDummyProtocol();
		
public:
	CServProviderBase *NewSAPL(TUint aProtocol);
	virtual CHostResolvProvdBase *NewHostResolverL();
	virtual CServiceResolvProvdBase *NewServiceResolverL();
	virtual CNetDBProvdBase* NewNetDatabaseL();
	TBool CanCreateSockets();
	void InitL(TDesC& aTag);
	void BindL(CProtocolBase *aProtocol, TUint anId);
	virtual void BindToL(CProtocolBase *protocol);
	void StartL(void);
	TInt Send(RMBufChain &,CProtocolBase* aSourceProtocol);
	void Process(RMBufChain &,CProtocolBase* aSourceProtocol);
	TInt Send(TDes8 &, TSockAddr *to,TSockAddr *from,CProtocolBase* aSourceProtocol);
	void Process(TDes8 & ,TSockAddr *from,TSockAddr *to,CProtocolBase* aSourceProtocol);
	void Identify(TServerProtocolDesc *)const;
	TInt GetOption(TUint level,TUint name,TDes8 &anOption,CProtocolBase* aSourceProtocol);
	TInt SetOption(TUint level,TUint name,const TDesC8& option,CProtocolBase* aSourceProtocol);
	void Error(TInt anError,CProtocolBase* aSourceProtocol);
	TBool IsStarted(){return iIsStarted;}	
	
protected:
	TInt iType;
	TServerProtocolDesc* iProtoDesc;
	TBool iIsStarted;
	CArrayFixFlat<CProtocolBase *> iProtocols;
	};

//---------------------------------------------------------------------------------------------------------

class CInterfaceProtocol;
NONSHARABLE_CLASS(CIfHolder) : public CBase
	{
public:
	CIfHolder(CInterfaceProtocol&);
	~CIfHolder();
	TDblQueLink iLink;
	CNifIfBase *iIf;
	};

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CInterfaceProtocol) : public CDummyProtocol, public MNifIfUser
	{
friend class CIfHolder;
public:
	CInterfaceProtocol(TServerProtocolDesc* aProtoDesc);
	~CInterfaceProtocol();
	virtual TInt GetOption(TUint level,TUint name,TDes8 &anOption,CProtocolBase* aSourceProtocol);
	virtual void IfUserBindFailure(TInt aResult, TAny* aId);
	virtual void IfUserNewInterfaceL(CNifIfBase* aIf, TAny* aId);
	virtual void IfUserInterfaceDown(TInt aResult, CNifIfBase* aIf);
	virtual CProtocolBase* IfUserProtocol();
	virtual void IfUserOpenNetworkLayer();
	virtual void IfUserCloseNetworkLayer();
	virtual TBool IfUserIsNetworkLayerActive();
	virtual void Close();
	void StartAutoInterfaceL();
private:
	TDblQue<CIfHolder> iInterfaces;
	TName iInterfaceName;
	TInt iInterfaceCount;
	};

//---------------------------------------------------------------------------------------------------------

class CAsyncSocketErrorer;

NONSHARABLE_CLASS(CDummyProvd) : public CServProviderBase
	{
public:
	~CDummyProvd();
	static CDummyProvd *NewL(CDummyProtocol &aProtocol);
	virtual void LocalName(TSockAddr &anAddr)const;
	virtual TInt SetLocalName(TSockAddr &anAddr);
	virtual void RemName(TSockAddr &anAddr)const;
	virtual TInt SetRemName(TSockAddr &anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8 &anOption)const;
	virtual TInt SetOption(TUint level,TUint name,const TDesC8 &anOption);
	virtual void Ioctl(TUint level,TUint name,TDes8 *anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt Write(RMBufChain& aData, TUint options, TSockAddr* anAddr=NULL);
	virtual TUint Write(const TDesC8& aDesc, TUint options, TSockAddr* anAddr);
	virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint options,TSockAddr* anAddr=NULL);
	virtual void GetData(TDes8& aDesc,TUint aOptions,TSockAddr*aAddr = NULL);
	virtual void ActiveOpen(void);
	virtual void ActiveOpen(const TDesC8 &aConnectionData);
	virtual TInt PassiveOpen(TUint aQue);
	virtual TInt PassiveOpen(TUint aQue,const TDesC8 &aConnectionData);
	virtual void AutoBind( void );
	virtual void Shutdown(TCloseType option);
	virtual void Shutdown(TCloseType option,const TDesC8 &aDisconnectData);
	virtual void Start();
	
	virtual	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);
	void DestroyAsyncErrorer();

private:
	class CFlowOnTimer;
	friend class CDummyProvd::CFlowOnTimer;

    static TInt NoBearerCB(TAny* aArg);
	void ConstructL();
	TBool IsTransportType(TUint aType);
	CDummyProvd(CDummyProtocol &aProtocol);
	void HangModule(void);
	TInt SetBlockedConnect(const TDesC8 &aOption);
	TInt OptToErr(const TDesC8 &aOption, TInt &aRes);
	TInt GobbleMBufs();
	void FreeMBufs();
	void FreeMBufs(TUint aNumber);

private:
	CArrayFixFlat<RMBufChain>* iDataArray;
	TBool iIsBound;
	CDummyProtocol *iProtocol;
	TSockAddr iAddr;
	CAsyncSocketErrorer* iAsyncErrorer;
	TBool iInputStopped;
	TInt iListenErrorCode;
	TBool iCancelIoctl; // Set when an IOCTL has been cancelled.
	
	TBool iBlockOnClose; //Used by and test Ioctl to cause the socket to hang on close
	TBool iCompleteIoctl;
	TBool iConnectCompleteState; //Whether connects should be completes or not;
	TBool iConnectPending; //Whether a connect is awaiting completion; 
	RBuf8 iListenData;
	TInt iErrorForNextWrite; //Whether write should send an error if == KErrNone the operates normally
	TInt iErrorForNextShutdown; //Whether write should send an error if == KErrNone the operates normally
	TInt iErrorForNextConnect; //Whether write should send an error if == KErrNone the operates normally

	TBool iFlowOffWrite;
	TInt iFlowOffWriteTimeout;
	CFlowOnTimer* iFlowOnTimer;
	RMBufChain iChain;
	TBool iErrNoMBufs;
	CAsyncCallBack iNoBearerCB;
	};

//---------------------------------------------------------------------------------------------------------

class CDNSQueryProcessor;
NONSHARABLE_CLASS(CDatagramHostResolver): public CHostResolvProvdBase
	{
private:
	CDatagramHostResolver();
public:
	static CDatagramHostResolver* NewL();
	void   ConstructL();
           ~CDatagramHostResolver();

    virtual void GetByName(TNameRecord &iName);
	virtual void GetByAddress(TNameRecord &iName);
	virtual void SetHostName(TDes &aNameBuf);
	virtual void GetHostName(TDes &aNameBuf);
	virtual TInt SetOption(TUint level,TUint name,const TDesC8 &anOption);
	virtual void CancelCurrentOperation();

    virtual void Query(const TDesC8& aQryBuf, TDes8& aResBuf, TInt aCounter);
	virtual	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);
private:
    
    CDNSQueryProcessor*  ipDNSQueryProcessor; //< simulates DNS queries processing

    };

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDatagramServResolver) : public CServiceResolvProvdBase
	{
private:
	CDatagramServResolver();
public:
	static  CDatagramServResolver* NewL();
	virtual void GetByName(const TDesC &aNameBuf,TInt32 &aPortNum);
	virtual void GetByNumber(TDes &aNameBuf,TInt32 aPortNum);
	virtual void RegisterService(const TDesC &aNameBuf,TInt32 aPortNum);
	virtual void RemoveService(const TDesC &aNameBuf,TInt32 aPortNum);
	virtual void CancelCurrentOperation();
	virtual	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);
	};

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDatagramNetDataBase) : public  CNetDBProvdBase
	{
public:
	virtual void Query(TDes8& aBuffer);
	virtual void Add(TDes8& aBuffer);
	virtual void Remove(TDes8& aBuffer);
	virtual void CancelCurrentOperation();
	static CDatagramNetDataBase* NewL();
	virtual	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);
private:
	CDatagramNetDataBase();
	TInt iCount;
	};

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CAsyncSocketErrorer) : public CActive
	{
public:
	static CAsyncSocketErrorer* NewL(MSocketNotify& aSocket,TInt aErrorCode,TUint aOpMask,
                                      CDummyProtocol * apProtocols, CDummyProvd* aOwner, const TDesC8* aListenData);
	~CAsyncSocketErrorer();
private:
	CAsyncSocketErrorer(MSocketNotify& aSocket,TInt aErrorCode,TUint aOpMask,CDummyProtocol * apProtocols, CDummyProvd* aOwner, const TDesC8* aListenData = NULL);
	void RunL();
	void DoCancel();
	MSocketNotify& iSocket;
	TInt iErrorCode;
	TUint iOpMask;
    CDummyProtocol* ipProtocols;
	CDummyProvd* iOwner;
	const TDesC8* iListenData;
	};


//---------------------------------------------------------------------------------------------------------

/**
* This class simulates DNS query processing.
* It checks incoming query data, forges DNS query responses
* and simulates processing by generating random delays.
* @see CDatagramHostResolver
*/
NONSHARABLE_CLASS(CDNSQueryProcessor) : public CActive
{
public:
   
   static   CDNSQueryProcessor* NewL(); 
           ~CDNSQueryProcessor();
    void    SetNotifier(MResolverNotify *aNotifier) {iDnsNotifier = aNotifier;}
   
    void    ProcessQuery(const TDnsQuery& aQry, TDes8& aResBuf, TInt aCounter);
    

private:    
    CDNSQueryProcessor();

    void    RunL();
    void    DoCancel();
    void    ConstructL(void);
    
    void    CompleteImmediately();
    void    CompleteImmediately(TInt aResultCode);

    void    SetQryProcessDelay();
    void    SetCompletionCode(TInt aResultCode) {iCompleteResult = aResultCode;}

    //-- queries processors
    void    QryProcessA     (const TDnsQuery& aQry, TDnsRespA&      aQryResult, TInt aCounter);
    void    QryProcessSRV   (const TDnsQuery& aQry, TDnsRespSRV&    aQryResult, TInt aCounter);
    void    QryProcessPTR   (const TDnsQuery& aQry, TDnsRespPTR&    aQryResult, TInt aCounter);
    void    QryProcessNAPTR (const TDnsQuery& aQry, TDnsRespNAPTR&  aQryResult, TInt aCounter);
    void    QryProcessMX    (const TDnsQuery& aQry, TDnsRespMX&     aQryResult, TInt aCounter);
    void    QryProcessAAAA  (const TDnsQuery& aQry, TDnsRespAAAA&   aQryResult, TInt aCounter);

private:    

    RTimer              iDelayTimer;        //< generates delays simulating queries completion
    MResolverNotify    *iDnsNotifier;       //< notifier, QueryComplete will be called  
    TInt                iCompleteResult;    //< Query completion result
    TUint16             iCurrQryType;       //< current query type, used by QueryGetNext
    TInt64              iRndSeed;           //< random seed, for delays
};





#endif //__PDUMMY_H__

