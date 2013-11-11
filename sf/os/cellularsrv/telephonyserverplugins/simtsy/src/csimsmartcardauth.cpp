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
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csimsmartcardauthTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimPhone.h"
#include "csimsmartcardauth.h"
#include "utils.h"

CSimSmartCardAuth* CSimSmartCardAuth::NewL(CSimPhone *aPhone)
	{
	CSimSmartCardAuth* obj = new(ELeave) CSimSmartCardAuth(aPhone);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
	return obj;
	}

CSimSmartCardAuth::CSimSmartCardAuth(CSimPhone *aPhone)
: iPhone(aPhone)
	{
	iGBAAuthInfoList = new (ELeave) CArrayFixFlat<TGBAAuthInfo> (1);
	iMBMSInfoList = new (ELeave) CArrayFixFlat<TMBMSInfo> (1);
	}

void CSimSmartCardAuth::ConstructL()
	{
	ParseAuthInfoL();
	ParseGBAAuthInfoL();
	ParseMBMSAuthInfoL();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_CONSTRUCTL_1, "CSimSmartCardAuth created");
	}

CSimSmartCardAuth::~CSimSmartCardAuth()
	{
	iAuthInfoList.Close();
	if(iGBAAuthInfoList)
		{
		iGBAAuthInfoList->Delete(0,iGBAAuthInfoList->Count());
		delete iGBAAuthInfoList;
		}
	if(iMBMSInfoList)
		{
		iMBMSInfoList->Delete(0,iMBMSInfoList->Count());
		delete iMBMSInfoList;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_DTOR_1, "CSimSmartCardAuth destroyed");
	}

TInt CSimSmartCardAuth::GetScAuthenticationData(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg1, const RMobilePhone::TAID* aAID)
	{

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_GETSCAUTHENTICATIONDATA_1, "CSimSmartCardAuth::GetScAuthenticationData called");

	RMobilePhone::TSmartCardAuthenticateDataV6Pckg
			* authenticateDataPckgd =reinterpret_cast<RMobilePhone::TSmartCardAuthenticateDataV6Pckg*>(aPckg1);
	RMobilePhone::TSmartCardAuthenticateDataV6& authenticateData=(*authenticateDataPckgd) ();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion (authenticateData);
	if ( err != KErrNone)
		{
		iPhone->ReqCompleted (aTsyReqHandle, err);
		return KErrNone;
		}
	
	
	if ( authenticateData.ExtensionId ()== RMobilePhone::KETelMobileGbaBootstrapAuthDataV8)
		{
		RMobilePhone::TGbaBootstrapAuthDataV8Pckg
				* tGbaBootstrapAuthDataV8Pckgd =reinterpret_cast<RMobilePhone::TGbaBootstrapAuthDataV8Pckg*> (aPckg1);
		RMobilePhone::TGbaBootstrapAuthDataV8 tGbaBootstrapAuthDataV8 = (*tGbaBootstrapAuthDataV8Pckgd) ();

			// This is the USIM authorisation.
			if ( ((aAID == NULL) || (*aAID == iGBAAuthInfoList->At(index).iAID)) 
					&&tGbaBootstrapAuthDataV8.iAUTN == iGBAAuthInfoList->At(index).iAUTN 
					&&tGbaBootstrapAuthDataV8.iRAND == iGBAAuthInfoList->At(index).iRAND)
				{
				TInt ret = iGBAAuthInfoList->At(index).iAuthErr;
				if ( ret != KErrNone)
					{
					// Return the AUTS and blank everything else
					tGbaBootstrapAuthDataV8.iAUTS = iGBAAuthInfoList->At(index).iAUTS;
					tGbaBootstrapAuthDataV8.iRES = _L8("");
					tGbaBootstrapAuthDataV8.iIK = _L8("");
					tGbaBootstrapAuthDataV8.iCK = _L8("");
					if ( ret == KErrMMEtelScApplicationNotActive)
						{
						if ( !(iGBAAuthInfoList->At(index).iApplnActive == EFalse))
							{
							iPhone->ReqCompleted (aTsyReqHandle, KErrCorrupt);
							return KErrNone;
							}
						}
					}
				else
					{
					tGbaBootstrapAuthDataV8.iRES = iGBAAuthInfoList->At(index).iRES;
					tGbaBootstrapAuthDataV8.iAUTS = _L8("");
					tGbaBootstrapAuthDataV8.iIK = _L8("");
					tGbaBootstrapAuthDataV8.iCK = _L8("");
					}

				iPhone->ReqCompleted (aTsyReqHandle, ret);
				//iGBAAuthInfoList->Delete(index);
				return KErrNone;
				}
			iPhone->ReqCompleted (aTsyReqHandle, KErrArgument);
			return KErrNone;
		}
	else if (authenticateData.ExtensionId ()== RMobilePhone::KETelMobileGbaNafAuthDataV8)
		{
		RMobilePhone::TGbaNafAuthDataV8Pckg *gbaNafAuthPckg =
				reinterpret_cast<RMobilePhone::TGbaNafAuthDataV8Pckg*>(aPckg1);

		RMobilePhone::TGbaNafAuthDataV8 &gbaNafAuthData=  (*gbaNafAuthPckg)();
		// This is the USIM authorisation.
			if ( ((aAID == NULL) || (*aAID == iGBAAuthInfoList->At(index).iAID)) 
					&&gbaNafAuthData.iImpi == iGBAAuthInfoList->At(index).iIMPI 
					&&gbaNafAuthData.iNafId == iGBAAuthInfoList->At(index).iNAFID)
				{
				TInt ret = iGBAAuthInfoList->At(index).iAuthErr;
				if ( ret != KErrNone)
					{
					// return the Ks_Ext_NAF as NULL
					gbaNafAuthData.iKsExtNaf = _L8("");
					if ( ret == KErrMMEtelAuthKeyNotFound)
						{
						if ( !(iGBAAuthInfoList->At(index).iRAND == _L8("INV_RAND")))
							{
							iPhone->ReqCompleted (aTsyReqHandle, KErrCorrupt);
							return KErrNone;
							}
						}
					if ( ret == KErrMMEtelScApplicationNotActive)
						{
						if ( !(iGBAAuthInfoList->At(index).iApplnActive == EFalse))
							{
							iPhone->ReqCompleted (aTsyReqHandle, KErrCorrupt);
							return KErrNone;
							}
						}
					}
				else
					{
					gbaNafAuthData.iKsExtNaf = iGBAAuthInfoList->At(index).iKsExtNaf;
					}

				iPhone->ReqCompleted (aTsyReqHandle, ret);
				return KErrNone;
				}
			iPhone->ReqCompleted (aTsyReqHandle, KErrArgument);
			return KErrNone;
			}
	else if ( authenticateData.ExtensionId ()== RMobilePhone::KETelMobileMbmsMskUpdateDataV8)
			{
			RMobilePhone::TMbmsMskUpdateDataV8Pckg
					*tTMbmsMskUpdateDataV8Pckg =reinterpret_cast<RMobilePhone::TMbmsMskUpdateDataV8Pckg*> (aPckg1);

			RMobilePhone::TMbmsMskUpdateDataV8 &tTMbmsMskUpdateDataV8= (*tTMbmsMskUpdateDataV8Pckg) ();

			if ( ((aAID == NULL) || (*aAID == iMBMSInfoList->At(index).iAID))
					&& tTMbmsMskUpdateDataV8.iInputMikey == iMBMSInfoList->At(index).iInputMikey)
				{
				TInt ret = iMBMSInfoList->At(index).iAuthErr;
				if ( ret != KErrNone)
					{
					// return the Ks_Ext_NAF as NULL
					tTMbmsMskUpdateDataV8.iOutputMikey = _L8 ("");
					if ( ret == KErrMMEtelAuthKeyNotFound)
						{
						/* Checking for the hardcoded (existing) KsIntNAF with the one in config file 
						 * If different, it means that the KsIntNaf has been overwritten 
						 */
						if ((iMBMSInfoList->At(index).iKsIntNaf == _L8("CORRECT_KS_INT")))
							{
							iPhone->ReqCompleted (aTsyReqHandle,KErrCorrupt);
							index++;
							return KErrNone;
							}
						if(!(iMBMSInfoList->At(index).iMukIdTlv == _L8("INVALID_MUKID")))
							{
							iPhone->ReqCompleted (aTsyReqHandle,KErrCorrupt);
							index++;
							return KErrNone;
							}
						}
					if ( ret == KErrMMEtelScApplicationNotActive)
						{
						if ( !(iMBMSInfoList->At(index).iApplnActive == EFalse))
							{
							iPhone->ReqCompleted (aTsyReqHandle,KErrCorrupt);
							index++;
							return KErrNone;
							}
						}
					}
				else
					{
					tTMbmsMskUpdateDataV8.iOutputMikey = iMBMSInfoList->At(index).iOutputMikey;
					}

				iPhone->ReqCompleted (aTsyReqHandle, ret);
				index++;
				return KErrNone;
				}

			iPhone->ReqCompleted (aTsyReqHandle, KErrArgument);
			index++;
			return KErrNone;
			}
	else if ( authenticateData.ExtensionId ()== RMobilePhone::KETelMobileMbmsMtkGenerateDataV8)
		{
		RMobilePhone::TMbmsMtkGenerateDataV8Pckg
				*tTMbmsMtkGenerateDataV8Pckg =reinterpret_cast<RMobilePhone::TMbmsMtkGenerateDataV8Pckg*> (aPckg1);

		RMobilePhone::TMbmsMtkGenerateDataV8 &tTMbmsMtkGenerateDataV8= (*tTMbmsMtkGenerateDataV8Pckg) ();

		if ( ((aAID == NULL) || (*aAID == iMBMSInfoList->At(index).iAID))
				&& tTMbmsMtkGenerateDataV8.iMikey == iMBMSInfoList->At(index).iInputMikey)
			{
			TInt ret = iMBMSInfoList->At(index).iAuthErr;
			if ( ret != KErrNone)
				{
				// return the Ks_Ext_NAF as NULL
				tTMbmsMtkGenerateDataV8.iMtk = _L8 ("");
				tTMbmsMtkGenerateDataV8.iSaltKey = _L8 ("");
				if ( ret == KErrMMEtelScApplicationNotActive)
					{
					if ( !(iMBMSInfoList->At(index).iApplnActive == EFalse))
						{
						iPhone->ReqCompleted (aTsyReqHandle,KErrCorrupt);
						index++;
						return KErrNone;
						}
					}
				}
			else
				{
				tTMbmsMtkGenerateDataV8.iMtk = iMBMSInfoList->At(index).iMtk;
				tTMbmsMtkGenerateDataV8.iSaltKey = iMBMSInfoList->At(index).iSaltkey;
				}
				iPhone->ReqCompleted (aTsyReqHandle, ret);
			index++;
			return KErrNone;
			}

		iPhone->ReqCompleted (aTsyReqHandle, KErrArgument);
		index++;
		return KErrNone;
		}
	else if ( authenticateData.ExtensionId ()== RMobilePhone::KETelMobileMbmsMskDeleteDataV8)
		{
		RMobilePhone::TMbmsMskDeleteDataV8Pckg
				*tTMbmsMskDeleteDataV8Pckg =reinterpret_cast<RMobilePhone::TMbmsMskDeleteDataV8Pckg*> (aPckg1);

		RMobilePhone::TMbmsMskDeleteDataV8 &tTMbmsMskDeleteDataV8= (*tTMbmsMskDeleteDataV8Pckg) ();

		if ( ((aAID == NULL) || (*aAID == iMBMSInfoList->At(index).iAID))
				&& tTMbmsMskDeleteDataV8.iMskIdKeyGroup == iMBMSInfoList->At(index).iMskIdGrp
				&& tTMbmsMskDeleteDataV8.iKeyDomainId == iMBMSInfoList->At(index).iKeyDmn)
			{
			TInt ret = iMBMSInfoList->At(index).iAuthErr;
			if ( ret != KErrNone)
				{
				if ( ret == KErrMMEtelScApplicationNotActive)
					{
					if ( !(iMBMSInfoList->At(index).iApplnActive == EFalse))
						{
						iPhone->ReqCompleted (aTsyReqHandle,KErrCorrupt);
						index++;
						return KErrNone;
						}
					}
				}
			else
				{
				// MSK IDs are successfully deleted from the EFs.
				}
				iPhone->ReqCompleted (aTsyReqHandle, ret);
			index++;
			return KErrNone;
			}

		iPhone->ReqCompleted (aTsyReqHandle, KErrArgument);
		index++;
		return KErrNone;
		}
	else if ( authenticateData.ExtensionId ()== RMobilePhone::KETelMobileMbmsMukDeleteDataV8)
		{
		RMobilePhone::TMbmsMukDeleteDataV8Pckg
				*tTMbmsMukDeleteDataV8Pckg =reinterpret_cast<RMobilePhone::TMbmsMukDeleteDataV8Pckg*> (aPckg1);

		RMobilePhone::TMbmsMukDeleteDataV8 &tTMbmsMukDeleteDataV8= (*tTMbmsMukDeleteDataV8Pckg) ();

		if ( ((aAID == NULL) || (*aAID == iMBMSInfoList->At(index).iAID))
				&& tTMbmsMukDeleteDataV8.iMukIdTlv == iMBMSInfoList->At(index).iMukIdTlv)
			{
			TInt ret = iMBMSInfoList->At(index).iAuthErr;
			if ( ret != KErrNone)
				{
				if ( ret == KErrMMEtelScApplicationNotActive)
					{
					if ( !(iMBMSInfoList->At(index).iApplnActive == EFalse))
						{
						iPhone->ReqCompleted (aTsyReqHandle,KErrCorrupt);
						index++;
						return KErrNone;
						}
					}
				}
			else
				{
				// MUK IDs are successfully deleted from the EFs.
				}
				iPhone->ReqCompleted (aTsyReqHandle, ret);
			index++;
			return KErrNone;
			}

		iPhone->ReqCompleted (aTsyReqHandle, KErrArgument);
		index++;
		return KErrNone;
		}
	else
		{
		if(authenticateData.iCK==KConfigScWlan())
			{
			if(*aAID==KWlanAid())
				{
				if(authenticateData.iAUTN == KConfigScWlanAuthAUTN() && authenticateData.iRAND == KConfigScWlanAuthRAND())
					{
					authenticateData.iRES=KConfigScWlanAuthRES();
					authenticateData.iIK=KConfigScWlanAuthIK();
					authenticateData.iCK=KConfigScWlanAuthCK();
					iPhone->ReqCompleted (aTsyReqHandle, KErrNone);
					}
				else if(authenticateData.iAUTN == KConfigScWlanAuthAUTN())
					{
					iPhone->ReqCompleted (aTsyReqHandle, KErrMMEtelSqnVerificationFailed);
					}
				else
					{
					iPhone->ReqCompleted (aTsyReqHandle, KErrMMEtelMacVerificationFailed);	
					}
				}
			else
				{
				iPhone->ReqCompleted (aTsyReqHandle, KErrMMEtelScApplicationNotActive);	
				}
			}
		else if(authenticateData.iCK==KConfigScWlanNotSupported())
			{
			iPhone->ReqCompleted (aTsyReqHandle, KErrNotSupported);	
			}
		else if ( authenticateData.iAUTN == _L8(""))
			{
			// Authenticate if EAP-SIM is being used.

			// Find the iRAND value 
			index = 0;
			while ( (index < iAuthInfoList.Count()) && (authenticateData.iRAND != iAuthInfoList[index].iRAND))
				{
				index++;
				}

			if ( index == iAuthInfoList.Count ())
				{
				// The approriate iRAND value could not be found, so return KErrCorrupt.
				iPhone->ReqCompleted (aTsyReqHandle, KErrCorrupt);
				index++;
				return KErrNone;
				}
			else
				{
				// Return the iRES and KC values that match the iRAND given, and blank everything else
				authenticateData.iAUTS = _L8("");
				authenticateData.iRES = iAuthInfoList[index].iRES;
				authenticateData.iIK = _L8("");
				authenticateData.iCK = _L8("");
				authenticateData.iKc = iAuthInfoList[index].iKc;

				iPhone->ReqCompleted (aTsyReqHandle, KErrNone);
				}
			}
		else
			{
			for (index = 0; index < iAuthInfoList.Count (); index++)
				{
				// This is the USIM authorisation.
				if ( authenticateData.iAUTN == iAuthInfoList[index].iAUTN &&authenticateData.iRAND == iAuthInfoList[index].iRAND &&((aAID == NULL) || (*aAID == iAuthInfoList[index].iAID)))
					{
					TInt ret = iAuthInfoList[index].iAuthErr;
					if ( ret != KErrNone)
						{
						// Return the AUTS and blank everything else
						authenticateData.iAUTS = iAuthInfoList[index].iAUTS;
						authenticateData.iRES = _L8("");
						authenticateData.iIK = _L8("");
						authenticateData.iCK = _L8("");
						authenticateData.iKc = _L8("");
						}
					else
						{
						authenticateData.iRES = iAuthInfoList[index].iRES;
						authenticateData.iIK = iAuthInfoList[index].iIK;
						authenticateData.iCK = iAuthInfoList[index].iCK;
						authenticateData.iKc = iAuthInfoList[index].iKc;
						}

					iPhone->ReqCompleted (aTsyReqHandle, ret);
					iAuthInfoList.Remove (index);
					iAuthInfoList.Compress ();
					index++;
					return KErrNone;
					}
				iPhone->ReqCompleted (aTsyReqHandle, KErrCorrupt);	
				}
			}
		}
	index++;
	return KErrNone;
	}

TInt CSimSmartCardAuth::GetScAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_GETSCAUTHENTICATIONDATACANCEL_1, "CSimSmartCardAuth::GetScAuthenticationDataCancel called");
	iPhone->ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

const CTestConfigSection* CSimSmartCardAuth::CfgFile()
	{
	return iPhone->CfgFile();
	}

/**
Parses the list of SmartCardAuthInfo tags from the config.txt file.
*/
void CSimSmartCardAuth::ParseAuthInfoL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_1, "CSimSmartCardAuth::ParseAuthInfoL called");

	CTestConfigItem* item = NULL;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_2, "Starting to Parse Smart Card Authentication Info");
	TInt count = CfgFile()->ItemCount(KScAuthInfo);

	TInt index;
	for(index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KScAuthInfo, index));
		if(item == NULL)
			{
			break;
			}
		CleanupStack::PushL(item);

		TInt ret = KErrNone, authErr = KErrNone;
		TPtrC8 AUTN, RAND, RES, IK, CK, Kc, AUTS, appId;
		TAuthInfo authInfo;
		TInt dataFrmt;

		//Get the data format
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 8, appId);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element APPID returned %d (element no. %d) from tag %s.",ret,8,KScAuthInfo);
			_LIT8(emptyAID, "");
			appId.Set(emptyAID);
			}
		else
			{
			ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 9, dataFrmt);
			if(ret != KErrNone)
				{
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element DATAFRMT returned %d (element no. %d) from tag %s.",ret,9,KScAuthInfo);
				}
			else if (dataFrmt >= EMaxConfigDataFormat)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_5, "WARNING IN CONFIGURATION FILE PARSING - Invalid format for SC Authenticate tag");
				dataFrmt = EConfigDataFormatAscii;
				}

			authInfo.iAID.Copy(appId);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iAID);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get the AUTN
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, AUTN);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTN returned %d (element no. %d) from tag %s.",ret,0,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAUTN.Copy(AUTN);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iAUTN);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get the RAND
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, RAND);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element RAND returned %d (element no. %d) from tag %s.",ret,1,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iRAND.Copy(RAND);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iRAND);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get the RES
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, RES);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element RES returned %d (element no. %d) from tag %s.",ret,2,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iRES.Copy(RES);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iRES);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}
		
		//Get the IK
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 3, IK);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element IK returned %d (element no. %d) from tag %s.",ret,3,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iIK.Copy(IK);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iIK);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get the CK
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 4, CK);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element CK returned %d (element no. %d) from tag %s.",ret,4,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iCK.Copy(CK);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iCK);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get the CK
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 5, Kc);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element KC returned %d (element no. %d) from tag %s.",ret,5,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iKc.Copy(Kc);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iKc);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get the AUTS
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 6, AUTS);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTS returned %d (element no. %d) from tag %s.",ret,6,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAUTS.Copy(AUTS);

			switch (dataFrmt)
				{
			case EConfigDataFormatMixedBinaryAndAscii:
				ParseMixedBinaryAsciiDataL(authInfo.iAUTS);
				break;
			//case EConfigDataFormatAscii: // Do nothing
			//default:
				}
			}

		//Get authentication error
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 7, authErr);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEAUTHINFOL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTHERR returned %d (element no. %d) from tag %s.",ret,7,KScAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAuthErr = authErr;
			}

		iAuthInfoList.Append(authInfo);
		CleanupStack::Pop(item);
		} // end FOR Loop
	}

/**
Parses the list of GBAAuthInfo tags from the config.txt file.
*/
void CSimSmartCardAuth::ParseGBAAuthInfoL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_1, "CSimSmartCardAuth::ParseGBAAuthInfoL called");

	CTestConfigItem* item = NULL;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_2, "Starting to Parse GBA Authentication Info");
	TInt count = CfgFile()->ItemCount(KGBAAuthInfo);

	TInt index;
	for(index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KGBAAuthInfo, index));
		if(item == NULL)
			{
			break;
			}
		CleanupStack::PushL(item);

		TInt ret = KErrNone, authErr = KErrNone;
		TPtrC8 AUTN, RAND, appId, RES, AUTS, NAFID,IMPI, KSExtNaf, BtID, KeyLifeTime ;
		TBool OtherApplnBusy, applnActive;
		TGBAAuthInfo authInfo;
		

		//Get the AUTN
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, AUTN);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTN returned %d (element no. %d) from tag %s.",ret,0,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAUTN.Copy(AUTN);
			}

		//Get the RAND
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, RAND);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element RAND returned %d (element no. %d) from tag %s.",ret,1,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iRAND.Copy(RAND);
			}

		//Get the APPID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, appId);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element APPID returned %d (element no. %d) from tag %s.",ret,2,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAID.Copy(appId);
			}
		
		//Get the RES
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 3, RES);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element RES returned %d (element no. %d) from tag %s.",ret,3,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iRES.Copy(RES);
			}
		
		//Get the AUTS
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 4, AUTS);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTS returned %d (element no. %d) from tag %s.",ret,4,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAUTS.Copy(AUTS);
			}
		
		//Get the NAFID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 5, NAFID);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element NAFID returned %d (element no. %d) from tag %s.",ret,5,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iNAFID.Copy(NAFID);
			}
		
		//Get the IMPI
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 6, IMPI);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element IMPI returned %d (element no. %d) from tag %s.",ret,6,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iIMPI.Copy(IMPI);
			}
		
		//Get the KsExtNaf
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 7, KSExtNaf);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element KSEXTNAF returned %d (element no. %d) from tag %s.",ret,7,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iKsExtNaf.Copy(KSExtNaf);
			}
		
		//Get the BtID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 8, BtID);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element BTID returned %d (element no. %d) from tag %s.",ret,8,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iBtid.Copy(BtID);
			}
		
		//Get the KeyLifeTime
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 9, KeyLifeTime);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element KEYLIFETIME returned %d (element no. %d) from tag %s.",ret,9,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iKeyLifeTime.Copy(KeyLifeTime);
			}

		//Get the OtherApplnBusy
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 10, OtherApplnBusy);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element OTHERAPPLNBUSY returned %d (element no. %d) from tag %s.",ret,10,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iOtherApplnBusy = OtherApplnBusy;
			}

		//Get the CurrentApplnActive
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 11, applnActive);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element APPLNACTIVE returned %d (element no. %d) from tag %s.",ret,11,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iApplnActive = applnActive;
			}

		//Get authentication error
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 12, authErr);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEGBAAUTHINFOL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTHERR returned %d (element no. %d) from tag %s.",ret,12,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAuthErr = authErr;
			}

		iGBAAuthInfoList->AppendL(authInfo);
		CleanupStack::Pop(item);
		} // end FOR Loop
	}

/**
 Parses the list of MBMSAuthInfo tags from the config.txt file.
*/
void CSimSmartCardAuth::ParseMBMSAuthInfoL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_1, "CSimSmartCardAuth::ParseMBMSAuthInfoL called");

	CTestConfigItem* item = NULL;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_2, "Starting to Parse MBMS Authentication Info");

	TInt count = CfgFile()->ItemCount (KMBMSInfo);
	TMBMSInfo tMbmsInfo;

	for (TInt index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KMBMSInfo, index));
		if ( item == NULL)
			{
			break;
			}
		CleanupStack::PushL (item);

		TInt ret = KErrNone;
		TPtrC8 iMikey, oMikey, mtk, saltKey, keyDmn, MskIdgrp;
		TPtrC8 MukId, MukIdi, MukIdr, MukTimeStamp, appId, KSIntNaf;
		TBool applnActive;
		TInt authErr;

		//Get the input Mikey
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 0, iMikey);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element IMIKEY returned %d (element no. %d) from tag %s.", ret, 0, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iInputMikey.Copy (iMikey);
			}

		//Get the output Mikey
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 1, oMikey);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element OMIKEY returned %d (element no. %d) from tag %s.", ret, 1, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iOutputMikey.Copy (oMikey);
			}

		//Get the MTK
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 2, mtk);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element MTK returned %d (element no. %d) from tag %s.", ret, 2, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMtk.Copy (mtk);
			}

		//Get the SaltKey
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 3,
				saltKey);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element SALTKEY returned %d (element no. %d) from tag %s.", ret, 3, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iSaltkey.Copy (saltKey);
			}

		//Get the KeyDomain
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 4, keyDmn);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element KEYDMN  returned %d (element no. %d) from tag %s.", ret, 4, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iKeyDmn.Copy (keyDmn);
			}

		//Get the MskIdgrp
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 5,
				MskIdgrp);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element MSKIDGRP returned %d (element no. %d) from tag %s.", ret, 5, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMskIdGrp.Copy (MskIdgrp);
			}

		//Get the iMukIdTlv
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 6, MukId);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element MUKID  returned %d (element no. %d) from tag %s.", ret, 6, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukIdTlv.Copy (MukId);
			}

		//Get the MukIdi
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 7, MukIdi);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element MUKIDI returned %d (element no. %d) from tag %s.", ret, 7, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukIdi.Copy (MukIdi);
			}

		//Get the MukIdr 
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 8, MukIdr);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element MUKIDR  returned %d (element no. %d) from tag %s.", ret, 8, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukIdr.Copy (MukIdr);
			}

		//Get the MukTimeStamp
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 9,MukTimeStamp);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element IMIKEY returned %d (element no. %d) from tag %s.", ret, 9, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukTimestamp.Copy (MukTimeStamp);
			}

		//Get the APPID
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 10, appId);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element APPID returned %d (element no. %d) from tag %s.", ret, 10, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iAID.Copy (appId);
			}

		//Get the KsIntNaf
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 11,KSIntNaf);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element KSINTNAF returned %d (element no. %d) from tag %s.", ret, 11, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iKsIntNaf.Copy (KSIntNaf);
			}

		//Get the CurrentApplnActive
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 12,applnActive);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element APPLNACTIVE returned %d (element no. %d) from tag %s.", ret, 12, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iApplnActive = applnActive;
			}

		//Get authentication error
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 13,authErr);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSMARTCARDAUTH_PARSEMBMSAUTHINFOL_16, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTHERR returned %d (element no. %d) from tag %s.", ret, 13, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iAuthErr = authErr;
			}
		
		iMBMSInfoList->AppendL(tMbmsInfo);
		CleanupStack::Pop (item);
		}
	}
	



