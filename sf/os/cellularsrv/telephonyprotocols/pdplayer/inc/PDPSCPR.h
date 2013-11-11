// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// PDP SubConnection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPSPR_H
#define SYMBIAN_PDPSPR_H

#include <etelqos.h>
#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>
#include <networking/qos3gpp_subconparams.h>
#include <cs_subconparams.h>
#include <networking/pdpprovision.h>
#include <comms-infras/linkprovision.h>
#include <networking/ipaddrinfoparams.h>
#include <comms-infras/commsdebugutility.h>

#include "PDPFSM.h"
#include "PDPParamMapper.h"

namespace PDPSCprStates
    {
    class TSelfInit;
    class TCreateSecondaryOrMbmsPDPCtx;
    class TCreatePrimaryPDPCtx;
    class TActivatePDPContext;
    class TSetQoS;
    class TSetTFT;
    class TRaiseParamsChanged;
    class TRaiseParamsRejectedL;
    class TParamsEvent;
	class TFillInImsExtParams;
    class TFillInGrantedParams;
    class TNoTagIfRejoinNeededOrDataClientReady;
    class TAwaitingPDPContextDestroyed;
    class TNoTagOrProviderStopped;
    class TSendDataClientStarted;
    class TSendDataClientStopped;
    class TApplyNewDefault;
    class TRetrieveSipAddr;
    class TModifyActivePDPContext;
	class TSetMbmsParameters;
	class TCleanupFSM;
    }

namespace PDPSCprActivities
    {
    DECLARE_ACTIVITY_MAP(activityMap)
    }
class CPDPDefaultSubConnectionProvider;
class CPDPSubConnectionProviderFactory;
class CPDPSubConnectionProvider : public CCoreSubConnectionProvider, public MPdpFsmEventHandler
/** Common subclass for default and non-default PDP subconnection providers.

@internalTechnology
@released Since 9.4 */
    {
    friend class CPDPSubConnectionProviderFactory;
    friend class PDPSCprStates::TSelfInit;
    friend class PDPSCprStates::TCreateSecondaryOrMbmsPDPCtx;
    friend class PDPSCprStates::TCreatePrimaryPDPCtx;
    friend class PDPSCprStates::TActivatePDPContext;
    friend class PDPSCprStates::TSetQoS;
    friend class PDPSCprStates::TSetTFT;
    friend class PDPSCprStates::TRaiseParamsChanged;
    friend class PDPSCprStates::TRaiseParamsRejectedL;
    friend class PDPSCprStates::TParamsEvent;
    friend class PDPSCprStates::TFillInGrantedParams;
    friend class PDPSCprStates::TFillInImsExtParams;
    friend class PDPSCprStates::TNoTagIfRejoinNeededOrDataClientReady;
    friend class PDPSCprStates::TAwaitingPDPContextDestroyed;
    friend class PDPSCprStates::TNoTagOrProviderStopped;
    friend class PDPSCprStates::TSendDataClientStarted;
    friend class PDPSCprStates::TSendDataClientStopped;
    friend class PDPSCprStates::TApplyNewDefault;
    friend class PDPSCprStates::TRetrieveSipAddr;
	friend class PDPSCprStates::TSetMbmsParameters;
    friend class PDPSCprStates::TModifyActivePDPContext;
    friend class PDPSCprStates::TCleanupFSM;


public:
	enum { EInvalidContextId = KMaxPdpContexts };
    typedef CPDPSubConnectionProviderFactory FactoryType;
	virtual ~CPDPSubConnectionProvider();

    //-====================================
    //MPdpFsmEventHandler methods
    //-====================================
	virtual void Event(TInt aEvent, TInt aParam);

    //-====================================
    //ACFNode overrides
    //-====================================
    void Received(MeshMachine::TNodeContextBase& aContext);
    void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

    virtual void LinkUp();
    virtual void LinkDown(TInt aCause);

    CSubConQosR99ParamSet* GrantedQoS();
    TInt QoSRank();

	TBool ContentionRequested() const 
		{
		return iContentionRequested;	
		}
	void SetContentionRequested(TBool aContentionRequested)
		{
		iContentionRequested = aContentionRequested;
		}

protected:
    //-====================================
    //Construction methods bundle - accessible thru the factory only
    //-====================================
    CPDPSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);
    CPDPSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory,
                                   const MeshMachine::TNodeActivityMap& aActivityMap);
	static CPDPSubConnectionProvider* NewL(CPDPSubConnectionProviderFactory& aFactory);
	void ConstructL();

private:
	TTFTInfo GetTftInfoL(CSubConIPAddressInfoParamSet* aParamSet);
	TTFTOperationCode GetOperationCodeL(CSubConIPAddressInfoParamSet* aParamSet);
	TUint FindPacketFilterIdL(CSubConIPAddressInfoParamSet::TSubConIPAddressInfo aParamInfo);
	TUint FindIdOfMatchingParamSetL(CSubConIPAddressInfoParamSet::TSubConIPAddressInfo aParamInfo);
	void NewPacketFilterAddedL(CSubConIPAddressInfoParamSet::TSubConIPAddressInfo aParamInfo, TUint aId);
	void PacketFilterRemovedL(TUint aId);

public:
    //-====================================
    //PDP Ctx manipulation
    //-====================================
	TContextId iPDPFsmContextId;
	SpudMan::TPdpContextType iContextType; //maintain type of subconnection provider
    CPdpFsmInterface* iPdpFsmInterface;
    CPDPDefaultSubConnectionProvider* iDefaultSCPR;
    TInt iActivityAwaitingResponse;
    MPDPParamMapper::TQosParameterRelease iParamsRelease;

    //quite a big structure so kept here to avoid stack overflow.
    RPacketContext::TDataChannelV2 iTempDataChannelV2;
    TInt  iProvisionFailure;
protected:
    __FLOG_DECLARATION_MEMBER;
    TInt iLinkUps;

private:
	// keeps record of assigned Id's for packetFilter
	RArray<TUint> iPacketFilterId;
	TUint8 iPacketFilterMaskId;
	TBool iIsModeGsm;
	TBool iContentionRequested; 
    };

class THighestQoSQuery :  public Factories::MFactoryQuery
/** Query scanning for the CPDPSubConnectionProvider with the highest QoS as per
    3GPP TS 23.107 Annex C.
    Usage note: Use against CPDPSubConnectionProviderFactory::Find, which will
    always return NULL (the query needs to traverse all the providers). Examine
    THighestQoSQuery::HighestQoSProvider() after calling Find to get the highest
    QoS provider.

@internalTechnology
@released Since 9.5 */
    {
    public:
    THighestQoSQuery(Messages::TNodeId aCtrlProvider)
    :iCtrlProvider(aCtrlProvider),
     iHighestQoSProvider(NULL)
     {}

    CPDPSubConnectionProvider* HighestQoSProvider()
        {
        return iHighestQoSProvider;
        }

    protected:
	virtual TMatchResult Match(Factories::TFactoryObjectInfo& aObjectInfo);

	private:
	Messages::TNodeId iCtrlProvider;
	CPDPSubConnectionProvider* iHighestQoSProvider;
    };

#endif //SYMBIAN_PDPSPR_H
