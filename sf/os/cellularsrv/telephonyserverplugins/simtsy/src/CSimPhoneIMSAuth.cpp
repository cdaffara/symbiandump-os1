// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the Phone IMS Authorization/Authentication code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhoneIMSAuthTraces.h"
#endif

#include <testconfigfileparser.h>
#include <etelmmerr.h>
#include "CSimPhone.h"

//
// CSimPhoneIMSAuth
//

CSimPhoneIMSAuth* CSimPhoneIMSAuth::NewL(CSimPhone* aPhone)
/**
 * Standard two phase constructor.
 * @param aPhone The phone object from which the Phone Smart Card applications will open
 */
	{
	CSimPhoneIMSAuth* obj=new(ELeave) CSimPhoneIMSAuth(aPhone);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop();
	return obj;
	}

CSimPhoneIMSAuth::CSimPhoneIMSAuth(CSimPhone* aPhone)
	: iPhone(aPhone)
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this Phone SmartCard App was opened.
 */
	{}


void CSimPhoneIMSAuth::ConstructL()
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves due to not enough memory or if any data member does not construct for any reason.
*/
	{
	iTimer=CSimTimer::NewL(iPhone);
	iGetAuthorizationData = new CArrayPtrFlat<CListReadAllAttempt>(1);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_CONSTRUCTL_1, "Starting to parse Phone IMS Authorization/Authentication config params...");
	ParseAuthorizationInfoL();
	ParseAuthenticationInfoL();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_CONSTRUCTL_2, "Finished parsing Phone IMS Authorization/Authentication config params...");
	}


CSimPhoneIMSAuth::~CSimPhoneIMSAuth()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{
	delete iTimer;
	if(iGetAuthorizationData)
		{
		iGetAuthorizationData->ResetAndDestroy();
		}
	delete iGetAuthorizationData;

	TInt authInfoCount = iAuthorizationInfoList.Count();
	TInt ii;
	for(ii = 0; ii < authInfoCount; ii++)
		{
		iAuthorizationInfoList[ii].iIMPUArray.Close();
		}
	iAuthorizationInfoList.Close();

	iAuthenticationInfoList.Close();

	//RAJ TODO
	}


const CTestConfigSection* CSimPhoneIMSAuth::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_CFGFILE_1, ">>CSimPhoneIMSAuth::CfgFile");
	return iPhone->CfgFile();
	}

void CSimPhoneIMSAuth::TimerCallBack(TInt /*aId*/)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*
* @param aId an id identifying which timer callback is being called
*/	
	{
	iTimerStarted = EFalse;
	iCurrentAuthorizationInfo++;
	if(iAuthInfoChangeNotifyPending)
		{
		iAuthInfoChangeNotifyPending = EFalse;
		iPhone->ReqCompleted(iAuthInfoChangeNotifyReqHandle, KErrNone);
		}
	}
	
void CSimPhoneIMSAuth::ParseAuthorizationInfoL()
/**
* Parses the list of Authorization Info tags from the Config file
*
*/
	{
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	TPtrC8 IMPI, IMPUValue, HNDN;
	TInt IMPUCount, authorizationDataSource, infoChangeDelay;
	TAuthorizationInfo authorizationInfo;
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_1, "Starting to Parse IMS Authorization Info");
	TInt count = CfgFile()->ItemCount(KAuthorizationInfo);
	
	TInt index;
	for(index=0;index<count;index++)
		{
		item=CfgFile()->Item(KAuthorizationInfo,index);
		TInt delimiterNum = 0;
		if(!item)
			{
			break;
			}
			
		//Get the IMPI
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterNum,IMPI);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_2, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
			continue;
			}
		else
			{
			authorizationInfo.iIMPI.Copy(IMPI);
			delimiterNum++;
			}

		//Get number of elements in the IMPU Array
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterNum,IMPUCount);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_3, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
			continue;
			}
		else
			{
			delimiterNum++;
			RArray<RMobilePhone::TIMPU> IMPUArray;
			TBool IMPUError = EFalse;
			
			//Get the IMPU values
			TInt ii;
			for(ii = 0;ii < IMPUCount; ii++)
				{
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterNum,IMPUValue);
				if(ret!=KErrNone)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_4, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
					IMPUError = ETrue;
					break;
					}
				else
					{
					IMPUArray.Append(IMPUValue);
					delimiterNum++;
					}
				}
			
			if(IMPUError)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_5, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
				continue;
				}
			else
				{
				authorizationInfo.iIMPUArray = IMPUArray;	
				}
			}
			
		//Get the HNDN
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterNum,HNDN);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_6, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
			continue;
			}
		else
			{
			authorizationInfo.iHomeNetworkDomainName.Copy(HNDN);
			delimiterNum++;
			}
			
		//Get the Authorization Data Source
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterNum,authorizationDataSource);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_7, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
			continue;
			}
		else
			{
			authorizationInfo.iAuthenticationDataSource = 
				static_cast<RMobilePhone::TAuthorizationDataSource>(authorizationDataSource);
				
			delimiterNum++;
			}
		
		//Get the Authorization Info change delay
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,delimiterNum,infoChangeDelay);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHORIZATIONINFOL_8, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHORIZATION INFO TAG");
			continue;
			}
		else
			{
			authorizationInfo.iInfoChangeDelay = infoChangeDelay;
			}		
		
		iAuthorizationInfoList.Append(authorizationInfo);
		}//end FOR Loop
	iCurrentAuthorizationInfo = 0;
	}

void CSimPhoneIMSAuth::ParseAuthenticationInfoL()
/**
* Parses the list of Authentication Info tags from the Config file
*
*/
	{
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	TPtrC8 AUTN, RAND, RES, IK, CK, AUTS;
	TInt authErr;
	TAuthenticationInfo authenticationInfo;
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_1, "Starting to Parse IMS Authentication Info");
	TInt count = CfgFile()->ItemCount(KAuthenticationInfo);
	
	TInt index;
	for(index=0; index<count; index++)
		{
		item=CfgFile()->Item(KAuthenticationInfo,index);
		if(!item)
			{
			break;
			}
			
		//Get the AUTN
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,AUTN);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_2, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iAUTN.Copy(AUTN);
			}

		//Get the RAND
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,RAND);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_3, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iRAND.Copy(RAND);
			}

		//Get the RES
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,RES);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_4, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iRES.Copy(RES);
			}
		
		//Get the IK
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,IK);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_5, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iIK.Copy(IK);
			}

		//Get the CK
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,CK);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_6, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iCK.Copy(CK);
			}

		//Get the AUTS
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,AUTS);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_7, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iAUTS.Copy(AUTS);
			}
		
		//Get authentication error
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,authErr);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PARSEAUTHENTICATIONINFOL_8, "ERROR IN CONFIGURATION FILE PARSING: BAD AUTHENTICATION INFO TAG");
			continue;
			}
		else
			{
			authenticationInfo.iAuthenticationErr = authErr;
			}
		
		iAuthenticationInfoList.Append(authenticationInfo);	
		}//end FOR Loop
	}

TInt CSimPhoneIMSAuth::GetAuthorizationInfoPhase1(const TTsyReqHandle aTsyReqHandle, 
			RMobilePhone::TClientId* aClientId,TInt* aBufSize)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_GETAUTHORIZATIONINFOPHASE1_1, "CSimPhoneIMSAuth::GetAuthorizationInfoPhase1 called");
	TInt ret(KErrNone);
	TInt leaveCode(KErrNone);
	TRAP(leaveCode, ret=ProcessGetAuthorizationInfoPhase1L(aTsyReqHandle, aClientId, aBufSize););
	if (leaveCode != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
		
	return ret;
	}


TInt CSimPhoneIMSAuth::ProcessGetAuthorizationInfoPhase1L(const TTsyReqHandle aTsyReqHandle,
			RMobilePhone::TClientId* aClientId, TInt* aBufSize)
	{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_PROCESSGETAUTHORIZATIONINFOPHASE1L_1, "CSimPhoneIMSAuth::GetAuthorizationInfoPhase1 called");

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(*aClientId,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	RMobilePhone::CImsAuthorizationInfoV5* iAuthInfo = RMobilePhone::CImsAuthorizationInfoV5::NewL();
    CleanupStack::PushL(iAuthInfo);

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(*iAuthInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
        CleanupStack::PopAndDestroy(2, read); // iAuthInfo, read		
		return KErrNone;
		}

	iAuthInfo->iIMPI = iAuthorizationInfoList[iCurrentAuthorizationInfo].iIMPI;

	TInt IMPUCount = iAuthorizationInfoList[iCurrentAuthorizationInfo].iIMPUArray.Count();
	TInt index;
	for(index = 0; index < IMPUCount; index++)
		{
		iAuthInfo->iIMPUArray.Append(iAuthorizationInfoList[iCurrentAuthorizationInfo].iIMPUArray[index]);
		}

	iAuthInfo->iHomeNetworkDomainName = 
		iAuthorizationInfoList[iCurrentAuthorizationInfo].iHomeNetworkDomainName;
	iAuthInfo->iAuthenticationDataSource = 
		iAuthorizationInfoList[iCurrentAuthorizationInfo].iAuthenticationDataSource;
	
	HBufC8* iBuff=NULL;
	iAuthInfo->ExternalizeL(iBuff);
    CleanupStack::PopAndDestroy(iAuthInfo);	
	CleanupDeletePushL(iBuff);
		
	CBufFlat* buf=CBufFlat::NewL(iBuff->Length());
	CleanupStack::PushL(buf);
	buf->InsertL(0,*iBuff);	
	
	read->iListBuf = buf;
	CleanupStack::Pop(buf);	
	CleanupStack::PopAndDestroy(iBuff);
	
	iGetAuthorizationData->AppendL(read);
    CleanupStack::Pop(read);
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	
	}


TInt CSimPhoneIMSAuth::GetAuthorizationInfoPhase2(const TTsyReqHandle aTsyReqHandle,
			RMobilePhone::TClientId* aClientId, TDes8* aBuffer)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_GETAUTHORIZATIONINFOPHASE2_1, "CSimPhoneIMSAuth::GetAuthorizationInfoPhase2 called");
	CListReadAllAttempt* read=NULL;

	// Find the get Authorization Info attempt from this client
	TInt index;
	for (index = 0; index < iGetAuthorizationData->Count(); index++)
		{
		read = iGetAuthorizationData->At(index);
		if ((read->iClient.iSessionHandle==aClientId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClientId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iGetAuthorizationData->Delete(index);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrBadHandle);
	return KErrNone;	
	}


TInt CSimPhoneIMSAuth::GetAuthorizationInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_GETAUTHORIZATIONINFOCANCEL_1, "CSimPhoneIMSAuth::GetAuthorizationInfoCancel called");

	// Remove the read all attempt from iGetAuthorizationData
	CListReadAllAttempt* read=NULL;
	TInt index;
	for (index = 0; index <iGetAuthorizationData->Count(); index++)
		{
		read = iGetAuthorizationData->At(index);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetAuthorizationData->Delete(index);
			break;
			}
		}
		
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CSimPhoneIMSAuth::NotifyImsAuthorizationInfoChanged(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_NOTIFYIMSAUTHORIZATIONINFOCHANGED_1, "CSimPhoneIMSAuth::NotifyImsAuthorizationInfoChanged called");
	__ASSERT_ALWAYS(!iAuthInfoChangeNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));

	TInt count=iAuthorizationInfoList.Count();
	if(iCurrentAuthorizationInfo < count)
		{
		iTimer->Start(iAuthorizationInfoList[iCurrentAuthorizationInfo].iInfoChangeDelay,this);
		iTimerStarted = ETrue;
		}

	iAuthInfoChangeNotifyPending=ETrue;
	iAuthInfoChangeNotifyReqHandle = aTsyReqHandle;		

	return KErrNone;
	}


TInt CSimPhoneIMSAuth::NotifyImsAuthorizationInfoChangedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_NOTIFYIMSAUTHORIZATIONINFOCHANGEDCANCEL_1, "CSimPhoneIMSAuth::NotifyImsAuthorizationInfoChangedCancel called");
	if(iAuthInfoChangeNotifyPending)
		{
		if(iTimerStarted)
			{
			iTimer->Cancel();
			}

		iAuthInfoChangeNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle, KErrCancel);
		}

	return KErrNone;
	}


TInt CSimPhoneIMSAuth::GetAuthenticationData(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthenticationData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_GETAUTHENTICATIONDATA_1, "CSimPhoneIMSAuth::AuthentificationData called");

	RMobilePhone::TImsAuthenticateDataV5Pckg* authenticateDataPckgd =
			(RMobilePhone::TImsAuthenticateDataV5Pckg*)aAuthenticationData;
	RMobilePhone::TImsAuthenticateDataV5& authenticateData=(*authenticateDataPckgd)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(authenticateData);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}

	TInt index;
	for(index = 0; index < iAuthenticationInfoList.Count(); index++)
		{
		if(authenticateData.iAUTN == iAuthenticationInfoList[index].iAUTN &&
			authenticateData.iRAND == iAuthenticationInfoList[index].iRAND)
			{
			TInt ret = iAuthenticationInfoList[index].iAuthenticationErr;
			if(ret != KErrNone)
				{
				//return the AUTS and blank everything else
				authenticateData.iAUTS = iAuthenticationInfoList[index].iAUTS;
				authenticateData.iRES = _L8("");
				authenticateData.iIK = _L8("");
				authenticateData.iCK = _L8("");
				}
			else
				{
				authenticateData.iRES = iAuthenticationInfoList[index].iRES;
				authenticateData.iIK = iAuthenticationInfoList[index].iIK;
				authenticateData.iCK = iAuthenticationInfoList[index].iCK;
				}

			iPhone->ReqCompleted(aTsyReqHandle, ret);
			return KErrNone;
			}
		}

	iPhone->ReqCompleted(aTsyReqHandle, KErrCorrupt);
	return KErrNone;
	}


TInt CSimPhoneIMSAuth::GetAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEIMSAUTH_GETAUTHENTICATIONDATACANCEL_1, "CSimPhoneIMSAuth::AuthentificationDatCancel called");
	iPhone->ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}
