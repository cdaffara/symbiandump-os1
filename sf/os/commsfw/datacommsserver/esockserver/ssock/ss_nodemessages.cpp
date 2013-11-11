// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#define SYMBIAN_NETWORKING_UPS
#include "ss_nodemessages.h"


#include <comms-infras/ss_log.h>
#include <ss_fact.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include "ss_flowrequest.h"
#include <comms-infras/ss_corepractivities.h>
#include "ss_transporthook.h"
#include <elements/metatype.h>
#include <elements/sm_core.h>
#include <ss_glob.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_commsdataobject.h>
#include <elements/interfacetable.h>
#include <es_panic.h>
#include <es_sock.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_protflow.h>

#include <elements/cftransportmsg.h>
#include <elements/nm_messages_child.h>
#include <addressupdate.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_panic_internal.h>
#endif

#include "ss_nodemessages_dataclient.h"
#include <comms-infras/ss_nodemessages_internal.h>

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_upsaccesspointconfigext.h>
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksndmsg, "ESockSSocksndmsg");
#endif


using namespace ESock;
using namespace Elements;
using namespace Meta;
using namespace Messages;

// Ugly hack: mingw does not evaulate macros early enough to pass as template arguments so we hard code the
// value for the x86gcc platform.  A compile time assertion ensures that the value stays correct.
#ifdef __X86GCC__
	#define KOffsetTSigUidFactoryQuery_iQuery	24
#else
	#define KOffsetTSigUidFactoryQuery_iQuery	( _FOFF(TSigPlayerRoleUidFactoryQuery, iQuery) )
#endif

__ASSERT_COMPILE( KOffsetTSigUidFactoryQuery_iQuery == _FOFF(TSigPlayerRoleUidFactoryQuery, iQuery) );

using namespace Factories;

//We can safely reuse this UID for singature's TIDs (but they mean different things)
const TInt KESockInternalMessagesImplementationUid = TCFMessage::ERealmId;

enum EMessageTypeId //message signatures only (NOT messages) messages are declared under class TCFMessage
	{
	//  spare                       =1,
    //	spare						=2,
	ESignatureSigAddrUpdate			=3,
	ESignatureConnectionInfo		=4,
	ESignatureProvisionConfig		=5,
	ESignatureSelectionPrefs		=6,
	ESignatureSelectComplete		=7,
	ESignaturePlaneNotification		=8,

	ESignatureSubConnNotification	=9,
	ESignatureSubConnOpenedEvent	=10,
	ESignatureSubConnClosedEvent	=11,
	ESignatureStateChange			=12,

	ESignatureUidFactoryQuery		=13,
	ESignatureBundleRMessage2		=14,
	ESignatureBundle				=15,
	ESignatureAvailabilitySubscriptionOptions = 16,
	ESignatureAvailabilityStatus    = 17,
	ESignatureNodeIdClientTypeNumber = 18,

//	spare							=37,
//	spare							=38,
//	spare							=39,

	//ESignatureNumberSubConnParams	=44,

	ESignatureFlowParams				=45,
	ESignatureConnPrefList 				= 46,
	ESignatureUpsParameters 			= 47,	
	ESignatureFamilyBundle			=48,
	ESignatureNumberFamilyBundle	=49,
	ESignatureUpsStatusChange 		=50,
	ESignatureMetaExtContainer      =51,
	ESignatureConnectionInfoPtr     =52,

	EFlowRequestBase				=1000,
	EImplicitFlowRequest			=1001,
	EConnFlowRequest				=1002,
	ESubConnFlowRequest				=1003,
	EReBindLayerRequest				=1004,
	EDataClientJoiningRequest       =1005,
	EGetOrSetParameters				=1006,
	EAlwaysFindFactoryQuery			=1007,
	EMetaConnectionFactoryQuery		=1008,
	EDefaultConnectionFactoryQuery	=1009,	
	EDefaultSCPRFactoryQuery		=1010,
	EDefaultFlowFactoryQuery		=1011,
	EDefaultProtocolIntfFactoryQuery =1012,	
	ETierTypeIdFactoryQuery     =1013,
	};

//no need to serialise return value
EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigSubConNotification, KESockInternalMessagesImplementationUid, ESignatureSubConnNotification )
	REGISTER_ATTRIBUTE( TSigSubConNotification, iRefCountOwnedSubConNotification, TMeta<CRefCountOwnedSubConNotification*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigPlaneNotification, KESockInternalMessagesImplementationUid, ESignaturePlaneNotification )
	REGISTER_ATTRIBUTE( TSigPlaneNotification, iRefCountOwnedNotification, TMeta<CRefCountOwnedNotification*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigSubConnOpenedEvent, KESockInternalMessagesImplementationUid, ESignatureSubConnOpenedEvent )
	REGISTER_ATTRIBUTE( TSigSubConnOpenedEvent, iSubConnectionOpenedEvent, TMeta<TSubConnectionOpenedEvent> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigSubConnClosedEvent, KESockInternalMessagesImplementationUid, ESignatureSubConnClosedEvent )
	REGISTER_ATTRIBUTE( TSigSubConnClosedEvent, iSubConnectionClosedEvent, TMeta<TSubConnectionClosedEvent> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigPlayerRoleUidFactoryQuery, KESockInternalMessagesImplementationUid, ESignatureUidFactoryQuery )
	REGISTER_ATTRIBUTE(TSigPlayerRoleUidFactoryQuery, iQuery, TMetaHandler )
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigAddrUpdate, KESockInternalMessagesImplementationUid, ESignatureSigAddrUpdate )
	REGISTER_ATTRIBUTE( TSigAddrUpdate, iAddrUpdate, TMeta<TAddrUpdate> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigConnectionInfo, KESockInternalMessagesImplementationUid, ESignatureConnectionInfo )
	REGISTER_ATTRIBUTE( TSigConnectionInfo, iConnectionInfo, TMeta<TConnectionInfo> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigSelectionPrefs, KESockInternalMessagesImplementationUid, ESignatureSelectionPrefs )
	REGISTER_ATTRIBUTE( TSigSelectionPrefs, iSelectionPrefs, TMetaObject<TSelectionPrefs> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigConnPrefList, KESockInternalMessagesImplementationUid, ESignatureConnPrefList)
	REGISTER_ATTRIBUTE( TSigConnPrefList, iConnPrefList, TMeta<RConnPrefList> )
END_ATTRIBUTE_TABLE_BASE( Messages::TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigSelectComplete, KESockInternalMessagesImplementationUid, ESignatureSelectComplete )
	REGISTER_ATTRIBUTE( TSigSelectComplete, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigSelectComplete, iProviderInfo, TMeta<TProviderInfo> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigBundleRMessage2, KESockInternalMessagesImplementationUid, ESignatureBundleRMessage2 )
	REGISTER_ATTRIBUTE( TSigBundleRMessage2, iBundle, TMeta<TAny*> )
	REGISTER_ATTRIBUTE( TSigBundleRMessage2, iMessage, TMeta<RMessage2*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigBundle, KESockInternalMessagesImplementationUid, ESignatureBundle )
	REGISTER_ATTRIBUTE( TSigBundle, iBundle, TMeta<TAny*> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigFlowParams, KESockInternalMessagesImplementationUid, ESignatureFlowParams )
	REGISTER_ATTRIBUTE( TSigFlowParams, iFlowParams, TMeta<TFlowParams> )
END_ATTRIBUTE_TABLE_BASE( Messages::TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigAvailabilitySubscriptionOptions, KESockInternalMessagesImplementationUid, ESignatureAvailabilitySubscriptionOptions )
	REGISTER_ATTRIBUTE( TSigAvailabilitySubscriptionOptions, iAvailabilitySubscriptionOptions, TMeta<TAvailabilitySubscriptionOptions> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigAvailabilityStatus, KESockInternalMessagesImplementationUid, ESignatureAvailabilityStatus )
	REGISTER_ATTRIBUTE( TSigAvailabilityStatus, iAvailabilityStatus, TMeta<TAvailabilityStatus> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNodeIdClientTypeNumber, KESockInternalMessagesImplementationUid, ESignatureNodeIdClientTypeNumber )
	REGISTER_ATTRIBUTE( TSigNodeIdClientTypeNumber, iNodeId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( TSigNodeIdClientTypeNumber, iClientType, TMeta<TClientType> )
	REGISTER_ATTRIBUTE( TSigNodeIdClientTypeNumber, iValue, TMeta<TUint> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigFamilyBundle, KESockInternalMessagesImplementationUid, ESignatureFamilyBundle )
	REGISTER_ATTRIBUTE( TSigFamilyBundle, iFamilyBundle, TMeta<RCFParameterFamilyBundleC> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberFamilyBundle, KESockInternalMessagesImplementationUid, ESignatureNumberFamilyBundle )
	REGISTER_ATTRIBUTE( TSigNumberFamilyBundle, iValue, TMetaNumber )
	REGISTER_ATTRIBUTE( TSigNumberFamilyBundle, iFamilyBundle, TMeta<RCFParameterFamilyBundleC> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureVoid )

START_ATTRIBUTE_TABLE( TCFFlowRequestBase, KESockInternalMessagesImplementationUid, EFlowRequestBase )
	REGISTER_ATTRIBUTE( TCFFlowRequestBase, iSubSessionUniqueId, TMetaNumber )
 	REGISTER_ATTRIBUTE( TCFFlowRequestBase, iFlowParams, TMeta<TFlowParams> )	
END_ATTRIBUTE_TABLE_BASE( TSignalBase, ESignalBase )

START_ATTRIBUTE_TABLE( TCFImplicitFlowRequest, KESockInternalMessagesImplementationUid, EImplicitFlowRequest )
END_ATTRIBUTE_TABLE_BASE( TCFFlowRequestBase, EFlowRequestBase )

START_ATTRIBUTE_TABLE( TCFConnFlowRequest, KESockInternalMessagesImplementationUid, EConnFlowRequest )
	REGISTER_ATTRIBUTE( TCFConnFlowRequest, iSession, TMeta<TAny*> )
	REGISTER_ATTRIBUTE( TCFConnFlowRequest, iHandle, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TCFFlowRequestBase, EFlowRequestBase )

START_ATTRIBUTE_TABLE( TCFSubConnFlowRequest, KESockInternalMessagesImplementationUid, ESubConnFlowRequest )
	REGISTER_ATTRIBUTE( TCFSubConnFlowRequest, iSession, TMeta<TAny*> )
	REGISTER_ATTRIBUTE( TCFSubConnFlowRequest, iHandle, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( TCFFlowRequestBase, EFlowRequestBase )

START_ATTRIBUTE_TABLE( PRActivities::CRejoinDataClientActivity::TCFDataClientJoiningRequest, KESockInternalMessagesImplementationUid, EDataClientJoiningRequest )
	REGISTER_ATTRIBUTE( PRActivities::CRejoinDataClientActivity::TCFDataClientJoiningRequest, iDataClient, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE( PRActivities::CRejoinDataClientActivity::TCFDataClientJoiningRequest, iDataClientType, TMeta<TClientType> )
END_ATTRIBUTE_TABLE_BASE( TNodeSignal, ENodeSignal )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigStateChange, KESockInternalMessagesImplementationUid, ESignatureStateChange )
	REGISTER_ATTRIBUTE( TSigStateChange, iStateChange, TMeta<TStateChange> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

#ifdef SYMBIAN_NETWORKING_UPS
EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigUpsParameters, KESockInternalMessagesImplementationUid, ESignatureUpsParameters )
	REGISTER_ATTRIBUTE( TSigUpsParameters, iParams, TMeta<TPolicyCheckRequestParams> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigUpsStatusChangeParameters, KESockInternalMessagesImplementationUid, ESignatureUpsStatusChange )
	REGISTER_ATTRIBUTE( TSigUpsStatusChangeParameters, iParams, TMeta<TUPSStatusChangeParams> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(CUPSAccessPointConfigExt, CUPSAccessPointConfigExt::EUPSAccessPointConfigUid, CUPSAccessPointConfigExt::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()
#endif //SYMBIAN_NETWORKING_UPS

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TAlwaysFindFactoryQuery, KESockInternalMessagesImplementationUid, EAlwaysFindFactoryQuery)	
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TMetaConnectionFactoryQuery, KESockInternalMessagesImplementationUid, EMetaConnectionFactoryQuery)	
	REGISTER_ATTRIBUTE(TMetaConnectionFactoryQuery, iProviderInfo, TMeta<TProviderInfo>)
	REGISTER_ATTRIBUTE(TMetaConnectionFactoryQuery, iTierImplUid, TMeta<TUint>)  
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TTierTypeIdFactoryQuery, KESockInternalMessagesImplementationUid, ETierTypeIdFactoryQuery)  
    REGISTER_ATTRIBUTE(TTierTypeIdFactoryQuery, iTierTypeId, TMeta<TUint>)  
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDefaultConnectionFactoryQuery, KESockInternalMessagesImplementationUid, EDefaultConnectionFactoryQuery)	
	REGISTER_ATTRIBUTE(TDefaultConnectionFactoryQuery, iMCprId, TMeta<TNodeId>)	
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDefaultSCPRFactoryQuery, KESockInternalMessagesImplementationUid, EDefaultSCPRFactoryQuery)	
	REGISTER_ATTRIBUTE(TDefaultSCPRFactoryQuery, iControlProvider, TMeta<TNodeId>)
	REGISTER_ATTRIBUTE(TDefaultSCPRFactoryQuery, iSCPRType, TMeta<TSubConnOpen::TSubConnType>)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDefaultFlowFactoryQuery, KESockInternalMessagesImplementationUid, EDefaultFlowFactoryQuery)	
	REGISTER_ATTRIBUTE(TDefaultFlowFactoryQuery, iCprId, TMeta<TNodeId>)
	REGISTER_ATTRIBUTE(TDefaultFlowFactoryQuery, iSCprId, TMeta<TNodeId>)
	REGISTER_ATTRIBUTE(TDefaultFlowFactoryQuery, iMessageId, TMeta<TNodeSignal::TMessageId>)
	REGISTER_ATTRIBUTE(TDefaultFlowFactoryQuery, iProtocolType, TMeta<TSubConnOpen::TSubConnType>)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDefaultProtocolIntfFactoryQuery, KESockInternalMessagesImplementationUid, EDefaultProtocolIntfFactoryQuery)	
	REGISTER_ATTRIBUTE(TDefaultProtocolIntfFactoryQuery, iCprId, TMeta<TNodeId>)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigMetaExtContainer, KESockInternalMessagesImplementationUid, ESignatureMetaExtContainer )
	{ _FOFF( TSigMetaExtContainer, iConfig ), ESock::TMetaExtensionContainerMetaType::NewL },
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigConnectionInfoPtr, KESockInternalMessagesImplementationUid, ESignatureConnectionInfoPtr )
    REGISTER_ATTRIBUTE( TSigConnectionInfoPtr, iPtr, TMeta<CConnectionInfo*> )
END_ATTRIBUTE_TABLE_BASE( Messages::TSignatureBase, ESignatureBase )


DEFINE_MVIP_CTR(TSigStateChange);
DEFINE_MVIP_CTR(TSigSelectionPrefs);
DEFINE_MVIP_CTR(TSigConnPrefList);
DEFINE_MVIP_CTR(TSigFlowParams);
DEFINE_MVIP_CTR(TSigSubConNotification);
DEFINE_MVIP_CTR(TSigPlaneNotification);
DEFINE_MVIP_CTR(TSigSubConnOpenedEvent);
DEFINE_MVIP_CTR(TSigSubConnClosedEvent);
DEFINE_MVIP_CTR(TSigAddrUpdate);
DEFINE_MVIP_CTR(TSigConnectionInfo);
DEFINE_MVIP_CTR(TSigBundleRMessage2)
DEFINE_MVIP_CTR(TSigBundle)
DEFINE_MVIP_CTR(TSigSelectComplete)
DEFINE_MVIP_CTR(TSigPlayerRoleUidFactoryQuery)
DEFINE_MVIP_CTR(TSigAvailabilitySubscriptionOptions)
DEFINE_MVIP_CTR(TSigAvailabilityStatus)
DEFINE_MVIP_CTR(TSigNodeIdClientTypeNumber)
DEFINE_MVIP_CTR(TSigMetaExtContainer)
DEFINE_MVIP_CTR(TSigConnectionInfoPtr)

#ifdef SYMBIAN_NETWORKING_UPS
DEFINE_MVIP_CTR(TSigUpsParameters)
DEFINE_MVIP_CTR(TSigUpsStatusChangeParameters)
#endif

DEFINE_MVIP_CTR(TSigFamilyBundle)
DEFINE_MVIP_CTR(TSigNumberFamilyBundle)
DEFINE_MVIP_CTR(TCFImplicitFlowRequest)
DEFINE_MVIP_CTR(TCFConnFlowRequest)
DEFINE_MVIP_CTR(TCFSubConnFlowRequest)

DEFINE_MVIP_CTR(PRActivities::CRejoinDataClientActivity::TCFDataClientJoiningRequest)
DEFINE_MVIP_CTR(TAlwaysFindFactoryQuery)
DEFINE_MVIP_CTR(TMetaConnectionFactoryQuery)
DEFINE_MVIP_CTR(TDefaultConnectionFactoryQuery)
DEFINE_MVIP_CTR(TDefaultSCPRFactoryQuery)
DEFINE_MVIP_CTR(TDefaultFlowFactoryQuery)
DEFINE_MVIP_CTR(TDefaultProtocolIntfFactoryQuery)
DEFINE_MVIP_CTR(TTierTypeIdFactoryQuery)


const TImplementationProxy SignatureImplementationTable[] =
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY(ESignatureSigAddrUpdate,TSigAddrUpdate),				//14
	MVIP_CTR_ENTRY(ESignatureConnectionInfo,TSigConnectionInfo),		//15
	MVIP_CTR_ENTRY(ESignatureSelectionPrefs,TSigSelectionPrefs),		//18
	MVIP_CTR_ENTRY(ESignatureSelectComplete,TSigSelectComplete),		//19
	MVIP_CTR_ENTRY(ESignaturePlaneNotification,TSigPlaneNotification),	//20
	MVIP_CTR_ENTRY(ESignatureSubConnNotification,TSigSubConNotification),	//21
	MVIP_CTR_ENTRY(ESignatureSubConnOpenedEvent,TSigSubConnOpenedEvent), //22
	MVIP_CTR_ENTRY(ESignatureSubConnClosedEvent,TSigSubConnClosedEvent), //23
	MVIP_CTR_ENTRY(ESignatureStateChange,TSigStateChange),					//12
	MVIP_CTR_ENTRY(ESignatureUidFactoryQuery,TSigPlayerRoleUidFactoryQuery),	//33
	MVIP_CTR_ENTRY(ESignatureBundleRMessage2,TSigBundleRMessage2),		//40
	MVIP_CTR_ENTRY(ESignatureBundle,TSigBundle),						//41
	MVIP_CTR_ENTRY(ESignatureAvailabilitySubscriptionOptions,TSigAvailabilitySubscriptionOptions), //42
	MVIP_CTR_ENTRY(ESignatureAvailabilityStatus,TSigAvailabilityStatus),//43
	MVIP_CTR_ENTRY(ESignatureNodeIdClientTypeNumber, TSigNodeIdClientTypeNumber), //44
	MVIP_CTR_ENTRY(ESignatureFlowParams,TSigFlowParams),	//45
	MVIP_CTR_ENTRY(ESignatureConnPrefList,TSigConnPrefList),//46
#ifdef SYMBIAN_NETWORKING_UPS
	MVIP_CTR_ENTRY(ESignatureUpsParameters,TSigUpsParameters),//47
#endif 	
	MVIP_CTR_ENTRY(ESignatureFamilyBundle,TSigFamilyBundle),				//48
	MVIP_CTR_ENTRY(ESignatureNumberFamilyBundle,TSigNumberFamilyBundle),	//49
#ifdef SYMBIAN_NETWORKING_UPS
	MVIP_CTR_ENTRY(ESignatureUpsStatusChange,TSigUpsStatusChangeParameters),//50
#endif
	MVIP_CTR_ENTRY(ESignatureMetaExtContainer,TSigMetaExtContainer),   //51
	MVIP_CTR_ENTRY(ESignatureConnectionInfoPtr,TSigConnectionInfoPtr),   //52
	MVIP_CTR_ENTRY(EImplicitFlowRequest,TCFImplicitFlowRequest),	    //1001
	MVIP_CTR_ENTRY(EConnFlowRequest,TCFConnFlowRequest),			    //1002
	MVIP_CTR_ENTRY(ESubConnFlowRequest,TCFSubConnFlowRequest),	        //1003
	MVIP_CTR_ENTRY(EDataClientJoiningRequest,PRActivities::CRejoinDataClientActivity::TCFDataClientJoiningRequest),//1005
	MVIP_CTR_ENTRY(EGetOrSetParameters,TGetOrSetParameters),				//1006
	MVIP_CTR_ENTRY(EAlwaysFindFactoryQuery, TAlwaysFindFactoryQuery),	//1007
	MVIP_CTR_ENTRY(EMetaConnectionFactoryQuery, TMetaConnectionFactoryQuery), //1008
	MVIP_CTR_ENTRY(EDefaultConnectionFactoryQuery, TDefaultConnectionFactoryQuery), //1008	
	MVIP_CTR_ENTRY(EDefaultSCPRFactoryQuery, TDefaultSCPRFactoryQuery), //1008	
	MVIP_CTR_ENTRY(EDefaultFlowFactoryQuery, TDefaultFlowFactoryQuery),
	MVIP_CTR_ENTRY(EDefaultProtocolIntfFactoryQuery, TDefaultProtocolIntfFactoryQuery),	
	MVIP_CTR_ENTRY(ETierTypeIdFactoryQuery, TTierTypeIdFactoryQuery), //1013
	};


void TCFMessage::RegisterL()
	{
	TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(KESockInternalMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

void TCFMessage::DeRegister()
	{
	TlsGlobals::Get().DeregisterInterface(TUid::Uid(KESockInternalMessagesImplementationUid));
	}

#if 0
#ifdef ESOCK_EXTLOG_ACTIVE

// for logging bundle
#include <comms-infras/es_parameterbundle.h>

TInt TNodeSignalBase::PrintableSignature(TDes8& aResult) const
/**
Generate printable output of signature parameters in the specified buffer

@param aResult buffer to receive printable output.
@return number of parameters output
*/
	{
	_LIT8(KArgNumber, "%d");
	_LIT8(KArgNumberNumber, "%d,%d");
	_LIT8(KArgThreeNumbers, "%d,%d,%d");
	_LIT8(KArgHexAndTwoDecimalNumbers, "%08x,%d,%d");
#ifdef SYMBIAN_NETWORKING_UPS
	_LIT8(KArgHexNumber, "%08x");
#endif //SYMBIAN_NETWORKING_UPS

	STypeId id = GetTypeId();
	if (id.iUid.iUid == KESockInternalMessagesImplementationUid)
		{
		switch (id.iType)
			{
		case ESignatureNumber:
			aResult.AppendFormat(KArgNumber(), static_cast<const TSigNumber*>(this)->iValue);
			return 1;
		case ESignatureStateChange:
			{
			const TStateChange& state = static_cast<const TSigStateChange*>(this)->iStateChange;
			aResult.AppendFormat(KArgNumberNumber(), state.iStage, state.iError);
			return 2;
			}
		case ESignatureNumberNumber:
			{
			const TSigNumberNumber* sig = static_cast<const TSigNumberNumber*>(this);
			aResult.AppendFormat(KArgNumberNumber(), sig->iValue1, sig->iValue2);
			return 2;
			}
		case ESignatureErrResponse:
			{
			const TErrResponse& resp = static_cast<const TSigErrResponse*>(this)->iErrResponse;
			aResult.AppendFormat(KArgThreeNumbers(), resp.iAction, resp.iError, resp.iMessageId.MessageId());
			return 3;
			}
		case ESignatureErrContext:
			{
			const TErrContext& ctx = static_cast<const TSigErrContext*>(this)->iErrContext;
			aResult.AppendFormat(KArgHexAndTwoDecimalNumbers(), ctx.iOriginator.Printable(), ctx.iMessageId.MessageId(), ctx.iActivityId);
			return 3;
			}
		case ESignatureMessageIdNumber:
			{
			const TSigMessageIdNumber* sig = static_cast<const TSigMessageIdNumber*>(this);
#ifdef SYMBIAN_NETWORKING_UPS
			_LIT8(KArgStringNumber, "%s,%d");
			// TPrintableMessage is just for printing out a MessageId in human readable form.
			class TPrintableMessage : public TCFSignatureBase
				{
			public:
				TPrintableMessage(const TCFNodeSignalBase::TMessageId& aMessageId)
				: TCFSignatureBase(aMessageId, *TCommsId::KNullCommsId, ECFActivityNull)
					{
					}
				} msg(sig->iMsgId);
			aResult.AppendFormat(KArgStringNumber(), msg.Printable(), sig->iValue);
#else
			aResult.AppendFormat(KArgNumberNumber(), sig->iMsgId.MessageId(), sig->iValue);
#endif
			return 2;
			}
#ifdef SYMBIAN_NETWORKING_UPS
		case ESignatureCommsBinderNumber:
			{
			const TSigCommsBinderNumber* sig = static_cast<const TSigCommsBinderNumber*>(this);
			aResult.AppendFormat(KArgHexNumber(), sig->iCommsBinder.iCommsId.Printable());
			return 1;
			}

		case ESignatureCommsBinder:
			{
			const TSigCommsBinder* sig = static_cast<const TSigCommsBinder*>(this);
			aResult.AppendFormat(KArgHexNumber(), sig->iCommsBinder.iCommsId.Printable());
			return 1;
			}
		case ESignatureSelectComplete:
			{
			const TSigSelectComplete* sig = static_cast<const TSigSelectComplete*>(this);
			aResult.AppendFormat(KArgHexNumber(), sig->iCommsId.Printable());
			return 1;
			}
		case ESignatureCommsId:
			{
			const TSigCommsId* sig = static_cast<const TSigCommsId*>(this);
			aResult.AppendFormat(KArgHexNumber(), sig->iCommsId.Printable());
			return 1;
			}
#endif

		case ESignatureBundle:
			{
			const TSigBundle* bun = static_cast<const TSigBundle*>(this);
			aResult.AppendFormat(_L8("Bundle (PSCs:%d)"), bun->iBundle->Ptr()->CountParamSetContainers());
			return 1;
			}
		case ESignatureFlowParams:
			{
			const TSigFlowParams* flowParams = static_cast<const TSigFlowParams*>(this);
			aResult.AppendFormat(
				_L8("Flow params: addrFamily:%d, protocol:%d, socketType:%d, flowRequest:%d"),
				flowParams->iFlowParams.iAddrFamily,
				flowParams->iFlowParams.iProtocol,
				flowParams->iFlowParams.iSocketType,
				flowParams->iFlowParams.iFlowRequestType);
			return 4;
			}
		default:
			break;
			}
		}
	return 0;
	}

#endif
#endif



//-======================================================
// TMessageSigMetaExtContainer
//-======================================================

EXPORT_C void TSigMetaExtContainer::DispatchL(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient)
    {
    Messages::TSignatureBase::DispatchL(aSender, aRecipient);
    iConfig.Close();
    }

EXPORT_C void TSigMetaExtContainer::Error(const TRuntimeCtxId& aSender, const TRuntimeCtxId& aRecipient, TInt aError)
    {
    iConfig.Close();
    Messages::TSignatureBase::Error(aSender, aRecipient, aError);
    }

//-======================================================
//
//
// CRefCountOwnerBase methods
//
//
//-======================================================
EXPORT_C CRefCountOwnerBase::CRefCountOwnerBase(const TAny* aPtr)
: iPtr(const_cast<TAny*>(aPtr)),
  iRefCount(0)
    {
	LOG_NODE_CREATE(KCFNodeTag, CRefCountOwnerBase)
    }

EXPORT_C CRefCountOwnerBase::~CRefCountOwnerBase()
    {
	LOG_NODE_DESTROY(KCFNodeTag, CRefCountOwnerBase)
    }

EXPORT_C void CRefCountOwnerBase::Close()
/*
To be called by the message receiver when it's finished
inspecting the pointer.
*/
    {
    // LockedDec() returns the value prior to decrement
    if (User::LockedDec(iRefCount) == 1)
        {
        // If this CRefCountOwnerBase was created in a higher priority thread than
        // the release of the final reference is running in, the TCFDataClient::TStop
        // can dispatch and delete this object almost immediately.

        // DO NOT put any processing after this PostTo()
        RClientInterface::OpenPostMessageClose(Id(), Id(),
        	TCFDataClient::TStop(0).CRef());
        }
    }

EXPORT_C void CRefCountOwnerBase::Open()
/*
To be called by the message sender each time the same
pointer is sent away.
*/
    {
#ifdef _DEBUG
    if (!User::LockedInc(iRefCount) && iRefOpened)
    	{
    	// This panic means that the objects ref count has been decremented to zero
    	// and an async deletion has been initiated. You are now trying to Open() a
    	// new ref on this soon to be dead object. Dangling pointers maybe??
    	User::Panic(KESockFaultPanic, EDeadObjectReused);
    	}
    iRefOpened = ETrue;
#else
    User::LockedInc(iRefCount);
#endif
    }

EXPORT_C void CRefCountOwnerBase::ReceivedL(const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
   	{
	(void)aMessage;
   	__ASSERT_DEBUG(aMessage.IsMessage<TCFDataClient::TStop>(), User::Panic(KSpecAssert_ESockSSocksndmsg, 1));
//   	delete iPtr;
	DeletePtr();
   	iPtr = NULL;
   	delete this;
   	}

EXPORT_C Meta::SMetaData* TVCFromSockManAttributeCreationPolicy::New(TPtrC8& aSrc, TPtr8& aDst)
	{
	CMetaDataVirtualCtorInPlace* vctr = SockManGlobals::Get()->SelfWorker()->TransportGlobals().VirtualCtor();
	if (vctr ==NULL)
		{
		return NULL;
		}
	__ASSERT_DEBUG(aSrc.Length() >= sizeof(STypeId), User::Panic(KSpecAssert_ESockSSocksndmsg, 2)); //At least the type id must be present			
	return vctr->New ( aSrc, aDst );
	}



EXPORT_C MFactoryQuery::TMatchResult TAlwaysFindFactoryQuery::Match(Factories::TFactoryObjectInfo& /* aObjectInfo */)
	{
	return MFactoryQuery::EMatch; //Return the first and only.			
	}

EXPORT_C TAlwaysFindFactoryQuery ::TAlwaysFindFactoryQuery ()
	{
		
	}

EXPORT_C TMetaConnectionFactoryQuery::TMetaConnectionFactoryQuery()
	{
		
	}

EXPORT_C TMetaConnectionFactoryQuery::TMetaConnectionFactoryQuery(const TProviderInfo& aProviderInfo,const TUid aTierImplUid)
: iProviderInfo(aProviderInfo),iTierImplUid(aTierImplUid)
	{
		
	}
	
EXPORT_C MFactoryQuery::TMatchResult TMetaConnectionFactoryQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
	{
	CMetaConnectionProviderBase* prov = static_cast<CMetaConnectionProviderBase*>(aFactoryObjectInfo.iInfo.iFactoryObject);
	return prov->ProviderInfo() == iProviderInfo? MFactoryQuery::EMatch : MFactoryQuery::EContinue;		
	}

EXPORT_C TTierTypeIdFactoryQuery::TTierTypeIdFactoryQuery()
    {
        
    }

EXPORT_C TTierTypeIdFactoryQuery::TTierTypeIdFactoryQuery(const TUid aTierTypeId)
: iTierTypeId(aTierTypeId)
    {
    
    }
    
EXPORT_C MFactoryQuery::TMatchResult TTierTypeIdFactoryQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
    {
    CTierManagerBase* prov = static_cast<CTierManagerBase*>(aFactoryObjectInfo.iInfo.iFactoryObject);
    return prov->TierId() == iTierTypeId ? MFactoryQuery::EMatch : MFactoryQuery::EContinue;     
    }

EXPORT_C TDefaultConnectionFactoryQuery::TDefaultConnectionFactoryQuery(const Messages::TNodeId& aMCprId)
: iMCprId(aMCprId)
	{
		
	}

EXPORT_C MFactoryQuery::TMatchResult TDefaultConnectionFactoryQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
	{
	CConnectionProviderBase* prov = static_cast<CConnectionProviderBase*>(aFactoryObjectInfo.iInfo.iFactoryObject);

	if (prov->ControlProvider())
		{
		// Each MCPR instance can only have one data client.  The comparison here uses the NodeId of the MCPR
		// to implement this check.
		//ASSERT(prov->ControlProvider());
		return prov->ControlProvider()->RecipientId() == iMCprId ? MFactoryQuery::EMatch : MFactoryQuery::EContinue;
		}
	else
		{
		// This is to support the half-way house bluetooth code
		// There is a dummy Cpr owning lower scpr's that are not
		// created by the framework in the usual way.
		// This dummy Cpr is also not created in the usual way
		// and hence will not have a ControlProvider()
		return MFactoryQuery::EContinue;
		}		
	}

EXPORT_C TDefaultConnectionFactoryQuery::TDefaultConnectionFactoryQuery()
: iMCprId(TNodeId::NullId())
	{
		
	}

EXPORT_C TDefaultSCPRFactoryQuery::TDefaultSCPRFactoryQuery()
: iControlProvider(TNodeId::NullId()),
iSCPRType((TSubConnOpen::TSubConnType)0)
	{
		
	}

//used to look up sub-connection from control side
EXPORT_C TDefaultSCPRFactoryQuery::TDefaultSCPRFactoryQuery(const Messages::TNodeId& aControlProvider, TSubConnOpen::TSubConnType aSCPRType)
: iControlProvider(aControlProvider),
iSCPRType(aSCPRType)
	{
		
	}

EXPORT_C MFactoryQuery::TMatchResult TDefaultSCPRFactoryQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
	{
	if (iSCPRType == TSubConnOpen::ECreateNew)
    	{
    	return MFactoryQuery::ECancel;
    	}
	CSubConnectionProviderBase* sc = static_cast<CSubConnectionProviderBase*>(aFactoryObjectInfo.iInfo.iFactoryObject);
	MFactoryQuery::TMatchResult match = aFactoryObjectInfo.iInfo.iFlag == iSCPRType ? MFactoryQuery::EMatch : MFactoryQuery::EContinue;
	__ASSERT_DEBUG(sc->ControlProvider(), User::Panic(KSpecAssert_ESockSSocksndmsg, 3));

	if (match == MFactoryQuery::EMatch)
	   	{
    	match = iControlProvider == sc->ControlProvider()->RecipientId() ? MFactoryQuery::EMatch : MFactoryQuery::EContinue;
    	}
	return match;			
	}

EXPORT_C TDefaultFlowFactoryQuery::TDefaultFlowFactoryQuery()
: iCprId(TNodeId::NullId()),
iSCprId(TNodeId::NullId())
	{
		
	}

EXPORT_C TDefaultFlowFactoryQuery::TDefaultFlowFactoryQuery(const Messages::TNodeId& aSCprId)
: iCprId(TNodeId::NullId()),
iSCprId(aSCprId)
	{
		
	}

EXPORT_C TDefaultFlowFactoryQuery::TDefaultFlowFactoryQuery(const Messages::TNodeId& aCprId, const Messages::TNodeId& aSCprId, Messages::TNodeSignal::TMessageId aMessageId, TInt aProtocolType)
: iCprId(aCprId),
iSCprId(aSCprId),
iMessageId(aMessageId),
iProtocolType(aProtocolType)
	{
		
	}

EXPORT_C TDefaultFlowFactoryQuery::TDefaultFlowFactoryQuery(const Messages::TNodeId& aCprId, const Messages::TNodeId& aSCprId)
: iCprId(aCprId),
iSCprId(aSCprId)
	{
		
	}

EXPORT_C MFactoryQuery::TMatchResult TDefaultFlowFactoryQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
	{
	CSubConnectionFlowBase* flow = static_cast<CSubConnectionFlowBase*>(aFactoryObjectInfo.iInfo.iFactoryObject);
	if (flow->HasControlPlane() && flow->ControlProvider() == iSCprId)
		{
		return MFactoryQuery::EMatch;
		}
	return MFactoryQuery::EContinue;		
	}

EXPORT_C TDefaultProtocolIntfFactoryQuery::TDefaultProtocolIntfFactoryQuery()
: iCprId(TNodeId::NullId())
	{
		
	}

EXPORT_C TDefaultProtocolIntfFactoryQuery::TDefaultProtocolIntfFactoryQuery(const Messages::TNodeId& aCprId)
: iCprId(aCprId)
	{
		
	}

EXPORT_C MFactoryQuery::TMatchResult TDefaultProtocolIntfFactoryQuery::Match(Factories::TFactoryObjectInfo& aFactoryObjectInfo)
	{
	CProtocolIntfBase* protIntf = static_cast<CProtocolIntfBase*>(aFactoryObjectInfo.iInfo.iFactoryObject);
	return (protIntf->ControlProviderId() == iCprId ? MFactoryQuery::EMatch : MFactoryQuery::EContinue);		
	}

