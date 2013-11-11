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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#if !defined(__TCOMPOSITIONBACKEND_H__)
#define __TCOMPOSITIONBACKEND_H__

#include <e32base.h>
#include <graphics/surface.h>
#include <graphics/compositionsurfaceupdate.h>
#include <graphics/extensioncontainer.h>
#include "trequestorder.h"

/**
The following constants signify the delay in microseconds between two 
emulations of composition. During the emulation mock receiver will 
process a number of outstanding requests and issues the signal 
back to the SUS. Content update receiver with highest priority will process the 
requests at highest rate.
*/
const TInt KCompositionInterval = 1000000 / 20; // for composition threads with normal priority
const TInt KCompositionIntervalLong = KCompositionInterval * 2; // for composition threads with lower priority
const TInt KCompositionIntervalShort = KCompositionInterval / 2; // for composition threads with higher priority

enum RequestType
	{
	EReqEmpty = 0,
	EReqAvailable,
	EReqDisplayed,
	EReqDisplayedXTimes,	
	};
	
class RequestObject
	{
public:
	RequestType iType;
	TRequestStatus* iStatus;
	TUint32* iTimeStamp;
	TInt iDisplayedXTimes;
	};

NONSHARABLE_CLASS(CTContentUpdateReceiver) : 
public CExtensionContainer,
public MCompositionSurfaceUpdate
	{
public:
	static CTContentUpdateReceiver* NewL(TInt aScreen);
	~CTContentUpdateReceiver();
	static TInt ThreadFunction(TAny* aAny);
	TInt Screen() {return iScreen;}
	void Stop();
	void SetCompositionOrder(CRequestOrder* aOrder) { iCompositionOrder = aOrder;}
	IMPORT_C void SetVisible(TBool aVisible);
	IMPORT_C TInt SetInternalPriority(TThreadPriority aInternalPriority);
	inline TBool OutstandingRequest(); //returns ETrue if there are any oustanding requests need to be processed by the mock receiver

	/**	
	Mark the surface as dirty and request for composition.
	The function doesn't take onwership of aRegion
	*/	
	virtual void ContentUpdated(const TSurfaceId& 	aSurface, 
										TInt 			aBuffer, 
										const TRegion* 	aRegion, 
										TRequestStatus* aStatusAvailable, 
										TRequestStatus* aStatusDisplayed, 
										TUint32* 		aTimeStamp, 
										TRequestStatus* aStatusDispXTimes, 
										TInt* 			aDisplayedXTimes);

	virtual void	Delete(void){}
	virtual TInt ApiVersion(void) {return 0;}
	virtual TVersion InternalVersion(void){return TVersion();}
protected:  //From CBase
    virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
private:
	CTContentUpdateReceiver(TInt aScreen);
	void ConstructL();
	TInt CheckNewNotifications();
	void DoSetInternalPriorityL();
	static TInt CallBack(TAny *aAny);
    TInt Add(TRequestStatus *aStatus, RequestType aType, 
            TInt aDisplayedXTimes = 1, TUint32* aTimeStamp = NULL);
	void Remove(TInt aIndex);		
private:
	TThreadId iThreadId; //thread from which ContentUpdated is called
	TThreadId iReceiverThreadId; 
	TBool iStop;
	TInt iScreen;
	CPeriodic* iPeriodic;
	RequestObject iArray[1024]; //should be big enough
	TInt iNumberElements;
	TBool iVisible;
	TBool iSetInternalPriority;
	TThreadPriority iInternalPriority;
	CRequestOrder* iCompositionOrder;
	RFastLock iLock;
	RSemaphore iPriorityLock;
	};

IMPORT_C TInt StartTestUpdateReceiver(CTContentUpdateReceiver*& aReceiver, TInt aScreen);
IMPORT_C void CloseTestUpdateReceiver(CTContentUpdateReceiver* aReceiver);



inline TBool CTContentUpdateReceiver::OutstandingRequest()
	{ return iNumberElements > 0;}
	

#endif	// __TCOMPOSITIONBACKEND_H__
