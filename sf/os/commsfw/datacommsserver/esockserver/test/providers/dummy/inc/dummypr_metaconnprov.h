// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dummy MCPR
//
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_DUMMYPR_METACONNPROV_H
#define SYMBIAN_DUMMYPR_METACONNPROV_H

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/mobilitymcpr.h>

class CMCPrPubSubAvailability;
class CMCPrPubSubStopTrigger;

struct TCprConfigModifier
	{
	enum
	/*
	Bits on the CprConfig.
	This is a very naive way of instructing the dummy providers to respond to
	various scenarios in a varying way. This needs to be retought when migrating away
	from NS.
	*/
		{
		EDontStart = 0,					  //when lit on CprConfig, the CPR will fail to start
		ESCPRHangOnRebind = 1,			  //when lit on CprConfig, the SCPR will hang on rebind
		ESCPRHangOnStart  = 2,			  //when lit on CprConfig, the SCPR will hang on start
		ESCPRFailOnRebind = 3,            //when lit on CprConfig, the SCPR will fail on rebind
		ECPRWaitOnThreeSecDestroy = 4,   //when lit on CprConfig, the CPR will wait for 3 sec on destroy before resuming
		};

	static TBool Is(TInt aValue, TInt aFlags)
	    {
	    aFlags = (1 << aFlags);
	    return aValue & aFlags;
	    }
	};

class CDummyMetaConnectionProvider : public CMobilityMetaConnectionProvider
    {
friend class CDummyMetaConnectionProviderFactory;
protected:
    static CDummyMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory,
	                                            const ESock::TProviderInfo& aProviderInfo);

    CDummyMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                 const ESock::TProviderInfo& aProviderInfo,
                                 const MeshMachine::TNodeActivityMap& aActivityMap);

	virtual ~CDummyMetaConnectionProvider();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	virtual void ConstructL();

	virtual void StartAvailabilityMonitoringL(const Messages::TNodeCtxId& aAvailabilityActivity); //Default implementation, usually empty on the mid-tier MCprs
    virtual void CancelAvailabilityMonitoring(); //Default implementation, usually empty on the mid-tier MCprs

    CMCPrPubSubAvailability* iPubSubAvailability;
    CMCPrPubSubStopTrigger* iPubSubStopTrigger;
    };


class CDummyMCPRControlClientJoinActivity : public MeshMachine::CNodeParallelActivityBase
    {
public:
    static MeshMachine::CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
    void ReplaceOriginator(Messages::RNodeInterface& aOriginator);
protected:
    CDummyMCPRControlClientJoinActivity( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount )
    :MeshMachine::CNodeParallelActivityBase( aActivitySig, aNode, aNextActivityCount )
     {};
     void Cancel(MeshMachine::TNodeContextBase& /*aContext*/) 
         {
         //CDummyMCPRControlClientJoinActivity ignores TCancels, for it's easier than actually handling cancelations. Handling cancelations in join scenarios is not practically
         //interesting as joins are normally very swift. This join isn't swift as it's been modified to artificially yield (again easier to yield in Join than elsewhere)
         };
     
private:
    ~CDummyMCPRControlClientJoinActivity() 
        {
        };
public:
    typedef MeshMachine::TNodeContext<CDummyMetaConnectionProvider,PRStates::TContext, CDummyMCPRControlClientJoinActivity> TContext;
    DECLARE_SMELEMENT_HEADER( TAddControlClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TAddControlClient )
    
    DECLARE_SMELEMENT_HEADER( TSendJoinComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendJoinComplete )
    
    friend class TAddControlClient;    
    };


#endif //SYMBIAN_DUMMYPR_METACONNPROV_H

