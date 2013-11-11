// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_sock.h>
#include <btsdp.h>
#include "engine.h"
#include "agutil.h"
#include "requester.h"
#include "agtypes.h"
#include "agconsts.h"
#include "sdpconsts.h"
#include "sdputil.h"
#include "SDPDatabase.h"
#include "SDPAttribute.h"
#include "ExtractorVisitor.h"
#include "BuilderVisitor.h"

void AgPanic(TSdpAgPanic aCode)
	{
	User::Panic(_L("SdpAgent"),aCode);
	}

/** Adapter class.
	
	Very similar to CSdpServRecord ... this class does it
	the right way. Perhaps CSdpServRecord should just use
	one of these to build attributes. */
NONSHARABLE_CLASS(CSdpAttrBuilder) : public CBase, public MSdpElementBuilder
	{
public:
	CSdpAttrBuilder(MSdpAgentNotifier& aNotifier, TSdpServRecordHandle aHandle);
	~CSdpAttrBuilder();

	// MSdpElementBuilder interface definition
	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	virtual MSdpElementBuilder* BuildDESL();
	virtual MSdpElementBuilder* StartListL();
	virtual MSdpElementBuilder* EndListL();

	void CompleteAttribute();

private:
	MSdpAgentNotifier& iNotifier;
	CSdpAttr* iCurrentAttr;
	TSdpServRecordHandle iHandle;
	TBool iBuiltDES;
	};

CSdpAttrBuilder::CSdpAttrBuilder(MSdpAgentNotifier& aNotifier, TSdpServRecordHandle aHandle)
	:iNotifier(aNotifier), iHandle(aHandle)
	{
	}

CSdpAttrBuilder::~CSdpAttrBuilder()
	{
	delete iCurrentAttr;
	}

// MSdpElementBuilder interface definition
MSdpElementBuilder* CSdpAttrBuilder::BuildUintL(const TDesC8& aUint)
	{
	if(!iBuiltDES)
		User::Leave(KErrGeneral);
	if (aUint.Length()!=2)
		User::Leave(KErrSdpBadAttributeId);

	CompleteAttribute();
	TSdpAttributeID id = BigEndian::Get16(&aUint[0]);
	iCurrentAttr = CSdpAttr::NewL(id, this);
	return iCurrentAttr;
	}

MSdpElementBuilder* CSdpAttrBuilder::BuildDESL()
	{// Start of attribute list
	if(iBuiltDES)
		User::Leave(KErrGeneral);
	iBuiltDES = ETrue;
	return this;
	}

MSdpElementBuilder* CSdpAttrBuilder::StartListL()
	{// Start of attribute list
	if(!iBuiltDES)
		User::Leave(KErrGeneral);
	return this;
	}

MSdpElementBuilder* CSdpAttrBuilder::EndListL()
	{
	if(!iBuiltDES)
		User::Leave(KErrGeneral);
	CompleteAttribute();
	return this;
	}

void CSdpAttrBuilder::CompleteAttribute()
	{
	if(!iCurrentAttr)
		{
		return;
		}
	iNotifier.AttributeRequestResult(iHandle, iCurrentAttr->AttributeID(), iCurrentAttr->ReleaseValue());
	delete iCurrentAttr;
	iCurrentAttr = 0;
	}


/************************************************************************/
//
//   SDP agent client API (user friendly style)
//
/************************************************************************/
		
/**
This virtual function allows the M- classes to be extended in future in a binary
compatible way by providing a method that clients can override in future to
allow extra callbacks to be made via aObject.
*/	
EXPORT_C void MSdpAgentNotifier::MSAN_ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	}

// API Facade

EXPORT_C CSdpAgent* CSdpAgent::NewL(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr)
/** Creates a new SDP Agent object.

@param aNotifier Interface implemented by the query requester, which the agent 
calls asynchronously to pass responses to queries
@param aDevAddr The Bluetooth address of the remote device to query
@return New service discovery agent
@capability LocalServices */
	{
	CSdpAgent* self = CSdpAgent::NewLC(aNotifier, aDevAddr);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CSdpAgent* CSdpAgent::NewLC(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr)
/** Creates a new SDP Agent object.

Allocate and construct a service discovery agent, leaving the object on the 
cleanup stack.

@param aNotifier Interface implemented by the query requester, which the agent 
calls asynchronously to pass responses to queries
@param aDevAddr The Bluetooth address of the remote device to query
@return New service discovery agent
@capability LocalServices */
	{
	CSdpAgent* self = new(ELeave) CSdpAgent();
	CleanupStack::PushL(self);
	self->ConstructL(aNotifier, aDevAddr);
	return self;
	}


EXPORT_C CSdpAgent::~CSdpAgent()
/**
**/
/** Destructor. */
	{
	delete iAgentEngine;
	}


void CSdpAgent::ConstructL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr)
/**
**/
	{
	iAgentEngine = CSdpAgentEng::NewL(aNotifier, aDevAddr);
	}

CSdpAgent::CSdpAgent()
/**
**/
	{
	}

EXPORT_C void CSdpAgent::SetRecordFilterL(const CSdpSearchPattern& aUUIDFilter)
/** Sets the classes of service to query for on the remote device.

Responses from the agent will only contain records for services that belong 
to the classes listed in aUUIDFilter. Service classes are represented as unique 
identifiers (UUIDs).

@param aUUIDFilter A list of UUIDs that will be matched in SDP Service Search 
Requests. The function takes a copy of the object. Any previous UUID list 
is deleted.
@capability LocalServices */
	{
	iAgentEngine->SetRecordFilterL(aUUIDFilter);
	}

EXPORT_C void CSdpAgent::SetAttributePredictorListL(const CSdpAttrIdMatchList& /*aMatchList*/)
/** This does nothing!
(It used to create a copy of an attribute match list supplied, and place it in the CSdpAgentEng object.)

@param aMatchList Attribute - now unused
@capability LocalServices
@deprecated*/
	{
	}

EXPORT_C void CSdpAgent::NextRecordRequestL()
/** Gets a handle to the record for the next (or first) service on the remote device 
that matches the service class filter previously set.

The function is asynchronous: on completion, it calls NextRecordRequestComplete() 
on the MSdpAgentNotifier interface passed in the NewL(). 

@see MSdpAgentNotifier::NextRecordRequestComplete()
@capability LocalServices */
	{
	iAgentEngine->NextRecordRequestL();
	}

EXPORT_C void CSdpAgent::AttributeRequestL(TSdpServRecordHandle aHandle, 
										  TSdpAttributeID aAttrID)
/** Gets the specified attribute for a remote service. 

The function is asynchronous: on completion, it calls MSdpAgentNotifier::AttributeRequestComplete(). 


If the attribute is found, the function passes it by also calling MSdpAgentNotifier::AttributeRequestResult().

@param aHandle The service for which to get the attribute, specified by its 
record handle 
@param aAttrID The ID of the attribute to get
@see MSdpAgentNotifier::AttributeRequestComplete()
@see MSdpAgentNotifier::AttributeRequestResult()
@capability LocalServices */
	{
	CSdpAttrIdMatchList* list = CSdpAttrIdMatchList::NewL();
	CleanupStack::PushL(list);
	list->AddL(TAttrRange(aAttrID));
	iAgentEngine->AttributeRequestL(aHandle, *list);
	CleanupStack::PopAndDestroy(list);
	}

EXPORT_C void CSdpAgent::AttributeRequestL(TSdpServRecordHandle aHandle, 
										  const CSdpAttrIdMatchList& aMatchList)
/** Gets the specified attributes for a remote service. 

The function is asynchronous: on completion, it calls MSdpAgentNotifier::AttributeRequestComplete(). 


The function also calls MSdpAgentNotifier::AttributeRequestResult() for each 
attribute found.

@param aHandle The service for which to get the attribute, specified by its 
record handle 
@param aMatchList A list of attributes to get
@see MSdpAgentNotifier::AttributeRequestComplete()
@see MSdpAgentNotifier::AttributeRequestResult()
@capability LocalServices */
	{
	iAgentEngine->AttributeRequestL(aHandle, aMatchList);
	}

EXPORT_C void CSdpAgent::AttributeRequestL(MSdpElementBuilder* aBuilder,
										  TSdpServRecordHandle aHandle, 
										  TSdpAttributeID aAttrID)
/** Gets the specified attribute for a remote service.

The function is asynchronous: on completion, it calls MSdpAgentNotifier::AttributeRequestComplete(). 


If the attribute is found, the function calls aBuilder's interface to set 
the attribute ID and value.

@param aBuilder Object implementing the MSdpElementBuilder interface. It will 
be called with each type found in the response.
@param aHandle The service for which to get the attribute, specified by its 
record handle
@param aAttrID The ID of the attribute to get
@see MSdpAgentNotifier::AttributeRequestComplete()
@capability LocalServices */
	{
	CSdpAttrIdMatchList* list = CSdpAttrIdMatchList::NewL();
	CleanupStack::PushL(list);
	list->AddL(TAttrRange(aAttrID));
	iAgentEngine->AttributeRequestL(aBuilder, aHandle, *list);
	CleanupStack::PopAndDestroy(list);
	}

EXPORT_C void CSdpAgent::AttributeRequestL(MSdpElementBuilder* aBuilder,
										  TSdpServRecordHandle aHandle, 
										  const CSdpAttrIdMatchList& aMatchList)
/** Gets the specified attributes for a remote service.

The function is asynchronous: on completion, it calls MSdpAgentNotifier::AttributeRequestComplete(). 


As each attribute is found, the function calls aBuilder's interface to set 
the attribute ID and value.

@param aBuilder Object implementing the MSdpElementBuilder interface. It will 
be called with each type found in the response.
@param aHandle The service for which to get the attribute, specified by its 
record handle
@param aMatchList A list of attributes to get
@see MSdpAgentNotifier::AttributeRequestComplete()
@capability LocalServices */
	{
	iAgentEngine->AttributeRequestL(aBuilder, aHandle, aMatchList);
	}

EXPORT_C void CSdpAgent::Cancel()
/**Attempts to cancel an SDP Agent request.

Calls cancel on active objects making request.
Resets all variables associated with a request and its state.

NB The cancel operation will not, and cannot stop results from an SDP query
being sent by the remote.
@capability LocalServices */
	{
	iAgentEngine->Cancel();
	}

/************************************************************************/
//
//   ENGINE for SDP agent client API
//
/************************************************************************/


CSdpAgentEng* CSdpAgentEng::NewL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr)
/** Creates a new SDP Agent Engine object.

The SDP Agent Engine is the class for the data member of the SDP Agent
which actually does the work. (The SDP Agent is a facade class.)

@param MSdpAgentNotifier   Interface to user object that will receive
notifiactions of agent commands completing.
@param TBTDevAddr	Address of the remote device the SDP agent will query. */
	{
	CSdpAgentEng* self = new(ELeave) CSdpAgentEng(aNotifier, aDevAddr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpAgentEng::~CSdpAgentEng()
/** Creates a new SDP Agent Engine object.

The SDP Agent Engine is the class for the data member of the SDP Agent
which actually does the work. (The SDP Agent is a facade class.)

@param MSdpAgentNotifier   Interface to user object that will receive
notifiactions of agent commands completing.
@param TBTDevAddr	Address of the remote device the SDP agent will query.*/
	{
				
	delete iUUIDFilter;
	delete iMatchList;
	delete iDefaultAttributeBuilder;
	delete iParser;
	delete iBuiltRecHandles;
	
	if (iSearchRequester)
		{
		iSearchRequester->Cancel();
		delete iSearchRequester;
		}
	
	if (iAttributeRequester)
		{
		iAttributeRequester->Cancel();
		delete iAttributeRequester;
		}
	
	iSdpSession.Close(); //DEFECT_FIX
	}


void CSdpAgentEng::ConstructL()
/** Sets ip basic requirements for SDP Agent.

Perform ipc connection with ESock.

Create active objects which look after requests.

Create a parser to parse responses. (A builder for the parser is provided at point of request.)*/
	{
	TInt ret=iSdpSession.Connect();
    if(ret!=KErrNone && ret!=KErrAlreadyExists)
		User::Leave(ret);
	//iRequester.Open(ss);

	iSearchRequester = CSdpSearchRequester::NewL(iSdpSession, *this);
	iBuiltRecHandles = new(ELeave) CArrayFixSeg<TSdpServRecordHandle>(8);

	iParser = CElementParser::NewL(0);
	iAttributeRequester = CSdpAttributeRequester::NewL(iSdpSession, *this);
	}

CSdpAgentEng::CSdpAgentEng(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr)
:iCancelCalled(EFalse),
 iNotifier(aNotifier),
 iRemoteAddress(aDevAddr)
 

/** Constructor: caches the results notifier and the remote device address to be used.*/
	{
	}

void CSdpAgentEng::SetRecordFilterL(const CSdpSearchPattern& aUUIDFilter)
/** This function creates a new COPY of a UUID filter from the UUID filter
supplied, and places it in the CSdpAgentEng object.

If an old UUID filter list exists, it will be deleted. (A UUID filter is
a list of UUIDs that must be contained in a service record in a remote
database if the handle of that record is to be returned by a service search.) */
	{
	delete iUUIDFilter;
	iUUIDFilter = 0;
	iUUIDFilter = CSdpSearchPattern::NewL();
	for(TInt i = 0;i < aUUIDFilter.Count(); ++i)
		{
		TUUID uuid = aUUIDFilter.At(i);
		iUUIDFilter->AddL(uuid);
		}
	iSearchRequester->Cancel();
	iSearchRequestContState.Zero();
	iBuiltRecHandles->Reset();
	iServiceSearchState = EIdle;
	}

void CSdpAgentEng::NextRecordRequestL()
/** Retrieves the next record handle from the remote server, that matches
the UUID filter previously set by SetRecordFileterL(). On completion, 
an upcall will occur on MSdpAgentNotifier::NextRecordRequestComplete().
**/
	{
	__ASSERT_ALWAYS(iUUIDFilter, AgPanic(ESdpAgRecordFilterNotSet));

	iCancelCalled = EFalse;
	if(TryToCompleteNextRecordRequest())
		{
		return;
		}
	// Need to request some fresh results
	iSearchRequester->SearchRequestL(iRemoteAddress,
									 *iUUIDFilter,
									 KSdpAgMaxResultCount, 
									 iSearchRequestContState);
	iServiceSearchState = ERequesting;
	}

void CSdpAgentEng::AttributeRequestL(TSdpServRecordHandle aHandle, 
									 const CSdpAttrIdMatchList& aMatchList)
/** Retrieve attributes from a record on the remote server.

On completion, an up call on MSdpAgentNotifier::AttributeRequestComplete 
will occur. Each attribute found will be passed up through a call to 
MSdpAgentNotifier::AttributeRequestResult.

@param aHandle	Record handle to retrieve attribute from
@param aMatchList  List Attribute IDs to retrieve. */

	{
	delete iDefaultAttributeBuilder;
	iDefaultAttributeBuilder = 0;
	iDefaultAttributeBuilder = new(ELeave) CSdpAttrBuilder(iNotifier, aHandle);
	AttributeRequestL(iDefaultAttributeBuilder, aHandle, aMatchList);
	}

void CSdpAgentEng::AttributeRequestL(MSdpElementBuilder* aBuilder,
									 TSdpServRecordHandle aHandle, 
									 const CSdpAttrIdMatchList& aMatchList)
/** Retrieve attributes from a record on the remote server.

On completion, an up call on MSdpAgentNotifier::AttributeRequestComplete 
will occur. As attributes are found, calls the MSdpElementBuilder
class to describe the attribute. This will be of the form 
n * [AttrID:AttrVal], where n is the number of attributes actually found.

@param aBuilder  Class implementing MSdpElementBuilder interface to handle the result
@param aHandle	Record handle to retrieve attribute from
@param aMatchList  List Attribute IDs to retrieve. */
	{
	//Reset as appropriate
	iCancelCalled = EFalse;
	iAttributeRequestContState.Zero();
	delete iMatchList;
	iMatchList = 0;
	iMatchList = CSdpAttrIdMatchList::NewL(aMatchList);
	iAttributeRequestHandle = aHandle;
	iParser->Reset(aBuilder);
	iAttributeRequester->Cancel();

	SendAttributeRequestL();
	}

void CSdpAgentEng::Cancel()
	{
	//Tell this object not to handle anymore upcalls (till new request is made)
	iCancelCalled = ETrue;

	//Cancel active objects
	iSearchRequester->Cancel();
	iAttributeRequester->Cancel();

	//Reset Service Search Stuff
	iSearchRequestContState.Zero();
	iBuiltRecHandles->Reset();
	iServiceSearchState = EIdle;
	iNextRecHandle = 0;
	iTotalRecCount = 0;

	//Reset Attribute Request Stuff
	iAttributeRequestContState.Zero();
	}

void CSdpAgentEng::HandleServiceSearchResponseL(TUint16 aTotalRecCount,
											 TUint16 aCurrentRecCount,
											 const TDesC8& aRecHandles,
											 const TDesC8& aContState)
/** Upcall from service search requester. */
	{
	if(iCancelCalled)
		return;

	__ASSERT_DEBUG(aCurrentRecCount * KSdpRecordHandleSize == aRecHandles.Length(),
		AgPanic(ESdpAgServiceSearchResultError));
	iServiceSearchState = EResultsReceived;
	iTotalRecCount = aTotalRecCount;
	for(TInt i = 0; i < aCurrentRecCount; ++i)
		{
		iBuiltRecHandles->AppendL(BigEndian::Get32(&aRecHandles[i*KSdpRecordHandleSize]));
		}
	iSearchRequestContState = aContState;

	TryToCompleteNextRecordRequest();
	iServiceSearchState = EResultsReceived;

	}

void CSdpAgentEng::HandleServiceSearchError(TInt aError)
/** Pass service search error up to notifier - the handle supplied
is simply a dummy ... the notifier needs to check for errors. */
	{
	if(iCancelCalled)
		return;

	iNotifier.NextRecordRequestComplete(aError, 0xffffffff, 0);
	}

void CSdpAgentEng::HandleAttributeResponseL(const TDesC8& aAttributeList, 
										    const TDesC8& aContState)
/** Up Called by Attribute Request Active Object "RunL"
Parse out the attribute response, and handle accordingly. */
	{
	if(iCancelCalled)
		return;
	
	TBool moreExpected = iParser->BufferedParseL(aAttributeList);

	if(iDefaultAttributeBuilder && iParser->Builder() == iDefaultAttributeBuilder)
		{// Could have just parsed an attr. Send it up (rather than wait till next iteration)
		iDefaultAttributeBuilder->CompleteAttribute(); //calls iNotifier.AttributeRequestResult()
		}

	if (aContState.Length() == 0)
		{
		if (moreExpected)
			{// Parsers not done, but we have no cont state. Oh dear.
			User::Leave(KErrUnderflow);
			}

		iNotifier.AttributeRequestComplete(iAttributeRequestHandle, KErrNone);
		}
	else
		{
		iAttributeRequestContState = aContState;
		SendAttributeRequestL();
		}
	}

void CSdpAgentEng::HandleAttributeError(TInt aError)
/**
	Pass attribute error up to notifier.
**/
	{
	if(iCancelCalled)
		return;

	iNotifier.AttributeRequestComplete(iAttributeRequestHandle, aError);
	}

void CSdpAgentEng::SendAttributeRequestL()
/** Ask the active object which performs attribute requests to do just that.

The parameters used for the request are those currently stored as data members 
in this	object. These will have been previousfly supplied by the user.*/
	{
	iAttributeRequester->AttributeRequestL(iRemoteAddress, iAttributeRequestHandle,
			KSdpAgMaxByteCount, *iMatchList, iAttributeRequestContState);
	}

TBool CSdpAgentEng::TryToCompleteNextRecordRequest()
/** Checks whether it is necessary or not to ask remote device for more records.

SdpAgent collects as many of the remote device's record handles that conform to
the record filter as it can on each service search request sent. Thus the SdpAgent
will only perform a fresh service search if either the record filter has been changed
(=> iBuiltRecordHandles->Count() returns 0) or the remote device returned a non "null"
continuation state and all the record handles currently returned by the remote device 
have been processed.

@return ETrue if NOT necessary.*/
	{
	if(iServiceSearchState == EResultsReceived)
		{
		if(iNextRecHandle < iBuiltRecHandles->Count())
			{//We've got an answer already
			 //N.B. The function call iNotifier.NextRecordRequestComplete must come last 
			 //because it can call TryToCompleteNextRecordRequest() recursively.
			iNotifier.NextRecordRequestComplete(KErrNone, iBuiltRecHandles->At(iNextRecHandle++), iTotalRecCount);
			return ETrue;
			}
		else if(iSearchRequestContState.Length() == 0 || iNextRecHandle >= iTotalRecCount)
			{// No more answers available
			iNotifier.NextRecordRequestComplete(KErrEof, 0, 0);
			return ETrue;
			}
		}
	return EFalse;
	}

