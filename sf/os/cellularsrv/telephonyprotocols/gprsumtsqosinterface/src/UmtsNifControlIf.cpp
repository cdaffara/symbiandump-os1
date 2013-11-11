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
//

#include "UmtsNifControlIf.h"

GLDEF_C TInt E32Dll()
	{
	return(KErrNone);
	}

//
// Fills the strcuture with invalid default values at creation
//
//lint -e{1927} would want to use inilializer list
EXPORT_C TContextParameters::TContextParameters()
	{
	iContextType = EContextTypeUnknown;	// Default value to indicate invalid
	iContextInfo.iContextId = (TInt8)-1;// Default value to indicate invalid
	iContextInfo.iStatus	= RPacketContext::EStatusUnknown;
    iContextConfig.Reset();	
	iTFTOperationCode		= 0;		// Default value to indicate invalid
	iReasonCode				= KErrGeneral;
	}


// Class to handle one Traffic Flow Template
// Includes 1 - 8 Filters
//lint -e{1927} would want to use inilializer list
EXPORT_C TTFTInfo::TTFTInfo() 
	{
	iFilterCount = 0;
	iLastReturnedFilter = 0;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	iIMCNSubsystemflag = 0;
#endif
	
	for(TUint counter = 0; counter < KMaxNumberOfPacketFilters; counter++)
		{
		iFilterSlots[counter] = EFalse; // All free
		}
	iSblpParams = NULL;
	}

EXPORT_C TInt TTFTInfo::Set(const TTFTInfo& aInfo)
	{
	TUint counter;
	iFilterCount = aInfo.iFilterCount;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	iIMCNSubsystemflag = aInfo.iIMCNSubsystemflag;
#endif
		
	// Loop&copy the new set of filters
	for(counter = 0; counter < KMaxNumberOfPacketFilters; counter++)
		{
		iFilterSlots[counter] = aInfo.iFilterSlots[counter];
		iPacketFilters[counter] = aInfo.iPacketFilters[counter];
		}
	
	// Added for Sblp Implementation. Set the SBlP Parameters also
	if ( NULL == iSblpParams )
		{
	    TRAPD(err, iSblpParams = RPacketContext::CTFTMediaAuthorizationV3::NewL());
		if(err != KErrNone)
			{
			return err;
			}
		}
	// Get The Params 
	aInfo.GetSblpToken(*iSblpParams);
	
	return KErrNone;
	}


EXPORT_C TTFTInfo::~TTFTInfo() 
	{
	// Additional Checking to prevent any accidental memory Leaks
	delete iSblpParams;
	}

//
// Adds a packet filter to the TFT. If a filter already exists with given id, it will 
// be replaced
//
EXPORT_C TInt TTFTInfo::AddPacketFilter(RPacketContext::TPacketFilterV2 aFilter)
	{
	TUint8 counter;

	// Search for existing with same ID to replace
	for(counter = 0; counter < KMaxNumberOfPacketFilters; counter++)
		{
		if(	iFilterSlots[counter] &&
			iPacketFilters[counter].iId == aFilter.iId) 
			{
			iPacketFilters[counter] = aFilter; // Replace existing
			return KErrNone; 
			}
		}
	
	if(iFilterCount == KMaxNumberOfPacketFilters)  // Already full
		{
		return KErrGeneral; 
		}

	for(counter = 0; counter < KMaxNumberOfPacketFilters; counter++)
		if (iFilterSlots[counter] == EFalse)
			{
			iFilterSlots[counter] = ETrue;	// Reserve filter slot
			iPacketFilters[counter] = aFilter;
			iFilterCount++;	
			iLastReturnedFilter = 0;		// Resets the iterator
			return KErrNone;
			}

	__ASSERT_DEBUG(counter < KMaxNumberOfPacketFilters, User::Panic(_L("No space for filter"), -1));		

	return KErrGeneral;
	}
//
// Fetch a filter with given id
//
EXPORT_C TInt TTFTInfo::GetPacketFilter(RPacketContext::TPacketFilterV2& aFilter)
	{
	TUint8 counter;
	for(counter = 0; counter < KMaxNumberOfPacketFilters; counter++)
		{
		if(	iFilterSlots[counter] &&
			iPacketFilters[counter].iId == aFilter.iId) 
			{
			aFilter = iPacketFilters[counter];
			break;
			}

		}
		
	if(counter >= KMaxNumberOfPacketFilters)
		return KErrNotFound;
	else
		return KErrNone;
	}

//
// Remove a filter with given id
//
EXPORT_C TInt TTFTInfo::RemovePacketFilter(RPacketContext::TPacketFilterV2 aFilter)
	{
	TUint8 counter;
	for(counter = 0; counter < KMaxNumberOfPacketFilters; counter++)
		{
		if(	iFilterSlots[counter] &&
			iPacketFilters[counter].iId == aFilter.iId) 
			{
			iFilterSlots[counter] = EFalse;
			break;
			}
		}

	if(counter >= KMaxNumberOfPacketFilters)
		return KErrNotFound;
	else 
		{
		iFilterCount--;
		iLastReturnedFilter = 0; // Resets the iterator

		return KErrNone;
		}

	}
//
// Iterate the next filter in TFT
//
EXPORT_C TInt TTFTInfo::NextPacketFilter(RPacketContext::TPacketFilterV2& aFilter)
	{
	if(iLastReturnedFilter >= KMaxNumberOfPacketFilters) // Iterator finished
		return KErrNotFound;

	TUint8 counter;
	for(counter = iLastReturnedFilter; counter < KMaxNumberOfPacketFilters; counter++)
		{
		if(iFilterSlots[counter]) 
			{
			iLastReturnedFilter = counter;
			aFilter = iPacketFilters[counter];			
			iLastReturnedFilter++;
			break;
			}
		}
	if(counter == KMaxNumberOfPacketFilters)
		return KErrNotFound;
	else
		return KErrNone;
	}

//
// Reset the iterator
//
EXPORT_C void TTFTInfo::SetToFirst()
	{
	iLastReturnedFilter = 0;
	}

//
// Reset the iterator
//
EXPORT_C TUint8 TTFTInfo::FilterCount()
	{
	return iFilterCount;
	}

#ifdef SYMBIAN_NETWORKING_UMTSR5
EXPORT_C TInt TTFTInfo::SetIMCNSubsystemflag(TBool aIMCNSubsystemflag)
	{
	iIMCNSubsystemflag = aIMCNSubsystemflag;
	return KErrNone;
	}

EXPORT_C TInt TTFTInfo::GetIMCNSubsystemflag(TBool &aIMCNSubsystemflag) const
	{
	aIMCNSubsystemflag = iIMCNSubsystemflag;
	return KErrNone;
	}
#endif


EXPORT_C TTFTInfo& TTFTInfo::operator=(const TTFTInfo& aTFTInfo)
	{
	// Assigning to self is NOP.
	if (this == &aTFTInfo)
		return *this;
	
	iFilterCount = aTFTInfo.iFilterCount;
	iLastReturnedFilter = aTFTInfo.iLastReturnedFilter;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	iIMCNSubsystemflag = aTFTInfo.iIMCNSubsystemflag;	
#endif
	
	// not using something like Mem::Copy since RPacketContext::TPacketFilterV2 might change in the future
	for (TUint i = 0; i < KMaxNumberOfPacketFilters; ++i) 
		{
		iPacketFilters[i] = aTFTInfo.iPacketFilters[i];
		iFilterSlots[i] = aTFTInfo.iFilterSlots[i];
		}	
		
	if (aTFTInfo.iSblpParams) 
		{ 
		AddSblpToken(aTFTInfo.iSblpParams->iAuthorizationToken,aTFTInfo.iSblpParams->iFlowIds);
		} 
	
	return *this; 
	}

//coverity[pass_by_value]
EXPORT_C TInt TTFTInfo::AddSblpToken(RPacketContext::TAuthorizationToken aAuthorizationToken,
                                    RArray<RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier> aFlowIds)
	{
	// Check whether memory is allocated to the Member or not
	if ( NULL == iSblpParams )
		{
		TRAPD(err, iSblpParams = RPacketContext::CTFTMediaAuthorizationV3::NewL());
		if(err != KErrNone)
			{
			return err;
			}
		}
	
    iSblpParams->iAuthorizationToken.Copy(aAuthorizationToken);

    iSblpParams->iFlowIds.Reset();
    TInt i;
    for(i=0; i<aFlowIds.Count();i++)
    	{
        iSblpParams->iFlowIds.Append(aFlowIds[i]);
    	}

	return KErrNone;
	}

// Getter Function
EXPORT_C TInt TTFTInfo::GetSblpToken ( RPacketContext::CTFTMediaAuthorizationV3 & aSblpParams ) const
	{
	// Check whether memory is allocated to the Member or not
	if ( NULL == iSblpParams )
		{
		return KErrNotFound;
		}
	
	// Copy using the Internalize mechanism of CTFTMediaAuthorisationV3
	HBufC8 * externalizeData = NULL;
	// First Externalize 
	TRAPD(err,iSblpParams->ExternalizeL(externalizeData));
	if(err != KErrNone)
		{
		return err;
		}	
	TPtr8  internalizePtr = externalizeData->Des();
	TRAPD(err1,aSblpParams.InternalizeL(internalizePtr));
	if(err1 != KErrNone)
		{
		return err1;
		}	
	// Cleanup Memory
	delete externalizeData;

	return KErrNone;
	}

// removal Function
EXPORT_C TInt TTFTInfo::RemovSblpToken ()
	{
	// delete Sblp Params
	if (iSblpParams != NULL)
		{
		delete iSblpParams;
		iSblpParams = NULL;
		}
	return KErrNone;
	}


EXPORT_C TContextConfig::TContextConfig()
	{
	// Current default values for data come from Etel class constructors.
	// If they need other default values, they should be initialized here.
	// RPacketContext::TContextConfigGPRS	
	// RPacketQoS::TQoSR99_R4Requested		
	// RPacketQoS::TQoSR99_R4Negotiated
	// RPacketQos::TQoSR5Requested
	// RPacketQos::TQoSR5Negotiated	 	
	}

EXPORT_C TContextConfig::~TContextConfig()
	{
	}

EXPORT_C void TContextConfig::Reset()
	{
	TContextConfig temp;
	Set(temp);
	}

EXPORT_C TInt TContextConfig::Set(const TContextConfig& aConfig) 
	{
	aConfig.GetUMTSQoSNeg(iUMTSQoS);
	aConfig.GetUMTSQoSReq(iUMTSQoSReq);
	aConfig.GetTFTInfo(iTFTInfo);	
	
	iContextConfig = aConfig.iContextConfig;
	iContextConfig.iPdpType = aConfig.iContextConfig.iPdpType;
	iContextConfig.iAccessPointName = aConfig.iContextConfig.iAccessPointName;
	iContextConfig.iPdpAddress = aConfig.iContextConfig.iPdpAddress;
	iContextConfig.iPdpCompression = aConfig.iContextConfig.iPdpCompression;
	
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetPdpType(RPacketContext::TProtocolType aPdpType)
	{
	iContextConfig.iPdpType = aPdpType;	
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetAccessPointName(const RPacketContext::TGSNAddress& aAccessPointName)
	{
	iContextConfig.iAccessPointName = aAccessPointName;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetPdpAddress(const RPacketContext::TProtocolAddress& aPdpAddress)
	{
	iContextConfig.iPdpAddress = aPdpAddress;	
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetPdpCompression(TUint aPdpCompression)
	{
	iContextConfig.iPdpCompression = aPdpCompression;	
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetTFTInfo(const TTFTInfo& aTFTInfo)
	{
	iTFTInfo.Set(aTFTInfo);
	return KErrNone;	
	}

EXPORT_C TInt  TContextConfig::GetPdpType(RPacketContext::TProtocolType& aPdpType) const
	{
	aPdpType = iContextConfig.iPdpType;	
	return KErrNone;
	}
	
EXPORT_C TInt  TContextConfig::GetAccessPointName(RPacketContext::TGSNAddress& aAccessPointName) const
	{
	aAccessPointName = iContextConfig.iAccessPointName;
	return KErrNone;
	}

EXPORT_C TInt  TContextConfig::GetPdpAddress(RPacketContext::TProtocolAddress& aPdpAddress) const
	{
	aPdpAddress = iContextConfig.iPdpAddress;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetPdpCompression(TUint& aPdpCompression) const
	{
	aPdpCompression = iContextConfig.iPdpCompression;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetTFTInfo(TTFTInfo& aTFTInfo) const
	{		
	aTFTInfo.Set(iTFTInfo);
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetUMTSQoSReq(const RPacketQoS::TQoSR99_R4Requested& aUMTSQoSRequest)
	{	
    iUMTSQoSReq.iReqTrafficClass = aUMTSQoSRequest.iReqTrafficClass;
    iUMTSQoSReq.iMinTrafficClass = aUMTSQoSRequest.iMinTrafficClass;
    iUMTSQoSReq.iReqDeliveryOrderReqd = aUMTSQoSRequest.iReqDeliveryOrderReqd;
    iUMTSQoSReq.iMinDeliveryOrderReqd = aUMTSQoSRequest.iMinDeliveryOrderReqd;
    iUMTSQoSReq.iReqDeliverErroneousSDU = aUMTSQoSRequest.iReqDeliverErroneousSDU;
    iUMTSQoSReq.iMinDeliverErroneousSDU = aUMTSQoSRequest.iMinDeliverErroneousSDU;
    iUMTSQoSReq.iReqMaxSDUSize = aUMTSQoSRequest.iReqMaxSDUSize;
    iUMTSQoSReq.iMinAcceptableMaxSDUSize = aUMTSQoSRequest.iMinAcceptableMaxSDUSize;   
    iUMTSQoSReq.iReqMaxRate = aUMTSQoSRequest.iReqMaxRate; 
    iUMTSQoSReq.iMinAcceptableMaxRate = aUMTSQoSRequest.iMinAcceptableMaxRate; 
    iUMTSQoSReq.iReqBER = aUMTSQoSRequest.iReqBER; 
    iUMTSQoSReq.iMaxBER = aUMTSQoSRequest.iMaxBER; 
    iUMTSQoSReq.iReqSDUErrorRatio = aUMTSQoSRequest.iReqSDUErrorRatio;    
    iUMTSQoSReq.iMaxSDUErrorRatio = aUMTSQoSRequest.iMaxSDUErrorRatio; 
    iUMTSQoSReq.iReqTrafficHandlingPriority = aUMTSQoSRequest.iReqTrafficHandlingPriority; 
    iUMTSQoSReq.iMinTrafficHandlingPriority = aUMTSQoSRequest.iMinTrafficHandlingPriority; 
    iUMTSQoSReq.iReqTransferDelay = aUMTSQoSRequest.iReqTransferDelay; 
    iUMTSQoSReq.iMaxTransferDelay = aUMTSQoSRequest.iMaxTransferDelay; 
    iUMTSQoSReq.iReqGuaranteedRate = aUMTSQoSRequest.iReqGuaranteedRate;    
    iUMTSQoSReq.iMinGuaranteedRate = aUMTSQoSRequest.iMinGuaranteedRate;
    
#ifdef SYMBIAN_NETWORKING_UMTSR5    
    iUMTSQoSReq.iSignallingIndication = 0;
    iUMTSQoSReq.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
#endif    

	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetUMTSQoSReq(RPacketQoS::TQoSR99_R4Requested& aUMTSQoSRequest) const
	{
	aUMTSQoSRequest = iUMTSQoSReq;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetUMTSQoSNeg(const RPacketQoS::TQoSR99_R4Negotiated& aUMTSQoS)
	{	
	iUMTSQoS.iTrafficClass = aUMTSQoS.iTrafficClass;
	iUMTSQoS.iDeliveryOrderReqd = aUMTSQoS.iDeliveryOrderReqd;	
	iUMTSQoS.iDeliverErroneousSDU = aUMTSQoS.iDeliverErroneousSDU;
	iUMTSQoS.iMaxSDUSize = aUMTSQoS.iMaxSDUSize;
	iUMTSQoS.iMaxRate = aUMTSQoS.iMaxRate;
	iUMTSQoS.iBER = aUMTSQoS.iBER;
	iUMTSQoS.iSDUErrorRatio = aUMTSQoS.iSDUErrorRatio;
	iUMTSQoS.iTrafficHandlingPriority = aUMTSQoS.iTrafficHandlingPriority;
	iUMTSQoS.iTransferDelay = aUMTSQoS.iTransferDelay;
	iUMTSQoS.iGuaranteedRate = aUMTSQoS.iGuaranteedRate;

#ifdef SYMBIAN_NETWORKING_UMTSR5
    iUMTSQoS.iSignallingIndication = 0;
    iUMTSQoS.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
#endif
    	
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetUMTSQoSNeg(RPacketQoS::TQoSR99_R4Negotiated& aUMTSQoS) const
	{
	aUMTSQoS = iUMTSQoS;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetContextConfig(const RPacketContext::TContextConfigGPRS& aContextConfig)
	{
	iContextConfig = aContextConfig;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetContextConfig(RPacketContext::TContextConfigGPRS& aContextConfig) const
	{
	aContextConfig = iContextConfig;
	return KErrNone;
	}

#ifdef SYMBIAN_NETWORKING_UMTSR5
EXPORT_C TInt TContextConfig::SetUMTSQoSNeg(const RPacketQoS::TQoSR5Negotiated& aR5QoSNeg)
	{	
	iUMTSQoS = aR5QoSNeg;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetUMTSQoSNeg(RPacketQoS::TQoSR5Negotiated& aR5QoSNeg) const
	{
	aR5QoSNeg = iUMTSQoS;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::SetUMTSQoSReq(const RPacketQoS::TQoSR5Requested& aR5QoSReq)
	{	
	iUMTSQoSReq = aR5QoSReq;
	return KErrNone;
	}

EXPORT_C TInt TContextConfig::GetUMTSQoSReq(RPacketQoS::TQoSR5Requested& aR5QoSReq) const
	{
	aR5QoSReq = iUMTSQoSReq;
	return KErrNone;
	}
#endif

