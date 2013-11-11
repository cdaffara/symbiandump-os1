// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimContextHelperTraces.h"
#endif

#include "CSimContextHelper.h"

#include "CSimPacketContext.h"

                                                                                                                   //const RPacketContext::TContextConfigGPRS&  aContextConfigV1        
void CSimContextHelper::SetTContextConfigParamFromTContextConfigGPRS( TContextConfigParam& aContextConfigToSet, const RPacketContext::TContextConfigGPRS&  aContextConfigV1 )
	{		
	aContextConfigToSet.iProtocolType = aContextConfigV1.iPdpType;
	aContextConfigToSet.iGsnAddress.Set(aContextConfigV1.iAccessPointName);
	aContextConfigToSet.iProtocolAddress.Set(aContextConfigV1.iPdpAddress);
	aContextConfigToSet.iAnonymousAccess = aContextConfigV1.iAnonymousAccessReqd;
	aContextConfigToSet.iUseEdge = aContextConfigV1.iUseEdge;
	aContextConfigToSet.iProtocolConfigOption.iAuthProtocol = aContextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol;
	aContextConfigToSet.iProtocolConfigOption.iUsername.Set(aContextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername);
	aContextConfigToSet.iProtocolConfigOption.iPassword.Set(aContextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword);
	aContextConfigToSet.iProtocolConfigOption.iChallenge.Set(aContextConfigV1.iProtocolConfigOption.iChallenge);
	aContextConfigToSet.iProtocolConfigOption.iResponse.Set(aContextConfigV1.iProtocolConfigOption.iResponse);
	aContextConfigToSet.iProtocolConfigOption.iPrimaryDNS.Set(aContextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	aContextConfigToSet.iProtocolConfigOption.iSecondaryDNS.Set(aContextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	aContextConfigToSet.iProtocolConfigOption.iId = aContextConfigV1.iProtocolConfigOption.iId;
	aContextConfigToSet.iProtocolConfigOption.iMiscBuffer = aContextConfigV1.iProtocolConfigOption.iMiscBuffer;
	
	if((aContextConfigV1.iPdpCompression & RPacketContext::KPdpHeaderCompression) != 0)
		{
		aContextConfigToSet.iPdpHeaderCompression = RPacketContext::EManufacturerDefaultHeaderCompression;
		}
	else
		{
		aContextConfigToSet.iPdpHeaderCompression = RPacketContext::ENoHeaderCompression;
		}

	if((aContextConfigV1.iPdpCompression & RPacketContext::KPdpDataCompression) != 0)
		{
		aContextConfigToSet.iPdpDataCompression = RPacketContext::EManufacturerDefaultDataCompression;
		}
	else
		{
		aContextConfigToSet.iPdpDataCompression = RPacketContext::ENoDataCompression;
		}	
	}
		
void CSimContextHelper::SetTContextConfigParamFromTContextConfigR99_R4( TContextConfigParam& aContextConfigToSet, const RPacketContext::TContextConfigR99_R4& aContextConfigV1 )
	{			
	aContextConfigToSet.iProtocolType = aContextConfigV1.iPdpType;
	aContextConfigToSet.iGsnAddress.Set(aContextConfigV1.iAccessPointName);
	aContextConfigToSet.iProtocolAddress.Set(aContextConfigV1.iPdpAddress);
	aContextConfigToSet.iAnonymousAccess = 0; /*RPacketContext::TAnonymousAccess::ENotApplicable;*/
	aContextConfigToSet.iUseEdge = aContextConfigV1.iUseEdge;
	aContextConfigToSet.iProtocolConfigOption.iAuthProtocol = aContextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol;
	aContextConfigToSet.iProtocolConfigOption.iUsername.Set(aContextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername);
	aContextConfigToSet.iProtocolConfigOption.iPassword.Set(aContextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword);
	aContextConfigToSet.iProtocolConfigOption.iChallenge.Set(aContextConfigV1.iProtocolConfigOption.iChallenge);
	aContextConfigToSet.iProtocolConfigOption.iResponse.Set(aContextConfigV1.iProtocolConfigOption.iResponse);
	aContextConfigToSet.iProtocolConfigOption.iPrimaryDNS.Set(aContextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	aContextConfigToSet.iProtocolConfigOption.iSecondaryDNS.Set(aContextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	aContextConfigToSet.iProtocolConfigOption.iId = aContextConfigV1.iProtocolConfigOption.iId;
	aContextConfigToSet.iPdpHeaderCompression = RPacketContext::ENoHeaderCompression;
	aContextConfigToSet.iPdpDataCompression = RPacketContext::ENoDataCompression;
	aContextConfigToSet.iProtocolConfigOption.iMiscBuffer = aContextConfigV1.iProtocolConfigOption.iMiscBuffer;
	}
		
void CSimContextHelper::SetTContextConfigParamFromTContextConfig_R5(TContextConfigParam& aContextConfigToSet,const RPacketContext::TContextConfig_R5& aContextConfigV1 )
	{
	aContextConfigToSet.iProtocolType = aContextConfigV1.iPdpType;
	aContextConfigToSet.iGsnAddress.Set(aContextConfigV1.iAccessPointName);
	aContextConfigToSet.iProtocolAddress.Set(aContextConfigV1.iPdpAddress);
	aContextConfigToSet.iAnonymousAccess = 0; /*RPacketContext::TAnonymousAccess::ENotApplicable;*/
	aContextConfigToSet.iUseEdge = aContextConfigV1.iUseEdge;
	aContextConfigToSet.iProtocolConfigOption.iAuthProtocol = aContextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol;
	aContextConfigToSet.iProtocolConfigOption.iUsername.Set(aContextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername);
	aContextConfigToSet.iProtocolConfigOption.iPassword.Set(aContextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword);
	aContextConfigToSet.iProtocolConfigOption.iChallenge.Set(aContextConfigV1.iProtocolConfigOption.iChallenge);
	aContextConfigToSet.iProtocolConfigOption.iResponse.Set(aContextConfigV1.iProtocolConfigOption.iResponse);
	aContextConfigToSet.iProtocolConfigOption.iPrimaryDNS.Set(aContextConfigV1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	aContextConfigToSet.iProtocolConfigOption.iSecondaryDNS.Set(aContextConfigV1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	aContextConfigToSet.iProtocolConfigOption.iId = aContextConfigV1.iProtocolConfigOption.iId;
	aContextConfigToSet.iPdpHeaderCompression = aContextConfigV1.iPdpHeaderCompression;
	aContextConfigToSet.iPdpDataCompression = aContextConfigV1.iPdpDataCompression;
	aContextConfigToSet.iProtocolConfigOption.iMiscBuffer = aContextConfigV1.iProtocolConfigOption.iMiscBuffer;
	}

TBool CSimContextHelper::EqualTContextConfigParam(const TContextConfigParam& aContextConfigLHS, const TContextConfigParam& aContextConfigRHS )
	{
	if((aContextConfigLHS.iProtocolType != aContextConfigRHS.iProtocolType) ||
		(aContextConfigLHS.iGsnAddress.Compare(aContextConfigRHS.iGsnAddress) != 0 ) ||
		(aContextConfigLHS.iAnonymousAccess != aContextConfigRHS.iAnonymousAccess) ||
		(aContextConfigLHS.iUseEdge != aContextConfigRHS.iUseEdge) ||
		(aContextConfigLHS.iProtocolConfigOption.iAuthProtocol != aContextConfigRHS.iProtocolConfigOption.iAuthProtocol) ||
		(aContextConfigLHS.iProtocolConfigOption.iUsername.Compare(aContextConfigRHS.iProtocolConfigOption.iUsername) != 0) ||
		(aContextConfigLHS.iProtocolConfigOption.iPassword.Compare(aContextConfigRHS.iProtocolConfigOption.iPassword) != 0) ||
		// These line are commented as these parameters currently are usually returned by the network and not required for
		// setting the context configuration
		//		(aContextConfigLHS.iProtocolConfigOption.iPrimaryDNS.Compare(aContextConfigRHS.iProtocolConfigOption.iPrimaryDNS) != 0) ||
		//		(aContextConfigLHS.iProtocolConfigOption.iSecondaryDNS.Compare(aContextConfigRHS.iProtocolConfigOption.iSecondaryDNS) != 0) ||
		(aContextConfigLHS.iProtocolConfigOption.iId != aContextConfigRHS.iProtocolConfigOption.iId) ||
		(aContextConfigLHS.iPdpHeaderCompression != aContextConfigRHS.iPdpHeaderCompression) ||
		(aContextConfigLHS.iPdpDataCompression != aContextConfigRHS.iPdpDataCompression))
	    // challenge is generated randomly and response is generated using MD5 in CHAP protocol, so it is not possible to check
	    {
		return EFalse;
		}
	else if (aContextConfigLHS.iProtocolConfigOption.iAuthProtocol != RPacketContext::EProtocolCHAP) 
	    {// if not CHAP we still try to check challenge and response just in case
        if ((aContextConfigLHS.iProtocolConfigOption.iChallenge.Compare(aContextConfigRHS.iProtocolConfigOption.iChallenge) != 0) ||
	        (aContextConfigLHS.iProtocolConfigOption.iResponse.Compare(aContextConfigRHS.iProtocolConfigOption.iResponse) != 0))
            {
            return EFalse;
            }
	    }

		// else considered equal.
		
	return ETrue;
	}

void CSimContextHelper::ConvertConfigParams(const TContextConfigParam& aInputConfig, RPacketContext::TContextConfigGPRS& aOutputConfig)
	{
	aOutputConfig.iPdpType = static_cast<RPacketContext::TProtocolType>(aInputConfig.iProtocolType);
	aOutputConfig.iAccessPointName.Copy(aInputConfig.iGsnAddress);
	aOutputConfig.iPdpAddress.Copy(aInputConfig.iProtocolAddress);
	
	//Translate the R5 SIMTSY internal representation of PDP Data/Header into the GPRS representation
	TUint pdpCompressionParam = 0;
	if(aInputConfig.iPdpDataCompression != static_cast<TUint>(RPacketContext::ENoDataCompression))
		{
		pdpCompressionParam = (pdpCompressionParam | RPacketContext::KPdpDataCompression);
		}
	if(aInputConfig.iPdpHeaderCompression != RPacketContext::ENoHeaderCompression)
		{
		pdpCompressionParam = (pdpCompressionParam | RPacketContext::KPdpHeaderCompression);
		}
		
	aOutputConfig.iPdpCompression = pdpCompressionParam;
	aOutputConfig.iAnonymousAccessReqd = static_cast<RPacketContext::TAnonymousAccess>(aInputConfig.iAnonymousAccess);		
	aOutputConfig.iUseEdge = static_cast<TBool>(aInputConfig.iUseEdge);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iProtocol = static_cast<RPacketContext::TAuthProtocol>(aInputConfig.iProtocolConfigOption.iAuthProtocol);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aInputConfig.iProtocolConfigOption.iUsername);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aInputConfig.iProtocolConfigOption.iPassword);
	aOutputConfig.iProtocolConfigOption.iChallenge.Copy(aInputConfig.iProtocolConfigOption.iChallenge);
	aOutputConfig.iProtocolConfigOption.iResponse.Copy(aInputConfig.iProtocolConfigOption.iResponse);
	aOutputConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(aInputConfig.iProtocolConfigOption.iPrimaryDNS);
	aOutputConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(aInputConfig.iProtocolConfigOption.iSecondaryDNS);
	aOutputConfig.iProtocolConfigOption.iId = static_cast<TUint8>(aInputConfig.iProtocolConfigOption.iId);
	aOutputConfig.iProtocolConfigOption.iMiscBuffer.Copy(aInputConfig.iProtocolConfigOption.iMiscBuffer);
	}

void CSimContextHelper::ConvertConfigParams(const TContextConfigParam& aInputConfig, RPacketContext::TContextConfigR99_R4& aOutputConfig)
	{
	aOutputConfig.iPdpType = static_cast<RPacketContext::TProtocolType>(aInputConfig.iProtocolType);
	aOutputConfig.iAccessPointName.Copy(aInputConfig.iGsnAddress);
	aOutputConfig.iPdpAddress.Copy(aInputConfig.iProtocolAddress);
	aOutputConfig.iUseEdge = aInputConfig.iUseEdge;
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iProtocol = static_cast<RPacketContext::TAuthProtocol>(aInputConfig.iProtocolConfigOption.iAuthProtocol);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aInputConfig.iProtocolConfigOption.iUsername);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aInputConfig.iProtocolConfigOption.iPassword);
	aOutputConfig.iProtocolConfigOption.iChallenge.Copy(aInputConfig.iProtocolConfigOption.iChallenge);
	aOutputConfig.iProtocolConfigOption.iResponse.Copy(aInputConfig.iProtocolConfigOption.iResponse);
	aOutputConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(aInputConfig.iProtocolConfigOption.iPrimaryDNS);
	aOutputConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(aInputConfig.iProtocolConfigOption.iSecondaryDNS);
	aOutputConfig.iProtocolConfigOption.iId = static_cast<TUint8>(aInputConfig.iProtocolConfigOption.iId);
	aOutputConfig.iProtocolConfigOption.iMiscBuffer.Copy(aInputConfig.iProtocolConfigOption.iMiscBuffer);
	}

void CSimContextHelper::ConvertConfigParams(const TContextConfigParam& aInputConfig, RPacketContext::TContextConfig_R5& aOutputConfig)
	{
	aOutputConfig.iPdpType = static_cast<RPacketContext::TProtocolType>(aInputConfig.iProtocolType);
	aOutputConfig.iAccessPointName.Copy(aInputConfig.iGsnAddress);
	aOutputConfig.iPdpAddress.Copy(aInputConfig.iProtocolAddress);
	aOutputConfig.iUseEdge = aInputConfig.iUseEdge;
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iProtocol = static_cast<RPacketContext::TAuthProtocol>(aInputConfig.iProtocolConfigOption.iAuthProtocol);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aInputConfig.iProtocolConfigOption.iUsername);
	aOutputConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aInputConfig.iProtocolConfigOption.iPassword);
	aOutputConfig.iProtocolConfigOption.iChallenge.Copy(aInputConfig.iProtocolConfigOption.iChallenge);
	aOutputConfig.iProtocolConfigOption.iResponse.Copy(aInputConfig.iProtocolConfigOption.iResponse);
	aOutputConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(aInputConfig.iProtocolConfigOption.iPrimaryDNS);
	aOutputConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(aInputConfig.iProtocolConfigOption.iSecondaryDNS);
	aOutputConfig.iProtocolConfigOption.iId = static_cast<TUint8>(aInputConfig.iProtocolConfigOption.iId);
	aOutputConfig.iPdpDataCompression = static_cast<RPacketContext::TDataCompression>(aInputConfig.iPdpDataCompression);
	aOutputConfig.iPdpHeaderCompression = static_cast<RPacketContext::THeaderCompression>(aInputConfig.iPdpHeaderCompression);
	aOutputConfig.iProtocolConfigOption.iMiscBuffer.Copy(aInputConfig.iProtocolConfigOption.iMiscBuffer);
	}

void CSimContextHelper::GetContextConfigParamSettingsL( const CTestConfigSection* const aCfgFile, const TPtrC8 aTag, 
													   CArrayFixFlat<TContextConfigParam>* aContextConfigParams )
	{
	TContextConfigParam configParam;
		
	TInt protocolType, pdpCompression, anonymousAccess, useEdge, protocol, id;
	TInt pdpDataCompression, pdpHeaderCompression, numAdditionalParams;
	TPtrC8 gsnAddress, protocolAddress, username, password, challenge;
	TPtrC8 response, primaryDNS, secondaryDNS;

	configParam.iProtocolConfigOption.iMiscBuffer.Zero();

	RPacketContext::TPcoId pcoId;
	
	TUint itemOffSet = 0; //Used for nested Addional Param items
	TInt delimiterField;

	TInt count = aCfgFile->ItemCount(aTag);

	for(TInt i=0;i<count;i++)
		{
		const CTestConfigItem* item = aCfgFile->Item(aTag,i);
		if(!item)
			break;
		
		//Get the Protocol Type from the config file
		TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,protocolType);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_1, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::PROTOCOLTYPE returned %d (element no. %d) from tag %s.",ret,0,aTag);
			continue;
			}
		else
			{
			configParam.iProtocolType = protocolType;	
			}
	
		
		//Get the GGSN Address from the config file
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,gsnAddress);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::GSNADDRESS returned %d (element no. %d) from tag %s.",ret,1,aTag);
			continue;
			}
		else
			{
			configParam.iGsnAddress.Set(gsnAddress);
			}
			
			
		//Get the ProtocolAddress from the config file
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,protocolAddress);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::PROTOCOLADDRESS returned %d (element no. %d) from tag %s.",ret,2,aTag);
			continue;
			}
		else
			{
			configParam.iProtocolAddress.Set(protocolAddress);
			}

	
		//Get the pdpCompression from the config file 
		//If the tag is a GPRS config tag, map the GPRS configuration to the R5 format used internally in SIMTSY
		if(aTag == KSetContextConfigGPRS)
			{
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,pdpCompression);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::PDPCOMPRESSION returned %d (element no. %d) from tag %s.",ret,3,aTag);
				continue;
				}
			else
				{
				//Set the GPRS PDP compression Config params
				//configParam.iPdpCompression = pdpCompression;
				
				//Set the R5 PDP header compression config params based on GPRS params
				if((pdpCompression & RPacketContext::KPdpHeaderCompression) != 0)
					{
					configParam.iPdpHeaderCompression = RPacketContext::EManufacturerDefaultHeaderCompression;
					}
				else
					{
					configParam.iPdpHeaderCompression = RPacketContext::ENoHeaderCompression;
					}
			
				if((pdpCompression & RPacketContext::KPdpDataCompression) != 0)
					{
					configParam.iPdpDataCompression = RPacketContext::EManufacturerDefaultDataCompression;
					}
				else
					{
					configParam.iPdpDataCompression = RPacketContext::ENoDataCompression;
					}
				}

			}
		else if(aTag == KSetContextConfigRel5)
			{
			//Get the PDP Header Compression params
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,12,pdpHeaderCompression);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::PDPHEADERCOMPRESSION returned %d (element no. %d) from tag %s.",ret,12,aTag);
				continue;
				}
			else
				{
				configParam.iPdpHeaderCompression = pdpHeaderCompression;
				}
			
			//Get the PDP Data compression params
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,13,pdpDataCompression);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::PDPDATACOMPRESSION returned %d (element no. %d) from tag %s.",ret,13,aTag);
				continue;
				}
			else
				{
				configParam.iPdpDataCompression = pdpDataCompression;
				}			
			}
		else
			{
			configParam.iPdpHeaderCompression = RPacketContext::ENoHeaderCompression;
			configParam.iPdpDataCompression = RPacketContext::ENoDataCompression;	
			}
			
			
		//Get the Anonymous Access param if this is a GPRS PDP context config param tag
		if(aTag == KSetContextConfigGPRS)
			{
			ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,anonymousAccess);
			if(ret!=KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::ANONYMOUSACCESS returned %d (element no. %d) from tag %s.",ret,4,aTag);
				continue;
				}
			else
				{
				configParam.iAnonymousAccess = anonymousAccess;
				}
			}
		else
			{
			configParam.iAnonymousAccess = 0; /*RPacketContext::TAnonymousAccess::ENotApplicable;*/
			}


		//Get the UseEdge config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 5;
			}
		else
			{
			delimiterField = 3;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,useEdge);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTEXTCONFIGPARAMS::USEEDGE returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			continue;
			}
		else
			{
			configParam.iUseEdge = useEdge;	
			}
		
		
		//Get the Autorisation Protocol config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 6;
			}
		else
			{
			delimiterField = 4;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,protocol);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::PROTOCOL returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iAuthProtocol = protocol;	
			}


		//Get the Username config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 7;
			}
		else
			{
			delimiterField = 5;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,username);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::USERNAME returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iUsername.Set(username);	
			}


		//Get the password config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 8;
			}
		else
			{
			delimiterField = 6;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,password);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::PASSWORD returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iPassword.Set(password);	
			}

		
		//Get the Challenge config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 9;
			}
		else
			{
			delimiterField = 7;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,challenge);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::CHALLENGE returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iChallenge.Set(challenge);	
			}


		//Get the Response config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 10;
			}
		else
			{
			delimiterField = 8;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,response);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::RESPONSE returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iResponse.Set(response);	
			}

		
		//Get the Primary DNS config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 11;
			}
		else
			{
			delimiterField = 9;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,primaryDNS);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::PRIMARYDNS returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iPrimaryDNS.Set(primaryDNS);	
			}
		

		//Get the Secondary DNS config param from the config file
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 12;
			}
		else
			{
			delimiterField = 10;
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,secondaryDNS);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::SECONDARYDNS returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iSecondaryDNS.Set(secondaryDNS);	
			}
			
		
		//Get the ID config param from the config file.
		if(aTag == KSetContextConfigGPRS)
			{
			delimiterField = 13;
			}
		else
			{
			delimiterField = 11;
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,id);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_16, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::ID returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			configParam.iProtocolConfigOption.iId = id;	
			}
			

		//Get the Additional Params List

		//Create buffer to hold the TLV structure
		RPacketContext::TMiscProtocolBuffer miscBuffer;
		miscBuffer.Zero();


		//Get the number of items in the Additional Params List
		if (aTag == KSetContextConfigRel99)
			{
			delimiterField = 12;
			}
		else
			{
			delimiterField = 14;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterField,numAdditionalParams);
	
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_17, "WARNING - CONFIGURATION FILE PARSING - Reading element PROTOCOLCONFIGOPTION::NUMADDITIONALPARAMS returned %d (element no. %d) from tag %s.",ret,delimiterField,aTag);
			}
		else
			{
			TInt nestedItemNumber;
			TUint16 digit, format;
			const CTestConfigItem* item2;
 
			TPtr8 pcoPtr(const_cast<TUint8*>(miscBuffer.Ptr()), 
					RPacketContext::KMiscProtocolBufferLength);

			// Attach TTlv to the buffer
			TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLV(pcoPtr,0);

			for(nestedItemNumber=itemOffSet; nestedItemNumber<itemOffSet+numAdditionalParams; nestedItemNumber++)
				{

				// Get the relevant Additional Param Item
				
				// Find the correct tag
				if (aTag == KSetContextConfigRel5)
					{
					item2 = aCfgFile->Item(KAdditionalParamItem, nestedItemNumber);
					}
				else if (aTag == KSetContextConfigGPRS)
					{
					item2 = aCfgFile->Item(KAdditionalParamItemGPRS, nestedItemNumber);
					}
				else 
					{
					// Otherwise it is KSetContextConfigRel99
					item2 = aCfgFile->Item(KAdditionalParamItemRel99, nestedItemNumber);
					}
					
				if(!item2)
					{
					break;
					}

				TPtrC8 additionalParamId, additionalParamData, additionalParamDataFormat;
				
				// Get TPcoProtocolId
				ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,0,additionalParamId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_18, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMID returned %d (element no. %d) from tag %s.",ret,0,aTag);
					continue;
					}
				else
					{
					if(AsciiToNum(additionalParamId, digit) == KErrNone)
						{
						pcoId.SetId(digit);
						}
					else
						{
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_19, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMID returned %d (element no. %d) from tag %s.",KErrArgument,0,aTag);
						continue;
						}
					}

				// Read AdditionalParamDataFormat field to find out how 
				// to interpret data in the AdditionalParamData field
				ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,2,additionalParamDataFormat);
				if (ret != KErrNone)
					{
					// AdditionalParamDataFormat not been specified,
					// default to plain ASCII
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_20, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",ret,2,aTag);
					format = EConfigDataFormatAscii;
					}
				else
					{
					if (AsciiToNum(additionalParamDataFormat, format) != KErrNone)
						{
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_21, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,aTag);
						format = EConfigDataFormatAscii;
						}
						
					// Check that the AdditionalParamDataFormat value
					// specified in the config file is a valid one,
					// default to ASCII if not.
					if (format >= EMaxConfigDataFormat)
						{
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_22, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMDATAFORMAT returned %d (element no. %d) from tag %s.",KErrArgument,2,aTag);
						format = EConfigDataFormatAscii;
						}
					}

				// Read AdditionalParamData
				ret=CTestConfig::GetElement(item2->Value(),KStdDelimiter,1,additionalParamData);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_23, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMCONTENT returned %d (element no. %d) from tag %s.",ret,1,aTag);
					continue;
					}
				else
					{
					// Add AdditionalParamData to PCO buffer depending on format
					switch (format)
						{
					case EConfigDataFormatMixedBinaryAndAscii:
						{
						// Parse the string here
						// Will store the parsed data when algorithm has completed
						TBuf8<RPacketContext::KMiscProtocolBufferLength> additionalParamDataBuffer;
						additionalParamDataBuffer.Copy (additionalParamData);

						if (ParseMixedBinaryAsciiDataL(additionalParamDataBuffer) != KErrNone)
							{
							OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCONTEXTHELPER_GETCONTEXTCONFIGPARAMSETTINGSL_24, "WARNING - CONFIGURATION FILE PARSING - Reading element ADDITIONALPARAMITEM::ADDITIONALPARAMCONTENT returned %d (element no. %d) from tag %s.",KErrArgument,1,aTag);
							SimPanic(EInvalidParameterFormatInConfigFile);							
							}
						TPtr8 additionalParamDataPtr(const_cast<TUint8*>(additionalParamDataBuffer.Ptr()),
							additionalParamDataBuffer.Length(), additionalParamDataBuffer.Length());
						theTLV.AppendItemL(pcoId,additionalParamDataPtr);
						miscBuffer.SetLength(pcoPtr.Length());
						} // case EAdditionalParamDataFormatMixedBinaryAndAscii
						break;
					case EConfigDataFormatAscii:
					default:
						// No string handling required for plain ASCII.
						TPtr8 additionalParamDataPtr(const_cast<TUint8*>(additionalParamData.Ptr()),
							additionalParamData.Length(), additionalParamData.Length());
						theTLV.AppendItemL(pcoId,additionalParamDataPtr);
						miscBuffer.SetLength(pcoPtr.Length());
						break;
						} // switch (format)
					} // End else
				} // End for loop

			configParam.iProtocolConfigOption.iMiscBuffer = miscBuffer;
			itemOffSet= itemOffSet + numAdditionalParams;
			}


		//Get if the context is valid from the config file.
		if(aTag == KSetContextConfigRel99)
			{
			delimiterField = 13;
			}
		else
			{
			delimiterField = 15;
			}			
			
		aContextConfigParams->AppendL(configParam);
		}
	}
