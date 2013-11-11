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

#ifndef BTSDPAGENTENGINE_H
#define BTSDPAGENTENGINE_H

#include <e32base.h>
#include <bttypes.h>
#include <btsdp.h>
#include "agtypes.h"

class CSdpAttrBuilder;
class CSdpRequesterBase;
class CSdpAttributeRequester;
class CSdpSearchRequester;

/**
   Engine that actually implements the CSdpAgent facade funtionality.
**/
NONSHARABLE_CLASS(CSdpAgentEng) : public CBase
	{
friend class CSdpAttributeRequester;
friend class CSdpSearchRequester;
public:
	static CSdpAgentEng* NewL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr);
	void ConstructL();
	~CSdpAgentEng();
	void SetRecordFilterL(const CSdpSearchPattern& aUUIDFilter);
	void NextRecordRequestL();

	void AttributeRequestL(TSdpServRecordHandle aHandle, 
						   const CSdpAttrIdMatchList& aMatchList);
	void AttributeRequestL(MSdpElementBuilder* aBuilder,
		                   TSdpServRecordHandle aHandle, 
						   const CSdpAttrIdMatchList& aMatchList);
	void Cancel();

private:
	// upcalls from requesters AOs. Not exported
	void HandleServiceSearchResponseL(TUint16 aTotalRecCount,
									 TUint16 aCurrentRecCount,
									 const TDesC8& aRecHandles,
									 const TDesC8& aContState);
	void HandleServiceSearchError(TInt aError);
	void HandleAttributeResponseL(const TDesC8& aAttributeList, 
								  const TDesC8& aContState);
	void HandleAttributeError(TInt aError);

	void SendAttributeRequestL();

	TBool TryToCompleteNextRecordRequest();
	 //FIXME: do we need this or do we search?
	CSdpAgentEng(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr);
	void ResetForNewAttributeRequest();
	void ResetForNewRecordRequest();

private:
	enum TServiceSearchState
		{
		EIdle,
		ERequesting,
		EResultsReceived
		};

	TContState iSearchRequestContState;
	CSdpSearchPattern* iUUIDFilter;
	CArrayFix<TSdpServRecordHandle>* iBuiltRecHandles;
	TInt iNextRecHandle;
	TInt iTotalRecCount;
	TServiceSearchState iServiceSearchState;
	TBool iCancelCalled;

	TSdpServRecordHandle iAttributeRequestHandle;
	TContState iAttributeRequestContState;
	CSdpAttrIdMatchList* iMatchList;
	CSdpAttrBuilder* iDefaultAttributeBuilder;
	CElementParser* iParser;

	MSdpAgentNotifier& iNotifier;
	TBTDevAddr iRemoteAddress;
	RSdpSession iSdpSession;
	CSdpSearchRequester* iSearchRequester;
	CSdpAttributeRequester* iAttributeRequester;
	};

#endif
