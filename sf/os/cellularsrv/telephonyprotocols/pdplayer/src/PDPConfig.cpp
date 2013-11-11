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
// PDP Config structures for TProvision message.
// 
//

/**
 @file
 @internalComponent
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PDPConfigTraces.h"
#endif

#include <hash.h>
#include <e32math.h>
#include <comms-infras/ss_log.h>
#include <in_sock.h>
#include <comms-infras/metadata.h>
#include <comms-infras/agentmcpr.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_tiermanagerutils.h>

#include "PDPProvision.h"

using namespace ESock;

//
// Attribute table for provisioning structure passed to CFProtocol
//
START_ATTRIBUTE_TABLE(CGPRSProvision, CGPRSProvision::EUid, CGPRSProvision::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CTSYProvision, CGPRSProvision::EUid, CTSYProvision::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CDefaultPacketQoSProvision, CDefaultPacketQoSProvision::EUid, CDefaultPacketQoSProvision::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CImsExtProvision, CImsExtProvision::EUid, CImsExtProvision::ETypeId)
// No attributes defined, as no serialisation takes place.
END_ATTRIBUTE_TABLE()

CGPRSProvision* CGPRSProvision::NewLC(ESock::CCommsDatIapView* aIapView)
	{
	CGPRSProvision* self = new (ELeave) CGPRSProvision;
	CleanupStack::PushL(self);
	self->InitialiseConfigL(aIapView);
	return self;
	}
TInt CGPRSProvision::UmtsGprsRelease() const
	{
	return iUmtsGprsRelease;
	}

void CGPRSProvision::InitialiseConfigL(ESock::CCommsDatIapView* aIapView)
    {	
	TInt getErr = aIapView->GetInt(KCDTIdUmtsGprsRelease | KCDTIdOutgoingGprsRecord, iUmtsGprsRelease);
		
	if (getErr == KErrNotFound || getErr == KErrUnknown)
	    {
	    // default to GPRS
	    getErr = KErrNone;
	    iUmtsGprsRelease = TPacketDataConfigBase::KConfigGPRS;
	    }
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_INITIALISECONFIGL_1, "InitialiseConfigL() for packet network [%d]", iUmtsGprsRelease);
	TBool useEdge = EFalse;
	
	aIapView->GetBoolL(KCDTIdWCDMAUseEdge | KCDTIdOutgoingGprsRecord, useEdge);
	
	getErr = InitialiseScratchContext(iUmtsGprsRelease);
	//-====================================================
	//LENIENT PROVISION
	//-====================================================
	//With some provision parameters (e.g.: pdpAddress), CGPRSProvision 
	//tries to be lenient as technically they can be supplied by
	//someone further in the chain (e.g.: GPRS), if not found in the db.
	switch (iUmtsGprsRelease)
		{
    	case TPacketDataConfigBase::KConfigGPRS:
			{
            RPacketContext::TContextConfigGPRS& contextGPRS = GetScratchContextAs<RPacketContext::TContextConfigGPRS>();
            RetrievePdpTypeL(contextGPRS.iPdpType, aIapView);
            RetrieveApnNameL(contextGPRS.iAccessPointName, aIapView);
            RetrieveIPAndDnsSettingsL(contextGPRS.iPdpAddress,
                contextGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns,
                contextGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, aIapView);
            RetrieveAuthenticationInfoL(contextGPRS.iProtocolConfigOption, aIapView);
            // GPRS specific
        	RetrieveGprsCompression(contextGPRS.iPdpCompression, aIapView);
        	RetrieveGprsAnonymousAccess(contextGPRS.iAnonymousAccessReqd, aIapView);
        	contextGPRS.iUseEdge = useEdge;
			}
			break;
	    case TPacketDataConfigBase::KConfigRel5:
			{
            // Rel 5 specific
            RPacketContext::TContextConfig_R5& contextR5 = GetScratchContextAs<RPacketContext::TContextConfig_R5>();
            RetrieveUmtsCompressionL(contextR5.iPdpHeaderCompression, contextR5.iPdpDataCompression, aIapView);
            
            RetrievePdpTypeL(contextR5.iPdpType, aIapView);
            RetrieveApnNameL(contextR5.iAccessPointName, aIapView);
            RetrieveIPAndDnsSettingsL(contextR5.iPdpAddress,
            		contextR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns,
            		contextR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, aIapView);
            RetrieveAuthenticationInfoL(contextR5.iProtocolConfigOption, aIapView);
            contextR5.iUseEdge = useEdge;

    		RetrieveUmtsPacketFlowIdentifierL(contextR5.iPFI, aIapView);
			}
			break;
	    case TPacketDataConfigBase::KConfigRel99Rel4:
			{
            RPacketContext::TContextConfigR99_R4& contextUMTS = GetScratchContextAs<RPacketContext::TContextConfigR99_R4>();

            RetrievePdpTypeL(contextUMTS.iPdpType, aIapView);
            RetrieveApnNameL(contextUMTS.iAccessPointName, aIapView);
            RetrieveIPAndDnsSettingsL(contextUMTS.iPdpAddress,
                contextUMTS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns,
                contextUMTS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, aIapView);
            RetrieveAuthenticationInfoL(contextUMTS.iProtocolConfigOption, aIapView);
            contextUMTS.iUseEdge = useEdge;
            RetrieveUmtsPacketFlowIdentifierL(contextUMTS.iPFI, aIapView);
			}
			break;
	    default:
	    	User::Leave(KErrNotSupported);
		}
    }

void CGPRSProvision::RetrieveGprsCompression(TUint& aCompression, ESock::CCommsDatIapView* aIapView) const
	{
	aCompression = 0;
	TBool isCompression = EFalse;
	aIapView->GetBool(KCDTIdWCDMADataCompression | KCDTIdOutgoingGprsRecord, isCompression);
	if (isCompression)
		{
		aCompression |= RPacketContext::KPdpDataCompression;
		}

	isCompression = EFalse;
	aIapView->GetBool(KCDTIdWCDMAHeaderCompression | KCDTIdOutgoingGprsRecord, isCompression);
	if (isCompression)
		{
		aCompression |= RPacketContext::KPdpHeaderCompression;
		}
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEGPRSCOMPRESSION_1, "CGPRSProvision [this=0x%08x]::InitialiseConfigL() KCDTIdWCDMADataCompression|KCDTIdWCDMAHeaderCompression [%u]", (TUint)this, aCompression);
	}

void CGPRSProvision::RetrieveGprsAnonymousAccess(RPacketContext::TAnonymousAccess& aAnonymous, ESock::CCommsDatIapView* aIapView) const
	{	
	TBool isAnonymous = EFalse;
	aIapView->GetBool(KCDTIdWCDMAAnonymousAccess | KCDTIdOutgoingGprsRecord,isAnonymous);
	if (isAnonymous)
		aAnonymous = RPacketContext::ERequired;
	else
		aAnonymous = RPacketContext::ENotRequired;
	
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEGPRSANONYMOUSACCESS_1, "CGPRSProvision [this=%08x]::InitialiseConfigL() KCDTIdWCDMAAnonymousAccess [%d]", (TUint)this, aAnonymous);
	}

void CGPRSProvision::RetrieveApnNameL(TDes8& aApnName, ESock::CCommsDatIapView* aIapView) const
	{
	HBufC* buf = NULL;
	TInt getErr;

	if ((getErr = aIapView->GetText(KCDTIdAPN | KCDTIdOutgoingGprsRecord, buf)) == KErrNone)
    		{
    		aApnName.Copy(*buf);
    		delete buf;
    		}
    	else if (getErr != KErrNotFound )
    		{
    		User::Leave(getErr);
    		}
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEAPNNAMEL_1, "CGPRSProvision [this=%08x]::InitialiseConfigL() KCDTIdAPN [%s]", (TUint)this, aApnName);
	}

void CGPRSProvision::RetrievePdpTypeL(RPacketContext::TProtocolType& aPdpType, ESock::CCommsDatIapView* aIapView) const	
	{
	TUint32 pdpType(0);

	aIapView->GetIntL(KCDTIdWCDMPDPType | KCDTIdOutgoingGprsRecord, pdpType);
	aPdpType = static_cast<RPacketContext::TProtocolType>(pdpType);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEPDPTYPEL_1, "CGPRSProvision [this=%08x]::InitialiseConfigL() KCDTIdWCDMPDPType=%d", (TUint)this, aPdpType);
	}

void CGPRSProvision::RetrieveAuthenticationInfoL(RPacketContext::TProtocolConfigOptionV2& aProtocolConfigOption, ESock::CCommsDatIapView* aIapView)
    {
    HBufC* buf = NULL;
    TInt getErr;

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEAUTHENTICATIONINFOL_1, "CGPRSProvision [this=%08x]::RetrieveAuthenticationInfoL()", (TUint)this);

    getErr = aIapView->GetText(KCDTIdWCDMAIfAuthName | KCDTIdOutgoingGprsRecord, buf);
    if ( getErr == KErrNone )
        {
        ASSERT(buf);
        aProtocolConfigOption.iAuthInfo.iUsername.Copy(*buf);
        delete buf;
        buf = NULL;
        
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEAUTHENTICATIONINFOL_2, "CGPRSProvision [this=%08x]::RetrieveAuthenticationInfoL() KCDTIdWCDMAIfAuthName [%s] ", (TUint)this, aProtocolConfigOption.iAuthInfo.iUsername);
        
		aProtocolConfigOption.iId = 1;
		
        getErr = aIapView->GetText(KCDTIdWCDMAIfAuthPass | KCDTIdOutgoingGprsRecord, buf);
        if ( getErr == KErrNone )
            {
            ASSERT(buf);
            aProtocolConfigOption.iAuthInfo.iPassword.Copy(*buf);
            delete buf;
            buf = NULL;
            
            OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEAUTHENTICATIONINFOL_3, "CGPRSProvision [this=%08x]::RetrieveAuthenticationInfoL() KCDTIdWCDMAIfAuthPass [%S]", (TUint)this, aProtocolConfigOption.iAuthInfo.iPassword);
            }
        else if (getErr != KErrNotFound)
            {
            User::Leave(getErr);
            }
        }
    else if (getErr != KErrNotFound)
        {
        User::Leave(getErr);
        }
    }

void CGPRSProvision::RetrieveIPAndDnsSettingsL(TDes8& aPdpAddress, TDes8& aDns1, TDes8& aDns2, ESock::CCommsDatIapView* aIapView) const
	{
	HBufC* buf = NULL;

	aPdpAddress.SetLength(0);
	aDns1.SetLength(0);
	aDns2.SetLength(0);
	
	TBool fromServer;
	aIapView->GetBoolL(KCDTIdWCDMAIPAddrFromServer | KCDTIdOutgoingGprsRecord, fromServer);
	if (!fromServer)
	    {
    	aIapView->GetTextL(KCDTIdWCDMAIPAddr | KCDTIdOutgoingGprsRecord, buf);
    	TInetAddr addr;
    	User::LeaveIfError(addr.Input(*buf));
    	// sending ASCII to MA but checking the validity of the address above.
    	aPdpAddress.Copy(*buf);
    	delete buf;
    	buf = NULL;
	    }
	
    // We can only use IPv4 or IPv6 - we use the first one listed in the IfNetworks field
	aIapView->GetTextL(KCDTIdWCDMAIfNetworks | KCDTIdOutgoingGprsRecord, buf);

	TInt pos = buf->Find(_L(","));
	if (pos == KErrNotFound)
	    {
	    pos = buf->Length();
	    }

	TBufC<KCommsDbSvrMaxFieldLength> temp(buf->Mid(0, pos));
	delete buf;
	buf = NULL;
	_LIT(KIp4, "ip");
	_LIT(KIp6, "ip6");
	if (temp.CompareF(KIp4) == 0)
	    {
	    // IPv4 settings
    	aIapView->GetBoolL(KCDTIdWCDMAIPDNSAddrFromServer | KCDTIdOutgoingGprsRecord, fromServer);
    	if (!fromServer)
    	    {
    	    aIapView->GetTextL(KCDTIdWCDMAIPNameServer1 | KCDTIdOutgoingGprsRecord, buf);
            TInetAddr addr;
            User::LeaveIfError(addr.Input(*buf));
            // sending ASCII to MA but checking the validity of the address above.
    	    aDns1.Copy(*buf);
        	delete buf;
        	buf = NULL;
    	    aIapView->GetTextL(KCDTIdWCDMAIPNameServer2 | KCDTIdOutgoingGprsRecord, buf);
    	    User::LeaveIfError(addr.Input(*buf));
            // sending ASCII to MA but checking the validity of the address above.
        	aDns2.Copy(*buf);
    	    delete buf;
        	buf = NULL;
    	    }
	    }
	else if (temp.CompareF(KIp6) == 0)
	    {
	    // IPv6 settings
    	aIapView->GetBoolL(KCDTIdWCDMAIP6DNSAddrFromServer | KCDTIdOutgoingGprsRecord, fromServer);
    	if (!fromServer)
    	    {
    	    aIapView->GetTextL(KCDTIdWCDMAIP6NameServer1 | KCDTIdOutgoingGprsRecord, buf);
            TInetAddr addr;
            User::LeaveIfError(addr.Input(*buf));
            // sending ASCII to MA but checking the validity of the address above.
            aDns1.Copy(*buf);
    	    delete buf;
    		buf = NULL;
    	    aIapView->GetTextL(KCDTIdWCDMAIP6NameServer2 | KCDTIdOutgoingGprsRecord, buf);
            User::LeaveIfError(addr.Input(*buf));
            // sending ASCII to MA but checking the validity of the address above.
          	aDns2.Copy(*buf);
        	delete buf;
            buf = NULL;
    	    }
	    }
	else
	    {
	    // Anything else is a serious misconfiguration
	    User::Leave(KErrArgument);
	    }
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEIPANDDNSSETTINGSL_1, "PDP address [%s], DNS1 [%s], DNS2 [%s]", aPdpAddress, aDns1, aDns2);
	}

void CGPRSProvision::RetrieveUmtsCompressionL(RPacketContext::THeaderCompression& aHeaderCompression, RPacketContext::TDataCompression& aDataCompression, ESock::CCommsDatIapView* aIapView) const
	{
	TUint32 headerComp;
	TUint32 dataComp;
	TInt err;
	
	err = aIapView->GetInt(KCDTIdR5DataCompression | KCDTIdOutgoingGprsRecord, headerComp);
	if (err == KErrNotFound || err == KErrUnknown)
	    {
	    err = KErrNone;
	    headerComp = RPacketContext::ENoHeaderCompression;
	    }
	User::LeaveIfError(err);
	
	aHeaderCompression = static_cast<RPacketContext::THeaderCompression>(headerComp);
	
	err = aIapView->GetInt(KCDTIdR5HeaderCompression | KCDTIdOutgoingGprsRecord, dataComp);
	if (err == KErrNotFound || err == KErrUnknown)
	    {
	    err = KErrNone;
	    dataComp = RPacketContext::ENoDataCompression;
	    }
	User::LeaveIfError(err);

	aDataCompression = static_cast<RPacketContext::TDataCompression>(dataComp);	
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEUMTSCOMPRESSIONL_1, "Header Compression [%d], Data Compression [%d]", aHeaderCompression, aDataCompression);
	}

void CGPRSProvision::RetrieveUmtsPacketFlowIdentifierL(RPacketContext::TPacketFlowIdentifier& aPfi, ESock::CCommsDatIapView* aIapView) const
	{
	TUint32 pfi;
	TInt err = aIapView->GetInt(KCDTIdPacketFlowIdentifier | KCDTIdOutgoingGprsRecord, pfi);
	if (err == KErrNotFound || err == KErrUnknown)
	    {
	    err = KErrNone;
	    pfi = RPacketContext::EBestEffort;
	    }
	User::LeaveIfError(err);
	aPfi = static_cast<RPacketContext::TPacketFlowIdentifier>(pfi);
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGPRSPROVISION_RETRIEVEUMTSPACKETFLOWIDENTIFIERL_1, "Packet Flow Identifier [%d]", aPfi);
	}

TInt CGPRSProvision::InitialiseScratchContext(TUint32 aConfigRel) const
	{
    switch(aConfigRel)
        {
        case TPacketDataConfigBase::KConfigGPRS:
            {
            RPacketContext::TContextConfigGPRS tmp;
            Mem::Copy(const_cast<TUint8*>(iScratchContext.Ptr()), &tmp, sizeof(tmp));
            }
            break;
            
        case TPacketDataConfigBase::KConfigRel99Rel4:
            {
            RPacketContext::TContextConfigR99_R4 tmp;
            Mem::Copy(const_cast<TUint8*>(iScratchContext.Ptr()), &tmp, sizeof(tmp));
            }
            break;
            
        case TPacketDataConfigBase::KConfigRel5:
            {
            RPacketContext::TContextConfig_R5 tmp;
            Mem::Copy(const_cast<TUint8*>(iScratchContext.Ptr()), &tmp, sizeof(tmp));
            }
            break;
        default:
        	return KErrNotSupported;
        }
    return KErrNone;
    }


CTSYProvision* CTSYProvision::NewLC(ESock::CCommsDatIapView* aIapView)
	{
	CTSYProvision* self = new (ELeave) CTSYProvision;
	CleanupStack::PushL(self);
	self->InitialiseConfigL(aIapView);
	return self;
	}

void CTSYProvision::InitialiseConfigL(ESock::CCommsDatIapView* aIapView)
    {
	HBufC* buf = NULL;
	
	aIapView->GetTextL(KCDTIdTsyName, buf);
	iTsyName.Copy(*buf);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTSYPROVISION_INITIALISECONFIGL_1, "CTSYProvision [this=%08x]::InitialiseConfigL() KCDTIdTsyName=%S", (TUint)this, iTsyName);
	delete buf;
    }

CDefaultPacketQoSProvision* CDefaultPacketQoSProvision::NewL(ESock::CCommsDatIapView* aIapView)
	{
	CDefaultPacketQoSProvision* self = new (ELeave) CDefaultPacketQoSProvision;
	CleanupStack::PushL(self);
	self->InitialiseConfigL(aIapView);
	CleanupStack::Pop(self);
	return self;
	}
	
 void CDefaultPacketQoSProvision::InitialiseConfigL(ESock::CCommsDatIapView* aIapView)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDEFAULTPACKETQOSPROVISION_INITIALISECONFIGL_1, "Default Qos Provision");
    TUint32 data;
	    
	aIapView->GetIntL(KCDTIdWCDMAReqTrafficClass,data);
	iParams.iReqTrafficClass = (RPacketQoS::TTrafficClass)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMinTrafficClass,data);
	iParams.iMinTrafficClass = (RPacketQoS::TTrafficClass)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqDeliveryOrder,data);
	iParams.iReqDeliveryOrderReqd = (RPacketQoS::TDeliveryOrder)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMinDeliveryOrder,data);
	iParams.iMinDeliveryOrderReqd = (RPacketQoS::TDeliveryOrder)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqDeliverErroneousSDU,data);
	iParams.iReqDeliverErroneousSDU = (RPacketQoS::TErroneousSDUDelivery)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMinDeliverErroneousSDU,data);
	iParams.iMinDeliverErroneousSDU = (RPacketQoS::TErroneousSDUDelivery)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqMaxSDUSize,data);
	iParams.iReqMaxSDUSize = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMinAcceptableMaxSDUSize,data);
	iParams.iMinAcceptableMaxSDUSize = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqMaxUplinkRate,data);
	iParams.iReqMaxRate.iUplinkRate = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqMinUplinkRate,data);
	iParams.iMinAcceptableMaxRate.iUplinkRate = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqMaxDownlinkRate,data);
	iParams.iReqMaxRate.iDownlinkRate = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqMinDownlinkRate,data);
	iParams.iMinAcceptableMaxRate.iDownlinkRate = (TInt)data;
		
	aIapView->GetIntL(KCDTIdWCDMAReqBER,data);
	iParams.iReqBER = (RPacketQoS::TBitErrorRatio)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMaxBER,data);
	iParams.iMaxBER = (RPacketQoS::TBitErrorRatio)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqSDUErrorRatio,data);
	iParams.iReqSDUErrorRatio = (RPacketQoS::TSDUErrorRatio)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMaxSDUErrorRatio,data);
	iParams.iMaxSDUErrorRatio = (RPacketQoS::TSDUErrorRatio)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqTrafficHandlingPriority,data);
	iParams.iReqTrafficHandlingPriority = (RPacketQoS::TTrafficHandlingPriority)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMinTrafficHandlingPriority,data);
	iParams.iMinTrafficHandlingPriority = (RPacketQoS::TTrafficHandlingPriority)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqTransferDelay,data);
	iParams.iReqTransferDelay = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMaxTransferDelay,data);
	iParams.iMaxTransferDelay = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqGuaranteedUplinkRate,data);
	iParams.iReqGuaranteedRate.iUplinkRate = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAMinGuaranteedUplinkRate,data);			
	iParams.iMinGuaranteedRate.iUplinkRate = (TInt)data;
	
	aIapView->GetIntL(KCDTIdWCDMAReqGuaranteedDownlinkRate,data);
	iParams.iReqGuaranteedRate.iDownlinkRate = (TInt)data;
		
	aIapView->GetIntL(KCDTIdWCDMAMinGuaranteedDownlinkRate,data);
	iParams.iMinGuaranteedRate.iDownlinkRate = (TInt)data;

	TBool bdata = EFalse;
#ifdef _DEBUG
	TInt debugErr = 
#endif
	aIapView->GetBool(KCDTIdWCDMASignallingIndication,bdata);
	iParams.iSignallingIndication = bdata;
    }
    
CImsExtProvision* CImsExtProvision::NewLC(ESock::CCommsDatIapView* aIapView)
	{
	CImsExtProvision* self = new (ELeave) CImsExtProvision;
	CleanupStack::PushL(self);
	self->InitialiseConfigL(aIapView);
	return self;
	}

void CImsExtProvision::InitialiseConfigL(ESock::CCommsDatIapView* aIapView)
    {
	TBool bdata = EFalse;
	aIapView->GetBool(KCDTIdWCDMAImCmSignallingIndication,bdata);
	iImsSignalIndicator = bdata;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CIMSEXTPROVISION_INITIALISECONFIGL_1, "Ims Signal Indicator [%d]", iImsSignalIndicator);
    }


