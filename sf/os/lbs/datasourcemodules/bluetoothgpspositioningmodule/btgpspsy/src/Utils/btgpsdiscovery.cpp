// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// btgpsdiscovery implementation
// 
//

/**
 @file btgpsdiscovery.cpp
 @internal technology
*/

#include "btgpsdiscovery.h"
#include "btgpssimstubs.h"


CBTGPSDiscovery::CBTGPSDiscovery (MBTGPSSdpResultReceiver* aBTEngSdpResultReceiver)
	{
	iBTSdpResultReceiver = aBTEngSdpResultReceiver;
	iChannel = 0;
	iValid = EFalse;
	}

CBTGPSDiscovery* CBTGPSDiscovery::NewLC (MBTGPSSdpResultReceiver* aBTEngSdpResultReceiver)
	{
	CBTGPSDiscovery* self = new ( ELeave ) CBTGPSDiscovery(aBTEngSdpResultReceiver);
	CleanupStack::PushL (self);
	return self;
	}

CBTGPSDiscovery* CBTGPSDiscovery::NewL(MBTGPSSdpResultReceiver* aBTEngSdpResultReceiver)
	{
	CBTGPSDiscovery* self = CBTGPSDiscovery::NewLC (aBTEngSdpResultReceiver);
	CleanupStack::Pop (self);
	return self;
	}

CBTGPSDiscovery::~CBTGPSDiscovery ()
	{
	delete iSdpAgent;
#ifdef	LBS_BTGPSPSY_SIM
	delete iSdpAgentSim;
#endif
	}

/**
 * Starts an SDP query. The remote SDP database is searched for the value
 * of the ProtocolDescriptorList attribute in a service record containing
 * the specified UUID, the equivalent of an SDP ServiceAttributeSearch
 * transaction with ProtocolDescriptorList as attribute. This can e.g. be
 * used to search the remote RFCOMM channel. When completed, the results
 * are passed back to the client through the callback interface method
 * MBTEngSdpResultReceiver::AttributeSearchComplete().
 * @param aAddr Target Bluetooth device address for the SDP query.
 * @param aService The UUID to search for.
 * @return KErrNone if successful, otherwise the error code indicating the error situation. 
 */
TInt CBTGPSDiscovery::RemoteProtocolChannelQueryL(const TBTDevAddr& aAddr, TInt aService)
	{
	if(iSdpAgent)
		{
		return KErrInUse;
		}

#ifdef	LBS_BTGPSPSY_SIM
	// See if the device is simulated, in which case use the simulated SdpAgent.
	// otherwise use the regular one.
	iSdpAgentSim = CSdpAgentSim::NewL(*this);
	if (iSdpAgentSim->GetChannelStart(aAddr, iChannel) != KErrNone) 
#endif
		{
		CSdpAgent* sdpAgent = CSdpAgent::NewL(*this, aAddr);
		CleanupStack::PushL(sdpAgent);
		    
		// create the attribute ID list to match against
		CSdpSearchPattern* searchPattern = CSdpSearchPattern::NewL();
		CleanupStack::PushL(searchPattern);
		  
		// search for BNEP, as it's the common field in all PAN records
		searchPattern->AddL(aService);
		  
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
	
	return KErrNone;
	}

/**
 * This function to return the protocol channel number from the
 * result array of a ServiceAttributeSearch. This value can be used to
 * obtain e.g. the remote RFCOMM channel after a RemoteProtocolChannelQueryL
 * has completed.
 * @param aResultArray The array containing the SDP attribute.
 * @param aType On return, contains the channel number.
 * @return KerrNone if successful, KerrArgument if the SDP attribute is not
 * of type ProtocolDescriptorLis
 */
TInt CBTGPSDiscovery::ParseRfcommChannel(TInt& aType)
	{
	aType = iChannel;
	return KErrNone;
	}

/** Called when an service record request (CSdpAgent::NextRecordRequestComplete()) 
operation completes.

@param aError KErrNone if successful;
              KErrEof if there are no more SDP records left to be read; or an SDP error.
@param aHandle Service record for which the query was made
@param aTotalRecordsCount Total number of matching records
@see CSdpAgent::NextRecordRequestL() */
void CBTGPSDiscovery::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt /*aTotalRecordsCount*/)
	{
	TInt err = aError;
	
	if(err == KErrNone) //more results available
		{
		iMatchesFound = ETrue;
		// Check the attributes of the returned record to see if it's a U, GN, or NAP record    
		TRAP(err,iSdpAgent->AttributeRequestL(aHandle, KSdpAttrIdProtocolDescriptorList));
		
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
		iBTSdpResultReceiver->ServiceAttributeSearchComplete(err);
		}
	else
		{
		iBTSdpResultReceiver->ServiceAttributeSearchComplete(KErrEof);
		}
	}

/** Called when an attribute request (CSdpAgent::AttributeRequestL()) wants to 
pass up a result.

@param aHandle Service record for which the query was made
@param aAttrID ID of the attribute obtained
@param aAttrValue Attribute value obtained
@see CSdpAgent::AttributeRequestL() */
void CBTGPSDiscovery::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
	{
	if((aAttrID == KSdpAttrIdProtocolDescriptorList) && (aAttrValue->Type()==ETypeDES))
		{
		CSdpAttrValueDES* sdpProfileDesList = static_cast<CSdpAttrValueDES*>(aAttrValue);   
		// cycle through the contents of the DES to find the remote role
		TRAPD(err, sdpProfileDesList->AcceptVisitorL(*this));
		if(KErrNone == err)
			{
			// Delete the attribute value now that we've finished with it   
			delete aAttrValue;
			// there shouldn't be another profile descriptor list, so we should now get AttributeRequestComplete() called
			}
		}
	}

/** Called when an attribute request (CSdpAgent::AttributeRequestL()) wants to 
signal the completion of a attribute request.

@param aHandle Service record for which the query was made
@param aError an error
@see CSdpAgent::AttributeRequestL() */
void CBTGPSDiscovery::AttributeRequestComplete(TSdpServRecordHandle /*aHandle*/, TInt /*aError*/)
	{
	// examine the next record returned
	TRAP_IGNORE(iSdpAgent->NextRecordRequestL());
	}

void CBTGPSDiscovery::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	if(iValid)
		{
		if(aType==ETypeUint)
			{
			iChannel = aValue.Uint();
			iBTSdpResultReceiver->ServiceAttributeSearchComplete( KErrNone);
			}
		iValid = EFalse;
		}
	
	if(aType==ETypeUUID)
		{
		if(aValue.UUID() == KRFCommUUID)
			{
			iValid = ETrue;
			}
		}
	}
