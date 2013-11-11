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
// @file ConnectionServer.TestSteps.cpp
// 
//

#include <commdbconnpref.h>
#include <simtsy.h>
#include <cs_subconevents.h>
#include "ConnectionServer.TestSteps.h"
#include "networking\qos3gpp_subconparams.h"
#include <etelpckt.h>			// no longer available from qos3gpp_subconparams.h
#include <in_sock.h>

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestCnctnSr, "ESockTestCnctnSr");
#endif


const TUint KMaxAccessPoints = 16;

_LIT(KAccessPointFilter, "QueryAPStatusFilter");
_LIT(KQueryAPMatchFormat, "QueryAPMatch%d");
_LIT(KExpectedNumberOfAPs, "ExpectedNumberOfAPs");
_LIT(KExpectedAPFormat, "ExpectedAP%d");
_LIT(KExpectedAPStatusFormat, "ExpectedAP%dStatus");

using namespace ConnectionServ;

// Create Connection Server
//-------------------------

CCreateRConnectionServStep::CCreateRConnectionServStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRConnectionServStep);
	}

TVerdict CCreateRConnectionServStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCreateRConnectionServStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
		|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCreateRConnectionServStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CreateConnectionServer(iConnectionServName);

    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not create conneciton server instance (%S)."), &iConnectionServName);
        INFO_PRINTF2(_L("Error: %d."),error);
		return EFail;
		}

	return EPass;
	}


// Connect Connection Server
//--------------------------
_LIT(KTierName, "TierName");
CConnectRConnectionServStep::CConnectRConnectionServStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KConnectRConnectionServStep);
	}

TInt CConnectRConnectionServStep::ConfigureFromIni()
	{
	// Default the name to nothing until we know what it is
	iConnectionServName.Set(KNullDesC);
	
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

	TBool tierIdExists = GetIntFromConfig(iSection, KConnectionServTierId, reinterpret_cast<TInt&>(iTierId));
	
	// If no tier id then try for a tier name instead
	if(!tierIdExists)
		{
		TPtrC tierName;
		if((GetStringFromConfig(iSection, KTierName, tierName) != 1))
			{
			INFO_PRINTF1(_L("Missing tier identifier. Require either TierName or TierId"));
			return KErrNotFound;
			}
		else
			{
			// Decode the tier name in to a tier id
			TInt error = TierNameToTierId(tierName, iTierId);
			if(error != KErrNone)
				{
				INFO_PRINTF1(_L("Unrecognised TierName field"));
				return KErrNotFound;
				}
			}
		}

    // All ok if we got this far
    return KErrNone;
	}

_LIT(KLinkTier, "LinkTier");
const TInt KLinkTierId = 0x10281DF5;
_LIT(KProtoTier, "ProtoTier");
const TInt KProtoTierId = 0x10281DF0;
_LIT(KNetworkTier, "NetworkTier");
const TInt KNetworkTierId = KAfInet; //KAfInet=0x0800
_LIT(KDummyTier, "DummyTier");
const TInt KDummyTierId = 0x1028302B;
_LIT(KWiFiTier, "WiFiTier");
const TInt KWiFiTierId = 0x1028300D;
_LIT(KMBMSTier, "MBMSTier");
const TInt KMBMSTierId = 0x2001A35B;

TInt CConnectRConnectionServStep::TierNameToTierId(const TDesC& aTierName, TUint& aTierId)
	{
	if(aTierName == KLinkTier)
		{
		aTierId = KLinkTierId;
		}
	else if (aTierName == KProtoTier)
		{
		aTierId = KProtoTierId;
		}
	else if (aTierName == KNetworkTier)
		{
		aTierId = KNetworkTierId;
		}
	else if (aTierName == KDummyTier)
		{
		aTierId = KDummyTierId;
		}
	else if (aTierName == KWiFiTier)
		{
		aTierId = KWiFiTierId;
		}
	else if (aTierName == KMBMSTier)  // MBMS_BROADCAST
		{  
		aTierId = KMBMSTierId;
		}
	else
		{
		return KErrNotFound;
		}
	
	return KErrNone;
	}

TVerdict CConnectRConnectionServStep::doSingleTestStep()
	{
	SetTestStepResult(EFail);

    TInt error = iEsockTest->ConnectConnectionServer(iConnectionServName, iTierId);

    if (error != KErrNone)
        {
        INFO_PRINTF2(_L("Could not connect connection server (%S)."), &iConnectionServName);
        INFO_PRINTF2(_L("Error: %d."), error);
        SetTestStepError(error);
        }
    else
    	{
    	SetTestStepResult(EPass);
    	}

	return TestStepResult();
	}


// Create Connection Server API Extension
//---------------------------------------
CRConnectionServApiExtStep::CRConnectionServApiExtStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRConnectionServApiExtStep);
	}


TInt CRConnectionServApiExtStep::ConfigureFromIni()
	{
	// Default the name to nothing until we know what it is
	iConnectionServName.Set(KNullDesC);
	
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

	TBool tierIdExists = GetIntFromConfig(iSection, KConnectionServTierId, reinterpret_cast<TInt&>(iTierId));
	
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CRConnectionServApiExtStep::doSingleTestStep()
	{
	SetTestStepResult(EFail);

	__UHEAP_MARK;
    RConnectionServ* connServ = iEsockTest->iConnectionServs.Find(iConnectionServName);
    if (connServ == NULL)
    	{
		return TestStepResult();
    	}

    INFO_PRINTF1(_L("RConnectionServApiExt: Opening data monitoring extension"));
 	TInt err = iExt.Open(*connServ);
    INFO_PRINTF1(_L("RConnectionServApiExt: Data monitoring extension opened"));
    
    
   	TRequestStatus tStatus;
   	TBuf8<1024> tResponseEventBuf;

    INFO_PRINTF1(_L("RConnectionServApiExt: RequestDataTransferred"));
    iExt.RequestDataTransferred(tResponseEventBuf, tStatus); 
    User::WaitForRequest(tStatus); // wait for 2 seconds (Dummy TM implementation)
    INFO_PRINTF1(_L("RConnectionServApiExt: RequestDataTransferred returned"));


    INFO_PRINTF1(_L("RConnectionServApiExt: RequestDataTransferred"));
    iExt.RequestDataTransferred(tResponseEventBuf, tStatus); 
    INFO_PRINTF1(_L("RConnectionServApiExt: CancelDataTransferredRequest"));
    iExt.CancelDataTransferredRequest();

	RTimer timer;
   	TRequestStatus timerStatus;
	timer.CreateLocal();
    timer.After(timerStatus, 1e6);

    User::WaitForRequest(tStatus, timerStatus); // if it works properly, tStatus will be signalled first
    INFO_PRINTF1(_L("RConnectionServApiExt: RequestDataTransferred returned"));
    if (tStatus != KRequestPending)
    	{
    	if (tStatus == KErrCancel)
    		{
			SetTestStepResult(EPass);
    		}
		timer.Cancel();
    	}

	timer.Close();
	iExt.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
	}



// Close Connection Server
//------------------------
CCloseRConnectionServStep::CCloseRConnectionServStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRConnectionServStep);
	}

TInt CCloseRConnectionServStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
		|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRConnectionServStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CloseConnectionServer(iConnectionServName);

    if (error != KErrNone)
        {
        INFO_PRINTF2(_L("Could not close connection server (%S)."), &iConnectionServName);
        INFO_PRINTF2(_L("Error: %d."), error);
        return EFail;
        }

	return EPass;
	}


_LIT(KAccessPointGenericParameterSet, "GenericParameterSet");
_LIT(KAccessPointParametersType, "ParametersType");
_LIT(KNone, "none");
//_LIT(KAccessPointWifiParameterSet, "WifiParameterSet");

// Connection server query base class
//-----------------------------------
TInt CRConnectionServQueryTestStepBase::BuildQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle)
	{
	// Build a query from the ini settings
	// Fill in list of APs to limit scope to
	// We have zero or more AP matches to read in
	for(TInt index = 0; index < KMaxAccessPoints; index++)
		{
		TBuf<64> accessPointMatch;
		accessPointMatch.Format(KQueryAPMatchFormat, index);
		TInt accessPointId;
		
		// Try to read the next AP
		if((GetIntFromConfig(iSection, accessPointMatch, accessPointId) != 1))
			{
			break;
			}
		else
			{
			// Add the newly found access point to the table
			TAccessPointInfo apInfo;
			apInfo.SetAccessPoint(accessPointId);
			aQueryBundle.AddMatchConditionL(apInfo);
			}
		}
	
	// Fetch and parse filter specification
	TPtrC filterDes;
	TAccessPointStatusFilter filter;
	if((GetStringFromConfig(iSection, KAccessPointFilter, filterDes) == 1) && (filterDes.Length() != 0))
		{
		// We are potentially explicitly including no filter
		if(filterDes == KNone)
			{
			INFO_PRINTF1(_L("Access point filter explicitly set to none"));
			}
		else
			{
			TRAPD(error, FetchFilterL(filterDes, filter);)
			if(error != KErrNone)
				{
				INFO_PRINTF1(_L("Problem with filter syntax"));
				return KErrArgument;
				}

			// Add the filter to the bundle
			aQueryBundle.AddMatchConditionL(filter);
			}
		}
	else
		{
		INFO_PRINTF1(_L("No filter specified. No filter added to query"));
		}
		
	// Specify which type of parameters we want back
	TPtrC paramsType;
	if((GetStringFromConfig(iSection, KAccessPointParametersType, paramsType) == 1))
		{
		if (paramsType.CompareF(KAccessPointGenericParameterSet) == 0)
			{
			aQueryBundle.AddParameterSetToReturnL
				(XAccessPointGenericParameterSet::Type());
			INFO_PRINTF2(_L("Parameter set type:%S"), &KAccessPointGenericParameterSet);
			}
/*		else if (paramsType.CompareF(KAccessPointWifiParameterSet) == 0)
			{
			aQueryBundle.AddParameterSetToReturnL
				(XAccessPointGenericParameterSet::Type());
			aQueryBundle.AddParameterSetToReturnL
				(XAccessPointWifiParameterSet::Type());
			INFO_PRINTF3(_L("Parameter set types: %S and %S"), &KAccessPointGenericParameterSet, &KAccessPointWifiParameterSet);
			}
*/		else if (paramsType.CompareF(KNone) == 0)
			{
			INFO_PRINTF1(_L("Parameter set type set to *none*. None added to query."));
			}
		else
			{
			INFO_PRINTF1(_L("Parameter set type not recognised."));
			return KErrArgument;
			}
		}
	// Otherwise we use the generic set by default
	else
		{
		aQueryBundle.AddParameterSetToReturnL
			(XAccessPointGenericParameterSet::Type());
		}

	
    // All ok if we got this far
    return KErrNone;
	}

_LIT(KInvalid, "invalid");

void CRConnectionServQueryTestStepBase::FetchFilterL(
	const TDesC& aFilterDes,
	TAccessPointStatusFilter& aFetchedFilter)
	{
	// Is the filter perhaps supposed to be invalid
	if(aFilterDes == KInvalid)
		{
		aFetchedFilter = TAccessPointStatusFilter(-1);
		return;
		}

	// Otherwise parse it out from the string given
	TLex lexer;
	lexer.Assign(aFilterDes);
	TChar cCh;
	TChar rCh;
	TChar aCh;
	TChar sCh;
	if(!
		(
			( lexer.Get() == TChar('C') ) && ( cCh = lexer.Get() ) &&
			( lexer.Get() == TChar('R') ) && ( rCh = lexer.Get() ) &&
			( lexer.Get() == TChar('A') ) && ( aCh = lexer.Get() ) &&
			( lexer.Get() == TChar('S') ) && ( sCh = lexer.Get() )
		)
	)
		{
		User::Leave(KErrArgument);
		}

	TAccessPointFlagFilterValue c, r, a, s;
	c = ReadFlagFilterValueL(cCh);
	r = ReadFlagFilterValueL(rCh);
	a = ReadFlagFilterValueL(aCh);
	s = ReadFlagFilterValueL(sCh);

	aFetchedFilter.Configured(c);
	aFetchedFilter.Restricted(r);
	aFetchedFilter.Available(a);
	aFetchedFilter.Started(s);
	}

TInt CRConnectionServQueryTestStepBase::BuildExpectedResultsL(
	RArray<TExpectedAccessPointParameters>& aExpectedAPParameters,
	TInt& aNumExpectedAPs)
	{
	// Fetch the number of accesspoints expected to be returned
	if(GetIntFromConfig(iSection, KExpectedNumberOfAPs, aNumExpectedAPs) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KExpectedNumberOfAPs);
		return KErrNotFound;
		}
	
	// Fetch the list of access points and their status values
	TUint specificAPCount = 0;
	for(TInt index = 1; index < KMaxAccessPoints; index++)
		{
		TBuf<64> accessPointMatch;
		accessPointMatch.Format(KExpectedAPFormat, index);
		TInt accessPointId;
		
		// Try to read the next AP
		if((GetIntFromConfig(iSection, accessPointMatch, accessPointId) != 1))
			{
			break;
			}
		else
			{
			// We have access point identifier
			TAccessPointInfo apInfo;
			apInfo.SetAccessPoint(accessPointId);

			// Now require expected status for the given access point identifier
			TBuf<64> expectedAPMatch;
			expectedAPMatch.Format(KExpectedAPStatusFormat, index);

			// Read in the access point status
			TPtrC apStatusBuf;
			if((GetStringFromConfig(iSection, expectedAPMatch, apStatusBuf) != 1))
			{
				INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &expectedAPMatch);
				User::Leave(KErrNotFound);
			}

			TExpectedAccessPointStatusMask apStatusMask;
			TRAPD(error, FetchFilterL(apStatusBuf, apStatusMask));
			if(error != KErrNone)
				{
				INFO_PRINTF1(_L("Problem with result mask syntax"));
				return KErrArgument;
				}

			// Append the expected parameters to Set the expected parameters
			TExpectedAccessPointParameters apParameters(apInfo, apStatusMask);

			// Add the new parameters to a list of expected results
			aExpectedAPParameters.Append(apParameters);
			}
		
		// Update our count of specified access points we are expecting to see
		specificAPCount = index;
		}
	
	// Check that number of expected APs is less than or equal to the number of specific APs expected
	if(specificAPCount > aNumExpectedAPs)
		{
		INFO_PRINTF1(_L("More specific APs than total number expected"));
		return KErrArgument;
		}
	
	// All ok if we got this far
	return KErrNone;
	}

TAccessPointFlagFilterValue CRConnectionServQueryTestStepBase::ReadFlagFilterValueL(TChar ch)
	{
	if(ch == TChar('?'))
		return EAccessPointFlagMatchAny;
	if(ch == TChar('T') || ch == TChar('Y'))
		return EAccessPointFlagMatchTrue;
	if(ch == TChar('F') || ch == TChar('N'))
		return EAccessPointFlagMatchFalse;
	if(ch == TChar('-') || ch == TChar('_'))
		return EAccessPointFlagIgnore;
	
	User::Leave(KErrArgument);
	return EAccessPointFlagIgnore;
	}

TBool CRConnectionServQueryTestStepBase::CompareAgainstExpected(
	TInt aNumExpected,
	RArray<TExpectedAccessPointParameters>& aAPsExpected,
	ConnectionServ::CConnectionServParameterBundle& aResultsBundle)
	{
	// Default to not being valid results
	TBool verdict = EFalse;
	
	// Firstly compare the numbers of access points expected
	TUint numParamSetContainers = aResultsBundle.CountParamSetContainers();

	if(numParamSetContainers == aNumExpected)
		{
		// We have the expected number of access points
		// Now check that each is expected and that its status is as expected
		// We require to find each AP in our 'expected' list in our 'results' list (with identical status)
		TBool anyExpectedAPsNotFound = EFalse;
		TUint numExpected = aAPsExpected.Count();
		for(TUint i = 0; i < numExpected; i++)
			{
			const TExpectedAccessPointStatusMask& expectedAPStatus = aAPsExpected[i].AccessPointStatus();
			const TAccessPointInfo& expectedAPInfo = aAPsExpected[i].AccessPointInfo();

			TBool found = EFalse;

			TUint j = 0;
			const CParameterSetContainer* ctnr;
			while(TBool(ctnr = aResultsBundle.GetParamSetContainer(j++)))
				{
				TUint k = 0;
				const XParameterSetBase *set;
				while(TBool(set = ctnr->GetParameterSet(k++)))
					{
					if(set->IsTypeOf(XAccessPointGenericParameterSet::Type()))
						{
						const XAccessPointGenericParameterSet* params = static_cast<const XAccessPointGenericParameterSet*>(set);

						// Compare the values
						if((const_cast<TAccessPointInfo&>(params->AccessPointInfo()) == const_cast<TAccessPointInfo&>(expectedAPInfo)) &&
							(expectedAPStatus.CompareToAPStatus(params->AccessPointStatus())))
							{
							found = ETrue;
							break;	
							}
						}
					}

				if(found)
					break;
				}

			if(!found)
				{
				anyExpectedAPsNotFound = ETrue;
				break;
				}
			}

		if(anyExpectedAPsNotFound == EFalse)
			{
			// If we got this far then everything must be as expected
			verdict = ETrue;
			}
		}
		
		return verdict;
	}

void CRConnectionServQueryTestStepBase::BundleToString(const CParameterBundleBase* aBundle,TDes& aBuf)
	{
	aBuf.Append(_L("Bundle:\n"));
	const CParameterSetContainer* ctnr;
	TInt i=0;
	while(TBool(ctnr = aBundle->GetParamSetContainer(i++)))
		{
		ParamSetCtrToString(ctnr,aBuf);
		}
	}
/*
void CRConnectionServQueryTestStepBase::WifiParamsToString(
		const XAccessPointWifiParameterSet* aSet,
		TDes& aBuf)
	{
	aBuf.Append(_L("WPS:"));
	
	TBuf<256> temp;
								
	aBuf.Append(_L(" Ssid:"));
	temp.Copy(aSet->Ssid());
	aBuf.Append(temp);
	aBuf.Append(_L(" ChannelId:"));
	aBuf.AppendNum(aSet->ChannelId());
	aBuf.Append(_L(" Rssi:"));
	aBuf.AppendNum(aSet->Rssi());
	}
*/
void CRConnectionServQueryTestStepBase::ParamSetCtrToString(const CParameterSetContainer* aCtnr,TDes& aBuf)
	{
	aBuf.Append(_L("\tContainer id "));
	aBuf.AppendNum(aCtnr->Id());
	aBuf.Append(_L(":\n"));
	const XParameterSetBase *set;
	TInt i=0;
	while(TBool(set = aCtnr->GetParameterSet(i++)))
		{
		aBuf.Append(_L("\t\t"));
		if(set->IsTypeOf(XAccessPointGenericParameterSet::Type()))
			{
			GenericParamsToString(static_cast<const XAccessPointGenericParameterSet*>(set),aBuf);
			}
/*		else if (set->IsTypeOf(XAccessPointWifiParameterSet::Type()))
			{
			WifiParamsToString(static_cast<const XAccessPointWifiParameterSet*>(set),aBuf);
			}
*/		else if (set->IsTypeOf(XMBMSServiceQuerySet::Type())) //MBMS_BROADCAST
			{
			MBMSParamsToString(static_cast<const XMBMSServiceQuerySet*>(set), aBuf);
			}
		else
			{
			aBuf.Append(_L("unrecognised data type"));
			}
		aBuf.Append(_L("\n"));
		}
	}

void CRConnectionServQueryTestStepBase::GenericParamsToString(
		const XAccessPointGenericParameterSet* aSet,
		TDes& aBuf)
	{
	aBuf.Append(_L("GPS: AP:"));
	
	TAccessPointInfo inf = aSet->AccessPointInfo();
	
	aBuf.AppendNum(inf.AccessPoint());
	aBuf.Append(' ');

	TAccessPointStatus stat = aSet->AccessPointStatus();

	aBuf.Append(_L("C:"));
	aBuf.Append(GetApFlagChar(stat.Configured()));	
	aBuf.Append(_L(" R:"));
	aBuf.Append(GetApFlagChar(stat.Restricted()));	
	aBuf.Append(_L(" A:"));
	aBuf.Append(GetApFlagChar(stat.Available()));	
	aBuf.Append(_L(" S:"));
	aBuf.Append(GetApFlagChar(stat.Started()));	
	}

TChar CRConnectionServQueryTestStepBase::GetApFlagChar(TAccessPointFlagValue val)
	{
	switch(val)
		{
		case EAccessPointFlagFalse: return 'N';
		case EAccessPointFlagTrue: return 'Y';
		}
	
	return '?';
	}
	
void CRConnectionServQueryTestStepBase::MBMSParamsToString(
		const XMBMSServiceQuerySet* aSet,
		TDes& aBuf)
	{
	aBuf.Append(_L("MBMS PS:"));

	aBuf.Append(_L(" QueryType:"));
	if(aSet->GetQueryType() == ConnectionServ::XMBMSServiceQuerySet::EBearerAvailability)
		{
		aBuf.Append(_L("EBearerAvailability"));
		}
	else if(aSet->GetQueryType() == ConnectionServ::XMBMSServiceQuerySet::EAddService)
		{
		aBuf.Append(_L("EAddService"));
		}
	else if(aSet->GetQueryType() == ConnectionServ::XMBMSServiceQuerySet::ERemoveService)
		{
		aBuf.Append(_L("ERemoveService"));
		}
	else if(aSet->GetQueryType() == ConnectionServ::XMBMSServiceQuerySet::ERemoveAllService)
		{
		aBuf.Append(_L("ERemoveAllService"));
		}
	else
		{
		aBuf.Append(_L(" "));
		}

	aBuf.Append(_L(" BearerAvailability:"));
	if(aSet->GetMBMSBearerAvailability() == EMbmsAvailabilityUnknown)
		{
		aBuf.Append(_L("EMbmsAvailabilityUnknown"));
		}
	else if (aSet->GetMBMSBearerAvailability() == EMbmsUnavailable)
		{
		aBuf.Append(_L("EMbmsUnavailable"));
		}
	else if (aSet->GetMBMSBearerAvailability() == EMbmsAvailable)
		{
		aBuf.Append(_L("EMbmsAvailable"));
		}
	else 
		{
		aBuf.Append(_L(" "));
		}
	}

// CancelAccessPointStatusQuery
//-----------------------

CCancelAccessPointStatusQueryStep::CCancelAccessPointStatusQueryStep(CCEsockTestBase*& aEsockTest)
:   CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KCancelAccessPointStatusQueryStep);
	}

TInt CCancelAccessPointStatusQueryStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
		|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}
		
	TInt error;
	TRAP(error, iQueryBundle = CConnectionServParameterBundle::NewL());
	if(error != KErrNone)
		return error;

	TRAP(error, iResultsBundle = CConnectionServParameterBundle::NewL());
	if(error != KErrNone)
		return error;

	TRAP(error, BuildQueryL(*iQueryBundle);)
	if(error != KErrNone)
		return KErrNotFound;

	TRAP(error, BuildExpectedResultsL(iExpectedAPParameters, iNumExpectedAPs);)
	if(error != KErrNone)
		return KErrNotFound;
	
	return KErrNone;
	}
	
CCancelAccessPointStatusQueryStep::~CCancelAccessPointStatusQueryStep()
	{
	iExpectedAPParameters.Close();
	delete iResultsBundle;
	delete iQueryBundle;
	}

TVerdict CCancelAccessPointStatusQueryStep::doSingleTestStep()
	{
	// Default to fail
	SetTestStepResult(EFail);

	// Perform the query already configured
	__ASSERT_DEBUG(iResultsBundle, User::Panic(KSpecAssert_ESockTestCnctnSr, 1));
	TInt error = iEsockTest->CancelAccessPointStatusQuery(iConnectionServName, *iQueryBundle, *iResultsBundle);

	if(error == KErrCancel)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}


//AccessPointStatusQuery

CAccessPointStatusQueryStep::CAccessPointStatusQueryStep(CCEsockTestBase*& aEsockTest)
:   CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KAccessPointStatusQueryStep);
	}

TInt CAccessPointStatusQueryStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
		|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

	// Check if an error is expected
	if((GetIntFromConfig(iSection, KExpectedError, iExpectedError) == 1))
		{
		INFO_PRINTF2(_L("Expecting Error %d"), iExpectedError);
		}
	
	TInt error;
	TRAP(error, iQueryBundle = CConnectionServParameterBundle::NewL());
	if(error != KErrNone)
		return error;

	TRAP(error, iResultsBundle = CConnectionServParameterBundle::NewL());
	if(error != KErrNone)
		return error;

	TRAP(error, error = BuildQueryL(*iQueryBundle);)
	if(error != KErrNone)
		return KErrNotFound;

	TRAP(error, error = BuildExpectedResultsL(iExpectedAPParameters, iNumExpectedAPs);)
	if(error != KErrNone)
		return KErrNotFound;
	
	return KErrNone;
	}
	
CAccessPointStatusQueryStep::~CAccessPointStatusQueryStep()
	{
	iExpectedAPParameters.Close();
	delete iResultsBundle;
	delete iQueryBundle;
	}

TVerdict CAccessPointStatusQueryStep::doSingleTestStep()
	{
	// Default to fail
	SetTestStepResult(EFail);

	// Perform the query already configured
	__ASSERT_DEBUG(iResultsBundle, User::Panic(KSpecAssert_ESockTestCnctnSr, 2));

	INFO_PRINTF1(_L("A different thread Exited"));
	TInt error = iEsockTest->ConnectionAccessPointStatusQuery(iConnectionServName, *iQueryBundle, *iResultsBundle);

	// Log the contents of the incoming results bundle
	TBuf<1000> logBuf;
	BundleToString(iResultsBundle, logBuf);
	INFO_PRINTF1(logBuf);
	
	// Compare against expected, and report any errors
	if((error == iExpectedError) && (error != KErrNone))
		{
		INFO_PRINTF3(_L("Query error %d matched Expected error %d"), error, iExpectedError);
		SetTestStepResult(EPass);
		}
	else if(error != KErrNone)
		{
		INFO_PRINTF2(_L("Could not query connection server (%S)."), &iConnectionServName);
		INFO_PRINTF2(_L("Error: %d."), error);
		SetTestStepError(error);
		}
	else
		{
		TBool match = CompareAgainstExpected(
			iNumExpectedAPs,
			iExpectedAPParameters,
			*iResultsBundle);
		
		if(match)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("Access point query results to not match those expected."));
			}
		}
	
	return TestStepResult();
	}


TBool TExpectedAccessPointStatusMask::CompareToAPStatus(const ConnectionServ::TAccessPointStatus& aStatus) const
	{
	// Compare each of the 4 properties we are expecting
	TBool configured = CompareStatusFlagToMaskFlag(
		aStatus.Configured(),
		Configured());

	TBool restricted = CompareStatusFlagToMaskFlag(
		aStatus.Restricted(),
		Restricted());

	TBool available = CompareStatusFlagToMaskFlag(
		aStatus.Available(),
		Available());

	TBool started = CompareStatusFlagToMaskFlag(
		aStatus.Started(),
		Started());

	return (configured && restricted && available && started);
	}

TBool TExpectedAccessPointStatusMask::CompareStatusFlagToMaskFlag(
	const ConnectionServ::TAccessPointFlagValue apFlag,
	const ConnectionServ::TAccessPointFlagFilterValue apMaskValue) const
	{
		// If mask is 'ignore' then we don't need to validate anything
		if(apMaskValue == ConnectionServ::EAccessPointFlagIgnore)
			{
			return ETrue;
			}

		if(apMaskValue == ConnectionServ::EAccessPointFlagMatchFalse)
			{
			if(apFlag != ConnectionServ::EAccessPointFlagFalse)
				{
				return EFalse;
				}
			else
				{
				return ETrue;
				}
			}

		if(apMaskValue == ConnectionServ::EAccessPointFlagMatchTrue)
			{
			if(apFlag != ConnectionServ::EAccessPointFlagTrue)
				{
				return EFalse;
				}
			else
				{
				return ETrue;
				}
			}

		// If expecting unknown then must be unknown
		if(apMaskValue == ConnectionServ::EAccessPointFlagMatchAny)
			{
			if(apFlag != ConnectionServ::EAccessPointFlagUnknown)
				{
				return EFalse;
				}
			else
				{
				return ETrue;
				}
			}

		// What are we doing here
		__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockTestCnctnSr, 3));
		return EFalse;
	}

// Request access point notification
//----------------------------------
static const TInt KNotifThreadStackSize = (32 * 1024); // TODO_CDG really how big should our stack be?
_LIT(KNotificationName, "NotificationName");

CRequestAccessPointNotificationStep::CRequestAccessPointNotificationStep(CCEsockTestBase*& aEsockTest)
	: CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KRequestAccessPointNotificationStep);
	}

TInt CRequestAccessPointNotificationStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	// Get the name of the connection server instance we are to use
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
		|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}
		
	// Get the name applied to the notification so we can look it up later when referring to the results
	if((GetStringFromConfig(iSection, KNotificationName, iNotifName) != 1)
		|| (iNotifName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNotificationName);
		return KErrNotFound;
		}
		
	TInt error;
	TRAP(error, iQueryBundle = CConnectionServParameterBundle::NewL());
	if(error != KErrNone)
		return error;

	TRAP(error, BuildQueryL(*iQueryBundle);)
	if(error != KErrNone)
		return KErrNotFound;

	return KErrNone;
	}

TVerdict CRequestAccessPointNotificationStep::doSingleTestStep()
	{
	// Fail by default
	SetTestStepResult(EFail);

	// Fetch the connection server on which to create the
	RConnectionServ* connServ = iEsockTest->iConnectionServs.Find(iConnectionServName);
	if(connServ == NULL)
		{
		INFO_PRINTF2(KErrString_NamedObjectNotFound, &iConnectionServName);
		}
	else
		{
		// Wrap up the request for the thread we will palm it off to
		CConnServNotificationWatcher* notifMgr = CConnServNotificationWatcher::NewLC
			(connServ, iQueryBundle);
		CleanupStack::Pop(notifMgr);
		
		// Give the notifing thread a handle to us
		notifMgr->iCommandThread.Open(RThread().Id(), EOwnerProcess);
			
		// Start a thread to manage the notifications as these occur with use of a scheduler
		TInt error = notifMgr->iNotifThread.Create(
			iNotifName,
			CConnServNotificationWatcher::EntryPoint,
			KNotifThreadStackSize,
			NULL,
			static_cast<TAny*>(notifMgr));

		// If all ok then kick off the notification thread
		if(error != KErrNone)
			{
			INFO_PRINTF2(_L("Could not start separate thread for capturing notifications. error:%d"), error);
			}
		else
			{
			// Get the thread started
			INFO_PRINTF1(_L("Worker thread kicked off to register for and collect the notifications."));
			notifMgr->iNotifThread.Resume();

			// Keep a reference to our notif manager for follow up test step
			TInt addError = iEsockTest->iNotifWatchers.Add(notifMgr, iNotifName);
			__ASSERT_DEBUG(addError == KErrNone, User::Panic(KSpecAssert_ESockTestCnctnSr, 4));

			// Await the completion of the notification setup (to avoid race conditions when, say,
			//  the next test step will tweak availability before the provider has had a chance to start).
			// Leaves if there was an error on startup..
			notifMgr->AwaitStartupCompletionL();
			INFO_PRINTF1(_L("Notification registration completed successfully."));
			
			SetTestStepResult(EPass);
			}
		}

	return TestStepResult();
	}



// Receive access point notification
//----------------------------------
CReceiveAccessPointNotificationStep::CReceiveAccessPointNotificationStep(CCEsockTestBase*& aEsockTest)
	: CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KReceiveAccessPointNotificationStep);
	}

_LIT(KWaitPeriod, "WaitPeriod");
_LIT(KExpectingNotification, "ExpectingNotification");

TInt CReceiveAccessPointNotificationStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	// Get the name of the connection
	if((GetStringFromConfig(iSection, KNotificationName, iNotifName) != 1)
		|| (iNotifName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNotificationName);
		return KErrNotFound;
		}
		
	// Offer the option of expecting no notification at all but default to expecting one
	iExpectingNotification = ETrue;
	// OPTIONAL
	GetBoolFromConfig(iSection, KExpectingNotification, iExpectingNotification);

	// Get the delay period before expecting a notification
	// OPTIONAL
	GetIntFromConfig(iSection, KWaitPeriod, iInitialDelayInSeconds);
		
	// Get a list of the expected results
	if(iExpectingNotification)
		{
		TRAPD(error, BuildExpectedResultsL(iExpectedAPParameters, iNumExpectedAPs);)
		if(error != KErrNone)
			return KErrNotFound;
		}
	
	return KErrNone;
	}

const TInt KMicroSecondsPerSecond = 1000000;

TVerdict CReceiveAccessPointNotificationStep::doSingleTestStep()
	{
	// Fail by default
	SetTestStepResult(EFail);

	if (!iExpectingNotification)
		{
		// RJL for SMP:
		// only delay for specified period if we're expecting to see nothing..
		//   else wait on notification queue
		User::After(iInitialDelayInSeconds * KMicroSecondsPerSecond);
		}
	
	// Fetch the notification watching entity
	CConnServNotificationWatcher* notifMgr = iEsockTest->iNotifWatchers.Find(iNotifName);
	if(!notifMgr)
		{
		INFO_PRINTF3(KErrString_NamedObjectNotFound, &iSection, &iNotifName);
		}
	else
		{
		// ##### SYNCHRONISE ACCESS #####
		if(iExpectingNotification)
			{
			// wait for items to arrive in notification queue
			notifMgr->WaitOnQueueL();
			}
		notifMgr->Wait();

		// Fetch the single oldest set of results only
		// Any following notifications captured will be collected in subsequent calls to this test step
		CConnectionServParameterBundle* resultsBundle = notifMgr->GetResultsBundle();
		//CleanupStack::PushL(resultsBundle); // Non leaving test function

		// Were we expecting a notification at all?
		if(iExpectingNotification)
			{
			// If indeed there were results then compare them with those expected
			if(resultsBundle != NULL)
				{
				// Firstly log the contents of the incoming results bundle
				TBuf<1000> logBuf;
				BundleToString(resultsBundle, logBuf);
				INFO_PRINTF1(logBuf);

				TBool match = CompareAgainstExpected(iNumExpectedAPs, iExpectedAPParameters, *resultsBundle);

				// If they matched those expected then we passed
				if(match)
					{
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("Access point notification results do not match those expected."));
					}
				}
			else
				{
				INFO_PRINTF1(_L("Received no notifications when expecting at least one."));
				}
			}
		else
			{
			// If we were not expecting a notification and received one then we failed
			if(resultsBundle != NULL)
				{
				INFO_PRINTF1(_L("Received a notificaiton when expecting none:"));
				TBuf<1000> logBuf;
				BundleToString(resultsBundle, logBuf);
				INFO_PRINTF1(logBuf);
				}
			else
				{
				INFO_PRINTF1(_L("No notification expected and none witnessed."));
				SetTestStepResult(EPass);
				}
			}
		
		// ##### SYNCHRONISE ACCESS #####
		notifMgr->Signal();

		// Get rid of the results we took ownership of
		//CleanupStack::PopAndDestroy(resultsBundle);
		}

	return TestStepResult();
	}


// Cancel access point notification
//----------------------------------
CCancelAccessPointNotificationStep::CCancelAccessPointNotificationStep(CCEsockTestBase*& aEsockTest)
	: CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KCancelAccessPointNotificationStep);
	}

TInt CCancelAccessPointNotificationStep::ConfigureFromIni()
	{
	// Get the name of the notification
	if((GetStringFromConfig(iSection, KNotificationName, iNotifName) != 1)
		|| (iNotifName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNotificationName);
		return KErrNotFound;
		}
		
	// Get the delay period before expecting a notification
	// OPTIONAL
	GetIntFromConfig(iSection, KWaitPeriod, iInitialDelayInSeconds);
		
	return KErrNone;
	}

TVerdict CCancelAccessPointNotificationStep::doSingleTestStep()
	{
	// Fail by default
	SetTestStepResult(EFail);

	// Wait first for any specified delay to provide for notifications that may need time to propagate
	User::After(iInitialDelayInSeconds * KMicroSecondsPerSecond);

	// Fetch the notification watching entity
	CConnServNotificationWatcher* notifMgr = iEsockTest->iNotifWatchers.Find(iNotifName);
	if(!notifMgr)
		{
		INFO_PRINTF3(KErrString_NamedObjectNotFound, &iSection, &iNotifName);
		}
	else
		{
		// Log on to our worker notif catcher thread in order to wait for it to finish
		TRequestStatus logonStatus;
		notifMgr->iNotifThread.Logon(logonStatus);

		// Send command to the thread managing the notification that we want cancelled
		// this will then wait upon the thread to complete the command
		notifMgr->CancelNotification();

		// Now wait for the thread to complete
		User::WaitForRequest(logonStatus);
		TInt logonError = logonStatus.Int();
	
		if(logonError == KErrNone)
			{
			INFO_PRINTF1(_L("Access point notification cancelled successfully."));
			SetTestStepResult(EPass);
			}
		
		// Remove our NotificationWatcher and Close the thread
		iEsockTest->iNotifWatchers.Remove(iNotifName);
		notifMgr->iNotifThread.Close();
		delete notifMgr;
		}
	
	return TestStepResult();
	}


// CConnServNotificationWatcher
//-----------------------------

const TUid KAccessPointNotificationPubSubUid = {0x10272F4B};

CConnServNotificationWatcher* CConnServNotificationWatcher::NewLC(
		ConnectionServ::RConnectionServ* aConnServ,
		ConnectionServ::CConnectionServParameterBundle* aQueryBundle)
	{
	CConnServNotificationWatcher* inst = new(ELeave) CConnServNotificationWatcher(aConnServ);
	CleanupStack::PushL(inst);
	inst->ConstructL(aQueryBundle);
	return inst;
	}

void CConnServNotificationWatcher::ConstructL(ConnectionServ::CConnectionServParameterBundle* aQueryBundle)
	{
	iQueryBundle = aQueryBundle;
	iMutex.CreateLocal();
	User::LeaveIfError(iQueueProp.Define(KAccessPointNotificationPubSubUid,reinterpret_cast<TUint32>(this),RProperty::EInt));
	User::LeaveIfError(iQueueProp.Attach(KAccessPointNotificationPubSubUid,reinterpret_cast<TUint32>(this)));
	}

CConnServNotificationWatcher::~CConnServNotificationWatcher()
	{
	iQueueProp.Delete(KAccessPointNotificationPubSubUid,reinterpret_cast<TUint32>(this));
	iQueueProp.Close();
	iMutex.Close();
	delete iQueryBundle;
	iNotificationBundles.ResetAndDestroy();
	iNotifThread.Close();
	}

void CConnServNotificationWatcher::CancelNotification()
		{
		iNotifThread.RequestComplete(iCancelStatus, KErrNone);
		}

void CConnServNotificationWatcher::AccessPointNotification(CConnectionServParameterBundle* aResult)
	{
	__ASSERT_ALWAYS(aResult, User::Invariant());
	
	// ##### SYNCHRONISE ACCESS #####
	Wait();
	
	/* Check whether the test case is for MBMS*/
	CConnectionServParameterSetContainer* parSetCtr = iQueryBundle->GetParamSetContainer(0);
	XMBMSServiceQuerySet* genParamSet = XMBMSServiceQuerySet::FindInParamSetContainer(*parSetCtr);
	
	if(genParamSet != NULL)
	{
			iHaveReceivedNotifSetupAck = ETrue;
	}
			
	
	// The first notification is simply acknowledgement of completion of notif set up so just consume

	if(!iHaveReceivedNotifSetupAck)
		{
		iHaveReceivedNotifSetupAck = ETrue;

		// We should check first that there are no bundles included which would be unexpected
		if(aResult->CountParamSetContainers() && iErrCode == KErrNone)
			{
			iErrCode = KErrTooBig;
			}
		
		// Send a signal to the "Request Notification" test step that setup has completed
		iQueueProp.Set(0);
		
		// Finally delete the incoming bundle
		delete aResult;
		}
	else
		{
		// Save our results away
		TInt error = iNotificationBundles.Append(aResult);

		// Update queue size property. Potentially will happen when 
		//  client is waiting for notifications (i.e. is subscribing to this
		//   property) which would cause the client to be signalled
		iQueueProp.Set(iNotificationBundles.Count());

		__ASSERT_ALWAYS(error == KErrNone, User::Invariant());
		}
	
	// ##### SYNCHRONISE ACCESS #####
	Signal();
	}


void CConnServNotificationWatcher::AccessPointNotificationError(TInt aErrCode)
	{
	iErrCode = aErrCode;
	iQueueProp.Set(iNotificationBundles.Count()); // Notify the waiting test step...
	}

void CConnServNotificationWatcher::DoCancel()
	{
	iConnServ.CancelAccessPointNotification();
	}

void CConnServNotificationWatcher::PerformNotificationRequestL()
	{
	// Perform the notification request
	iConnServ.AccessPointNotification(*iQueryBundle, *this);
	}

ConnectionServ::CConnectionServParameterBundle* CConnServNotificationWatcher::GetResultsBundle()
	{
	if(iNotificationBundles.Count())
		{
		// Return the top results bundle and remove it from the list we keep
		ConnectionServ::CConnectionServParameterBundle* bundle = iNotificationBundles[0];
		iNotificationBundles.Remove(0);
		
		// Update queue size property. Should only happen when client has control of my mutex,
		//  so nobody will be signalled on this property change.
		iQueueProp.Set(iNotificationBundles.Count());

		return bundle;
		}
	else
		{
		return NULL;
		}
	}

void CConnServNotificationWatcher::RunNotificationL()
	{
	// Create and install the active scheduler we know we will need
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Call the notification
	TRAP_IGNORE(PerformNotificationRequestL();)
	
	// Create an active object to wait on instruction to tell us to die
	CNotifCancelControl* cancelCtrl = CNotifCancelControl::NewL(*this);
	CleanupStack::PushL(cancelCtrl);
	cancelCtrl->Wait();
	
	// Start the active scheduler which will manage notification requests
	TRAP_IGNORE(CActiveScheduler::Start())

	// Cleanup
	CleanupStack::PopAndDestroy(cancelCtrl);
	CleanupStack::PopAndDestroy(scheduler);
	}

TInt CConnServNotificationWatcher::EntryPoint(TAny* iArgs)
	{
	// Need a cleanup stack
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	
	TRAP_IGNORE((static_cast<CConnServNotificationWatcher*>(iArgs))->RunNotificationL();)

	// Remove our cleanup stack
	delete cleanupStack;

	return KErrNone;
	}

// waits for signal from other thread
void CConnServNotificationWatcher::AwaitStartupCompletionL(void) const
	{
	RProperty queueProp;
	queueProp.Attach(KAccessPointNotificationPubSubUid,reinterpret_cast<TUint32>(this));
	TRequestStatus status;
	// Wait on notification queue to be set for the first time
	queueProp.Subscribe(status);
	User::WaitForRequest(status);
	queueProp.Close();
	User::LeaveIfError(iErrCode);
	}

// waits for signal from other thread
void CConnServNotificationWatcher::WaitOnQueueL(void) const
	{
	RProperty queueProp;
	queueProp.Attach(KAccessPointNotificationPubSubUid,reinterpret_cast<TUint32>(this));
	TRequestStatus status;
	// Wait on notification queue. This property is kept up to date with the number
	//  of items in the queue in the CConnServNotificationWatcher object.
	queueProp.Subscribe(status);
	TInt queueSize=0;
	queueProp.Get(queueSize);
	if(queueSize) // i.e. if things are in the queue
		{
		queueProp.Cancel();
		}
	User::WaitForRequest(status);
	queueProp.Close();
	User::LeaveIfError(iErrCode);
	}




// Availability simulation
//------------------------

_LIT(KAccessPointId, "AccessPointId");
_LIT(KAccessPointScore, "AccessPointScore");
_LIT(KAccessPointState, "AccessPointState");

_LIT(KAvailable, "Available");
_LIT(KUnavailable, "Unavailable");
_LIT(KStalled, "Stalled");

CSetAccessPointAvailabilityStep::CSetAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSetAccessPointAvailabilityStep);
	}

TVerdict CSetAccessPointAvailabilityStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CSetAccessPointAvailabilityStep::ConfigureFromIni()
	{
	// Read in access point ID (mandatory)
	if((GetIntFromConfig(iSection, KAccessPointId, iAccessPointId) != 1))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KAccessPointId);
		return KErrNotFound;
		}

	// Read in the availability attributes, score and status (mandatory)
	TBool haveState(EFalse);
	TBool haveScore(EFalse);

	if(GetIntFromConfig(iSection, KAccessPointScore, iAvailabilityScore))
		{
		haveScore = ETrue;
		}

	TPtrC availState;
	if(GetStringFromConfig(iSection, KAccessPointState, availState))
		{
		haveState = ETrue;
		}

	// Check we only have one or the other
	if((haveScore && haveState))
		{
		INFO_PRINTF1(_L("State and score are mutually exclusive."));
		return KErrArgument;
		}
	
	// If we have a score then make sure it is in valid range
	// Also if we have a score then if we also have state it must be available otherwise a contradiction
	if(haveScore)
		{
		if((iAvailabilityScore < ESock::TAvailabilityStatus::EMinAvailabilityScore)
			|| (iAvailabilityScore > ESock::TAvailabilityStatus::EMaxAvailabilityScore))
			{
			INFO_PRINTF3(KErrString_OutOfBounds, &iSection, &KAccessPointScore);
			return KErrArgument;
			}
		}

	// What settings do we have
	else if(haveState)
		{
		if(availState == KAvailable)
			{
			iAvailabilityScore = ESock::TAvailabilityStatus::EMaxAvailabilityScore;
			}
		else if(availState == KUnavailable)
			{
			iAvailabilityScore = ESock::TAvailabilityStatus::EMinAvailabilityScore;
			}
		else if(availState == KStalled)
			{
			iAvailabilityScore = ESock::TAvailabilityStatus::EUnknownAvailabilityScore;
			}
		else
			{
			INFO_PRINTF1(_L("Avail state not valid (must be 'Available' or 'Unavailable'"));
			return KErrArgument;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Must have either state or score."));
		return KErrArgument;
		}
	
	// All ok if we got this far
	return KErrNone;
	}

const TUid KAvailabilityTestingPubSubUid = {0x10272F42}; // TODO_CDG can't we grab this from somewhere else?

TVerdict CSetAccessPointAvailabilityStep::doSingleTestStep()
	{
	RProperty a;

	// Fail by default
	SetTestStepResult(EFail);

	// Set the availability of the access point as prescribed
	TInt error = a.Define(KAvailabilityTestingPubSubUid, iAccessPointId, RProperty::EInt);
	if((error == KErrAlreadyExists) || (error == KErrNone))
		{
		// Successfully created the property so attach to it for use
		error = a.Attach(KAvailabilityTestingPubSubUid, iAccessPointId);
		if(error != KErrNone)
			{
			INFO_PRINTF1(_L("Could not attach to property used for availability simulation"));
			SetTestStepError(error);
			}
		else
			{
			// Now set the availability as prescribed
			a.Set(iAvailabilityScore);

			// We have done what was expected of us so we passed
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Problem setting availability property"));
		SetTestStepError(error);
		}
	
	return TestStepResult();
	}

// ppp specific availability simulation
CSetPppAccessPointAvailabilityStep::CSetPppAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSetPppAccessPointAvailabilityStep);
	}

TVerdict CSetPppAccessPointAvailabilityStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}


_LIT(KRegistrationUnknown, "RegistrationUnknown");
_LIT(KNotRegisteredNoService, "NotRegisteredNoService");
_LIT(KNotRegisteredEmergencyOnly, "NotRegisteredEmergencyOnly");
_LIT(KNotRegisteredSearching, "NotRegisteredSearching");
_LIT(KRegisteredBusy, "RegisteredBusy");
_LIT(KRegisteredOnHomeNetwork, "RegisteredOnHomeNetwork");
_LIT(KRegistrationDenied, "RegistrationDenied");
_LIT(KRegisteredRoaming, "RegisteredRoaming");

TInt CSetPppAccessPointAvailabilityStep::ConfigureFromIni()
	{
	TPtrC netRegState;
	if(GetStringFromConfig(iSection, KAccessPointState, netRegState))
		{
		// What settings do we have
		if (netRegState == KRegistrationUnknown)
			{
			iRegStatus = RMobilePhone::ERegistrationUnknown;
			}
		else if (netRegState == KNotRegisteredNoService)
			{
			iRegStatus = RMobilePhone::ENotRegisteredNoService;
			}
		else if (netRegState == KNotRegisteredEmergencyOnly)
			{
			iRegStatus = RMobilePhone::ENotRegisteredEmergencyOnly;
			}
		else if (netRegState == KNotRegisteredSearching)
			{
			iRegStatus = RMobilePhone::ENotRegisteredSearching;
			}
		else if (netRegState == KRegisteredBusy)
			{
			iRegStatus = RMobilePhone::ERegisteredBusy;
			}
		else if (netRegState == KRegisteredOnHomeNetwork)
			{
			iRegStatus = RMobilePhone::ERegisteredOnHomeNetwork;
			}
		else if (netRegState == KRegistrationDenied)
			{
			iRegStatus = RMobilePhone::ERegistrationDenied;
			}
		else if (netRegState == KRegisteredRoaming)
			{
			iRegStatus = RMobilePhone::ERegisteredRoaming;
			}		
		}
	else
		{
		INFO_PRINTF1(_L("Must have network registration status set."));
		return KErrArgument;
		}
	
	// All ok if we got this far
	return KErrNone;
	}

TVerdict CSetPppAccessPointAvailabilityStep::doSingleTestStep()
	{
	RProperty a;

	// Fail by default
	SetTestStepResult(EFail);

	// Set the availability of the access point as prescribed
	TInt error = a.Define(KUidPSSimTsyCategory, KPSSimTsyRegStatChange, RProperty::EInt);
	if((error != KErrAlreadyExists) || (error != KErrNone))
		{
		// Successfully created the property so attach to it for use
		error = a.Attach(KUidPSSimTsyCategory, KPSSimTsyRegStatChange);
		if(error != KErrNone)
			{
			INFO_PRINTF1(_L("Could not attach to property used for ppp availability simulation"));
			}
		else
			{
			// Now set the availability as prescribed
			a.Set(iRegStatus);
			
			// We have done what was expected of us so we passed
			SetTestStepResult(EPass);
			}
		}
	
	return TestStepResult();
	}

// Packet Data (PDP) specific availability simulation
CSetPacketDataAccessPointAvailabilityStep::CSetPacketDataAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KSetPppAccessPointAvailabilityStep);
    }

TVerdict CSetPacketDataAccessPointAvailabilityStep::doTestStepPreambleL()
    {
    SetTestStepResult(EFail);

    if (iEsockTest==NULL)
        iEsockTest = new (ELeave) CCEsockTestBase;

    SetTestStepResult(EPass);
    return TestStepResult();
    }


TInt CSetPacketDataAccessPointAvailabilityStep::ConfigureFromIni()
    {
    TBool found(EFalse);
    TPtrC availState;
    if(GetStringFromConfig(iSection, KAccessPointState, availState))
        {
        if (availState == KAvailable)
            {
            iSetToAvailable = ETrue;
            found=ETrue;
            }
        else if (availState == KUnavailable)
            {
            iSetToAvailable = EFalse;
            found=ETrue;
            }
        }

    if( ! found)
        {
        INFO_PRINTF1(_L("Must have availability status set to Available or Unavailable."));
        return KErrArgument;
        }
    
    // All ok if we got this far
    return KErrNone;
    }

_LIT(KSimtsyName,"SIM");

TVerdict CSetPacketDataAccessPointAvailabilityStep::doSingleTestStep()
    {
    // Fail by default
    SetTestStepResult(EFail);

    // Action 1- RTelServer::Connect
    RTelServer telServer;
    TInt ret = telServer.Connect();
    if (ret!=KErrNone)
        {
        INFO_PRINTF1(_L("Failed to connect to telephony server"));
        }
    else
        {
        // Action 2- RTelServer::LoadPhoneModule
        ret=telServer.LoadPhoneModule(KSimtsyName);
        if (ret!=KErrNone)
            {
            INFO_PRINTF1(_L("Failed to load phone module"));
            }
        else
            {
            // Action 3- RPhone::Open
            RPhone phone;
            ret=phone.Open(telServer,KPhoneName) ;
            if (ret!=KErrNone)
                {
                INFO_PRINTF1(_L("Failed to open phone module"));
                }
            else
                {
                // Action 4- RPhone::Initialise
                ret=phone.Initialise();
                if (ret!=KErrNone)
                    {
                    INFO_PRINTF1(_L("Failed to initialise the phone"));
                    }
                else
                    {
                    // Action 5- RPacketService::Open
                    RPacketService packetService;
                    ret = packetService.Open(phone);
                    if (ret!=KErrNone)
                        {
                        INFO_PRINTF1(_L("Failed to initialise the packet service"));
                        }
                    else
                        {
                        // Action 6- RPacketService::NotifyStatusChange
                        TRequestStatus notifyStatus;
                        RPacketService::TStatus pktStatus;
                        packetService.NotifyStatusChange(notifyStatus,pktStatus);
                        
                        // Action 7- RPacketService::Attach/Detach
                        TRequestStatus status;
                        if(iSetToAvailable)
                            {
                            packetService.Attach(status);
                            }
                        else
                            {
                            packetService.Detach(status);
                            }
                        User::WaitForRequest(status);
                        User::WaitForRequest(notifyStatus);
                    
                        if(status.Int() != KErrNone )
                            {
                            INFO_PRINTF1(_L("Failed to attach/detach the packet service"));
                            }
                        else if( notifyStatus.Int() != KErrNone )
                            {
                            INFO_PRINTF1(_L("Failed to receive notification of packet service attach/detach"));
                            }
                        else
                            {
                            // Success at last!!!
                            SetTestStepResult(EPass);
                            }
                        
                        // now clean up:
                        
                        // Action 5- RPacketService::Open
                        packetService.Close();
                        }
                    // Action 4- RPhone::Initialise
                    // don't need to "un-Initialise" phone
                    }
                // Action 3- RPhone::Open
                phone.Close();
                }
            // Action 2- RTelServer::LoadPhoneModule
            telServer.UnloadPhoneModule(KSimtsyName);            
            }
        // Action 1- RTelServer::Connect
        telServer.Close();
        }
    // There! CleanupStack ShmeanupStack.
    User::LeaveIfError(ret);

    return TestStepResult();
    }



// wifi specific availability simulation
// Requires the "MockupWifiHardware" emulation mechanism to be enabled on udeb
CSetWifiAccessPointAvailabilityStep::CSetWifiAccessPointAvailabilityStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSetWifiAccessPointAvailabilityStep);
	}

TVerdict CSetWifiAccessPointAvailabilityStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

_LIT(KFakeWifiScanFile,"c:\\fake_wifi_scan.txt");
_LIT(KTempFakeWifiScanFile,"c:\\temp_fake_wifi_scan.txt");

_LIT8(KNewLine, "\n");
_LIT8(KLineFeed, "\r\n");

const TInt TotalNumberOfWifiFields = 10;
_LIT(KAPAvailable, "APAvailable");

// These mimic the entries in the "fake_wifi_scan.txt" file
_LIT(KBssid, "Bssid");
_LIT(KSsid, "Ssid");
_LIT(KSupportedRates, "SupportedRates");
_LIT(KNetworkType, "NetworkType");
_LIT(KAuthenticated, "Authenticated");
_LIT(KEncrypt, "Encrypt");
_LIT(KAPEncryptionType, "APEncryptionType");
_LIT(KAssociated, "Associated");
_LIT(KChannel, "Channel");
_LIT(KRssi, "Rssi");

TInt CSetWifiAccessPointAvailabilityStep::ConfigureFromIni()
	{
	if(GetIntFromConfig(iSection, KAPAvailable, iAPAvailable)
	&& GetStringFromConfig(iSection, KSsid, iSsid)
	&& GetStringFromConfig(iSection, KBssid, iBssid)
	&& GetStringFromConfig(iSection, KSupportedRates, iSupportedRates)
	&& GetStringFromConfig(iSection, KNetworkType, iNetworkType)
	&& GetStringFromConfig(iSection, KAuthenticated, iAuthenticated)
	&& GetStringFromConfig(iSection, KEncrypt, iEncrypt)
	&& GetStringFromConfig(iSection, KAPEncryptionType, iAPEncryptionType)
	&& GetStringFromConfig(iSection, KAssociated, iAssociated)
	&& GetStringFromConfig(iSection, KChannel, iChannel)
	&& GetStringFromConfig(iSection, KRssi, iRssi))
		{
		return KErrNone;
		}
	else
		{
		INFO_PRINTF1(_L("Incorrect Wifi Parameters"));
		return KErrArgument;
		}
	}

TVerdict CSetWifiAccessPointAvailabilityStep::doSingleTestStep()
	{
	// Pass by default
	SetTestStepResult(EPass);

	// Connect to the FileServer
	RFs rFS;
	if (rFS.Connect() != KErrNone)
		{
		INFO_PRINTF1(_L("Couldnt connect to FileServer"));
		SetTestStepResult(EFail);
		return TestStepResult(); // Fail now
		}
	//CleanupClosePushL(rFS); // Non leaving test function
	
	// If Available == 2 then we delete the file if it exists
	// In future we could make this "clear file" function more logical (magic number 2 is not clear)
	if (iAPAvailable == 2)
		{
		rFS.Delete(KFakeWifiScanFile);
		//CleanupStack::Pop(&rFS);
		rFS.Close(); // close the file system
		return TestStepResult(); // Pass now
		}

	// Create the "c:\fake_wifi_scan.txt" for Mock Wifi Testing removing the old version if it exists
	RFile rFile;
	if (rFile.Open(rFS, KFakeWifiScanFile, EFileStreamText | EFileRead | EFileShareReadersOrWriters) != KErrNone)
		{	
		if (rFile.Create(rFS, KFakeWifiScanFile, EFileStreamText | EFileRead | EFileShareReadersOrWriters) != KErrNone)
			{
			// File doesnt exist and couldnt be created
			INFO_PRINTF1(_L("fake_wifi_scan.txt file couldnt be opened or created"));
			SetTestStepResult(EFail);
			//CleanupStack::Pop(&rFS);
			rFS.Close(); // close the file system
			return TestStepResult(); // Fail now
			}
		}
	//CleanupClosePushL(rFile);// Non leaving test function

	// Create a new temporary "c:\temp_fake_wifi_scan.txt" for Mock Wifi Testing removing the old version if it exists
	RFile rNewFile;
	if (rNewFile.Replace(rFS, KTempFakeWifiScanFile, EFileStreamText | EFileWrite | EFileShareReadersOrWriters) != KErrNone)
		{
		// File doesnt exist and couldnt be created
		INFO_PRINTF1(_L("temp_fake_wifi_scan.txt couldnt be created"));
		SetTestStepResult(EFail);		
		//CleanupStack::Pop(&rFile);
		rFile.Close();
		//CleanupStack::Pop(&rFS);
		rFS.Close(); // close the file system
		return TestStepResult(); // Fail now
		}
	//CleanupClosePushL(rNewFile);// Non leaving test function

	// Loop through each line in the old file
	TInt WifiFieldNumber = TotalNumberOfWifiFields;
	TBool foundEntry = EFalse;
	TInt err = KErrNone;
	TBuf8<512> line;
	while (!err)
		{
		// Read one complete line from the old file
		line.SetLength(0);
		for(;;)
			{
			TBuf8<1> c;
			err = rFile.Read(c);
			if (err && err != KErrEof)
				{
				INFO_PRINTF1(_L("Error reading from fake_wifi_scan.txt"));
				SetTestStepResult(EFail);
				break;
				}
			if (c.Length() == 0)
				{
				err = KErrEof;
				break;
				}
			else
				{
				if (c[0] == '\n') // break out if it is CR
					break;
				else 
					line.Append(c[0]);
				}
			}
		
		// Now we have read a complete line from the old file
		if (err == KErrNone)
			{
			// Check if this line matches the SSID of the query
			TBuf8<512> buf8Ssid;
			buf8Ssid.Copy(iSsid);
			if (line.Find(buf8Ssid) != KErrNotFound)
				{
				// We have found the specified SSID
				foundEntry = ETrue;
				}
			
			// If we are updating OR removing the entry we dont add it (yet) to the output
			if (foundEntry && WifiFieldNumber)
				{
				// Decrement WifiFieldNumber
				WifiFieldNumber--;
				}
			else
				{
				TInt pos = 0;
				rNewFile.Seek(ESeekEnd, pos);
				if (rNewFile.Write(line) != KErrNone) SetTestStepResult(EFail);
				if (rNewFile.Write(KNewLine) != KErrNone) SetTestStepResult(EFail);
				}
			}
		}
		
	// If this AP is available, (re)add it to the list
	if (iAPAvailable == 1)
		{
		TBuf8<512> buf8;
				
		buf8.Copy(iSsid);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iBssid);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);

		buf8.Copy(iSupportedRates);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iNetworkType);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iAuthenticated);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iEncrypt);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iAPEncryptionType);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iAssociated);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iChannel);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		
		buf8.Copy(iRssi);
		if (rNewFile.Write(buf8) != KErrNone) SetTestStepResult(EFail);
		if (rNewFile.Write(KLineFeed) != KErrNone) SetTestStepResult(EFail);
		}

	//CleanupStack::Pop(&rNewFile);
	rNewFile.Close();	
		
	//CleanupStack::Pop(&rFile);
	rFile.Close();

	// Now we have created and updated temporary file, replace
	if (rFS.Replace(KTempFakeWifiScanFile, KFakeWifiScanFile) != KErrNone)
		{
		// File doesnt exist and couldnt be created
		INFO_PRINTF1(_L("fake_wifi_scan.txt couldnt be replaced"));
		SetTestStepResult(EFail);		
		}
			
	//CleanupStack::Pop(&rFS);
	rFS.Close(); // close the file system

	return TestStepResult();
	}

// BearerAvailabilityNotificationStep
//-----------------------------------
_LIT(KExpectedValue,"ExpectedValue");
_LIT(KExpectedValue1,"ExpectedValue1");
_LIT(KTypeOfNotification,"TypeOfNotification");
_LIT(KAccessBearer,"AccessBearer");
_LIT(KServiceMode,"ServiceMode");
_LIT(KNumOfTmgis,"NumOfTMGIs");
_LIT(KMCC,"MCCID");
_LIT(KMNC,"MNCID");
_LIT(KServiceId,"ServiceId");
_LIT(KAllTMGIvalid,"AllTMGIvalid");
_LIT(KAllTMGIInvalid,"AllTMGIInvalid");
_LIT(KRemoveAllService,"RemoveAllServices");

CRequestMBMSNtfnStep::CRequestMBMSNtfnStep(CCEsockTestBase*& aEsockTest)
	: CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KRequestMBMSNtfnStep);
	}

TInt CRequestMBMSNtfnStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	// Get the name of the connection server instance we are to use
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
			|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

	// Get the name applied to the notification so we can look it up later when referring to the results
	if((GetStringFromConfig(iSection, KNotificationName, iNotifName) != 1)
			|| (iNotifName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNotificationName);
		return KErrNotFound;
		}

	//Get the type of notification request
	GetIntFromConfig(iSection,KTypeOfNotification,reinterpret_cast<TInt&>(iNotificationType));

	TInt error;
	TRAP(error, iQueryBundle = CConnectionServParameterBundle::NewL());
	if(error != KErrNone)
		{
		return error;
		}

	if(iNotificationType == EMbmsBearerAvailNtfn)
		{
		TRAP(error, BuildBearerAvailQueryL(*iQueryBundle);)
		if(error != KErrNone)
			{
			return KErrNotFound;
			}
		}

	else if (iNotificationType == EMbmsServiceAvailNtfn)
		{
		TRAP(error, BuildServiceAvailQueryL(*iQueryBundle);)
		if(error != KErrNone)
			{
			return KErrNotFound;
			}
		}

	else if (iNotificationType == EMbmsRemoveServiceNtfn)
		{

		GetBoolFromConfig(iSection, KRemoveAllService, iRemoveAll);

		if(!iRemoveAll)
			{
			TRAP(error, BuildRemoveServiceQueryL(*iQueryBundle,ERemoveService);)
			if(error != KErrNone)
				{
				return KErrNotFound;
				}
			}
		else
			{
			TRAP(error, BuildRemoveServiceQueryL(*iQueryBundle,ERemoveAllServices);)
			if(error != KErrNone)
				{
				return KErrNotFound;
				}
			}

		}
		else if (iNotificationType == EMbmsActiveListNtfn)
		{
		TRAP(error, BuildActiveListQueryL(*iQueryBundle);)
		if(error != KErrNone)
			{
			return KErrNotFound;
			}
		}
		
		else if(iNotificationType == EMbmsMonitorListNtfn)
		{
			TRAP(error, BuildMonitorListQueryL(*iQueryBundle);)
		if(error != KErrNone)
			{
			return KErrNotFound;
			}	
		}
	return KErrNone;
	}

TVerdict CRequestMBMSNtfnStep::doSingleTestStep()
	{
	// Pass by default
	SetTestStepResult(EPass);

	// Fetch the connection server on which to post the notification.
	RConnectionServ* connServ = iEsockTest->iConnectionServs.Find(iConnectionServName);
	if(connServ == NULL)
		{
		INFO_PRINTF2(KErrString_NamedObjectNotFound, &iConnectionServName);
		}
	else
		{
		// Wrap up the request for the thread we will palm it off to
		CConnServNotificationWatcher* notifMgr = CConnServNotificationWatcher::NewLC
		(connServ, iQueryBundle);
		CleanupStack::Pop(notifMgr);

		// Give the notifing thread a handle to us
		notifMgr->iCommandThread.Open(RThread().Id(), EOwnerProcess);

		// Start a thread to manage the notifications as these occur with use of a scheduler
		TInt error = notifMgr->iNotifThread.Create(
				iNotifName,
				CConnServNotificationWatcher::EntryPoint,
				KNotifThreadStackSize,
				NULL,
				static_cast<TAny*>(notifMgr));

		// If all ok then kick off the notification thread
		if(error != KErrNone)
			{
			INFO_PRINTF2(_L("Could not start separate thread for capturing notifications. error:%d"), error);
			}
		else
			{
			// Get the thread started
			notifMgr->iNotifThread.Resume();

			// Keep a reference to our notif manager for follow up test step
			TInt addError = iEsockTest->iNotifWatchers.Add(notifMgr, iNotifName);
			__ASSERT_DEBUG(addError == KErrNone, User::Panic(KSpecAssert_ESockTestCnctnSr, 5));
			}
		}
	return TestStepResult();
	}

TInt CRequestMBMSNtfnStep::BuildBearerAvailQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle)
	{
	// Specify which type of parameters we want back
	ConnectionServ::CConnectionServParameterSetContainer* parSetCtr = ConnectionServ::CConnectionServParameterSetContainer::NewL(aQueryBundle);
	
	XMBMSServiceQuerySet* mbmsQuery = XMBMSServiceQuerySet::NewL(*parSetCtr);
	mbmsQuery->SetQueryType(XMBMSServiceQuerySet::EBearerAvailability);
	
	return KErrNone;
	}
	
TInt CRequestMBMSNtfnStep::BuildServiceAvailQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle)
	{
	if(GetIntFromConfig(iSection, KNumOfTmgis, reinterpret_cast<TInt&> (iNumTmgis)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNumOfTmgis);
		}
		
	if(GetIntFromConfig(iSection, KAccessBearer, reinterpret_cast<TInt&> (iAccessBearer)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KAccessBearer);
		}

	if(GetIntFromConfig(iSection, KServiceMode, reinterpret_cast<TInt&> (iServiceMode)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KServiceMode);
		}

	for(int i=0;i<iNumTmgis;i++)
		{
		ConnectionServ::CConnectionServParameterSetContainer* parSetCtr = ConnectionServ::CConnectionServParameterSetContainer::NewL(aQueryBundle);

		XMBMSServiceQuerySet* mbmsQuery = XMBMSServiceQuerySet::NewL(*parSetCtr);
		mbmsQuery->SetQueryType(XMBMSServiceQuerySet::EAddService);

		ConnectionServ::TMBMSChannelInfoV1* mbmsChannelInfo;
		XMBMSServiceParameterSet* mbmsParameters = XMBMSServiceParameterSet::NewL(*parSetCtr);
		mbmsChannelInfo = mbmsParameters->GetChannelInfo();
		TTmgi tmgi;

		TBuf<32> buf(KMCC);
		buf.AppendNum(i+1);

		if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iMCC)) != 1)
			{		
			INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
			}

		buf = KMNC;
		buf.AppendNum(i+1);

		if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iMNC)) != 1)
			{		
			INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
			}

		buf = KServiceId;
		buf.AppendNum(i+1);

		if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iServiceId)) != 1)
			{		
			INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
			}

		//constructing the TMGI
		tmgi.SetMCC(iMCC);  //1234
		tmgi.SetMNC(iMNC);
		tmgi.SetServiceId(iServiceId);

		TMbmsScope mbmsScope;
		mbmsScope = reinterpret_cast<TMbmsScope&>(iAccessBearer);

		//constructing the service Information.
		mbmsChannelInfo->SetTmgi(tmgi);
		mbmsChannelInfo->SetScope(mbmsScope);
					
		mbmsParameters->SetServiceMode(reinterpret_cast<TMbmsServiceMode&>(iServiceMode));
		}

	return KErrNone;

	}

TInt CRequestMBMSNtfnStep::BuildRemoveServiceQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle,TAction aAction)
	{
	if (aAction == ERemoveAllServices)
		{
		ConnectionServ::CConnectionServParameterSetContainer* parSetCtr = ConnectionServ::CConnectionServParameterSetContainer::NewL(aQueryBundle);
		XMBMSServiceQuerySet* mbmsQuery = XMBMSServiceQuerySet::NewL(*parSetCtr);
		mbmsQuery->SetQueryType(XMBMSServiceQuerySet::ERemoveAllService);
		return KErrNone;
		}
	else
		{
		// AccessBearer and ServiceMode are applicable only for remove specific MBMS service
		// Validity of TMGIs are NOT needed when we are removing all the services.

		GetBoolFromConfig(iSection, KAllTMGIvalid, iAllTmgiValid);
		GetBoolFromConfig(iSection, KAllTMGIInvalid, iAllTmgiInvalid);

		if(GetIntFromConfig(iSection, KAccessBearer, reinterpret_cast<TInt&> (iAccessBearer)) != 1)
			{		
			INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KAccessBearer);
			}

		if(GetIntFromConfig(iSection, KServiceMode, reinterpret_cast<TInt&> (iServiceMode)) != 1)
			{		
			INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KServiceMode);
			}

		if(GetIntFromConfig(iSection, KNumOfTmgis, reinterpret_cast<TInt&> (iNumTmgis)) != 1)
			{		
			INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNumOfTmgis);
			}

		iTmgiList = new (ELeave) CArrayFixFlat<TTmgi>(iNumTmgis);

		for(int i=0;i<iNumTmgis;i++)
			{

			ConnectionServ::CConnectionServParameterSetContainer* parSetCtr = ConnectionServ::CConnectionServParameterSetContainer::NewL(aQueryBundle);
			XMBMSServiceQuerySet* mbmsQuery = XMBMSServiceQuerySet::NewL(*parSetCtr);

			if(aAction == ERemoveService)
				{
				mbmsQuery->SetQueryType(XMBMSServiceQuerySet::ERemoveService);
				}
			else // aAction == EAddService
				{
				mbmsQuery->SetQueryType(XMBMSServiceQuerySet::EAddService);
				}
			
			ConnectionServ::TMBMSChannelInfoV1* mbmsChannelInfo;
			
			XMBMSServiceParameterSet* mbmsParameters = XMBMSServiceParameterSet::NewL(*parSetCtr);
			mbmsChannelInfo = mbmsParameters->GetChannelInfo();
			
			TTmgi tmgi;
			TBuf<32> buf(KMCC);
			buf.AppendNum(i+1);

			if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iMCC)) != 1)
				{		
				INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
				}

			buf = KMNC;
			buf.AppendNum(i+1);

			if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iMNC)) != 1)
				{		
				INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
				}

			buf = KServiceId;
			buf.AppendNum(i+1);

			if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iServiceId)) != 1)
				{		
				INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
				}

			//constructing the TMGI
			tmgi.SetMCC(iMCC);
			tmgi.SetMNC(iMNC);
			tmgi.SetServiceId(iServiceId);

			TMbmsScope mbmsScope;
			mbmsScope = reinterpret_cast<TMbmsScope&>(iAccessBearer);
			
			//constructing the service Information.
			mbmsChannelInfo->SetTmgi(tmgi);
			mbmsChannelInfo->SetScope(mbmsScope);
			
			iTmgiList->AppendL(tmgi);
			mbmsParameters->SetServiceMode(reinterpret_cast<TMbmsServiceMode&>(iServiceMode));

			}
		}

	return KErrNone;
	}

TInt CRequestMBMSNtfnStep::BuildActiveListQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle)
	{
	// Specify which type of parameters we want back
	ConnectionServ::CConnectionServParameterSetContainer* parSetCtr = ConnectionServ::CConnectionServParameterSetContainer::NewL(aQueryBundle);
	
	XMBMSServiceQuerySet* mbmsQuery = XMBMSServiceQuerySet::NewL(*parSetCtr);
	mbmsQuery->SetQueryType(XMBMSServiceQuerySet::ECountActiveServiceList);
	
	return KErrNone;
	}

TInt CRequestMBMSNtfnStep::BuildMonitorListQueryL(ConnectionServ::CConnectionServParameterBundle& aQueryBundle)
	{
	// Specify which type of parameters we want back
	ConnectionServ::CConnectionServParameterSetContainer* parSetCtr = ConnectionServ::CConnectionServParameterSetContainer::NewL(aQueryBundle);
	
	XMBMSServiceQuerySet* mbmsQuery = XMBMSServiceQuerySet::NewL(*parSetCtr);
	mbmsQuery->SetQueryType(XMBMSServiceQuerySet::ECountMonitorList);
	
	return KErrNone;
	}


CRequestMBMSNtfnStep::~CRequestMBMSNtfnStep()
	{}

CReceiveMBMSNtfnStep::CReceiveMBMSNtfnStep(CCEsockTestBase*& aEsockTest)
	: CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KReceiveMBMSNtfnStep);
	}

TInt CReceiveMBMSNtfnStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	// Get the name of the connection server instance we are to use
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
			|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

	// Get the name applied to the notification so we can look it up later when referring to the results
	if((GetStringFromConfig(iSection, KNotificationName, iNotifName) != 1)
			|| (iNotifName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNotificationName);
		return KErrNotFound;
		}

	// Get the delay period before expecting a notification
	GetIntFromConfig(iSection, KWaitPeriod, iInitialDelayInSeconds);

	//Get the expected value from the configuration file
	GetIntFromConfig(iSection,KExpectedValue,iExpectedValue);

	//Get the expected value from the configuration file
	GetIntFromConfig(iSection,KExpectedValue1,iExpectedValue1);

	//Get the type of notification request
	GetIntFromConfig(iSection,KTypeOfNotification,reinterpret_cast<TInt&>(iNotificationType));

	if(iNotificationType == EMbmsRemoveServiceNtfn)
		{
		GetBoolFromConfig(iSection, KRemoveAllService, iRemoveAll);
		if(!iRemoveAll)
			{
			// AccessBearer and ServiceMode are applicable only for remove specific MBMS service
			// Validity of TMGIs are NOT needed when we are removing all the services.

			GetBoolFromConfig(iSection, KAllTMGIvalid, iAllTmgiValid);
			GetBoolFromConfig(iSection, KAllTMGIInvalid, iAllTmgiInvalid);


			if(GetIntFromConfig(iSection, KNumOfTmgis, reinterpret_cast<TInt&> (iNumTmgis)) != 1)
				{		
				INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNumOfTmgis);
				return KErrNotFound;
				}

			iTmgiList = new (ELeave) CArrayFixFlat<TTmgi>(iNumTmgis);

			for(int i=0;i<iNumTmgis;i++)
				{

				ConnectionServ::TMBMSChannelInfoV1 mbmsChannelInfo;
				TTmgi tmgi;

				TBuf<32> buf(KMCC);
				buf.AppendNum(i+1);

				if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iMCC)) != 1)
					{		
					INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
					}

				buf = KMNC;
				buf.AppendNum(i+1);

				if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iMNC)) != 1)
					{		
					INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
					}

				buf = KServiceId;
				buf.AppendNum(i+1);

				if(GetIntFromConfig(iSection, buf, reinterpret_cast<TInt&> (iServiceId)) != 1)
					{		
					INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &buf);
					}

				//constructing the TMGI
				tmgi.SetMCC(iMCC);
				tmgi.SetMNC(iMNC);
				tmgi.SetServiceId(iServiceId);

				iTmgiList->AppendL(tmgi);
				}
			}
		}

	return KErrNone;
	}

TVerdict CReceiveMBMSNtfnStep::doSingleTestStep()
	{
	// Fail by default
	SetTestStepResult(EFail);

	// Fetch the connection server on which to post the notification.
	RConnectionServ* connServ = iEsockTest->iConnectionServs.Find(iConnectionServName);
	if(connServ == NULL)
		{
		INFO_PRINTF2(KErrString_NamedObjectNotFound, &iConnectionServName);
		}
	else
		{
		// Keep a reference to our notif manager for follow up test step
		CConnServNotificationWatcher* notifMgr = iEsockTest->iNotifWatchers.Find(iNotifName);
		if(!notifMgr)
			{
			INFO_PRINTF3(KErrString_NamedObjectNotFound, &iSection, &iNotifName);
			}
		else
			{

			User::After(iInitialDelayInSeconds * KMicroSecondsPerSecond);
			// ##### SYNCHRONISE ACCESS #####
			notifMgr->Wait();
			CConnectionServParameterBundle* resultsBundle = notifMgr->GetResultsBundle();
			
			if(iNotificationType == EMbmsRemoveServiceNtfn && (iRemoveAll || !iAllTmgiValid))
				{
					if(resultsBundle == NULL && !(iRemoveAll))
					{
						if(notifMgr->ErrCode() == iExpectedValue)
							{
							INFO_PRINTF1(_L("MBMS Remove With Invalid Services returned expected Error Value!!!"));
							SetTestStepResult(EPass);
							}
					}
					else if(resultsBundle != NULL && iRemoveAll)
					{
						TInt numParamSetContainers1 = resultsBundle->CountParamSetContainers();
						if ( numParamSetContainers1 == 0)
							{
							INFO_PRINTF1(_L("MBMS Remove All Services Successful!!!"));
							SetTestStepResult(EPass);
							}
					}
					else
					{
						INFO_PRINTF1(_L("MBMS Remove Services NOT Successful!!!"));
						SetTestStepResult(EFail);
					}
					return TestStepResult();
				}
			
			__ASSERT_DEBUG(resultsBundle, User::Panic(KSpecAssert_ESockTestCnctnSr, 6));
			
			// Firstly log the contents of the incoming results bundle  
			//TODO - complete for different Set
			/*TBuf<1000> logBuf;
			BundleToString(resultsBundle, logBuf);
			INFO_PRINTF1(logBuf);*/
			
			TInt numParamSetContainers = resultsBundle->CountParamSetContainers();
			__ASSERT_DEBUG(numParamSetContainers>0, User::Panic(KSpecAssert_ESockTestCnctnSr, 7));

			if(iNotificationType == EMbmsBearerAvailNtfn)
				{
				CConnectionServParameterSetContainer* parSetCtr = resultsBundle->GetParamSetContainer(0);
				XMBMSServiceQuerySet* genParamSet = XMBMSServiceQuerySet::FindInParamSetContainer(*parSetCtr);
				__ASSERT_DEBUG(genParamSet, User::Panic(KSpecAssert_ESockTestCnctnSr, 8));

				if(genParamSet->GetMBMSBearerAvailability() == iExpectedValue) 
					{
					INFO_PRINTF2(_L("Value of GetMBMSBearerAvailabilty = %d"), genParamSet->GetMBMSBearerAvailability());
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF2(_L("Value of GetMBMSBearerAvailabilty = %d"), genParamSet->GetMBMSBearerAvailability());
					INFO_PRINTF1(_L("Bearer Availability Notification results do not match those expected."));
					}
				}
			else if (iNotificationType == EMbmsServiceAvailNtfn)
				{
				CConnectionServParameterSetContainer* parSetCtr = resultsBundle->GetParamSetContainer(0);
				XMBMSServiceParameterSet* genParamSet = XMBMSServiceParameterSet::FindInParamSetContainer(*parSetCtr);

				if(genParamSet->GetMBMSServiceAvailability() == iExpectedValue)  				
					{
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("Service Availability results do not match to the expected value."));
					}
				}
			else if (iNotificationType == EMbmsRemoveServiceNtfn)
				{
				TBool tmgiRemoved = ETrue;
				if(iAllTmgiValid && !iAllTmgiInvalid)  // All Valid TMGI Scenario
					{
					for(int i=0;i<numParamSetContainers;i++)
						{
						CConnectionServParameterSetContainer* parSetCtr1 = resultsBundle->GetParamSetContainer(i);

						XMBMSServiceParameterSet* genParamSet1 = XMBMSServiceParameterSet::FindInParamSetContainer(*parSetCtr1);
						TMBMSChannelInfoV1* mbmsChannelInfo = genParamSet1->GetChannelInfo();

						for(int i=0;i<iTmgiList->Count();i++)
							{
							if( (iTmgiList->At(i).GetMCC() == mbmsChannelInfo->GetTmgi().GetMCC()) && 
									(iTmgiList->At(i).GetMNC() == mbmsChannelInfo->GetTmgi().GetMNC()) &&
									(iTmgiList->At(i).GetServiceId() == mbmsChannelInfo->GetTmgi().GetServiceId()))
								{
								tmgiRemoved = EFalse;
								}
							}
						}
					if(tmgiRemoved)  // Tmgi removed Successfully.
						{
						SetTestStepResult(EPass);
						}
					else
						{
						INFO_PRINTF1(_L("Access point notification results do not match those expected."));
						}
					}

				else if (iAllTmgiInvalid && !iAllTmgiValid) // All TMGIs InValid
					{
					
					for(int i=0;i<numParamSetContainers;i++)
						{
						CConnectionServParameterSetContainer* parSetCtr1 = resultsBundle->GetParamSetContainer(0);

						XMBMSServiceParameterSet* genParamSet1 = XMBMSServiceParameterSet::FindInParamSetContainer(*parSetCtr1);

						TMBMSChannelInfoV1* mbmsChannelInfo = genParamSet1->GetChannelInfo();
						for(int i=0;i<iTmgiList->Count();i++)
							{
							if( (iTmgiList->At(i).GetMCC() == mbmsChannelInfo->GetTmgi().GetMCC()) && 
									(iTmgiList->At(i).GetMNC() == mbmsChannelInfo->GetTmgi().GetMNC()) &&
									(iTmgiList->At(i).GetServiceId() == mbmsChannelInfo->GetTmgi().GetServiceId()))
								{
								tmgiRemoved = EFalse;
								}
							}
						}

					if(tmgiRemoved)  
						{
						SetTestStepResult(EPass);
						}
					else
						{
						INFO_PRINTF1(_L("Remove MBMS Service has not removed all the requested TMGIs."));
						}
					}
				else // Partial invalid TMGIs
					{

					for(int i=0;i<numParamSetContainers;i++)
						{
						CConnectionServParameterSetContainer* parSetCtr1 = resultsBundle->GetParamSetContainer(0);

						XMBMSServiceParameterSet* genParamSet1 = XMBMSServiceParameterSet::FindInParamSetContainer(*parSetCtr1);

						TMBMSChannelInfoV1* mbmsChannelInfo = genParamSet1->GetChannelInfo();

						for(int i=0;i<iTmgiList->Count();i++)
							{
							if((iTmgiList->At(i).GetMCC() == mbmsChannelInfo->GetTmgi().GetMCC()) && 
									(iTmgiList->At(i).GetMNC() == mbmsChannelInfo->GetTmgi().GetMNC()) &&
									(iTmgiList->At(i).GetServiceId() == mbmsChannelInfo->GetTmgi().GetServiceId()))
								{
								tmgiRemoved = EFalse;
								}
							}
						}

					TInt errorCode = notifMgr->ErrCode();

					if(tmgiRemoved && errorCode == KErrNone)  
						{
						SetTestStepResult(EPass);
						}
					else
						{
						INFO_PRINTF1(_L("Remove MBMS Service has not removed all the requested TMGIs."));
						}
					}
				}
				else if(iNotificationType == EMbmsActiveListNtfn)
				{
				CConnectionServParameterSetContainer* parSetCtr = resultsBundle->GetParamSetContainer(0);
				XMBMSServiceQuerySet* genParamSet = XMBMSServiceQuerySet::FindInParamSetContainer(*parSetCtr);
				__ASSERT_DEBUG(genParamSet, User::Panic(KSpecAssert_ESockTestCnctnSr, 9));
				TUint test = genParamSet->GetListCount();
				TUint test1 = genParamSet->GetListMaxCount();
				if(genParamSet->GetListCount() == iExpectedValue   && 
				   genParamSet->GetListMaxCount() == iExpectedValue1) 
					{
					INFO_PRINTF2(_L("Value of List Count = %d"), genParamSet->GetListCount());
					INFO_PRINTF2(_L("Value of Max Active Count = %d"), genParamSet->GetListMaxCount());
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF2(_L("Value of List Count = %d"), genParamSet->GetListCount());
					INFO_PRINTF2(_L("Value of Max Active Count = %d"), genParamSet->GetListMaxCount());
					}	
				}
				
				else if(iNotificationType == EMbmsMonitorListNtfn)
				{
				CConnectionServParameterSetContainer* parSetCtr = resultsBundle->GetParamSetContainer(0);
				XMBMSServiceQuerySet* genParamSet = XMBMSServiceQuerySet::FindInParamSetContainer(*parSetCtr);
				__ASSERT_DEBUG(genParamSet, User::Panic(KSpecAssert_ESockTestCnctnSr, 10));

				if(genParamSet->GetListCount() == iExpectedValue   && 
				   genParamSet->GetListMaxCount() == iExpectedValue1) 
					{
					INFO_PRINTF2(_L("Value of List Count = %d"), genParamSet->GetListCount());
					INFO_PRINTF2(_L("Value of Max Monitor Count = %d"), genParamSet->GetListMaxCount());
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF2(_L("Value of List Count = %d"), genParamSet->GetListCount());
					INFO_PRINTF2(_L("Value of Max Monitor Count = %d"), genParamSet->GetListMaxCount());
					}		
				}
			else {}
			notifMgr->Signal();
			}
		}
	return TestStepResult();
	}

CReceiveMBMSNtfnStep::~CReceiveMBMSNtfnStep()
	{
	}

// Cancellation of the BearerAvailability Notification  CCancelBearerAvailabilityNtfnStep

CCancelMBMSNtfnStep::CCancelMBMSNtfnStep(CCEsockTestBase*& aEsockTest)
:   CRConnectionServQueryTestStepBase(aEsockTest)
	{
	SetTestStepName(KCancelMBMSNtfnStep);
	}

TInt CCancelMBMSNtfnStep::ConfigureFromIni()
	{
	iConnectionServName.Set(KNullDesC);

	// Get the name of the connection server instance we are to use
	if((GetStringFromConfig(iSection, KConnectionServName, iConnectionServName) != 1)
			|| (iConnectionServName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KConnectionServName);
		return KErrNotFound;
		}

	// Get the name applied to the notification so we can look it up later when referring to the results
	if((GetStringFromConfig(iSection, KNotificationName, iNotifName) != 1)
			|| (iNotifName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KNotificationName);
		return KErrNotFound;
		}

	return KErrNone;
	}

TVerdict CCancelMBMSNtfnStep::doSingleTestStep()
	{
	// Fail by default
	SetTestStepResult(EFail);

	// Fetch the connection server on which to post the notification.
	RConnectionServ* connServ = iEsockTest->iConnectionServs.Find(iConnectionServName);
	if(connServ == NULL)
		{
		INFO_PRINTF2(KErrString_NamedObjectNotFound, &iConnectionServName);
		}
	else
		{
		// Fetch the notification watching entity
		CConnServNotificationWatcher* CancelNotifMgr = iEsockTest->iNotifWatchers.Find(iNotifName);
		if(!CancelNotifMgr)
			{
			INFO_PRINTF3(KErrString_NamedObjectNotFound, &iSection, &iNotifName);
			}
		else
			{
			// Log on to our worker notif catcher thread in order to wait for it to finish
			TRequestStatus logonStatus;
			CancelNotifMgr->iNotifThread.Logon(logonStatus);

			// Send command to the thread managing the notification that we want cancelled
			// this will then wait upon the thread to complete the command
			CancelNotifMgr->CancelNotification();

			// Now wait for the thread to complete
			User::WaitForRequest(logonStatus);
			TInt logonError = logonStatus.Int();

			if(logonError == KErrNone)
				{
				INFO_PRINTF1(_L("MBMS Service Availability Notification cancelled successfully."));
				SetTestStepResult(EPass);
				}

			// Remove our NotificationWatcher and Close the thread
			iEsockTest->iNotifWatchers.Remove(iNotifName);
			CancelNotifMgr->iNotifThread.Close();
			delete CancelNotifMgr;
			}
		}
	return TestStepResult();
	}
			
CCancelMBMSNtfnStep::~CCancelMBMSNtfnStep()
	{
	}




