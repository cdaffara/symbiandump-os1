// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#if !defined(__BINDMGR_H__)
#define __BINDMGR_H__

#include <cfshared.h>
#include <commschan.h>
using namespace CommsFW;
#include <rscommon.h>
using namespace RootServer;
#include <cfshared.h>
#include <cfmsgs.h>


// Uncomment this line to enable absurdly detailed logging
#define RS_DETLOG __CFLOG_VAR
#ifndef RS_DETLOG
#define RS_DETLOG(s)
#endif

// general bind manager constants

/** Defines the default number of elements in the sub-module array passed to a
server or CPM in a Discovery request.
 */
const TUint KDefaultSubmoduleArraySize = 4;

/** The time given to servers and CPMs to respond to messages before a job times
out and an error is reported to the binding manager client.
 */
const TUint KJobTimeoutMicroseconds = 60000000;

// forward declarations
class CModule;
class CJob;
class CBindInfo;
class CTask;

class MRootServerNotify;

class MBindManagerNotify
/** Mixin abstract interface class providing the up-call interfaces into the
BindManager from its internal components.
 */
    {
public:
    virtual void TaskCompleted(CModule &aModule,
                               const CTask &aTask,
                               TInt aStatus)=0;
    virtual void ReceivedMessage(const TCFModuleNameF& aModule,
                                 const TCFMessage& aNewMsg)=0;
    virtual void JobTimedOut(const TCFModuleNameF &aModuleName,
                             const TUint aTaskId)=0;
    };

class TransactionId
/** Generates ID numbers for messages that expect a responce message.
 */
    {
public:
    TransactionId(): iId(0x10000)
        {
        }
    TId GetId()
        {
        return (TId) ((iId++)? iId: 1);
        }
private:
    TInt iId;
    };


class CPersistentSubModuleAddressCon;

class CPersistentSubModuleAddress: public CBase
/* represents a submodule address intended to be made persistent as long as at least one client
 still references it */
    {
public:

    CPersistentSubModuleAddress(const TCFSubModuleAddress& aSubAddr,CPersistentSubModuleAddressCon* aContainerRef);
    // aContainerRef - the container that will store this object

    static CPersistentSubModuleAddress* NewL(const TCFSubModuleAddress& aSubAddr, CPersistentSubModuleAddressCon* aContainerRef);

    inline TCFSubModuleAddress& SubAddrRef();

    inline void Open();

    void Close(TInt aIndex = -1);

private:
    TCFSubModuleAddress iSubAddr;
    CPersistentSubModuleAddressCon* iContainerRef; // needed so that we can remove ourselves during Close op if this is called from cleanup stack unwind
    TInt iAccessCount;      //< Reference counting on this module addr. If it reaches 0 this object will be destroyed.
    };



class CPersistentSubModuleAddressCon : public CBase
/* Container to persist the moduleAddrs held within a module that may be lost on module (thread) death but
 possibly still required post-mortem. In particular, module name strings used by bind manager.
 */
    {
public:
    friend class CPersistentSubModuleAddress; // so their Close can remove items

    static CPersistentSubModuleAddressCon* NewL();

    ~CPersistentSubModuleAddressCon();

    TCFSubModuleAddress& SetOrRetrieveNameLC(const TCFSubModuleAddress& aRef);

    void Remove(const TCFSubModuleAddress& aRef);

private:
    RPointerArray<CPersistentSubModuleAddress> iPersistentModuleAddresses;
    };



class CBindManager : public CBase, public MBindManagerNotify
    {
/** Main singleton class providing Bind Manager functionality to the rootserver. This
class handles bind and unbind messaging through comms framework queues
and interfaces with the RootServer using asynch TReqestStatus objects.
 */
public:
    // NOTE: the relative ordering of the TBMBindState enums is relied upon by code
    enum TBMBindState
        {
        EBindInProgress,
        EBindError,
        EBound,
        EUnbindInProgress,
        EUnbindError,
        EUnbound    // NOTE: code relies upon this being the final value
        };

    class TBindingInfo
        {
    public:
        TRSBindType iType;
        TCFSubModuleAddress iSubModuleAddr1;
        TCFSubModuleAddress iSubModuleAddr2;
        TBMBindState iSubModuleState1;
        TBMBindState iSubModuleState2;
        TInt iSubModuleResult1;
        TInt iSubModuleResult2;
        };

    typedef RArray<TBindingInfo> RBindingInfoArray;

    static CBindManager* NewL(MRootServerNotify& aRootServerNotifier);
    void CreateModuleL(TRequestStatus& aStatus,
                       const TCFModuleNameF& aModule,
                       const RCFChannel::TMsgQueues& aInputQueues,
                       const RCFChannel::TMsgQueues& aOutputQueues);

    void UnbindAllAndShutDownL(TRequestStatus& aStatus,
                                        const TCFModuleNameF& aModule, TCFShutdownType aType);
    void ModuleEndedL(const TCFModuleNameF& aModule);
    TInt Cancel(TRequestStatus& aStatus);
    TInt EnumerateSubModules(const TCFModuleNameF& aModuleName, TInt aPosition, TCFSubModuleNameF& aName);

    TInt NumSubModules( const TCFModuleNameF& aName, TInt& aNumSubs );

    virtual ~CBindManager();

    virtual void TaskCompleted(CModule &aModule,
                               const CTask &aTask,
                               TInt aStatus);
    virtual void ReceivedMessage(const TCFModuleNameF& aModule,
                                 const TCFMessage& aNewMsg);
    virtual void JobTimedOut(const TCFModuleNameF &aModuleName,
                             const TUint aTaskId);
    void SendL( const TCFModuleNameF& aModule, const RMessage2& aMessage );

	void BindSubmodulesL(TRequestStatus& aStatus,
						 const TRSBindType aType,
						 const TCFSubModuleAddress& aUpperSubmodule,
						 const TCFSubModuleAddress& aLowerSubmodule,
						 TInt aForwardQLength,
						 TInt aReverseQLength);
	void UnbindSubmodulesL(TRequestStatus& aStatus,
								 	const TCFSubModuleAddress& aUpperSubmodule,
								 	const TCFSubModuleAddress& aLowerSubmodule);
	void UnbindAlreadyDeadL(TRequestStatus& aStatus,
									 const TCFModuleNameF& aModule);
	TInt EnumerateBindings(const TCFSubModuleAddress& aSubModule, TBool aRestart, TBindingInfo& aInfo);
private:
    CBindManager();

    CModule* GetModulePtrL(const TCFModuleNameF& aModule);

    CModule* GetModulePtrL(const TCFSubModuleAddress& aSubModuleAddr);

    void UnbindAllModuleL(TRequestStatus& aStatus,
                          const TCFModuleNameF& aModule,
                          const TBool aModuleIsDead,
                          CJob*& pJob);

    static void ConvertRSBindType(const TRSBindType aType,
                                  TCFBindType& aUpperBindType,
                                  TCFBindType& aLowerBindType);

	CBindInfo* FindBindInfo(const TCFSubModuleAddress& aUpperSubmodule,
							const TCFSubModuleAddress& aLowerSubmodule);

    void ConstructL();

private:
    MRootServerNotify *iNotify;
    TDblQue<CModule> iModuleList;
    TDblQue<CJob> iJobList;
    TDblQue<CBindInfo> iBindList;
    TransactionId iTransactionId;

    /** Index for enumeration of bindings */
    TInt iEnumBindingsIdx;
    CPersistentSubModuleAddressCon* iModuleNameStor; // persist module names for messages since the msgs may outlive modules

    };



inline TCFSubModuleAddress& CPersistentSubModuleAddress::SubAddrRef()
    {
    return iSubAddr;
    }

inline void CPersistentSubModuleAddress::Open()
    {
    ++iAccessCount;
    }

#endif // __BINDMGR_H__

