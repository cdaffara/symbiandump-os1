// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tmockltsydata.h"
#include <s32mem.h>

/**
TMockLtsyData0
*/
EXPORT_C TBool TMockLtsyData0::operator == (const TDesC8& /*aPackedData*/) const
    {
    return ETrue;
    }


/**
TMockLtsyCallData0 
*/
EXPORT_C TMockLtsyCallData0::TMockLtsyCallData0()
    :iCallId(0), iCallMode(RMobilePhone::EServiceUnspecified)
    {
    }
    
EXPORT_C TMockLtsyCallData0::TMockLtsyCallData0(TInt aCallId, RMobilePhone::TMobileService aCallMode)
    :iCallId(aCallId), iCallMode(aCallMode)
    {
    }
    
EXPORT_C TInt TMockLtsyCallData0::CallId() const
    {
    return iCallId;
    }
    
EXPORT_C RMobilePhone::TMobileService TMockLtsyCallData0::CallMode() const
    {
    return iCallMode;
    }

EXPORT_C TBool TMockLtsyCallData0::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyCallData0 data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
        return EFalse;
    return (iCallId==data.iCallId) && (iCallMode==data.iCallMode);
    }
        
EXPORT_C void TMockLtsyCallData0::SerialiseL(RBuf8& aBuffer) const
    {
    RDesWriteStream stream;
    aBuffer.Create(2*sizeof(TInt32));
    stream.Open(aBuffer);
    stream << static_cast<TInt32>(iCallId);
    stream << static_cast<TInt32>(iCallMode);
    stream.Close();
    }
    
EXPORT_C void TMockLtsyCallData0::DeserialiseL(const TDesC8& aPackedData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);
    TInt32 val;
    stream >> val;
    iCallId = val;
    stream >> val;
    iCallMode = static_cast<RMobilePhone::TMobileService>(val);
    stream.Close();
    }

EXPORT_C TBool TComparator<RMobileCall::TMobileCallInfoV1>::IsEqual(const RMobileCall::TMobileCallInfoV1& aData1, const RMobileCall::TMobileCallInfoV1& aData2)
    {
    TBool result;
    result =  (aData1.ExtensionId() == aData2.ExtensionId());
    result &= (aData1.iValid == aData2.iValid);
    result &= (aData1.iService == aData2.iService);
    result &= (aData1.iStatus == aData2.iStatus);
    result &= (aData1.iCallId == aData2.iCallId);
    result &= (aData1.iExitCode == aData2.iExitCode);
    result &= (aData1.iEmergency == aData2.iEmergency);
    result &= (aData1.iForwarded == aData2.iForwarded);
    result &= (aData1.iPrivacy == aData2.iPrivacy);
    result &= (aData1.iAlternatingCall == aData2.iAlternatingCall);
    result &= (aData1.iDuration == aData2.iDuration);
    result &= (aData1.iTch == aData2.iTch);
    result &= (aData1.iRemoteParty.iRemoteIdStatus == aData2.iRemoteParty.iRemoteIdStatus);
    result &= (aData1.iRemoteParty.iDirection == aData2.iRemoteParty.iDirection);
    result &= (aData1.iRemoteParty.iRemoteNumber.iTypeOfNumber == aData2.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    result &= (aData1.iRemoteParty.iRemoteNumber.iNumberPlan == aData2.iRemoteParty.iRemoteNumber.iNumberPlan);
    result &= (aData1.iRemoteParty.iRemoteNumber.iTelNumber == aData2.iRemoteParty.iRemoteNumber.iTelNumber);
    result &= (aData1.iRemoteParty.iCallingName == aData2.iRemoteParty.iCallingName);
    result &= (aData1.iDialledParty.iTypeOfNumber == aData2.iDialledParty.iTypeOfNumber);
    result &= (aData1.iDialledParty.iNumberPlan == aData2.iDialledParty.iNumberPlan);
    result &= (aData1.iDialledParty.iTelNumber == aData2.iDialledParty.iTelNumber);
    result &= TComparator<TDateTime>::IsEqual(aData1.iStartTime,aData2.iStartTime);
    result &= (aData1.iCallName == aData2.iCallName);
    result &= (aData1.iLineName == aData2.iLineName);
    return result;
    }
EXPORT_C TBool TComparator<RMobileCall::TMobileCallInfoV8>::IsEqual(const RMobileCall::TMobileCallInfoV8& aData1, const RMobileCall::TMobileCallInfoV8& aData2)
    {
    TBool result;
    result =  (aData1.ExtensionId() == aData2.ExtensionId());
    result &= (aData1.iValid == aData2.iValid);
    result &= (aData1.iService == aData2.iService);
    result &= (aData1.iStatus == aData2.iStatus);
    result &= (aData1.iCallId == aData2.iCallId);
    result &= (aData1.iExitCode == aData2.iExitCode);
    result &= (aData1.iEmergency == aData2.iEmergency);
    result &= (aData1.iForwarded == aData2.iForwarded);
    result &= (aData1.iPrivacy == aData2.iPrivacy);
    result &= (aData1.iAlternatingCall == aData2.iAlternatingCall);
    result &= (aData1.iDuration == aData2.iDuration);
    result &= (aData1.iTch == aData2.iTch);
    result &= (aData1.iRemoteParty.iRemoteIdStatus == aData2.iRemoteParty.iRemoteIdStatus);
    result &= (aData1.iRemoteParty.iDirection == aData2.iRemoteParty.iDirection);
    result &= (aData1.iRemoteParty.iRemoteNumber.iTypeOfNumber == aData2.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    result &= (aData1.iRemoteParty.iRemoteNumber.iNumberPlan == aData2.iRemoteParty.iRemoteNumber.iNumberPlan);
    result &= (aData1.iRemoteParty.iRemoteNumber.iTelNumber == aData2.iRemoteParty.iRemoteNumber.iTelNumber);
    result &= (aData1.iRemoteParty.iCallingName == aData2.iRemoteParty.iCallingName);
    result &= (aData1.iDialledParty.iTypeOfNumber == aData2.iDialledParty.iTypeOfNumber);
    result &= (aData1.iDialledParty.iNumberPlan == aData2.iDialledParty.iNumberPlan);
    result &= (aData1.iDialledParty.iTelNumber == aData2.iDialledParty.iTelNumber);
    result &= TComparator<TDateTime>::IsEqual(aData1.iStartTime,aData2.iStartTime);
    result &= (aData1.iCallName == aData2.iCallName);
    result &= (aData1.iLineName == aData2.iLineName);
    result &= (aData1.iSecurity == aData2.iSecurity);
    result &= (aData1.iCallParamOrigin == aData2.iCallParamOrigin);
    result &= (aData1.iIconId.iIdentifier == aData2.iIconId.iIdentifier);
    result &= (aData1.iIconId.iQualifier == aData2.iIconId.iQualifier);
    result &= (0 == aData1.iAlphaId.Compare (aData2.iAlphaId));
    result &= (aData1.iParamsCallControlModified == aData2.iParamsCallControlModified);
    result &= (0 == aData1.iSubAddress.Compare(aData2.iSubAddress));
    result &= (0 == aData1.iBearerCap1.Compare(aData2.iBearerCap1));
    result &= (0 == aData1.iBearerCap2.Compare(aData2.iBearerCap2));
    result &= (aData1.iBCRepeatIndicator == aData2.iBCRepeatIndicator);
   
    return result;
    }

EXPORT_C TBool TComparator<RMobileCall::TMobileCallParamsV1>::IsEqual(const RMobileCall::TMobileCallParamsV1& aData1, const RMobileCall::TMobileCallParamsV1& aData2)
    {
    TBool result;
    result = (aData1.iIdRestrict == aData2.iIdRestrict);
    result &= (aData1.iCug.iExplicitInvoke == aData2.iCug.iExplicitInvoke);
    result &= (aData1.iCug.iCugIndex == aData2.iCug.iCugIndex);
    result &= (aData1.iCug.iSuppressOA == aData2.iCug.iSuppressOA);
    result &= (aData1.iCug.iSuppressPrefCug == aData2.iCug.iSuppressPrefCug);
    result &= (aData1.iAutoRedial == aData2.iAutoRedial);

    return result;
    }

EXPORT_C TBool TComparator<RMobileCall::TMobileCallParamsV7>::IsEqual(const RMobileCall::TMobileCallParamsV7& aData1, const RMobileCall::TMobileCallParamsV7& aData2)
    {
    TBool result;
    result = (aData1.iIdRestrict == aData2.iIdRestrict);
    result &= (aData1.iCug.iExplicitInvoke == aData2.iCug.iExplicitInvoke);
    result &= (aData1.iCug.iCugIndex == aData2.iCug.iCugIndex);
    result &= (aData1.iCug.iSuppressOA == aData2.iCug.iSuppressOA);
    result &= (aData1.iCug.iSuppressPrefCug == aData2.iCug.iSuppressPrefCug);
    result &= (aData1.iAutoRedial == aData2.iAutoRedial);
    result &= (aData1.iBearerMode == aData2.iBearerMode);
    result &= (aData1.iCallParamOrigin == aData2.iCallParamOrigin);
    result &= (aData1.iIconId.iIdentifier == aData2.iIconId.iIdentifier);
    result &= (aData1.iIconId.iQualifier == aData2.iIconId.iQualifier);
    result &= (0 == aData1.iAlphaId.Compare (aData2.iAlphaId));
    result &= (0 == aData1.iSubAddress.Compare(aData2.iSubAddress));
    result &= (0 == aData1.iBearerCap1.Compare(aData2.iBearerCap1));
    result &= (0 == aData1.iBearerCap2.Compare(aData2.iBearerCap2));
    result &= (aData1.iBCRepeatIndicator == aData2.iBCRepeatIndicator);
      
    return result;
    }

EXPORT_C TBool TComparator<RMobileCall::TMobileDataCallParamsV8>::IsEqual(const RMobileCall::TMobileDataCallParamsV8& aData1, const RMobileCall::TMobileDataCallParamsV8& aData2)
    {
    TBool result;
    result = (aData1.iIdRestrict == aData2.iIdRestrict);
    result &= (aData1.iCug.iExplicitInvoke == aData2.iCug.iExplicitInvoke);
    result &= (aData1.iCug.iCugIndex == aData2.iCug.iCugIndex);
    result &= (aData1.iCug.iSuppressOA == aData2.iCug.iSuppressOA);
    result &= (aData1.iCug.iSuppressPrefCug == aData2.iCug.iSuppressPrefCug);
    result &= (aData1.iAutoRedial == aData2.iAutoRedial);
    result &= (aData1.iIconId.iIdentifier == aData2.iIconId.iIdentifier);
    result &= (aData1.iIconId.iQualifier == aData2.iIconId.iQualifier);
    result &= (0 == aData1.iAlphaId.Compare (aData2.iAlphaId));
    result &= (0 == aData1.iSubAddress.Compare(aData2.iSubAddress));
    result &= (0 == aData1.iBearerCap1.Compare(aData2.iBearerCap1));
    result &= (0 == aData1.iBearerCap2.Compare(aData2.iBearerCap2));
    result &= (aData1.iBCRepeatIndicator == aData2.iBCRepeatIndicator);
    result &= (aData1.iCallParamOrigin == aData2.iCallParamOrigin);
    
    return result;
    }

EXPORT_C TBool TComparator<TContextConfiguration>::IsEqual(const TContextConfiguration& aData1, const TContextConfiguration& aData2)
    {
    TBool result;
    
    result = (aData1.iContextName == aData2.iContextName);
    result &= (aData1.iConfiguration == aData1.iConfiguration); 
    result &= (aData1.iTftOperation == aData1.iTftOperation); 
    
    
    if(aData1.iQosParams97 && aData2.iQosParams97)
    	{
    	result &= (0 == memcmp(aData1.iQosParams97, aData2.iQosParams97, sizeof(RPacketQoS::TQoSGPRSRequested)));
       	}
    else
        {
        result &= (aData1.iQosParams97 == aData2.iQosParams97);
        }           
    	
    if(aData1.iQosParams99 && aData2.iQosParams99)
    	{
    	result &= (0 == memcmp(aData1.iQosParams99, aData2.iQosParams99, sizeof(RPacketQoS::TQoSR99_R4Requested)));
       	}
    else
        {
        result &= (aData1.iQosParams99 == aData2.iQosParams99);
        }
    	
    if(aData1.iQosParamsR5 && aData2.iQosParamsR5)
    	{
    	result &= (0 == memcmp(aData1.iQosParamsR5, aData2.iQosParamsR5, sizeof(RPacketQoS::TQoSR5Requested)));
       	}
    else
        {
        result &= (aData1.iQosParamsR5 == aData2.iQosParamsR5);
        }
    return result;
    }


EXPORT_C TBool TComparator<RPacketContext::CTFTMediaAuthorizationV3>::
            IsEqual(const RPacketContext::CTFTMediaAuthorizationV3& aData1, 
                    const RPacketContext::CTFTMediaAuthorizationV3& aData2)
    {
	TBool result =  aData1.ExtensionId() == aData2.ExtensionId();
	result &= aData1.iAuthorizationToken == aData2.iAuthorizationToken;
	result &= aData1.iFlowIds.Count() == aData2.iFlowIds.Count();

	for (TInt index = 0; index < aData1.iFlowIds.Count() && result; index++)
	    {
	    const RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier& identifier1(aData1.iFlowIds[index]);
	    const RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier& identifier2(aData2.iFlowIds[index]);
	    result &= (identifier1.iMediaComponentNumber == identifier2.iMediaComponentNumber);
	    result &= (identifier1.iIPFlowNumber == identifier2.iIPFlowNumber);
	    }
    return result;                
    }    
EXPORT_C TBool TComparator<RPacketContext::TContextConfigGPRS>::
            IsEqual(const RPacketContext::TContextConfigGPRS& aData1, 
                    const RPacketContext::TContextConfigGPRS& aData2)
    {
    return ( aData1.iPdpType              == aData2.iPdpType             ) &&
           ( aData1.iAccessPointName      == aData2.iAccessPointName     ) &&
           ( aData1.iPdpAddress           == aData2.iPdpAddress          ) &&
           ( aData1.iPdpCompression       == aData2.iPdpCompression      ) &&
           ( aData1.iAnonymousAccessReqd  == aData2.iAnonymousAccessReqd ) &&
           ( aData1.iNWIContext           == aData2.iNWIContext          ) &&
           ( aData1.iUseEdge              == aData2.iUseEdge          ) &&
           ( aData1.iProtocolConfigOption.iChallenge  == aData2.iProtocolConfigOption.iChallenge  ) &&
           ( aData1.iProtocolConfigOption.iResponse   == aData2.iProtocolConfigOption.iResponse   ) &&
           ( aData1.iProtocolConfigOption.iId         == aData2.iProtocolConfigOption.iId         ) &&
           ( aData1.iProtocolConfigOption.iMiscBuffer == aData2.iProtocolConfigOption.iMiscBuffer ) &&
           ( aData1.iProtocolConfigOption.iAuthInfo.iProtocol == 
             aData2.iProtocolConfigOption.iAuthInfo.iProtocol                  ) &&
           ( aData1.iProtocolConfigOption.iAuthInfo.iUsername == 
             aData2.iProtocolConfigOption.iAuthInfo.iUsername                  ) &&
           ( aData1.iProtocolConfigOption.iAuthInfo.iPassword == 
             aData2.iProtocolConfigOption.iAuthInfo.iPassword                  ) &&
           ( aData1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == 
             aData2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns            ) &&
           ( aData1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == 
             aData2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );
    } 
    
EXPORT_C TBool TComparator<RPacketContext::TContextConfigR99_R4>::
            IsEqual(const RPacketContext::TContextConfigR99_R4& aData1, 
                    const RPacketContext::TContextConfigR99_R4& aData2)
	{
    RPacketContext::TContextConfigGPRS data1_gprs;
	RPacketContext::TContextConfigGPRS data2_gprs;

	data1_gprs.iPdpType              = aData1.iPdpType;
	data1_gprs.iAccessPointName      = aData1.iAccessPointName;
	data1_gprs.iPdpCompression       = 0;//not in TContextConfigR99_R4
	data1_gprs.iAnonymousAccessReqd  = RPacketContext::ENotApplicable; //not in TContextConfigR99_R4
	data1_gprs.iUseEdge              = aData1.iUseEdge;
	data1_gprs.iProtocolConfigOption = aData1.iProtocolConfigOption;
	data1_gprs.iNWIContext           = aData1.iNWIContext;
	
	data2_gprs.iPdpType              = aData2.iPdpType;
	data2_gprs.iAccessPointName      = aData2.iAccessPointName;
	data2_gprs.iPdpCompression       = 0;//not in TContextConfigR99_R4
	data2_gprs.iAnonymousAccessReqd  = RPacketContext::ENotApplicable; //not in TContextConfigR99_R4
	data2_gprs.iUseEdge              = aData2.iUseEdge;
	data2_gprs.iProtocolConfigOption = aData2.iProtocolConfigOption;
	data2_gprs.iNWIContext           = aData2.iNWIContext;
	
	return TComparator<RPacketContext::TContextConfigGPRS>::IsEqual(data1_gprs, data2_gprs) && (aData1.iPFI == aData2.iPFI);
    }  
    
EXPORT_C TBool TComparator<RPacketContext::TContextConfig_R5>::
            IsEqual(const RPacketContext::TContextConfig_R5& aData1, 
                    const RPacketContext::TContextConfig_R5& aData2)
    {
    return TComparator<RPacketContext::TContextConfigR99_R4>::IsEqual(aData1, aData2) &&
           ( aData1.iPdpHeaderCompression == aData2.iPdpHeaderCompression )           &&
           ( aData1.iPdpDataCompression   == aData2.iPdpDataCompression )           ;
    } 
    
EXPORT_C TBool TComparator<RMobilePhone::TMobilePhoneSecurityCode>::
            IsEqual(const RMobilePhone::TMobilePhoneSecurityCode& aData1, 
                    const RMobilePhone::TMobilePhoneSecurityCode& aData2)
    {
    return aData1 == aData2;
    } 
    
EXPORT_C TBool TComparator<RMobilePhone::TCodeAndUnblockCode>::
            IsEqual(const RMobilePhone::TCodeAndUnblockCode& aData1, 
                    const RMobilePhone::TCodeAndUnblockCode& aData2)
    {
    return (aData1.iCode == aData2.iCode) && (aData1.iUnblockCode == aData2.iUnblockCode);
    } 
    
EXPORT_C TBool TComparator<RMobilePhone::TAPNEntryV3>::
            IsEqual(const RMobilePhone::TAPNEntryV3& aData1, 
                    const RMobilePhone::TAPNEntryV3& aData2)
    {
    return (aData1.iApn == aData2.iApn);
    } 

EXPORT_C TBool TComparator<CMobilePhoneNetworkListV2*>::
            IsEqual(const CMobilePhoneNetworkListV2* /*aData1*/, 
                    const CMobilePhoneNetworkListV2* /* aData2*/)
    {
	
	// This Comparator is not used
	// but to prevent  ambiguity in case of using this Comparator
	// EFalse is returned
	
	// if you need to compare data of CMobilePhoneNetworkListV2* - type 
	// please, be free to fill in this Comparator 
	
    return EFalse;                
    }

EXPORT_C TBool TComparator<RMmCustomAPI::TSimFileInfo>::IsEqual(const RMmCustomAPI::TSimFileInfo& aData1,const RMmCustomAPI::TSimFileInfo& aData2)
    {
    TBool result = (0 == aData1.iPath.Compare(aData2.iPath));
    result &= aData1.iOffSet == aData2.iOffSet;
    result &= aData1.iSize == aData2.iSize;    
    
    return result;                
    }
  

//******************************************************* 

_LIT(KEmpty, "");

/**
TMockLtsyPhoneBookData0
*/
EXPORT_C TMockLtsyPhoneBookData0::TMockLtsyPhoneBookData0()
    :iPhoneBookName(KEmpty)
    {
    }
    
EXPORT_C TMockLtsyPhoneBookData0::TMockLtsyPhoneBookData0(TName &aPhoneBookName)
    :iPhoneBookName(aPhoneBookName)
    {
    }
    
EXPORT_C TName& TMockLtsyPhoneBookData0::PhoneBookName() 
    {
    return iPhoneBookName;
    }
    
EXPORT_C TBool TMockLtsyPhoneBookData0::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyPhoneBookData0 data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
        return EFalse;
    return (iPhoneBookName==data.iPhoneBookName);
    }
        
EXPORT_C void TMockLtsyPhoneBookData0::SerialiseL(RBuf8& aBuffer) const
    {
    RDesWriteStream stream;
    aBuffer.Create(sizeof(TName));
    stream.Open(aBuffer);
    stream << static_cast<TName>(iPhoneBookName);
    stream.Close();
    }
    
EXPORT_C void TMockLtsyPhoneBookData0::DeserialiseL(const TDesC8& aPackedData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);
    TName val;
    stream >> val;
    iPhoneBookName = val;
    stream.Close();
    }
 

//********************************************


EXPORT_C TBool TComparator< BookStoreEntryArrayPtr >::IsEqual(const BookStoreEntryArrayPtr &aData1, 
                    const BookStoreEntryArrayPtr &aData2)
    {    

    if((aData1 != NULL) && (aData2 != NULL))
        {        
        if(aData1->Count() != aData2->Count())
            return EFalse;

        TBool result(ETrue);
        
        for ( TInt index = 0; index < aData1->Count(); index++ )
            {
            result &= TComparator<CPhoneBookStoreEntry*>::IsEqual(aData1->At(index), aData2->At(index));    
            }    

        return result;
        }
    else
        {
        return (aData1 == aData2);
        }
    }


EXPORT_C TBool TComparator<IAndEPtr>::IsEqual(const IAndEPtr &aData1, const IAndEPtr &aData2)
    {
    if((aData1 != NULL) && (aData2 != NULL))
        {        
        TBool result(ETrue);

        result &= (aData1->iIndex == aData2->iIndex);
        result &= (aData1->iNumSlots == aData2->iNumSlots);

        return result;    
        }
    else
        {
        return (aData1 == aData2);
        }
    }

EXPORT_C TBool TComparator<StoreEntryPtr>::IsEqual(const StoreEntryPtr &aData1, const StoreEntryPtr &aData2)
    {   
        
    if((aData1 != NULL) && (aData2 != NULL))
    	{    
    	   	    		
    	if (aData1->iLocation != aData2->iLocation)
    	    {    	        
		    return EFalse;
    	    }
		
		if(aData1->iText && aData2->iText)
            {
        	if (0 != aData1->iText->Compare(*(aData2->iText)))
        	    {            	        
        		return EFalse;
        	    }
            }
        else
            {
        	if (aData1->iText != aData2->iText)
        	    {            	        
        		return EFalse;
        	    }
            }
	
	
		if(aData1->iNumber && aData2->iNumber)
            {
        	if (0 != aData1->iNumber->Compare(*(aData2->iNumber)))
        	    {            	        
        		return EFalse;
        	    }
            }
        else
            {
        	if (aData1->iNumber != aData2->iNumber)
        	    {            	        
        		return EFalse;
        	    }
            }
					
		if ( !TComparator<DesCArrayPtr>::IsEqual(aData1->iEmail, aData2->iEmail))
		    {		        
			return EFalse;	
		    }
		
		if ( !TComparator<DesCArrayPtr>::IsEqual(aData1->iSne, aData2->iSne))
		    {		        
			return EFalse;
		    }
		
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS

        CArrayFixFlat<TPhBkAdditionalEntry>* iAnr;
        
        if (aData1->iAnr != NULL && aData1->iAnr != NULL)
        	{
    		if (aData1->iAnr->Count() != aData2->iAnr->Count())
    		    {        		        
    			return EFalse;
    		    }
    		    
    		for (TInt i = 0; i < aData1->iAnr->Count(); i++)
    			{
				if ( *(aData1->iAnr)[i].iAlphaTag != *(aData2->iAnr)[i].iAlphaTag) )
				    {        				        
					return EFalse;
				    }
				
				if ( *(aData1->iAnr)[i].iTelNum != *(aData2->iAnr)[i].iTelNum) )
				    {        				        
					return EFalse;  		
				    }
    			}
        	}
        else 
        	{
    		if( aData1->iAnr != aData2->iAnr)
    		    {    		        
    			return EFalse;
    		    }
        	}        
#else
        if ( !TComparator<DesCArrayPtr>::IsEqual(aData1->iAnr, aData2->iAnr))
            {                
			return EFalse;
            }
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS

#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS
		if ( !TComparator<DesCArrayPtr>::IsEqual(aData1->iGrp, aData2->iGrp))
		    {		        
			return EFalse;
		    }
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS
		
		// if EVERYTHING is equal
		return ETrue;		
    	}
  	    
    // if aData1 or aData2 is NULL	    
    return (aData1 == aData2);     
                
    }
 
                    
TBool TComparator<DesCArrayPtr>::IsEqual(const DesCArrayPtr &aData1, const DesCArrayPtr &aData2)
    {
    
    if((aData1 != NULL) && (aData2 != NULL))
        {        
        TBool result(ETrue);

        if( aData1->Count() == aData2->Count())
            {
            for(TInt i = 0; i < aData1->Count(); i++)
                {
                TPtrC ptr1 = (*aData1)[i];
                TPtrC ptr2 = (*aData2)[i];
                result &= ( !ptr1.Compare(ptr2));
                }
            }
        else
            {
            return EFalse;
            }

        return result;    
        }
    else
        {
        return (aData1 == aData2);
        }
    }
      
//********************************************

/**
TPacketQoSGPRSNegotiated
*/

EXPORT_C TPacketQoSGPRSNegotiated::TPacketQoSGPRSNegotiated() 
    : TQoSGPRSNegotiated()
    {
    }

EXPORT_C TBool TPacketQoSGPRSNegotiated::operator == (const TPacketQoSGPRSNegotiated& aPackedData) const
    {
    return ( iPrecedence     == aPackedData.iPrecedence    ) &&
           ( iDelay          == aPackedData.iDelay         ) &&
           ( iReliability    == aPackedData.iReliability   ) &&
           ( iPeakThroughput == aPackedData.iPeakThroughput) &&
           ( iMeanThroughput == aPackedData.iMeanThroughput);
    }

EXPORT_C void TPacketQoSGPRSNegotiated::ExternalizeL(RWriteStream& aStream) const
    {
    aStream << static_cast<TInt32>(iPrecedence    );
    aStream << static_cast<TInt32>(iDelay         );
    aStream << static_cast<TInt32>(iReliability   );
    aStream << static_cast<TInt32>(iPeakThroughput);
    aStream << static_cast<TInt32>(iMeanThroughput);
    }

EXPORT_C void TPacketQoSGPRSNegotiated::InternalizeL(RReadStream& aStream)
    {
    TInt32 valPrecedence   ;
    TInt32 valDelay         ;
    TInt32 valReliability   ;
    TInt32 valPeakThroughput;
    TInt32 valMeanThroughput;
    
    aStream >> valPrecedence    ;
    aStream >> valDelay         ;
    aStream >> valReliability   ;
    aStream >> valPeakThroughput;
    aStream >> valMeanThroughput;
    
    iPrecedence     = static_cast<RPacketQoS::TQoSPrecedence    >(valPrecedence    );
    iDelay          = static_cast<RPacketQoS::TQoSDelay         >(valDelay         );
    iReliability    = static_cast<RPacketQoS::TQoSReliability   >(valReliability   );
    iPeakThroughput = static_cast<RPacketQoS::TQoSPeakThroughput>(valPeakThroughput);
    iMeanThroughput = static_cast<RPacketQoS::TQoSMeanThroughput>(valMeanThroughput);
    }

/**
TPacketQoSR99_R4Negotiated
*/

EXPORT_C TPacketQoSR99_R4Negotiated::TPacketQoSR99_R4Negotiated() 
    : TQoSR99_R4Negotiated()
    {
    }

EXPORT_C TBool TPacketQoSR99_R4Negotiated::operator == (const TPacketQoSR99_R4Negotiated& aPackedData) const
    {
    return (iTrafficClass                 == aPackedData.iTrafficClass                ) &&
           (iDeliveryOrderReqd            == aPackedData.iDeliveryOrderReqd           ) &&
           (iDeliverErroneousSDU          == aPackedData.iDeliverErroneousSDU         ) &&
           (iMaxSDUSize                      == aPackedData.iMaxSDUSize                  ) &&
           (iMaxRate.iUplinkRate          == aPackedData.iMaxRate.iUplinkRate         ) &&
           (iMaxRate.iDownlinkRate        == aPackedData.iMaxRate.iDownlinkRate       ) &&
           (iBER                          == aPackedData.iBER                         ) &&
           (iSDUErrorRatio                == aPackedData.iSDUErrorRatio               ) &&
           (iTrafficHandlingPriority      == aPackedData.iTrafficHandlingPriority     ) &&
           (iTransferDelay                  == aPackedData.iTransferDelay                  ) &&
           (iGuaranteedRate.iUplinkRate   == aPackedData.iGuaranteedRate.iUplinkRate  ) &&
           (iGuaranteedRate.iDownlinkRate == aPackedData.iGuaranteedRate.iDownlinkRate);
    
    }

EXPORT_C void TPacketQoSR99_R4Negotiated::ExternalizeL(RWriteStream& aStream) const
    {
    aStream << static_cast<TInt32>(iTrafficClass                );
    aStream << static_cast<TInt32>(iDeliveryOrderReqd           );
    aStream << static_cast<TInt32>(iDeliverErroneousSDU         );
    aStream << static_cast<TInt32>(iMaxSDUSize                   );
    aStream << static_cast<TInt32>(iMaxRate.iUplinkRate         );
    aStream << static_cast<TInt32>(iMaxRate.iDownlinkRate       );
    aStream << static_cast<TInt32>(iBER                         );
    aStream << static_cast<TInt32>(iSDUErrorRatio               );
    aStream << static_cast<TInt32>(iTrafficHandlingPriority     );
    aStream << static_cast<TInt32>(iTransferDelay               );
    aStream << static_cast<TInt32>(iGuaranteedRate.iUplinkRate  );
    aStream << static_cast<TInt32>(iGuaranteedRate.iDownlinkRate);
    }
    
EXPORT_C void TPacketQoSR99_R4Negotiated::InternalizeL(RReadStream& aStream)
    {
    TInt32 valTrafficClass              ;
    TInt32 valDeliveryOrderReqd         ;
    TInt32 valDeliverErroneousSDU       ;
    TInt32 valMaxSDUSize                ;
    TInt32 valMaxRateUplinkRate         ;
    TInt32 valMaxRateDownlinkRate       ;
    TInt32 valBER                       ;
    TInt32 valSDUErrorRatio             ;
    TInt32 valTrafficHandlingPriority   ;
    TInt32 valTransferDelay             ;
    TInt32 valGuaranteedRateUplinkRate  ;
    TInt32 valGuaranteedRateDownlinkRate;
    
    aStream >> valTrafficClass              ;
    aStream >> valDeliveryOrderReqd         ;
    aStream >> valDeliverErroneousSDU       ;
    aStream >> valMaxSDUSize                ;
    aStream >> valMaxRateUplinkRate         ;
    aStream >> valMaxRateDownlinkRate       ;
    aStream >> valBER                       ;
    aStream >> valSDUErrorRatio             ;
    aStream >> valTrafficHandlingPriority   ;
    aStream >> valTransferDelay             ;
    aStream >> valGuaranteedRateUplinkRate  ;
    aStream >> valGuaranteedRateDownlinkRate;
    
    iTrafficClass            = static_cast<RPacketQoS::TTrafficClass>(valTrafficClass);
    iDeliveryOrderReqd       = static_cast<RPacketQoS::TDeliveryOrder>(valDeliveryOrderReqd);
    iDeliverErroneousSDU     = static_cast<RPacketQoS::TErroneousSDUDelivery>(valDeliverErroneousSDU);
    iMaxSDUSize                 = static_cast<TInt>(valMaxSDUSize);
    iMaxRate.iUplinkRate     = static_cast<TInt>(valMaxRateUplinkRate);
    iMaxRate.iDownlinkRate   = static_cast<TInt>(valMaxRateDownlinkRate);
    iBER                     = static_cast<RPacketQoS::TBitErrorRatio>(valBER);
    iSDUErrorRatio           = static_cast<RPacketQoS::TSDUErrorRatio>(valSDUErrorRatio);
    iTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(valTrafficHandlingPriority);
    iTransferDelay                  = static_cast<TInt>(valTransferDelay);
    iGuaranteedRate.iUplinkRate   = static_cast<TInt>(valGuaranteedRateUplinkRate);
    iGuaranteedRate.iDownlinkRate = static_cast<TInt>(valGuaranteedRateDownlinkRate);
    }

/**
TPacketQoSGPRSNegotiated
*/

EXPORT_C TPacketQoSR5Negotiated::TPacketQoSR5Negotiated() 
    : TQoSR5Negotiated()
    {
    }

EXPORT_C TBool TPacketQoSR5Negotiated::operator == (const TPacketQoSR5Negotiated& aPackedData) const
    {
    return ( iSignallingIndication        == aPackedData.iSignallingIndication        ) &&
           ( iSourceStatisticsDescriptor  == aPackedData.iSourceStatisticsDescriptor  ) &&
           (iTrafficClass                 == aPackedData.iTrafficClass                ) &&
           (iDeliveryOrderReqd            == aPackedData.iDeliveryOrderReqd           ) &&
           (iDeliverErroneousSDU          == aPackedData.iDeliverErroneousSDU         ) &&
           (iMaxSDUSize                   == aPackedData.iMaxSDUSize                  ) &&
           (iMaxRate.iUplinkRate          == aPackedData.iMaxRate.iUplinkRate         ) &&
           (iMaxRate.iDownlinkRate        == aPackedData.iMaxRate.iDownlinkRate       ) &&
           (iBER                          == aPackedData.iBER                         ) &&
           (iSDUErrorRatio                == aPackedData.iSDUErrorRatio               ) &&
           (iTrafficHandlingPriority      == aPackedData.iTrafficHandlingPriority     ) &&
           (iTransferDelay                == aPackedData.iTransferDelay               ) &&
           (iGuaranteedRate.iUplinkRate   == aPackedData.iGuaranteedRate.iUplinkRate  ) &&
           (iGuaranteedRate.iDownlinkRate == aPackedData.iGuaranteedRate.iDownlinkRate);
    }

EXPORT_C void TPacketQoSR5Negotiated::ExternalizeL(RWriteStream& aStream) const
    {
    aStream << static_cast<TInt32>(iSignallingIndication      );
    aStream << static_cast<TInt32>(iSourceStatisticsDescriptor);
    aStream << static_cast<TInt32>(iTrafficClass                );
    aStream << static_cast<TInt32>(iDeliveryOrderReqd           );
    aStream << static_cast<TInt32>(iDeliverErroneousSDU         );
    aStream << static_cast<TInt32>(iMaxSDUSize                  );
    aStream << static_cast<TInt32>(iMaxRate.iUplinkRate         );
    aStream << static_cast<TInt32>(iMaxRate.iDownlinkRate       );
    aStream << static_cast<TInt32>(iBER                         );
    aStream << static_cast<TInt32>(iSDUErrorRatio               );
    aStream << static_cast<TInt32>(iTrafficHandlingPriority     );
    aStream << static_cast<TInt32>(iTransferDelay               );
    aStream << static_cast<TInt32>(iGuaranteedRate.iUplinkRate  );
    aStream << static_cast<TInt32>(iGuaranteedRate.iDownlinkRate);
    }
    
EXPORT_C void TPacketQoSR5Negotiated::InternalizeL(RReadStream& aStream)
    {
    TInt32 valSignallingIndication;
    TInt32 valSourceStatisticsDescriptor;
    TInt32 valTrafficClass              ;
    TInt32 valDeliveryOrderReqd         ;
    TInt32 valDeliverErroneousSDU       ;
    TInt32 valMaxSDUSize                ;
    TInt32 valMaxRateUplinkRate         ;
    TInt32 valMaxRateDownlinkRate       ;
    TInt32 valBER                       ;
    TInt32 valSDUErrorRatio             ;
    TInt32 valTrafficHandlingPriority   ;
    TInt32 valTransferDelay             ;
    TInt32 valGuaranteedRateUplinkRate  ;
    TInt32 valGuaranteedRateDownlinkRate;
    
    aStream >> valSignallingIndication      ;
    aStream >> valSourceStatisticsDescriptor;
    aStream >> valTrafficClass              ;
    aStream >> valDeliveryOrderReqd         ;
    aStream >> valDeliverErroneousSDU       ;
    aStream >> valMaxSDUSize                ;
    aStream >> valMaxRateUplinkRate         ;
    aStream >> valMaxRateDownlinkRate       ;
    aStream >> valBER                       ;
    aStream >> valSDUErrorRatio             ;
    aStream >> valTrafficHandlingPriority   ;
    aStream >> valTransferDelay             ;
    aStream >> valGuaranteedRateUplinkRate  ;
    aStream >> valGuaranteedRateDownlinkRate;
    
    iSignallingIndication       = static_cast<TBool>(valSignallingIndication);
    iSourceStatisticsDescriptor = static_cast<RPacketQoS::TSourceStatisticsDescriptor>(valSourceStatisticsDescriptor);
    iTrafficClass               = static_cast<RPacketQoS::TTrafficClass>(valTrafficClass);
    iDeliveryOrderReqd          = static_cast<RPacketQoS::TDeliveryOrder>(valDeliveryOrderReqd);
    iDeliverErroneousSDU        = static_cast<RPacketQoS::TErroneousSDUDelivery>(valDeliverErroneousSDU);
    iMaxSDUSize                 = static_cast<TInt>(valMaxSDUSize);
    iMaxRate.iUplinkRate        = static_cast<TInt>(valMaxRateUplinkRate);
    iMaxRate.iDownlinkRate      = static_cast<TInt>(valMaxRateDownlinkRate);
    iBER                        = static_cast<RPacketQoS::TBitErrorRatio>(valBER);
    iSDUErrorRatio              = static_cast<RPacketQoS::TSDUErrorRatio>(valSDUErrorRatio);
    iTrafficHandlingPriority    = static_cast<RPacketQoS::TTrafficHandlingPriority>(valTrafficHandlingPriority);
    iTransferDelay              = static_cast<TInt>(valTransferDelay);
    iGuaranteedRate.iUplinkRate   = static_cast<TInt>(valGuaranteedRateUplinkRate);
    iGuaranteedRate.iDownlinkRate = static_cast<TInt>(valGuaranteedRateDownlinkRate);
    }

//********************************************


/**
TMockLtsyContextData0
*/
EXPORT_C TMockLtsyContextData0::TMockLtsyContextData0()
    {
    }
    
EXPORT_C TMockLtsyContextData0::TMockLtsyContextData0(
                                            RPacketContext::TContextConfigGPRS& aConfig,
                                            TInt aContextConfigurationType)
    : iConfig                   (aConfig),
      iContextConfigurationType (aContextConfigurationType)
    {
    }

EXPORT_C TBool TMockLtsyContextData0::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyContextData0 data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
        return EFalse;
    return TComparator<RPacketContext::TContextConfigGPRS>::
           		IsEqual(this->iConfig, data.iConfig)                     &&
           ( iContextConfigurationType == data.iContextConfigurationType );
    }

EXPORT_C void TMockLtsyContextData0::SerialiseL(RBuf8& aBuffer) const
    {
   	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);

	TSerializer<RPacketContext::TContextConfigGPRS>::SerialiseL(iConfig, dataBuf);

	aBuffer.Create(dataBuf.Size() + sizeof(TInt));
	aBuffer.Append(dataBuf);

	TInt context = iContextConfigurationType;
	TPckg<TInt> contextPckg(context);
	aBuffer.Append(contextPckg);

    CleanupStack::PopAndDestroy( &dataBuf );
    }
    
EXPORT_C void TMockLtsyContextData0::DeserialiseL(const TDesC8& aPackedData)
    {
	TInt cursor = 0;
	
	if (aPackedData.Size())
		{
		TUint dataSize = aPackedData.Length() - sizeof(TInt);
	    TSerializer<RPacketContext::TContextConfigGPRS>::
	    	DeserialiseL(aPackedData.Mid(cursor, dataSize), iConfig);
	    cursor += dataSize;

		TInt context = 0;
		TPckg<TInt> contextPckg(context);
    	contextPckg.Copy(aPackedData.Mid(cursor, sizeof(TInt)));
    	iContextConfigurationType = context;
		}
    }

EXPORT_C RPacketContext::TContextConfigGPRS& TMockLtsyContextData0::Config()
    {
    return iConfig;    
    }
    
EXPORT_C TInt& TMockLtsyContextData0::ContextConfigurationType()
    {
    return iContextConfigurationType;    
    }

//********************************************

/**
TMockLtsyPacketQoSData0
*/
EXPORT_C TMockLtsyPacketQoSData0::TMockLtsyPacketQoSData0()
    {
    }
    
EXPORT_C TMockLtsyPacketQoSData0::TMockLtsyPacketQoSData0(
                                  TPacketQoSGPRSNegotiated  &   aQoSGPRSNegotiated,
                                  TPacketQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
                                  TPacketQoSR5Negotiated    &     aQoSR5_Negotiated
                                                         ) 
    : iQoSGPRSNegotiated  (aQoSGPRSNegotiated),
      iQoSR99_R4Negotiated(aQoSR99_R4Negotiated),
      iQoSR5_Negotiated   (aQoSR5_Negotiated)
    {
    }
                    
EXPORT_C TBool TMockLtsyPacketQoSData0::operator == (const TDesC8& aPackedData) const
    {
    TMockLtsyPacketQoSData0 data;
    TRAPD(err, data.DeserialiseL(aPackedData));
    if (err)
        return EFalse;
    return ( iQoSGPRSNegotiated   == data.iQoSGPRSNegotiated   ) &&
           ( iQoSR99_R4Negotiated == data.iQoSR99_R4Negotiated ) &&
           ( iQoSR5_Negotiated    == data.iQoSR5_Negotiated    );
    }

EXPORT_C void TMockLtsyPacketQoSData0::SerialiseL(RBuf8& aBuffer) const
    {
    RDesWriteStream stream;
    aBuffer.Create(sizeof(TPacketQoSGPRSNegotiated  ) + 
                   sizeof(TPacketQoSR99_R4Negotiated) + 
                   sizeof(TPacketQoSR5Negotiated    ));
    stream.Open(aBuffer);
    stream << static_cast<TPacketQoSGPRSNegotiated  > (iQoSGPRSNegotiated  );
    stream << static_cast<TPacketQoSR99_R4Negotiated> (iQoSR99_R4Negotiated);
    stream << static_cast<TPacketQoSR5Negotiated    > (iQoSR5_Negotiated   );
    stream.Close();
    }
    
EXPORT_C void TMockLtsyPacketQoSData0::DeserialiseL(const TDesC8& aPackedData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);
    TPacketQoSGPRSNegotiated   valQoSGPRSNegotiated;
    TPacketQoSR99_R4Negotiated valQoSR99_R4Negotiated;
    TPacketQoSR5Negotiated     valiQoSR5_Negotiated;
    
    stream >> valQoSGPRSNegotiated;
    stream >> valQoSR99_R4Negotiated;
    stream >> valiQoSR5_Negotiated;
    
    iQoSGPRSNegotiated   = valQoSGPRSNegotiated  ;
    iQoSR99_R4Negotiated = valQoSR99_R4Negotiated;
    iQoSR5_Negotiated    = valiQoSR5_Negotiated  ;
    stream.Close();
    }

EXPORT_C TPacketQoSGPRSNegotiated& TMockLtsyPacketQoSData0::QoSGPRSNegotiated()
    {
    return iQoSGPRSNegotiated;
    }
    
EXPORT_C TPacketQoSR99_R4Negotiated& TMockLtsyPacketQoSData0::QoSR99_R4Negotiated()
    {
    return iQoSR99_R4Negotiated;
    }
    
EXPORT_C TPacketQoSR5Negotiated& TMockLtsyPacketQoSData0::QoSR5Negotiated()
    {
    return iQoSR5_Negotiated;
    }

EXPORT_C TBool TComparator<RMobilePhone::TMobilePhoneCFChangeV1>::IsEqual(const RMobilePhone::TMobilePhoneCFChangeV1& aData1, const RMobilePhone::TMobilePhoneCFChangeV1& aData2)
    {
	TBool ret = (aData1.iServiceGroup == aData2.iServiceGroup);
    ret = ret && (aData1.iAction == aData2.iAction);
    ret = ret && (aData1.iTimeout == aData2.iTimeout);
    ret = ret && (aData1.iNumber.iTypeOfNumber == aData2.iNumber.iTypeOfNumber);
    ret = ret && (aData1.iNumber.iNumberPlan == aData2.iNumber.iNumberPlan);
    ret = ret && (aData1.iNumber.iTelNumber == aData2.iNumber.iTelNumber);

    return ret;
    }
    
EXPORT_C TBool TComparator<TInfoName>::IsEqual(const TInfoName &aData1, const TInfoName &aData2)
    {
    return (aData1.Compare(aData2)==0);
    }
    
EXPORT_C TBool TComparator<TENStoreResponse>::IsEqual(const TENStoreResponse &aData1, const TENStoreResponse &aData2)
    {
    return (aData1.iECCNumber.Compare(aData2.iECCNumber)==0) &&
           (aData1.iLocation == aData2.iLocation);
    }
    
EXPORT_C TBool TComparator<TCbsCbmiAndLangAndFilter>::
                    IsEqual(const TCbsCbmiAndLangAndFilter &aData1,
                            const TCbsCbmiAndLangAndFilter &aData2)
    {
    TBool ret = (aData1.iSetting == aData2.iSetting);
    ret &= (aData1.iCbmiStorage.Compare(aData2.iCbmiStorage)==0);
    ret &= (aData1.iLanguageStorage.Compare(aData2.iLanguageStorage)==0);
    return ret;
    }
    
EXPORT_C TBool TComparator<RMobilePhone::TImsAuthenticateDataV5>::
                    IsEqual(const RMobilePhone::TImsAuthenticateDataV5 &aData1,
                            const RMobilePhone::TImsAuthenticateDataV5 &aData2)
    {
    TBool ret = (aData1.iAUTN.Compare(aData2.iAUTN)==0);
    ret &= (aData1.iRAND.Compare(aData2.iRAND)==0);
    return ret;
    }
    
EXPORT_C TBool TComparator<RMobileENStore::TEmergencyNumber>::
                     IsEqual(const RMobileENStore::TEmergencyNumber &aData1,
                             const RMobileENStore::TEmergencyNumber &aData2)
    {
    return (aData1.Compare(aData2)==0);
    }
       
    
EXPORT_C TBool TComparator<CArrayFixFlat<RPacketContext::TPacketFilterV2>*>::IsEqual(const CArrayFixFlat<RPacketContext::TPacketFilterV2>* aData1, const CArrayFixFlat<RPacketContext::TPacketFilterV2>* aData2)
    {
    TBool ret(EFalse);
    
    if (aData1) 
        {
        if (aData2)
            {
            ret = (aData1->Count()==aData2->Count());

            if (ret)
                {
                for (TInt i(0); i<aData1->Count(); i++)
                    {
                    ret &= (memcmp(&(aData1->At(i)),
                                   &(aData2->At(i)),
                                   sizeof(RPacketContext::TPacketFilterV2)) == 0);
                    }
                }
            }
        }
    else if (aData2){}
    else
        {
        ret = ETrue;
        }
    
    return ret;
    }


EXPORT_C TBool TComparator<RMobileONStore::TMobileONEntryV1Pckg*>::IsEqual(const RMobileONStore::TMobileONEntryV1Pckg* aData1, RMobileONStore::TMobileONEntryV1Pckg* aData2)
    {
    RMobileONStore::TMobileONEntryV1& entry1 = ( * (const_cast<RMobileONStore::TMobileONEntryV1Pckg* >(aData1) ))();
    RMobileONStore::TMobileONEntryV1& entry2 = ( * (const_cast<RMobileONStore::TMobileONEntryV1Pckg* >(aData2) ))();
    return (0 == memcmp( &entry1, &entry2, sizeof(entry1)));
    }

EXPORT_C TBool TComparator<RMobileONStore::TMobileONEntryV1>::IsEqual(const RMobileONStore::TMobileONEntryV1 aData1, RMobileONStore::TMobileONEntryV1 aData2)
    {
    TBool result = (aData1.iIndex == aData2.iIndex);
    result = result && (aData1.iMode == aData2.iMode);
    result = result && (aData1.iNumber.iNumberPlan == aData2.iNumber.iNumberPlan);
    result = result && (aData1.iNumber.iTelNumber == aData2.iNumber.iTelNumber);
    result = result && (aData1.iNumber.iTypeOfNumber == aData2.iNumber.iTypeOfNumber);
    result = result && (aData1.iService == aData2.iService);
    result = result && (aData1.iText == aData2.iText);
    return result;
    }


EXPORT_C TBool TComparator<RCall::TCommPort>::IsEqual(const RCall::TCommPort aData1, const RCall::TCommPort aData2)
    {
    
    TBool result(aData1.iCsy == aData2.iCsy);
    result &= (aData1.iPort == aData2.iPort);
    
    return result;    
    }    


EXPORT_C TBool TComparator<RMobilePhone::TMobilePhonePasswordChangeV2>::IsEqual(const RMobilePhone::TMobilePhonePasswordChangeV2 aData1, RMobilePhone::TMobilePhonePasswordChangeV2 aData2)
	{
	TBool res = ETrue;
	
	res = res && (aData1.iNewPassword.Compare(aData2.iNewPassword) == 0);
	res = res && (aData1.iVerifiedPassword.Compare(aData2.iVerifiedPassword) == 0);
	res = res && (aData1.iOldPassword.Compare(aData2.iOldPassword) == 0);
	return res;
	}
    
EXPORT_C TBool TComparator<TLockSettingPassword>::IsEqual(const TLockSettingPassword& aData1, const TLockSettingPassword& aData2)
	{
	TBool res = (aData1.iLock == aData2.iLock);
	res = res && (aData1.iSetting == aData2.iSetting);
	res = res && (aData1.iPassword == aData2.iPassword);
	return res;
	}
	

EXPORT_C void TConstructor<RPacketContext::CTFTMediaAuthorizationV3>::
		ConstructL(RPacketContext::CTFTMediaAuthorizationV3*& aData)
	{
   	aData = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	}


EXPORT_C void TSerializer<RPacketContext::CTFTMediaAuthorizationV3>::
		SerialiseL(const RPacketContext::CTFTMediaAuthorizationV3& aData, RBuf8& aBuffer)
	{
	HBufC8* entry( NULL );
	aData.ExternalizeL( entry );
	CleanupStack::PushL( entry );

	aBuffer.CreateL( *entry );

	CleanupStack::PopAndDestroy( entry );
	}


EXPORT_C void TSerializer<RPacketContext::CTFTMediaAuthorizationV3>::
		DeserialiseL(const TDesC8& aPackedData, RPacketContext::CTFTMediaAuthorizationV3& aData)
	{
	TPtr8 packedDataPtr(const_cast<TUint8*>( aPackedData.Ptr() ), aPackedData.Size(), aPackedData.Size());
	aData.InternalizeL( packedDataPtr );
	}
	
	
EXPORT_C void TSerializer<TContextConfiguration>::SerialiseL(const TContextConfiguration& aData, RBuf8& aBuffer)
	{
	TInt size = sizeof(TInt32) + sizeof(TInfoName); //iContextName
	size += sizeof(TInt32) + sizeof(RPacketContext::TProtocolType); //iConfiguration
	size += sizeof(TInt32) + sizeof(RPacketQoS::TQoSGPRSRequested); //iQosParams97
	size += sizeof(TInt32) + sizeof(RPacketQoS::TQoSR99_R4Requested); //iQosParams99
	size += sizeof(TInt32) + sizeof(RPacketQoS::TQoSR5Requested); //iQosParamsR5
	size += sizeof(TInt32) + sizeof(TPacketFilterFilterOperation); //iTftOperation
	
	RDesWriteStream stream;
    aBuffer.Create(size);
    stream.Open(aBuffer);
    CleanupClosePushL(stream);  
        
    //serialise iContextName
	TPckgBuf<TInfoName> contextNamePck(aData.iContextName);
	stream << contextNamePck;    

	//serialise iConfiguration
	TPckgBuf<RPacketContext::TProtocolType> protTypePck(aData.iConfiguration);
	stream << protTypePck;
	
	//serialise iQosParams97
	if(aData.iQosParams97)
		{
		TBool isData97(ETrue);
    	TPckgBuf <TBool> isDataPckg(isData97);				
		stream << isDataPckg;
		
		TPckgBuf<RPacketQoS::TQoSGPRSRequested> qosparam97Pck(*aData.iQosParams97);
		stream << qosparam97Pck;
		}
	else
		{
		TBool isData97(EFalse);
    	TPckgBuf <TBool> isDataPckg(isData97);
		stream << isDataPckg;
		}
	
	//serialise iQosParams99
	if(aData.iQosParams99)
		{
		TBool isData99(ETrue);
    	TPckgBuf <TBool> isDataPckg(isData99);
		stream << isDataPckg;
		
		TPckgBuf<RPacketQoS::TQoSR99_R4Requested> qosparam99Pck(*aData.iQosParams99);
		stream << qosparam99Pck;
		}
	else
		{
		TBool isData99(EFalse);
    	TPckgBuf <TBool> isDataPckg(isData99);
		stream << isDataPckg;
		}
	
	//serialise iQosParamsR5
	if(aData.iQosParamsR5)
		{
		TBool isDataR5(ETrue);
    	TPckgBuf <TBool> isDataPckg(isDataR5);
    	stream << isDataPckg;
		
		TPckgBuf<RPacketQoS::TQoSR5Requested> qosparamr5Pck(*aData.iQosParamsR5);
		stream << qosparamr5Pck;
		}
	else
		{
		TBool isDataR5(EFalse);
    	TPckgBuf <TBool> isDataPckg(isDataR5);
		stream << isDataPckg;
		}
		
	//ignored serialising arrays (iTftFilterArray and iFilterIdArray) for time being.
	//CArrayFixFlat<RPacketContext::TPacketFilterV2>* iTftFilterArray;
    //TInt iFilterIdArray[ KMaxNumberOfTftFilters ];
	
	//serialise iTftOperation
	TPckgBuf<TPacketFilterFilterOperation> tftOperationPck(aData.iTftOperation);
	stream << tftOperationPck;

	CleanupStack::PopAndDestroy(); //stream

	}





EXPORT_C void TSerializer<TContextConfiguration>::DeserialiseL(const TDesC8& aPackedData, TContextConfiguration& aData)
	{
	RDesReadStream stream;
    stream.Open(aPackedData);
        
    //deserialise iContextName
    TPckg<TInfoName> contextNamePck(aData.iContextName);
    stream >> contextNamePck;
    
    //deserialise iConfiguration
    TPckg<RPacketContext::TProtocolType> configurationPck(aData.iConfiguration);
    stream >> configurationPck;
    
    //deserialise iQosParams97
    TPckgBuf <TBool> isDataPckg97;    
    stream >> isDataPckg97;
    TBool isData97(isDataPckg97());
    if (isData97)
    	{
    	aData.iQosParams97 = new(ELeave) RPacketQoS::TQoSGPRSRequested;
    	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsRequested(*aData.iQosParams97);
    	stream >> qosGprsRequested;
    	}
    else
    	{
    	aData.iQosParams97 = NULL;
    	}
    
    
   	//deserialise iQosParams99
    TPckgBuf <TBool> isDataPckg99;
    stream >> isDataPckg99;
    TBool isData99(isDataPckg99());
    if (isData99)
    	{    
    	aData.iQosParams99 = new(ELeave) RPacketQoS::TQoSR99_R4Requested;
    	TPckg<RPacketQoS::TQoSR99_R4Requested> qos99Requested(*aData.iQosParams99);
    	stream >> qos99Requested;
    	}
    else
    	{
    	aData.iQosParams99 = NULL;
    	}
    
    //deserialise iQosParamsR5
    TPckgBuf <TBool> isDataPckgR5;    
    stream >> isDataPckgR5;
    TBool isDataR5(isDataPckgR5());
    if (isDataR5)
    	{    
    	aData.iQosParamsR5 = new(ELeave) RPacketQoS::TQoSR5Requested;
    	TPckg<RPacketQoS::TQoSR5Requested> qos5Requested(*aData.iQosParamsR5);
    	stream >> qos5Requested;
    	}
    else
    	{
    	aData.iQosParamsR5 = NULL;
    	}
   	
   //deserialise iTftOperation
    TPckg<TPacketFilterFilterOperation> tftOperationPck(aData.iTftOperation);
    stream >> tftOperationPck;
    
    stream.Close();
	}
	

	
// TDestroyer for TContextConfiguration
EXPORT_C void TDestroyer<TContextConfiguration>::Destroy(TContextConfiguration& aData)
	{
  	if(aData.iQosParams97)
	  	{
	  	delete aData.iQosParams97;
	  	aData.iQosParams97 = NULL;
	  	}
	  	
	if(aData.iQosParams99)
	  	{
	  	delete aData.iQosParams99;
	  	aData.iQosParams99 = NULL;
	  	}
	
	if(aData.iQosParamsR5)
	  	{
	  	delete aData.iQosParamsR5;
	  	aData.iQosParamsR5 = NULL;
	  	}  		
	}

//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for TOperatorLogo
//------------------------------------------------------------------------------

EXPORT_C void TSerializer<RMmCustomAPI::TOperatorLogo>::SerialiseL(const RMmCustomAPI::TOperatorLogo& aData, RBuf8& aBuffer)
    {  
    TPckgBuf<RMmCustomAPI::TProgrammableOperatorLogoInfo> logoInfo(aData.iLogoInfo);
    
    TUint operatorLogoSize = aData.iOperatorLogo->Size();
    TUint sizeOfMaxLengthData =  sizeof(TInt);
    TUint sizeOfLogoInfo = sizeof(logoInfo);
    TUint totalBufferSize = operatorLogoSize + sizeOfMaxLengthData + sizeOfLogoInfo;

    RDesWriteStream stream;
    aBuffer.Create(totalBufferSize);
    stream.Open(aBuffer);
    CleanupClosePushL(stream);

    stream << logoInfo;
    
    stream.WriteUint32L(operatorLogoSize);
    stream.WriteL(*aData.iOperatorLogo);
    
    CleanupStack::PopAndDestroy(); //stream

    }


EXPORT_C void TSerializer<RMmCustomAPI::TOperatorLogo>::DeserialiseL(const TDesC8& aPackedData, RMmCustomAPI::TOperatorLogo& aData)
    {
    RDesReadStream stream;
    stream.Open(aPackedData);
   
    TPckg<RMmCustomAPI::TProgrammableOperatorLogoInfo> logoInfo(aData.iLogoInfo);
    stream >> logoInfo;
   
    TUint sizeOfDescriptor = stream.ReadUint32L();
    RBuf8* operatorLogoPtr = new (ELeave) RBuf8;
    
    operatorLogoPtr->CreateL(sizeOfDescriptor);
    operatorLogoPtr->Copy(aPackedData.Right(sizeOfDescriptor));
          
    aData.iOperatorLogo = operatorLogoPtr;
    aData.iOperatorLogo->SetLength(sizeOfDescriptor);
    
    stream.Close();
    } 


// TDestroyer for TOperatorLogo
EXPORT_C void TDestroyer<RMmCustomAPI::TOperatorLogo>::Destroy(RMmCustomAPI::TOperatorLogo&  aData)
	{
  	RBuf8* dataPtr = reinterpret_cast<RBuf8*> (aData.iOperatorLogo);
	dataPtr->Close();
	
  	delete dataPtr;

	}

		
//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for  CPhoneBookStoreEntry*
//------------------------------------------------------------------------------
EXPORT_C void TSerializer<CPhoneBookStoreEntry*>::
		SerialiseL(const CPhoneBookStoreEntry* aPhoneBookStoreEntry, RBuf8& aBuffer)
	{
	
    // ------------------------- calculate the necessary size of aBuffer -----------------------------------
    TUint size (0);
     
    size += sizeof (TUint8);// first byte should be "Type byte" (see comments under declaration of EDataTypes) 
    // TUint16 iLocation;
    size += sizeof (TUint16);
    
    // HBufC* iText;
    size += sizeof (TInt32);  // place where iText length will be put 
    
    if (aPhoneBookStoreEntry->iText)
        {
        size += aPhoneBookStoreEntry->iText->Size();
        }
    
    // HBufC* iNumber;
    size += sizeof (TInt32);  // place where iNumber length will be put
    if (aPhoneBookStoreEntry->iNumber)
        {
        size += aPhoneBookStoreEntry->iNumber->Size();
        }
      
    // CDesCArray* iEmail;
    size += sizeof (TInt32);  // place where iEmail length will be put
    if (aPhoneBookStoreEntry->iEmail)
        {
        size += aPhoneBookStoreEntry->iEmail->Count() * sizeof (TInt32); //place where length of iEmail's entries  will be put
        size += aPhoneBookStoreEntry->iEmail->Count() * aPhoneBookStoreEntry->iEmail->Length();
        for (TInt i = 0; i < aPhoneBookStoreEntry->iEmail->Count(); i++)
           {
           TPtrC ptr ( (*aPhoneBookStoreEntry->iEmail)[i] );
           size += ptr.Length() * sizeof (TUint16);
           }           
        }
        
    // CDesCArray* iSne;
    size += sizeof (TInt32);  // place iSne count will be put
    if (aPhoneBookStoreEntry->iSne)
        {
        size += aPhoneBookStoreEntry->iSne->Count() * sizeof (TInt32); //place where length of iSne's entries  will be put
        size += aPhoneBookStoreEntry->iSne->Count() * aPhoneBookStoreEntry->iSne->Length();
        for (TInt i = 0; i < aPhoneBookStoreEntry->iSne->Count(); i++)
           {
           TPtrC ptr ( (*aPhoneBookStoreEntry->iSne)[i] );
           size += ptr.Length() * sizeof (TUint16);
           }
        }


 //  CArrayFixFlat<TPhBkAdditionalEntry>* iAnr (or CDesCArray* iAnr;)
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS   
    size += sizeof (TInt32);  // place for iAnr count
    if (aPhoneBookStoreEntry->iAnr)
        {
        size += aPhoneBookStoreEntry->iAnr->Count() * sizeof (TPhBkAdditionalEntry);         
        } 
#else
    size += sizeof (TInt32);  // place iAnr count will be put
    if (aPhoneBookStoreEntry->iAnr)
        { 
        size += aPhoneBookStoreEntry->iAnr->Count() * sizeof (TInt32); //place where length of iAnr's entries  will be put
        size += aPhoneBookStoreEntry->iAnr->Count() * aPhoneBookStoreEntry->iAnr->Length();
        for (TInt i = 0; i < aPhoneBookStoreEntry->iAnr->Count(); i++)
           {
           TPtrC ptr ( (*aPhoneBookStoreEntry->iAnr)[i] );
           size += ptr.Length() * sizeof (TUint16);
           }
        
        }
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS

 //  CDesCArray* iGrp;
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS          
 	size += sizeof (TInt32);
    if (aPhoneBookStoreEntry->iGrp)
    {  
    size += aPhoneBookStoreEntry->iGrp->Count() * sizeof (TInt32);
    size += aPhoneBookStoreEntry->iGrp->Count() * aPhoneBookStoreEntry->iGrp->Length();  
    for (TInt i = 0; i < aPhoneBookStoreEntry->iGrp->Count(); i++)
           {
           TPtrC ptr ( (*aPhoneBookStoreEntry->iGrp)[i] );
           size += ptr.Length() * sizeof (TUint16);
           }       
    }
      
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS       
 // ------------------------------------------------------------------------------------------------------


	RDesWriteStream stream;
    aBuffer.Create(size);
    stream.Open(aBuffer);
    CleanupClosePushL(stream);  
    
            		
	// first byte should be "Type byte" (see comments under declaration of EDataTypes)
	stream << static_cast<TUint8> (EptrPhoneBookStoreEntry); // means that we use CPhoneBookStoreEntry*
		
	// --- TUint16 iLocation; ---
	stream << static_cast<TUint16> (aPhoneBookStoreEntry->iLocation);
	
     // --- HBufC* iText itself ---
    if (aPhoneBookStoreEntry->iText)
    {
        // Length of HBufC* iText
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iText->Des().Length() ); 
        
        for (TInt i = 0; i < aPhoneBookStoreEntry->iText->Des().Length(); i++ ) 
           {            
           stream << static_cast<TUint16>  ( (aPhoneBookStoreEntry->iText->Des())[i] );
           }
    }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        }
            
    // --- HBufC* iNumber; itself ---
    if (aPhoneBookStoreEntry->iNumber)
        {
        // Length of HBufC* iText
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iNumber->Des().Length() ); 
        for (TInt i = 0; i < aPhoneBookStoreEntry->iNumber->Des().Length(); i++ ) 
            {                
            stream << static_cast<TUint16>  ( (aPhoneBookStoreEntry->iNumber->Des())[i] );
            }
        }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        }

     
    // --- CDesCArray* iEmail; ---
    if (aPhoneBookStoreEntry->iEmail)
        {       
        // Count of  iEmail entries
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iEmail->Count() ); 
        
        for (TInt i = 0; i < aPhoneBookStoreEntry->iEmail->Count(); i++)
           {
           TPtrC ptr ( (*aPhoneBookStoreEntry->iEmail)[i] );
            
            // Length of ptr
           stream << static_cast<TInt32> (ptr.Length()); 
           
           for (TInt j = 0; j < ptr.Length(); j++)
               {
               stream << static_cast<TUint16> (ptr[j]);
               }   
           }
        }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        }
     
      
    // --- CDesCArray* iSne; ---
    if (aPhoneBookStoreEntry->iSne)
        {  
        // Count of  iSne entries
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iSne->Count() ); 
        for (TInt i = 0; i < aPhoneBookStoreEntry->iSne->Count(); i++)
            {
            TPtrC ptr ( (*aPhoneBookStoreEntry->iSne)[i] );
            
            // Length of ptr
            stream << static_cast<TInt32> (ptr.Length()); 
            
            for (TInt j = 0; j < ptr.Length(); j++)
                {
                stream << static_cast<TUint16> (ptr[j]);
                }   
            }
        }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        }
 
 // --- CDesCArray* iAnr; ( or CArrayFixFlat<TPhBkAdditionalEntry>* iAnr;)---
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS

    // Count of  iAnr entries
    if (aPhoneBookStoreEntry->iAnr)
        { 
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iAnr->Count() );    
        for (TInt i = 0; i < aPhoneBookStoreEntry->iAnr->Count(); i++)
            {
            stream << static_cast<TBuf<KMaxAlphaStringLength>>((*aPhoneBookStoreEntry->iAnr)[i].iAlphaTag); 
            stream << static_cast<TBuf<KPBStoreNumSize>>((*aPhoneBookStoreEntry->iAnr)[i].iTelNum);       
            }
        }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        }
#else
    // Count of  iAnr entries
    if (aPhoneBookStoreEntry->iAnr)
        {     
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iAnr->Count() ); 
        for (TInt i = 0; i < aPhoneBookStoreEntry->iAnr->Count(); i++)
            {
                TPtrC ptr ( (*aPhoneBookStoreEntry->iAnr)[i] );
   
                // Length of ptr
                stream << static_cast<TInt32> (ptr.Length()); 
                for (TInt j = 0; j < ptr.Length(); j++)
                    {
                    stream << static_cast<TUint16> (ptr[j]);
                    }   
            }
        }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        }     
        
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS

 // --- CDesCArray* iGrp; ---
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS       
    if (aPhoneBookStoreEntry->iGrp)
        {  
        // Count of  iGrp entries
        stream << static_cast<TInt32> ( aPhoneBookStoreEntry->iGrp->Count() ); 
        
        for (TInt i = 0; i < aPhoneBookStoreEntry->iGrp->Count(); i++)
            {
            TPtrC ptr ( (*aPhoneBookStoreEntry->iGrp)[i] );
             
             // Length of ptr
            stream << static_cast<TInt32> (ptr.Length()); 
            for (TInt j = 0; j < ptr.Length(); j++)
                {
                stream << static_cast<TUint16> (ptr[j]);
                }   
            }
        }
    else
        {
        TInt32 zero(0);
        stream << static_cast<TInt32> (zero);
        } 
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS

	CleanupStack::PopAndDestroy(); //stream	
	}
	
EXPORT_C void TSerializer<CPhoneBookStoreEntry*>::
		DeserialiseL(const TDesC8& aPackedData, CPhoneBookStoreEntry*& aPhoneBookStoreEntry)
	{
	
	TDestroyer<CPhoneBookStoreEntry*>::Destroy(aPhoneBookStoreEntry);
    TInitialiser<CPhoneBookStoreEntry*>::Initialise(aPhoneBookStoreEntry);
	aPhoneBookStoreEntry = new( ELeave)CPhoneBookStoreEntry;
	aPhoneBookStoreEntry->ConstructL();
	
	RDesReadStream stream;
    stream.Open(aPackedData);
    CleanupClosePushL(stream);
    
    // get "Type byte" (see comments under declaration of EDataTypes)
    TUint8 dataType;
	stream >> dataType;   
    
    // --- get TUint16 iLocation;  ---
    stream >> aPhoneBookStoreEntry->iLocation;
    
    // --- get HBufC* iText; ---
    // Length of HBufC* iText
    TInt32 length;
    stream >> length;
    
    if (length > 0)
    	{
    	 // get HBufC*    
    	if( aPhoneBookStoreEntry->iText )
    	    {
    	    User::Leave(KErrArgument);
    	    }
    	aPhoneBookStoreEntry->iText = HBufC::NewL(length);
    	aPhoneBookStoreEntry->iText->Des().SetLength(length);
    
    	for (TInt i = 0; i < length; i++ )	
	        {            
			stream >> (aPhoneBookStoreEntry->iText->Des())[i];		
        	}
    	}        
	
	
 	// --- get HBufC* iNumber; ---
    // Length of HBufC* iNumber  
    stream >> length;
    
    if (length > 0)
    	{
    	// get HBufC*    
    	if( aPhoneBookStoreEntry->iNumber )
    	    {
    	    User::Leave(KErrArgument);
    	    }
    	aPhoneBookStoreEntry->iNumber = HBufC::NewL(length);
    
		aPhoneBookStoreEntry->iNumber->Des().SetLength(length);

    	for (TInt i = 0; i < length; i++ )	
        	{            
			stream >> (aPhoneBookStoreEntry->iNumber->Des())[i];		
        	}
    	}
           
    
    // --- get CDesCArray* iEmail; ---
    // Count of iEmail's entries 
    TInt32 count; 
    stream >> count;
    for (TInt i = 0; i < count; i++)
    	{
		// Length of des
		TInt32 desLength;
		stream >> desLength;
		HBufC* hBuf =  HBufC::NewL(desLength);
		hBuf->Des().SetLength(desLength);
		CleanupStack::PushL (hBuf);
		
		for (TInt j = 0; j < desLength; j++)
			{
			stream >> (hBuf->Des())[j];
			}
		
		aPhoneBookStoreEntry->iEmail->AppendL(hBuf->Des());
		CleanupStack::Pop (hBuf);
		delete hBuf;	
    	}
    	
    // --- get CDesCArray* iSne; ---
    // Count of iSne's entries      
    stream >> count;
    for (TInt i = 0; i < count; i++)
    	{
		// Length of des
		TInt32 desLength;
		stream >> desLength;
		HBufC* hBuf =  HBufC::NewL(desLength);
		hBuf->Des().SetLength(desLength);
		
		CleanupStack::PushL (hBuf);
		
		for (TInt j = 0; j < desLength; j++)
			{
			stream >> (hBuf->Des())[j];
			}
		
		aPhoneBookStoreEntry->iSne->AppendL(hBuf->Des());
		CleanupStack::Pop (hBuf);
		delete hBuf;	
    	}
   
    
    // --- get CDesCArray* iAnr; ( or CArrayFixFlat<TPhBkAdditionalEntry>* iAnr;)---
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS
	// Count of  iAnr entries
	stream >> count;   
    for (TInt i = 0; i < count; i++)
    	{
		stream >> *(aPhoneBookStoreEntry->iAnr)[i].iAlphaTag;	
		stream >> *(aPhoneBookStoreEntry->iAnr)[i].iTelNum;	    		
    	}
#else
  	stream >> count;
    for (TInt i = 0; i < count; i++)
    	{
		// Length of des
		TInt32 desLength;
		stream >> desLength;
		HBufC* hBuf =  HBufC::NewL(desLength);
		CleanupStack::PushL (hBuf);
		
		hBuf->Des().SetLength(desLength);
		
		for (TInt j = 0; j < desLength; j++)
			{
			stream >> (hBuf->Des())[j];
			}
		
		aPhoneBookStoreEntry->iAnr->AppendL(hBuf->Des());
		CleanupStack::Pop (hBuf);
		delete hBuf;	
    	}
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS

	// --- get CDesCArray* iGrp; ---
#ifdef RD_USIM_PHONEBOOK_GAS_AND_AAS       
    stream >> count;
    for (TInt i = 0; i < count; i++)
    	{
		// Length of des
		TInt32 desLength;
		stream >> desLength;
		HBufC* hBuf =  HBufC::NewL(desLength);
		
		hBuf->Des().SetLength(desLength);
		
		CleanupStack::PushL (hBuf);
		
		for (TInt j = 0; j < desLength; j++)
			{
			stream >> (hBuf->Des())[j];
			}
		
		aPhoneBookStoreEntry->iGrp->AppendL(hBuf->Des());
		CleanupStack::Pop (hBuf);
		delete hBuf;	
    	}
#endif //RD_USIM_PHONEBOOK_GAS_AND_AAS

	CleanupStack::PopAndDestroy(); //stream	
	
	}
	
	

// TDestroyer for CArrayPtrSeg<CPhoneBookStoreEntry>*
EXPORT_C void TDestroyer<CPhoneBookStoreEntry*>::
		Destroy(CPhoneBookStoreEntry*& aData)
	{
    if (aData)
    	{    
    	delete aData;
    	aData = NULL;
    	}
	}
	

//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for  CArrayPtrSeg<CPhoneBookStoreEntry>*
//------------------------------------------------------------------------------
EXPORT_C void TSerializer<CArrayPtrSeg<CPhoneBookStoreEntry>*>::
		SerialiseL(const CArrayPtrSeg<CPhoneBookStoreEntry>* aPhoneBookStoreEntry, RBuf8& aBuffer)
	{	
	   	
		TInt sizeOfSerialisedData(1);
		
		// ------------------------- calculate the necessary size of aBuffer -----------------------------------
		// get entries count of CArrayPtrSeg
		TInt32 cnt = aPhoneBookStoreEntry->Count();
		sizeOfSerialisedData += sizeof(TUint8);
		sizeOfSerialisedData += sizeof(TInt32);
		
		
		for (TInt i = 0; i < cnt; i++) // goes through all entries
			{
			RBuf8 tempData;
			CleanupClosePushL(tempData);
						
			// was ExtSerialiseHelper::SerialiseL ( (*aPhoneBookStoreEntry).At(i), tempData);		
			TSerializer<CPhoneBookStoreEntry*>::SerialiseL((*aPhoneBookStoreEntry).At(i), tempData);
		
			sizeOfSerialisedData += sizeof(TInt32); // for size of this entry		
			sizeOfSerialisedData += tempData.Size()*sizeof(TInt32);									
		
			CleanupStack::PopAndDestroy(); // tempData
			}	
		
		
		// ------------------------- ---------------------------------------------------------------------------
		
		RDesWriteStream stream;
    	aBuffer.Create(sizeOfSerialisedData); // create with min size (for entries count of CArrayPtrSeg)
    	stream.Open(aBuffer);
    	CleanupClosePushL(stream);
			
		
		
		// first byte should be "Type byte" (see comments under declaration of EDataTypes)
//		stream << static_cast<TUint8> (EArrayPtrSegOfPhoneBookStoreEntry); // means that we use CArrayPtrSeg<CPhoneBookStoreEntry>*
		stream.WriteUint8L(static_cast<TUint8> (EArrayPtrSegOfPhoneBookStoreEntry));
	
		// --- put  entries count of CArrayPtrSeg into aBuffer  ---		
//		stream << static_cast<TInt32> (cnt);			
		stream.WriteInt32L(cnt);
		
		for (TInt i = 0; i < cnt; i++) // goes through all entries
			{
			RBuf8 tempData;
			CleanupClosePushL(tempData);
									
			// was ExtSerialiseHelper::SerialiseL ( (*aPhoneBookStoreEntry).At(i), tempData);	
			TSerializer<CPhoneBookStoreEntry*>::SerialiseL((*aPhoneBookStoreEntry).At(i), tempData);	
										
			// put size of this entry into aBuffer
//			stream << static_cast<TInt32> (tempData.Size());
			stream.WriteInt32L(static_cast<TInt32> (tempData.Size()));
			// put this entry into aBuffer 
			stream << tempData;
			
			CleanupStack::PopAndDestroy(); // tempData
			}	
					
		
		CleanupStack::PopAndDestroy(); //stream
	}
	

EXPORT_C void TSerializer<CArrayPtrSeg<CPhoneBookStoreEntry>*>::
		DeserialiseL(const TDesC8& aPackedData, CArrayPtrSeg<CPhoneBookStoreEntry>*& aPhoneBookStoreEntry)
	{	
	 	
		TDestroyer<CArrayPtrSeg<CPhoneBookStoreEntry>*>::Destroy(aPhoneBookStoreEntry);
        TInitialiser<CArrayPtrSeg<CPhoneBookStoreEntry>*>::Initialise(aPhoneBookStoreEntry);
		aPhoneBookStoreEntry = new( ELeave)CArrayPtrSeg<CPhoneBookStoreEntry>(1);
		
		RDesReadStream stream;
    	stream.Open(aPackedData);
    	CleanupClosePushL(stream);
    	
    	
    	//get first "Type byte" (see comments under declaration of EDataTypes)
//		stream >> static_cast<TUint8> (dataType);
		stream.ReadUint8L();
    	
    	// --- get  entries count of CArrayPtrSeg into aBuffer  ---	
    	TInt32 cnt;    	
//    	stream >> static_cast<TInt32> (cnt);
		cnt = stream.ReadInt32L();
    	
    	// --- get all other Serialised data from serialisedData ---
    	for (TInt i = 0; i < cnt; i++)
    		{
			RBuf8 currentEntry;
			CleanupClosePushL(currentEntry);
			TInt32 sizeOfCurrentEntry(0);
			// get size of CurrentEntry
//			stream >> static_cast<TInt32> (sizeOfCurrentEntry);
			sizeOfCurrentEntry = stream.ReadInt32L();
			
			currentEntry.Create(sizeOfCurrentEntry);
			currentEntry.SetLength(sizeOfCurrentEntry);
			
			// fill currentEntry		
			stream >> currentEntry;
				
			// create new CPhoneBookStoreEntry 
			CPhoneBookStoreEntry* entryPtr = new(ELeave) CPhoneBookStoreEntry;
			CleanupStack::PushL(entryPtr);
			entryPtr->ConstructL();				
			// and Append it to Array			
			aPhoneBookStoreEntry->AppendL(entryPtr);				
			
			// Deserialise currentEntry to corresponding aPhoneBookStoreEntry entry
			TSerializer<CPhoneBookStoreEntry*>::DeserialiseL(currentEntry, (*aPhoneBookStoreEntry).At(i));
			// was
			//ExtSerialiseHelper::DeserialiseL(*aPhoneBookStoreEntry->At(i), currentEntry);
			
			CleanupStack::Pop(entryPtr);
			CleanupStack::PopAndDestroy(1); //currentEntry
    		}
    		
    	CleanupStack::PopAndDestroy(1); //stream
	}

// TDestroyer for CArrayPtrSeg<CPhoneBookStoreEntry>*
EXPORT_C void TDestroyer<CArrayPtrSeg<CPhoneBookStoreEntry>*>::
		Destroy(CArrayPtrSeg<CPhoneBookStoreEntry>*& aData)
	{
    if (aData)
    	{
    	aData->Reset();
    	delete aData;
    	aData = NULL;
    	}
	}

EXPORT_C void TSerializer<RPacketContext::CTFTMediaAuthorizationV3*>::
		SerialiseL(const RPacketContext::CTFTMediaAuthorizationV3* aData, RBuf8& aBuffer)
	{
	if (aData)
		{
		HBufC8* entry( NULL );
		TUint dataSize = 0;
		TPckg<TUint> sizeData(dataSize);

		aData->ExternalizeL( entry );
		CleanupStack::PushL( entry );

		aBuffer.CreateL(sizeof(TUint) + entry->Size());

		dataSize = entry->Size();
		aBuffer.Append( sizeData );
		aBuffer.Append( *entry );

		CleanupStack::PopAndDestroy( entry );
		}
	else
		{
		aBuffer.CreateL(KNullDesC8);
		}

	}

EXPORT_C void TSerializer<RPacketContext::CTFTMediaAuthorizationV3*>::
		DeserialiseL(const TDesC8& aPackedData, RPacketContext::CTFTMediaAuthorizationV3*& aData)
	{
	TInt cursor(0);
	TInt uintLen = sizeof(TUint);

	if (aData)
		{
		delete aData;
		aData = NULL;
		}

	RPacketContext::CTFTMediaAuthorizationV3* entry = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL( entry );
			
	TUint entrySize(0);
	TPckg<TUint> entrySizeData(entrySize);
	entrySizeData.Copy( aPackedData.Mid(cursor, uintLen) );
	cursor = uintLen;
			
	TPtr8 entryDataPtr(const_cast<TUint8*>( aPackedData.Ptr() + cursor ), entrySize, entrySize);
	entry->InternalizeL( entryDataPtr );

	aData = entry; 
	CleanupStack::Pop( entry );
	}
	
	
	
//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for  CMobilePhoneNetworkListV2*
//------------------------------------------------------------------------------	

EXPORT_C void TSerializer<CMobilePhoneNetworkListV2*>::
		SerialiseL(const CMobilePhoneNetworkListV2* aData, RBuf8& aBuffer)
	{
	if (aData)
		{				
		CBufBase* storeData = ( const_cast<CMobilePhoneNetworkListV2*>(aData) )->StoreLC();	
		
    	aBuffer.Create(storeData->Ptr(0));
    	
    	CleanupStack::PopAndDestroy(); // storeData   	
    	}
	else
		{
		aBuffer.CreateL(KNullDesC8);
		}

	}

EXPORT_C void TSerializer<CMobilePhoneNetworkListV2*>::
		DeserialiseL(const TDesC8& aPackedData, CMobilePhoneNetworkListV2*& aData)
	{

	if (aData)
		{
		delete aData;
		aData = NULL;
		}		
		
	aData = CMobilePhoneNetworkListV2::NewL();	
	CleanupStack::PushL(aData);
	
	aData->RestoreL(aPackedData);	
	
	CleanupStack::Pop(); // aData 

	}

// TDestroyer for TSendSmsDataAndAttributes
EXPORT_C void TDestroyer<TSendSmsDataAndAttributes>::
		Destroy(TSendSmsDataAndAttributes& aData)
	{
	if(aData.iAttributes)
	    {
	    delete aData.iAttributes;
	    aData.iAttributes = NULL;
	    }
	    
	if(aData.iMsgData)
	    {
	    delete aData.iMsgData;
	    aData.iMsgData = NULL;
	    }
	}
	
	
//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for  RMobileSmsMessaging::TSendSmsDataAndAttributes
//------------------------------------------------------------------------------	
	
EXPORT_C void TSerializer<TSendSmsDataAndAttributes>::
		SerialiseL(const TSendSmsDataAndAttributes aData, RBuf8& aBuffer)
	{	
	
    TUint msgDataSize = aData.iMsgData->Size();
	
	TUint totalSize = sizeof(TSendSmsDataAndAttributes) + msgDataSize + 
	    (aData.iAttributes?sizeof(RMobileSmsMessaging::TMobileSmsSendAttributesV1):0) +
	        + sizeof(TBool);    // could be pointer to zero sized data

	aBuffer.CreateL(totalSize);

	TPckg<TSendSmsDataAndAttributes> datPckg(aData);
	aBuffer.Append(datPckg);  

    if(aData.iAttributes)
        {
    	TPckg<RMobileSmsMessaging::TMobileSmsSendAttributesV1> attrPckg(*aData.iAttributes);
	    aBuffer.Append(attrPckg);  
        }

    TBool haveMsg(EFalse);
   	TPckg<TBool> haveMsgPckg(haveMsg);
   	
    if(aData.iMsgData)
        {
        haveMsg = ETrue;	    
        }

    aBuffer.Append(haveMsgPckg);  

    if(aData.iMsgData)
        {
        aBuffer.Append(*aData.iMsgData);  
        }
	
	}


EXPORT_C void TSerializer<TSendSmsDataAndAttributes>::
		DeserialiseL(const TDesC8& aPackedData, TSendSmsDataAndAttributes& aData)
	{
	TDestroyer<TSendSmsDataAndAttributes>::Destroy(aData);
    TInitialiser<TSendSmsDataAndAttributes>::Initialise(aData);

	
	TPckg<TSendSmsDataAndAttributes> dataPkcg(aData);
	dataPkcg.Copy( aPackedData.Left(sizeof(TSendSmsDataAndAttributes)) );
	
	TUint cursor(sizeof(TSendSmsDataAndAttributes));
	
	if(aData.iAttributes)
	    {
	    aData.iAttributes = new(ELeave) RMobileSmsMessaging::TMobileSmsSendAttributesV1;
	    TPckg<RMobileSmsMessaging::TMobileSmsSendAttributesV1> attrPkcg(*aData.iAttributes);
	    
	    attrPkcg.Copy( aPackedData.Mid( cursor, sizeof(RMobileSmsMessaging::TMobileSmsSendAttributesV1)) );
	                                
	    cursor += sizeof(RMobileSmsMessaging::TMobileSmsSendAttributesV1);
	    }
	    
	TBool haveData(EFalse);
	TPckg<TBool> haveDataPckg(haveData);	
	haveDataPckg.Copy( aPackedData.Mid( cursor, sizeof(TBool)) );
	cursor += sizeof(TBool);
	
	if(haveData)
	    {
	    TUint sizeLeft(aPackedData.Size() - cursor);
        aData.iMsgData = reinterpret_cast<TDes8*> (new(ELeave)TName);
        aData.iMsgData->Copy( aPackedData.Right(sizeLeft) );
        }		
	}
	
		
	
EXPORT_C TBool TComparator<TSendSmsDataAndAttributes>::IsEqual(const TSendSmsDataAndAttributes& aData1, const TSendSmsDataAndAttributes& aData2)
	{
    TBool result = (aData1.iIpc == aData2.iIpc);
	
	if((NULL == aData1.iAttributes) || (NULL == aData2.iAttributes))
	    {
	    result = result && (aData1.iAttributes == aData2.iAttributes);
	    }
	else
	    {
	    result = result && (aData1.iAttributes->iMsgRef == aData2.iAttributes->iMsgRef);
	    result = result && (aData1.iAttributes->iMore == aData2.iAttributes->iMore);
	    result = result && (0 == aData1.iAttributes->iSubmitReport.Compare(aData2.iAttributes->iSubmitReport));
	    result = result && 
	      (aData1.iAttributes->iDestination.iTypeOfNumber == 
	          aData2.iAttributes->iDestination.iTypeOfNumber);
	    result = result && 
	      (aData1.iAttributes->iDestination.iNumberPlan == 
	          aData2.iAttributes->iDestination.iNumberPlan);

	    result = result && 
	      (0 == aData1.iAttributes->iDestination.iTelNumber.Compare(aData2.iAttributes->iDestination.iTelNumber));
	    }

	if((NULL == aData1.iMsgData) || (NULL == aData2.iMsgData))
	    {
	    result = result && (aData1.iMsgData == aData2.iMsgData);
	    }
	else
	    {
	    result = result && (0 == aData1.iMsgData->Compare(*aData2.iMsgData));
	    }
	
 	return result;
	}
	


// TDestroyer for TDesC8*
EXPORT_C void TDestroyer<TDesC8*>::Destroy(TDesC8* aData)
	{
	if(aData)
	    {
	    delete aData;
	    }
	}
	


//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for  TDesC8*
//------------------------------------------------------------------------------	
	
EXPORT_C void TSerializer<TDesC8*>::SerialiseL(const TDesC8* aData, RBuf8& aBuffer)
	{	

    TBool haveMsg(EFalse);
   	TPckg<TBool> haveMsgPckg(haveMsg);
   	
    if(aData)
        {
        haveMsg = ETrue;	    
	    aBuffer.Create(sizeof(TBool) + aData->Size());
        aBuffer.Append(haveMsgPckg);  
	    aBuffer.Append(*aData);  
        }
    else
        {
	    aBuffer.Create(sizeof(TBool));
        aBuffer.Append(haveMsgPckg);  
        }
	}


EXPORT_C void TSerializer<TDesC8*>::DeserialiseL(const TDesC8& aPackedData, TDesC8*& aData)
	{

	TBool haveData(EFalse);
	TPckg<TBool> haveDataPckg(haveData);	
	haveDataPckg.Copy( aPackedData.Left( sizeof(TBool)) );

    if(haveData)
        {
        TUint size = aPackedData.Size() - sizeof(TBool);
        HBufC8* tmpPtr = HBufC8::NewL(size);
        tmpPtr->Des().Copy( aPackedData.Mid( sizeof(TBool), size) );        
        aData = static_cast<TDesC8*>(tmpPtr);        
        }
    else
        {
        aData = NULL;
        }
	}
	

EXPORT_C TBool TComparator<TDesC8*>::IsEqual(const TDesC8* aData1, const TDesC8* aData2)
	{
	TBool result(ETrue);
	
	if((NULL == aData1) || (NULL == aData2))
	    {
	    result = (aData1 == aData2);
	    }
	else
	    {
	    result = (*aData1 == *aData2);
	    }
	
 	return result;
	}

EXPORT_C TBool TComparator<TContextParams>::IsEqual(const TContextParams& aData1, const TContextParams& aData2)
	{
	TBool result =  aData1.iPdpType       == aData2.iPdpType      ;
	      result &= aData1.iContextType   == aData2.iContextType  ;
 	      result &= aData1.iContextStatus == aData2.iContextStatus;
 	      
 	result &= TComparator<TInfoName>::IsEqual(aData1.iContextName, aData2.iContextName) &&
 	          TComparator<TInfoName>::IsEqual(aData1.iPrimaryContextName, aData2.iPrimaryContextName);
 	          
 	return result;
	}

EXPORT_C TBool TComparator<RMobilePhone::TMobilePhoneNetworkManualSelection>::
    IsEqual(const RMobilePhone::TMobilePhoneNetworkManualSelection& aData1, 
            const RMobilePhone::TMobilePhoneNetworkManualSelection& aData2)
    {
    return (aData1.iCountry.Compare(aData2.iCountry)==0) &&
    	   (aData1.iNetwork.Compare(aData2.iNetwork)==0);
    }


EXPORT_C TBool TComparator<RMobilePhone::TMobileName>::IsEqual(const RMobilePhone::TMobileName& aData1, const RMobilePhone::TMobileName& aData2)
    {
    return (aData1 == aData2);
    }


EXPORT_C void TConstructor<CMobilePhoneBroadcastIdList>::
		ConstructL(CMobilePhoneBroadcastIdList*& aData)
	{
   	aData = CMobilePhoneBroadcastIdList::NewL();
	}


EXPORT_C void TSerializer<CMobilePhoneBroadcastIdList>::
		SerialiseL(const CMobilePhoneBroadcastIdList& aData, RBuf8& aBuffer)
	{
	CBufSeg* buf = CBufSeg::NewL(4);
	CleanupStack::PushL( buf );
	
	RBufWriteStream stream(*buf);
	stream.PushL();
	aData.ExternalizeL(stream);
	CleanupStack::PopAndDestroy( &stream );
	
	aBuffer.CreateL( buf->Size() );
	buf->Read(0, aBuffer);
	
	CleanupStack::PopAndDestroy( buf );
	
	}


EXPORT_C void TSerializer<CMobilePhoneBroadcastIdList>::
		DeserialiseL(const TDesC8& aPackedData, CMobilePhoneBroadcastIdList& aData)
	{
	RDesReadStream stream(aPackedData);
	stream.PushL();
	
	aData.InternalizeL(stream);
	
	CleanupStack::PopAndDestroy( &stream );
	}


EXPORT_C TBool TComparator<CMobilePhoneBroadcastIdList>::
    IsEqual(const CMobilePhoneBroadcastIdList& aData1, 
            const CMobilePhoneBroadcastIdList& aData2)
    {
    return TComparator< CMobilePhoneEditableList
            <RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1>* >::
            IsEqual(&aData1, &aData2);
    }


EXPORT_C TBool TComparator<RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1>::
    IsEqual(const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aData1, 
            const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& aData2)
    {
    return TComparator<RMobilePhone::TMultimodeType>::IsEqual(aData1, aData2) &&
            (aData1.iId == aData2.iId);
    }




EXPORT_C void TConstructor<RMobilePhone::TMultimodeType>::
		ConstructL(RMobilePhone::TMultimodeType*& aData)
	{
   	aData = reinterpret_cast<RMobilePhone::TMultimodeType*>(new (ELeave) RMobilePhone::TMobilePhoneSecurityCodeInfoV5);
	}

// TDestroyer for CArrayPtrSeg<CPhoneBookStoreEntry>*
EXPORT_C void TDestroyer<RMobilePhone::TMultimodeType*>::
		Destroy(RMobilePhone::TMultimodeType* aData)
	{
    if (aData)
    	{    
    	delete reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCodeInfoV5*>(aData);
    	aData = NULL;
    	}
	}
	

EXPORT_C void TConstructor<CMobilePhoneSmspList>::
		ConstructL(CMobilePhoneSmspList*& aData)
	{
   	aData = CMobilePhoneSmspList::NewL();
	}

//***************************************************

EXPORT_C TBool TComparator<CMobilePhoneListBase>::
		IsEqual(const CMobilePhoneListBase& aData1,
				const CMobilePhoneListBase& aData2)
	{
	TBool res( EFalse );
	
	RBuf8 dataBuf1;
	RBuf8 dataBuf2;
	dataBuf1.CleanupClosePushL();
	dataBuf2.CleanupClosePushL();
	
	TSerializer<CMobilePhoneListBase>::SerialiseL(aData1, dataBuf1);
	TSerializer<CMobilePhoneListBase>::SerialiseL(aData2, dataBuf2);
	
	res = (dataBuf1 == dataBuf2);
	
	CleanupStack::PopAndDestroy(2); // dataBuf1, dataBuf2
	
	return res;
	}

EXPORT_C void TSerializer<CMobilePhoneListBase>::
		SerialiseL(const CMobilePhoneListBase& aData, RBuf8& aBuffer)
	{
	CBufBase* dataBuf = const_cast<CMobilePhoneListBase*>(&aData)->StoreLC();
	aBuffer.Create(dataBuf->Size());
	dataBuf->Read(0, aBuffer);
	CleanupStack::PopAndDestroy(dataBuf);
	}


EXPORT_C void TSerializer<CMobilePhoneListBase>::
		DeserialiseL(const TDesC8& aPackedData, CMobilePhoneListBase& aData)
	{
	aData.RestoreL(aPackedData);
	}

EXPORT_C TBool TComparator<CMobilePhoneCWList>::
		IsEqual(const CMobilePhoneCWList& aData1,
				const CMobilePhoneCWList& aData2)
	{
	return TComparator<CMobilePhoneListBase>::IsEqual(aData1, aData2);
	}

EXPORT_C void TSerializer<CMobilePhoneCWList>::
		SerialiseL(const CMobilePhoneCWList& aData, RBuf8& aBuffer)
	{
	TSerializer<CMobilePhoneListBase>::SerialiseL(aData, aBuffer);
	}


EXPORT_C void TSerializer<CMobilePhoneCWList>::
		DeserialiseL(const TDesC8& aPackedData, CMobilePhoneCWList& aData)
	{
	TSerializer<CMobilePhoneListBase>::DeserialiseL(aPackedData, aData);
	}

EXPORT_C void TConstructor<CMobilePhoneCWList>::
		ConstructL(CMobilePhoneCWList*& aData)
	{
	aData = CMobilePhoneCWList::NewL();
	}

EXPORT_C void TConstructor<CMobilePhoneCFList>::
		ConstructL(CMobilePhoneCFList*& aData)
	{
	aData = CMobilePhoneCFList::NewL();
	}

EXPORT_C TBool TComparator<CMobilePhoneCFList>::
		IsEqual(const CMobilePhoneCFList& aData1,
				const CMobilePhoneCFList& aData2)
	{
	return TComparator<CMobilePhoneListBase>::IsEqual(aData1, aData2);
	}

EXPORT_C void TSerializer<CMobilePhoneCFList>::
		SerialiseL(const CMobilePhoneCFList& aData, RBuf8& aBuffer)
	{
	TSerializer<CMobilePhoneListBase>::SerialiseL(aData, aBuffer);
	}


EXPORT_C void TSerializer<CMobilePhoneCFList>::
		DeserialiseL(const TDesC8& aPackedData, CMobilePhoneCFList& aData)
	{
	TSerializer<CMobilePhoneListBase>::DeserialiseL(aPackedData, aData);
	}


EXPORT_C TBool TComparator<RMmCustomAPI::TApdu>::
					IsEqual(const RMmCustomAPI::TApdu& aData1, const RMmCustomAPI::TApdu& aData2)
    {
    TBool result(ETrue);
    
    if(aData1.iInfo && aData2.iInfo)
    	{
    	result &= (0 == aData1.iInfo->Compare(*aData2.iInfo) );
       	}
    else
        {
        result &= (aData1.iInfo == aData2.iInfo);
        }           

    if(aData1.iData && aData2.iData)
    	{
    	result &= (0 == aData1.iData->Compare(*aData2.iData) );
       	}
    else
        {
        result &= (aData1.iData == aData2.iData);
        }           
    	
    return result;
    }

EXPORT_C TBool TComparator<RMmCustomAPI::TApduParameters>::
                    IsEqual(const RMmCustomAPI::TApduParameters& aData1, const RMmCustomAPI::TApduParameters& aData2)
    {
    TBool result(ETrue);
    
    result = (aData1.iCardReaderId == aData2.iCardReaderId);
    result &= (0 == aData1.iCmdData.Compare(aData2.iCmdData));
    result &= (0 == aData1.iRspData.Compare(aData2.iRspData));
        
    return result;
    }



EXPORT_C TBool TComparator<RMmCustomAPI::TEmerNumberCheckMode>::
    	IsEqual(const RMmCustomAPI::TEmerNumberCheckMode& aData1, const RMmCustomAPI::TEmerNumberCheckMode& aData2)
    {

    TBool result = (aData1.iCheckMode == aData2.iCheckMode);
    result = result && (0 == aData1.iNumber.Compare(aData2.iNumber));

	return result;
    }
    
    

EXPORT_C TBool TComparator<RMmCustomAPI::TSimLockPassword>::IsEqual(const RMmCustomAPI::TSimLockPassword& aData1, const RMmCustomAPI::TSimLockPassword& aData2)
    {
    return (0 == aData1.Compare(aData2));
    }    

EXPORT_C TBool TComparator<RMobilePhone::TMobilePassword>::IsEqual(const RMobilePhone::TMobilePassword& aData1, const RMobilePhone::TMobilePassword& aData2)
    {
    return (0 == aData1.Compare(aData2));
    }    

EXPORT_C TBool TComparator<RMmCustomAPI::TViagCacheRecordId>::
        IsEqual(const RMmCustomAPI::TViagCacheRecordId& aData1,
                const RMmCustomAPI::TViagCacheRecordId& aData2)
	{
	return (aData1.iCacheId == aData2.iCacheId) && (aData1.iRecordId == aData2.iRecordId);
	}

EXPORT_C TBool TComparator<RMmCustomAPI::TViagCacheRecordContent>::
        IsEqual(const RMmCustomAPI::TViagCacheRecordContent& aData1,
                const RMmCustomAPI::TViagCacheRecordContent& aData2)
	{
	return (aData1.iCellId == aData2.iCellId) && (aData1.iLac == aData2.iLac);
	}


EXPORT_C TBool TComparator<RMmCustomAPI::TMmCellInfo>::
        IsEqual(const RMmCustomAPI::TMmCellInfo& aData1,
                const RMmCustomAPI::TMmCellInfo& aData2)
	{
	TBool ret  = (aData1.iMode == aData2.iMode);

    ret = ret && (aData1.iGsmCellInfo.iTA == aData2.iGsmCellInfo.iTA);
	
	for( TInt i = 0; i < KMaxNmrAmount; i++)
		{
	    ret = ret && (aData1.iGsmCellInfo.iNmr[i].iARFCN == aData2.iGsmCellInfo.iNmr[i].iARFCN);
	    ret = ret && (aData1.iGsmCellInfo.iNmr[i].iBSIC == aData2.iGsmCellInfo.iNmr[i].iBSIC);
	    ret = ret && (aData1.iGsmCellInfo.iNmr[i].iRxLEV == aData2.iGsmCellInfo.iNmr[i].iRxLEV);
		}

    ret = ret && (aData1.iWcdmaCellInfo.iPrimaryScrambilingCode == aData2.iWcdmaCellInfo.iPrimaryScrambilingCode);

    ret = ret && (aData1.iWcdmaCellInfo.iFrequencyInfo.iFddUL == aData2.iWcdmaCellInfo.iFrequencyInfo.iFddUL);
    ret = ret && (aData1.iWcdmaCellInfo.iFrequencyInfo.iFddDL == aData2.iWcdmaCellInfo.iFrequencyInfo.iFddDL);
    ret = ret && (aData1.iWcdmaCellInfo.iFrequencyInfo.iTddNt == aData2.iWcdmaCellInfo.iFrequencyInfo.iTddNt);
	
	for( TInt i = 0; i < KMaxNetworkMeasureReports; i++)
		{
	    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCarrierRSSI 
	    		   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCarrierRSSI);

	    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iFrequencyInfo.iFddUL 
	    		   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iFrequencyInfo.iFddUL);
	    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iFrequencyInfo.iFddDL 
	    		   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iFrequencyInfo.iFddDL);
	    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iFrequencyInfo.iTddNt 
	    		   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iFrequencyInfo.iTddNt);
			
		for( TInt j = 0; j < KMaxCellMeasuredResults; j++)
			{
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iCID 
		   			== aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iCID);

		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iPrimaryCPICH 
		   		       == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iPrimaryCPICH);
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iCpichEcN0 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iCpichEcN0);
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iCpichRscp 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iCpichRscp);
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iPathloss 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iFddInfo.iPathloss);

		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iCellParamID 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iCellParamID);
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iProposedTGSN 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iProposedTGSN);
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iPrimaryCcpchRscp 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iPrimaryCcpchRscp);
		    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iPathloss 
		   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iPathloss);
								
			for(TInt k = 0; k < KMaxTimeSlotIscpAmount; k++)
				{
			    ret = ret && (aData1.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iTimeslotISCP[k] 
			   			   == aData2.iWcdmaCellInfo.iNwkMeasureReport[i].iCellMeasuredResult[j].iTddInfo.iTimeslotISCP[k]);
				}				
			}						
		}
	
	return ret;
	}

/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationEapSim
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationEapSim>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationEapSim& aData1,
                const RMmCustomAPI::TSimAuthenticationEapSim& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iRandomParameters.Compare(aData2.iRandomParameters)	||
		aData1.iSRES.Compare(aData2.iSRES)							||
		aData1.iKC.Compare(aData2.iKC) )
		{
		ret = EFalse;
		}
        
	return ret;
	}
	
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationEapAka
*/	
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationEapAka>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationEapAka& aData1,
                const RMmCustomAPI::TSimAuthenticationEapAka& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iRandomParameters.Compare(aData2.iRandomParameters)	||
		aData1.iAUTN.Compare(aData2.iAUTN)							||
		aData1.iRES.Compare(aData2.iRES)							||
		aData1.iCK.Compare(aData2.iCK) 								||
		aData1.iIK.Compare(aData2.iIK)								||
		aData1.iAUTS.Compare(aData2.iAUTS) )
		{
		ret = EFalse;
		}       

	return ret;
	}


EXPORT_C TBool TComparator<RMobilePhone::TAID>::
    IsEqual(const RMobilePhone::TAID& aData1, const RMobilePhone::TAID& aData2)
    {
    return (0 == aData1.Compare(aData2));
    }


EXPORT_C TBool TComparator<RMobilePhone::TAuthInfoV8>::
    IsEqual(const RMobilePhone::TAuthInfoV8& aData1, const RMobilePhone::TAuthInfoV8& aData2)
    {
    return ((0 == aData1.iAID.Compare(aData2.iAID)) && (aData1.iData == aData2.iData));
    }

	

EXPORT_C TBool TComparator<RMobilePhone::TMobilePhoneCspFileV1>::IsEqual(
		const RMobilePhone::TMobilePhoneCspFileV1& aData1,
		const RMobilePhone::TMobilePhoneCspFileV1& aData2)
	{
	TBool result = (aData1.iCallOfferingServices == aData2.iCallOfferingServices);
	result = result && (aData1.iCallRestrictionServices == aData2.iCallRestrictionServices);
	result = result && (aData1.iOtherSuppServices == aData2.iOtherSuppServices);
	result = result && (aData1.iCallCompletionServices == aData2.iCallCompletionServices);
	result = result && (aData1.iTeleservices == aData2.iTeleservices);
	result = result && (aData1.iCphsTeleservices == aData2.iCphsTeleservices);
	result = result && (aData1.iCphsFeatures == aData2.iCphsFeatures);
	result = result && (aData1.iNumberIdentServices == aData2.iNumberIdentServices);
	result = result && (aData1.iPhase2PlusServices == aData2.iPhase2PlusServices);
	result = result && (aData1.iValueAddedServices == aData2.iValueAddedServices);
	
	return result;
	}

EXPORT_C TBool TComparator<RMobilePhone::TMobilePhoneServiceTableV1>::IsEqual(
		const RMobilePhone::TMobilePhoneServiceTableV1& aData1,
		const RMobilePhone::TMobilePhoneServiceTableV1& aData2)
	{
	TBool result = (aData1.iServices1To8 == aData2.iServices1To8);
	result = result && (aData1.iServices9To16 == aData2.iServices9To16);
	result = result && (aData1.iServices17To24 == aData2.iServices17To24);
	result = result && (aData1.iServices25To32 == aData2.iServices25To32);
	result = result && (aData1.iServices33To40 == aData2.iServices33To40);
	result = result && (aData1.iServices41To48 == aData2.iServices41To48);
	result = result && (aData1.iServices49To56 == aData2.iServices49To56);
	
	return result;
	}

EXPORT_C TBool TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(const CPcktMbmsMonitoredServiceList& aData1, const CPcktMbmsMonitoredServiceList& aData2)
    {
    TInt size1 = aData1.Enumerate();
    TInt size2 = aData2.Enumerate();
    if(size1 != size2)
    	{
    	return EFalse;
    	}
    RPacketService::TMbmsServiceAvailabilityV1 entry1, entry2;
    for (TInt i=0; i < aData1.Enumerate();i++)
	    {
	    entry1 = aData1.GetEntryL(i);
	    entry2 = aData2.GetEntryL(i);
	   	if (entry1.iTmgi.GetServiceId() != entry2.iTmgi.GetServiceId())
	   		{
	   		return EFalse;
	   		}
	    if (entry1.iTmgi.GetMCC() != entry2.iTmgi.GetMCC() )
	    	{
	    	return EFalse;
	    	}
	    if (entry1.iTmgi.GetMNC() != entry2.iTmgi.GetMNC())
	    	{
	    	return EFalse;
	    	}
	    if (entry1.iMbmsServiceMode != entry2.iMbmsServiceMode)
	    	{
	    	return EFalse;
	    	}
	    if (entry1.iMbmsAccessBearer != entry2.iMbmsAccessBearer)
	    	{
	    	return EFalse;
	    	}
	  	if (entry1.iMbmsAvailabilityStatus != entry2.iMbmsAvailabilityStatus)
	  		{
	  		return EFalse;
	  		}
	    }
    
    return ETrue;
    }
    
EXPORT_C void TSerializer<TMmMbmsMonitorServiceListParams>::SerialiseL(const TMmMbmsMonitorServiceListParams& aData, RBuf8& aBuffer)
	{

	CBufSeg* buf = CBufSeg::NewL(4);
	CleanupStack::PushL( buf );
	
	RBufWriteStream stream(*buf);
	stream.PushL();
	stream.WriteInt32L(aData.iActionType);
	
	if(aData.iServiceList != NULL)
		{
		stream.WriteInt8L(1);
		aData.iServiceList->ExternalizeL(stream);
		}
	else
		{
		stream.WriteInt8L(0);
		}
	if(aData.iListOfChanges != NULL)
		{
		stream.WriteInt8L(1);
		aData.iListOfChanges->ExternalizeL(stream);
		}
	else
		{
		stream.WriteInt8L(0);
		}

	CleanupStack::PopAndDestroy( &stream );
	
	aBuffer.CreateL( buf->Size() );
	buf->Read(0, aBuffer);
	CleanupStack::PopAndDestroy( buf );
	}
	
EXPORT_C void TSerializer<TMmMbmsMonitorServiceListParams>::DeserialiseL(const TDesC8& aPackedData, TMmMbmsMonitorServiceListParams& aData)
	{
	RDesReadStream stream(aPackedData);
	stream.PushL();
	
	aData.iActionType = static_cast<TMbmsAction>(stream.ReadInt32L());
	if(stream.ReadInt8L()==0)
		{
		aData.iServiceList = NULL;
		}
	else
		{
		aData.iServiceList = CPcktMbmsMonitoredServiceList::NewL();
		CleanupStack::PushL(aData.iServiceList);
		aData.iServiceList->InternalizeL(stream);
		}
	if(stream.ReadInt8L()==0)
		{
		aData.iListOfChanges = NULL;
		}
	else
		{
		aData.iListOfChanges = CPcktMbmsMonitoredServiceList::NewL();
		CleanupStack::PushL(aData.iListOfChanges);
		aData.iListOfChanges->InternalizeL(stream);
		CleanupStack::Pop(1);
		}
	if(aData.iServiceList != NULL)
		{
		CleanupStack::Pop(1);
		}
	CleanupStack::PopAndDestroy( &stream );

	}
	
// TDestroyer for TMmMbmsMonitorServiceListParams
EXPORT_C void TDestroyer<TMmMbmsMonitorServiceListParams>::Destroy(TMmMbmsMonitorServiceListParams& /*aData*/)
	{
	}

EXPORT_C TBool TComparator<TMbmsAction>::
	IsEqual(const TMbmsAction& aData1, 
	const TMbmsAction& aData2)
	{
	return (aData1 == aData2);
	}
	
EXPORT_C TBool TComparator<TMbmsNetworkServiceStatus>::
	IsEqual(const TMbmsNetworkServiceStatus& aData1, 
	const TMbmsNetworkServiceStatus& aData2)
	{
	return (aData1 == aData2);
	}

/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationGbaBootstrap
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationGbaBootstrap>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationGbaBootstrap& aData1,
                const RMmCustomAPI::TSimAuthenticationGbaBootstrap& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iRandomParameters.Compare(aData2.iRandomParameters)	||
		aData1.iAUTN.Compare(aData2.iAUTN)							||
		aData1.iAUTS.Compare(aData2.iAUTS)							||
		aData1.iRES.Compare(aData2.iRES) )
		{
		ret = EFalse;
		}

	return ret;
	}
	

/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate& aData1,
                const RMmCustomAPI::TSimAuthenticationGbaBootstrapUpdate& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iRandomParameters.Compare(aData2.iRandomParameters)	||
		aData1.iKeyLifeTime.Compare(aData2.iKeyLifeTime)							||
		aData1.iBTid.Compare(aData2.iBTid) )
		{
		ret = EFalse;
		}
        
	return ret;
	}
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationGbaNafDerivation
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationGbaNafDerivation>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationGbaNafDerivation& aData1,
                const RMmCustomAPI::TSimAuthenticationGbaNafDerivation& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iNafId.Compare(aData2.iNafId)	||
		aData1.iImpi.Compare(aData2.iImpi)							||
		aData1.iKsExtNaf.Compare(aData2.iKsExtNaf) )
		{
		ret = EFalse;
		}

	return ret;
	}
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationMgvMskUpdate
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationMgvMskUpdate>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aData1,
                const RMmCustomAPI::TSimAuthenticationMgvMskUpdate& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iMikey.Compare(aData2.iMikey) )
		{
		ret = EFalse;
		}
        
	return ret;
	}
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationMgvMtkGeneration
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationMgvMtkGeneration>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aData1,
                const RMmCustomAPI::TSimAuthenticationMgvMtkGeneration& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iMikey.Compare(aData2.iMikey)	||
		aData1.iMtkSalt.Compare(aData2.iMtkSalt) )
		{
		ret = EFalse;
		}
        
	return ret;
	}
	
/**
TComparator Specialisation for RMmCustomAPI::TSimAuthenticationMgvMskDeletion
*/
EXPORT_C TBool TComparator<RMmCustomAPI::TSimAuthenticationMgvMskDeletion>::
        IsEqual(const RMmCustomAPI::TSimAuthenticationMgvMskDeletion& aData1,
                const RMmCustomAPI::TSimAuthenticationMgvMskDeletion& aData2)
	{
	TBool ret (ETrue);
	if (aData1.iKeyDomainId.Compare(aData2.iKeyDomainId)	||
		aData1.iKeyGroupIdPart.Compare(aData2.iKeyGroupIdPart) )
		{
		ret = EFalse;
		}
        
	return ret;
	}

EXPORT_C void TConstructor<CPcktMbmsMonitoredServiceList>::
		ConstructL(CPcktMbmsMonitoredServiceList*& aData)
	{
	aData = CPcktMbmsMonitoredServiceList::NewL();
	}


EXPORT_C void TSerializer<CPcktMbmsMonitoredServiceList>::
		SerialiseL(const CPcktMbmsMonitoredServiceList& aData, RBuf8& aBuffer)
	{
	CBufSeg* buf = CBufSeg::NewL(4);
	CleanupStack::PushL( buf );
	
	RBufWriteStream stream(*buf);
	stream.PushL();
	aData.ExternalizeL(stream);
	CleanupStack::PopAndDestroy( &stream );
	
	aBuffer.CreateL( buf->Size() );
	buf->Read(0, aBuffer);
	
	CleanupStack::PopAndDestroy( buf );
	}


EXPORT_C void TSerializer<CPcktMbmsMonitoredServiceList>::
		DeserialiseL(const TDesC8& aPackedData, CPcktMbmsMonitoredServiceList& aData)
	{
	RDesReadStream stream(aPackedData);
	stream.PushL();
	
	aData.InternalizeL(stream);
	
	CleanupStack::PopAndDestroy( &stream );
	}


EXPORT_C void TConstructor<RPacketMbmsContext::CMbmsSession>::
		ConstructL(RPacketMbmsContext::CMbmsSession*& aData)
	{
	aData = RPacketMbmsContext::CMbmsSession::NewL();
	}


EXPORT_C void TSerializer<RPacketMbmsContext::CMbmsSession>::
		SerialiseL(const RPacketMbmsContext::CMbmsSession& aData, RBuf8& aBuffer)
	{
	HBufC8* entry( NULL );
	const_cast<RPacketMbmsContext::CMbmsSession&>(aData).ExternalizeL( entry );
	CleanupStack::PushL( entry );

	aBuffer.CreateL( *entry );

	CleanupStack::PopAndDestroy( entry );
	}


EXPORT_C void TSerializer<RPacketMbmsContext::CMbmsSession>::
		DeserialiseL(const TDesC8& aPackedData, RPacketMbmsContext::CMbmsSession& aData)
	{
	TPtr8 packedDataPtr(const_cast<TUint8*>( aPackedData.Ptr() ), aPackedData.Size(), aPackedData.Size());
	aData.InternalizeL( packedDataPtr );
	}

/**
TComparator Specialisation for TMmMbmsActiveServiceListParams
*/
EXPORT_C TBool TComparator<TMmMbmsActiveServiceListParams>::
        IsEqual(const TMmMbmsActiveServiceListParams& aData1,
                const TMmMbmsActiveServiceListParams& aData2)
	{
	if(aData1.iActionType != aData2.iActionType)
		{
		return EFalse;
		}
	if(aData1.iChangedItem != aData2.iChangedItem)
		{
		return EFalse;
		}
	if(aData1.iServiceList == NULL)
		{
		if(aData2.iServiceList == NULL)
			{
			return ETrue;
			}
		return EFalse;
		}
	if(aData2.iServiceList == NULL)
		{
		return EFalse;
		}
	if(aData1.iServiceList->ExtensionId() != aData2.iServiceList->ExtensionId())
		{
		return EFalse;
		}
	/*
	 TInt count = aData1.iServiceList->iSessionIdList.Count();
	if(count != aData2.iServiceList->iSessionIdList.Count())
		{
		return EFalse;
		}
	for(TInt i = 0; i < count; ++i)
		{
		TBool found=EFalse;
		TUint num1 = aData1.iServiceList->iSessionIdList[i];
		for(TInt j = 0; (j < count) && (!found) ; ++j)
			{
			if(num1 == aData2.iServiceList->iSessionIdList[j])
				{
				found = ETrue;
				}
			}
		if(!found)
			{
			return EFalse;
			}
		}
		*/
	return ETrue;
	}
	
/**
TComparator Specialisation for TMmMbmsMonitorServiceListParams
*/
EXPORT_C TBool TComparator<TMmMbmsMonitorServiceListParams>::
        IsEqual(const TMmMbmsMonitorServiceListParams& aData1,
                const TMmMbmsMonitorServiceListParams& aData2)
	{
	if(aData1.iActionType != aData2.iActionType)
		{
		return EFalse;
		}
	if(aData1.iServiceList == NULL)
		{
		if(aData2.iServiceList != NULL)
			{
			return EFalse;
			}
		}
	else if(aData2.iServiceList == NULL)
		{
		return EFalse; 
		}
	else if(!TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*aData1.iServiceList,*aData2.iServiceList))
		{
		return EFalse;
		}
	if(aData1.iListOfChanges == NULL)
		{
		if(aData2.iListOfChanges != NULL)
			{
			return EFalse;
			}
		}
	else if(aData2.iListOfChanges == NULL)
		{
		return EFalse; 
		}
	else if(!TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*aData1.iListOfChanges,*aData2.iListOfChanges))
		{
		return EFalse;
		}
	return ETrue;
	}
	


EXPORT_C void TSerializer<TMmMbmsActiveServiceListParams>::SerialiseL(const TMmMbmsActiveServiceListParams& aData, RBuf8& aBuffer)
	{

	CBufSeg* buf = CBufSeg::NewL(4);
	CleanupStack::PushL( buf );
	
	RBufWriteStream stream(*buf);
	stream.PushL();
	stream.WriteInt32L(aData.iActionType);
	stream.WriteInt32L(aData.iChangedItem);
	
	if(aData.iServiceList != NULL)
		{
		stream.WriteInt8L(1);
		stream.WriteInt32L(aData.iServiceList->iExtensionId);
		TInt count = aData.iServiceList->iSessionIdList.Count();
		stream.WriteInt32L(count);
		for(TInt i = 0; i < count; ++i)
			{
			stream.WriteInt32L(aData.iServiceList->iSessionIdList[i]);
			}
		}
	else
		{
		stream.WriteInt8L(0);
		}

	CleanupStack::PopAndDestroy( &stream );
	
	aBuffer.CreateL( buf->Size() );
	buf->Read(0, aBuffer);
	CleanupStack::PopAndDestroy( buf );
	}
	
EXPORT_C void TSerializer<TMmMbmsActiveServiceListParams>::DeserialiseL(const TDesC8& aPackedData, TMmMbmsActiveServiceListParams& aData)
	{
	RDesReadStream stream(aPackedData);
	stream.PushL();
	
	aData.iActionType = static_cast<TMbmsAction>(stream.ReadInt32L());
	aData.iChangedItem = static_cast<TMbmsSessionId>(stream.ReadInt32L());
	if(stream.ReadInt8L()==0)
		{
		aData.iServiceList = NULL;
		}
	else
		{
		aData.iServiceList = RPacketMbmsContext::CMbmsSession::NewL();
		CleanupStack::PushL(aData.iServiceList);
		aData.iServiceList->iExtensionId = static_cast<TUint>(stream.ReadInt32L());
		TInt count = stream.ReadInt32L();
		for(TInt i = 0; i < count; ++i)
			{
			TUint num = static_cast<TUint>(stream.ReadInt32L());
			aData.iServiceList->iSessionIdList.Append(num);
			}
		CleanupStack::Pop(1);
		}
	CleanupStack::PopAndDestroy( &stream );

	}


EXPORT_C void TSerializer<CMobilePhoneSmspList>::
		SerialiseL(const CMobilePhoneSmspList& aData, RBuf8& aBuffer)
	{
	CBufSeg* buf = CBufSeg::NewL(4);
	CleanupStack::PushL( buf );
	
	RBufWriteStream stream(*buf);
	stream.PushL();
	aData.ExternalizeL(stream);
	CleanupStack::PopAndDestroy( &stream );
	
	aBuffer.CreateL( buf->Size() );
	buf->Read(0, aBuffer);
	
	CleanupStack::PopAndDestroy( buf );
	
	}


EXPORT_C void TSerializer<CMobilePhoneSmspList>::
		DeserialiseL(const TDesC8& aPackedData, CMobilePhoneSmspList& aData)
	{
	RDesReadStream stream(aPackedData);
	stream.PushL();
	
	aData.InternalizeL(stream);
	
	CleanupStack::PopAndDestroy( &stream );
	}

EXPORT_C TBool TComparator<CMobilePhoneSmspList>::
    IsEqual(const CMobilePhoneSmspList& aData1, 
            const CMobilePhoneSmspList& aData2)
    {
    return TComparator< CMobilePhoneEditableList
            <RMobileSmsMessaging::TMobileSmspEntryV1>* >::
            IsEqual(&aData1, &aData2);
    }


EXPORT_C TBool TComparator<RMobileSmsMessaging::TMobileSmspEntryV1>::
    IsEqual(const RMobileSmsMessaging::TMobileSmspEntryV1& aData1, 
            const RMobileSmsMessaging::TMobileSmspEntryV1& aData2)
    {
    return TComparator<RMobilePhone::TMultimodeType>::IsEqual(aData1, aData2) &&
            (aData1.iIndex == aData2.iIndex);
    }

/**
TComparator specialisation for RMobileUssdMessaging::TMobileUssdAttributesV1
*/

EXPORT_C TBool TComparator<RMobileUssdMessaging::TMobileUssdAttributesV1>::IsEqual(const RMobileUssdMessaging::TMobileUssdAttributesV1& aData1, const RMobileUssdMessaging::TMobileUssdAttributesV1& aData2)
	{
	if(aData1.iFlags != aData2.iFlags)
		{
		return EFalse;
		}
	if(aData1.iFormat != aData2.iFormat)
		{
		return EFalse;
		}
	if(aData1.iType != aData2.iType)
		{
		return EFalse;
		}
	if (aData1.iDcs != aData2.iDcs)
		{
		return EFalse;
		}
	return ETrue;
    }

/**
TComparator specialisation for RMobilePhone::TMobilePhoneCBChangeV1
*/

EXPORT_C TBool TComparator<RMobilePhone::TMobilePhoneCBChangeV1>::IsEqual(const RMobilePhone::TMobilePhoneCBChangeV1& aData1, const RMobilePhone::TMobilePhoneCBChangeV1& aData2)
	{
	if(aData1.iServiceGroup != aData2.iServiceGroup)
		{
		return EFalse;
		}
	if(aData1.iAction != aData2.iAction)
		{
		return EFalse;
		}
	return TComparator<RMobilePhone::TMobilePassword>::IsEqual(aData1.iPassword, aData2.iPassword);
    }

EXPORT_C TBool TComparator<RMobilePhone::TMobileAddress>::IsEqual(const RMobilePhone::TMobileAddress& aData1, const RMobilePhone::TMobileAddress& aData2)
	{
	TBool result; 
    result = (aData1.iTypeOfNumber == aData2.iTypeOfNumber);
    result &= (aData1.iNumberPlan == aData2.iNumberPlan);
    result &= (aData1.iTelNumber == aData2.iTelNumber);
    return result;
	}

EXPORT_C TBool TComparator<RPacketContext::TMiscProtocolBuffer>::IsEqual(const RPacketContext::TMiscProtocolBuffer &aData1, 
                    const RPacketContext::TMiscProtocolBuffer &aData2)
	{	
	return (aData1.Compare(aData2)==0);
	}

EXPORT_C TBool TComparator<RPacketContext::TGSNAddress>::IsEqual(const RPacketContext::TGSNAddress &aData1, 
                    const RPacketContext::TGSNAddress &aData2)
	{	
	return (aData1.Compare(aData2)==0);
	}

EXPORT_C TBool TComparator<RPacketContext::TProtocolAddress>::IsEqual(const RPacketContext::TProtocolAddress &aData1, 
                    const RPacketContext::TProtocolAddress &aData2)
	{	
	return (aData1.Compare(aData2)==0);
	}

EXPORT_C TBool TComparator<RMobilePhone::TMobilePhonePasswordChangeV1>::IsEqual(const RMobilePhone::TMobilePhonePasswordChangeV1 aData1, RMobilePhone::TMobilePhonePasswordChangeV1 aData2)
	{
	TBool res = ETrue;
	
	res = res && (aData1.iNewPassword.Compare(aData2.iNewPassword) == 0);
	res = res && (aData1.iOldPassword.Compare(aData2.iOldPassword) == 0);
	return res;
	}

//------------------------------------------------------------------------------
// SerialiseL/DeserialiseL for  TDesC16
//------------------------------------------------------------------------------	
	
EXPORT_C void TSerializer<TDesC16*>::SerialiseL(const TDesC16* aData, RBuf8& aBuffer)
	{	
    TBool haveMsg(EFalse);
   	TPckg<TBool> haveMsgPckg(haveMsg);
   	
    if(aData)
        {
        haveMsg = ETrue;	    
	    aBuffer.Create(sizeof(TBool) + aData->Size());
        aBuffer.Append(haveMsgPckg);  
	    aBuffer.Append(*aData);  
        }
    else
        {
	    aBuffer.Create(sizeof(TBool));
        aBuffer.Append(haveMsgPckg);  
        }
	}


EXPORT_C void TSerializer<TDesC16*>::DeserialiseL(const TDesC8& aPackedData, TDesC16*& aData)
	{
	TBool haveData(EFalse);
	TPckg<TBool> haveDataPckg(haveData);	
	haveDataPckg.Copy( aPackedData.Left( sizeof(TBool)) );

    if(haveData)
        {
        TUint size = aPackedData.Size() - sizeof(TBool);
        HBufC16* tmpPtr = HBufC16::NewL(size);
        tmpPtr->Des().Copy( aPackedData.Mid( sizeof(TBool), size) );        
        aData = static_cast<TDesC16*>(tmpPtr);        
        }
    else
        {
        aData = NULL;
        }
	}

EXPORT_C TBool TComparator<RMobileSmsMessaging::TMobileSmsAttributesV1>::
	IsEqual(const RMobileSmsMessaging::TMobileSmsAttributesV1& aData1,
			const RMobileSmsMessaging::TMobileSmsAttributesV1& aData2)
	{
	return 	(TComparator<RMobilePhone::TMultimodeType>::IsEqual(aData1, aData2)) &&
			(aData1.iFlags == aData2.iFlags) &&
			(aData1.iDataFormat == aData2.iDataFormat) &&
			(aData1.iCdmaTeleservice == aData2.iCdmaTeleservice) &&
			(TComparator<RMobilePhone::TMobileAddress>::IsEqual(aData1.iGsmServiceCentre,aData2.iGsmServiceCentre));
	}

EXPORT_C TBool TComparator<RMobileSmsMessaging::TMobileSmsSendAttributesV1>::
	IsEqual(const RMobileSmsMessaging::TMobileSmsSendAttributesV1& aData1,
			const RMobileSmsMessaging::TMobileSmsSendAttributesV1& aData2)
	{
	return 	(TComparator<RMobileSmsMessaging::TMobileSmsAttributesV1>::IsEqual(aData1,aData2)) &&
			(aData1.iMsgRef ==  aData2.iMsgRef) &&
			(aData1.iMore ==  aData2.iMore) &&
			(TComparator<RMobileSmsMessaging::TMobileSmsGsmTpdu>::IsEqual(aData1.iSubmitReport,aData2.iSubmitReport)) &&
			(TComparator<RMobilePhone::TMobileAddress>::IsEqual(aData1.iDestination,aData2.iDestination));
	}

EXPORT_C TBool TComparator<RMobileSmsMessaging::TMobileSmsGsmTpdu>::
	IsEqual(const RMobileSmsMessaging::TMobileSmsGsmTpdu& aData1,
			const RMobileSmsMessaging::TMobileSmsGsmTpdu& aData2)
	{
	return 	aData1 == aData2;
	}

EXPORT_C TBool TComparator<RMobileSmsStore::TMobileGsmSmsEntryV1>::
	IsEqual(const RMobileSmsStore::TMobileGsmSmsEntryV1& aData1,
			const RMobileSmsStore::TMobileGsmSmsEntryV1& aData2)
	{
	return	TComparator<RMobileSmsStore::TMobileSmsEntryV1>::IsEqual(aData1,aData2) &&
			TComparator<RMobilePhone::TMobileAddress>::IsEqual(aData1.iServiceCentre,aData2.iServiceCentre) &&
			TComparator<RMobileSmsMessaging::TMobileSmsGsmTpdu>::IsEqual(aData1.iMsgData,aData2.iMsgData);
	}

EXPORT_C TBool TComparator<RMobileSmsStore::TMobileSmsEntryV1>::
	IsEqual(const RMobileSmsStore::TMobileSmsEntryV1& aData1,
			const RMobileSmsStore::TMobileSmsEntryV1& aData2)
	{
	return 	TComparator<RMobilePhoneStore::TMobilePhoneStoreEntryV1>::IsEqual(aData1,aData2) &&
			aData1.iMsgStatus == aData2.iMsgStatus;
	}

EXPORT_C TBool TComparator<RMobilePhoneStore::TMobilePhoneStoreEntryV1>::
	IsEqual(const RMobilePhoneStore::TMobilePhoneStoreEntryV1& aData1,
			const RMobilePhoneStore::TMobilePhoneStoreEntryV1& aData2)
	{
	return 	TComparator<RMobilePhone::TMultimodeType>::IsEqual(aData1,aData2) &&
			aData1.iIndex == aData2.iIndex;
	}

EXPORT_C TBool TComparator<RMobileCall::TIconId>::IsEqual(const RMobileCall::TIconId& aData1, const RMobileCall::TIconId& aData2)
	{
	if(aData1.iQualifier != aData2.iQualifier)
		{
		return EFalse;
		}
	if(aData1.iIdentifier != aData2.iIdentifier)
		{
		return EFalse;
		}
	return ETrue;
    }

EXPORT_C TBool TComparator<RMobileCall::TMobileCallHscsdInfoV8>::IsEqual(const RMobileCall::TMobileCallHscsdInfoV8& aData1, const RMobileCall::TMobileCallHscsdInfoV8& aData2)
    {
    TBool result;

    //TMobileCallHscsdInfoV1
    result = TComparator<RMobileCall::TMobileCallHscsdInfoV1>::IsEqual(aData1,aData2);
    //TMobileCallHscsdInfoV7
    result &= (aData1.iCallParamOrigin == aData2.iCallParamOrigin);
    result &= TComparator<RMobileCall::TIconId>::IsEqual(aData1.iIconId,aData2.iIconId);
    result &= (0 == aData1.iAlphaId.Compare(aData2.iAlphaId));
    result &= (aData1.iParamsCallControlModified == aData2.iParamsCallControlModified);
    //TMobileCallHscsdInfoV8
    result &= (0 == aData1.iSubAddress.Compare(aData2.iSubAddress));
	result &= (0 == aData1.iBearerCap1.Compare(aData2.iBearerCap1));    
	result &= (0 == aData1.iBearerCap2.Compare(aData2.iBearerCap2));
	result &= (aData1.iBCRepeatIndicator == aData2.iBCRepeatIndicator );
    
    return result;
    }

EXPORT_C TBool TComparator<RMobileCall::TMobileHscsdCallParamsV8>::IsEqual(const RMobileCall::TMobileHscsdCallParamsV8& aData1, const RMobileCall::TMobileHscsdCallParamsV8& aData2)
    {
    TBool result;
    //TMobileHscsdCallParamsV2
    result = TComparator<RMobileCall::TMobileHscsdCallParamsV2>::IsEqual(aData1,aData2);
    //TMobileHscsdCallParamsV7
    result &= (aData1.iCallParamOrigin == aData2.iCallParamOrigin);
    result &= TComparator<RMobileCall::TIconId>::IsEqual(aData1.iIconId,aData2.iIconId);
    result &= (0 == aData1.iAlphaId.Compare(aData2.iAlphaId));
    //TMobileHscsdCallParamsV8
    result &= (0 == aData1.iSubAddress.Compare(aData2.iSubAddress));
	result &= (0 == aData1.iBearerCap1.Compare(aData2.iBearerCap1));    
	result &= (0 == aData1.iBearerCap2.Compare(aData2.iBearerCap2));
	result &= (aData1.iBCRepeatIndicator == aData2.iBCRepeatIndicator );
    
    return result;
    }
