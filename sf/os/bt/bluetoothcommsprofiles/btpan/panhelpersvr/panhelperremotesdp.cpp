// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "panhelperlog.h"
#include "panhelperremotesdp.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_HELPERS);
#endif

using namespace PanHelper;

CPanHelperRemoteSdpQuerier* CPanHelperRemoteSdpQuerier::NewL()
/**
Return a newly constructed remote SDP querier
*/
	{
	CPanHelperRemoteSdpQuerier* self = new(ELeave) CPanHelperRemoteSdpQuerier();
	return(self);
	}

CPanHelperRemoteSdpQuerier::CPanHelperRemoteSdpQuerier() : iMatchesFound(EFalse)
/**
Do nothing
*/
	{	}

CPanHelperRemoteSdpQuerier::~CPanHelperRemoteSdpQuerier()
/**

*/
	{
	delete iSdpAgent;
	}
	
void CPanHelperRemoteSdpQuerier::QueryL(const TBTDevAddr& aAddr, TDes8& aSupportedRemoteRoles, TRequestStatus& aStatus)
/**
Perform an SDP query on the client
*/
	{
	if(iSdpAgent)
		{
		__ASSERT_DEBUG(EFalse, PanHelperPanic(ESecondQueryWhileFirstStillActive));
		User::Leave(KErrInUse);
		}
	iClientStatus = &aStatus;
	iSupportedRemoteRoles = &(STATIC_CAST(TPckg<PanHelper::TPanDeviceRolesList>&,aSupportedRemoteRoles)());
#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> tempDevAddrBuf;
	aAddr.GetReadable(tempDevAddrBuf, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG2(_L("CPanHelperRemoteSdpQuerier[%x]: Performing remote SDP query on %S..."), this, &tempDevAddrBuf);
#endif	
	CSdpAgent* sdpAgent = CSdpAgent::NewL(*this, aAddr);
	CleanupStack::PushL(sdpAgent);
	
	// create the attribute ID list to match against
	CSdpSearchPattern* searchPattern = CSdpSearchPattern::NewL();
	CleanupStack::PushL(searchPattern);
	
	// search for BNEP, as it's the common field in all PAN records
	searchPattern->AddL(KBnepUUID);
	
	// Set Record Filter copies searchPattern
	sdpAgent->SetRecordFilterL(*searchPattern);
	CleanupStack::PopAndDestroy(searchPattern);

	sdpAgent->NextRecordRequestL();

	// Now we have successfully issued the request we know that it will later be
	// completed, so we can keep sdpAgent and it will be deleted on request
	// completion.
	CleanupStack::Pop(sdpAgent);
	iSdpAgent = sdpAgent;
	}
	
void CPanHelperRemoteSdpQuerier::CancelQuery()
/**
Cancel any queries in progress
*/
	{
	if(iSdpAgent)
		{
		iSdpAgent->Cancel();
		}

	delete iSdpAgent;
	iSdpAgent = NULL;
	User::RequestComplete(iClientStatus, KErrCancel);
	}

void CPanHelperRemoteSdpQuerier::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt /*aTotalRecordsCount*/)
/**
We've got another SDP record, so parse it to (eventually) find out what PAN role it's advertising
*/
	{
	TInt err = aError;

	if(err == KErrNone) //more results available
		{
		iMatchesFound = ETrue;
		// Check the attributes of the returned record to see if it's a U, GN, or NAP record	
		TRAP(err,iSdpAgent->AttributeRequestL(aHandle, KSdpAttrIdBluetoothProfileDescriptorList));
		if(err == KErrNone)
			{
			return; 
			}
		}
		
	//Request has completed or error has occured
	
	if(err != KErrEof)
		{
		// A 'real' error has occured, cancel the query
		if(iSdpAgent)
			{
			iSdpAgent->Cancel();
			}
		}
	else
		{
		// err == KErrEof when no more results available so complete the request
		// with KErrNone as KErrEof not a 'real' error
		err = iMatchesFound?KErrNone:KErrNotFound;
		}
		
	delete iSdpAgent;
	iSdpAgent = NULL;
	User::RequestComplete(iClientStatus, err);
	}
	
void CPanHelperRemoteSdpQuerier::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID /*aAttrID*/, CSdpAttrValue* aAttrValue)
/**

*/
	{
	__ASSERT_ALWAYS(aAttrValue, PanHelperPanic(ENullSdpAttrValue));
	
	if(aAttrValue->Type()==ETypeDES)
		{
		CSdpAttrValueDES* sdpProfileDesList = static_cast<CSdpAttrValueDES*>(aAttrValue);
		
		// cycle through the contents of the DES to find the remote role
		TRAPD(err, sdpProfileDesList->AcceptVisitorL(*this));
		if(err)
			{
			LOG1(_L("CPanHelperRemoteSdpQuerier[%x]: attempting to read supplied DES failed, erroring client"), this);
			}
		}
	else		// we've been given a non-DES structure for an attribute that is supposed to be a DES
		{
		LOG1(_L("CPanHelperRemoteSdpQuerier[%x]: attribute request result passed back a non-DES attribute, erroring client"), this);
		}
	
	// Delete the attribute value now that we've finished with it	
	delete aAttrValue;

	// there shouldn't be another profile descriptor list, so we should now get AttributeRequestComplete() called	
	}
	
void CPanHelperRemoteSdpQuerier::AttributeRequestComplete(TSdpServRecordHandle, TInt /*aError*/)
/**

*/
	{
	// examine the next record returned
	TRAPD(err, iSdpAgent->NextRecordRequestL());
	if(err)
		{
		LOG2(_L("CPanHelperRemoteSdpQuerier[%x]: next record request failed with error %d"), this, err);
		}
	}


void CPanHelperRemoteSdpQuerier::StartListL(CSdpAttrValueList& /*aList*/)
/**

*/
	{
	// not interested in this
	}
	
void CPanHelperRemoteSdpQuerier::EndListL()
/**

*/
	{
	// not interested in this
	}

void CPanHelperRemoteSdpQuerier::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
/**
Process an attribute from a DES - specifically attributes from the profile descriptor list
*/
	{
	if(aType==ETypeUUID)
		{
		//CSdpAttrValueUUID* sdpUuidValue = static_cast<CSdpAttrValueUUID*>(&aValue);
		if(aValue.UUID()==KPanUUUID)
			{
			LOG1(_L("CPanHelperRemoteSdpQuerier[%x]: ...remote device supports PAN-U role..."), this);
			iSupportedRemoteRoles->SetSupportsU();
			}
		else if(aValue.UUID()==KPanGnUUID)
			{
			LOG1(_L("CPanHelperRemoteSdpQuerier[%x]: ...remote device supports PAN-GN role..."), this);
			iSupportedRemoteRoles->SetSupportsGn();
			}
		else if(aValue.UUID()==KPanNapUUID)
			{
			LOG1(_L("CPanHelperRemoteSdpQuerier[%x]: ...remote device supports PAN-NAP role..."), this);
			iSupportedRemoteRoles->SetSupportsNap();
			}
		}
	// otherwise, it's just the version or something, and we don't care
	}
