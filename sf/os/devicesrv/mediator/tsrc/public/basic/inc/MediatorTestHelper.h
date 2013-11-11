/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef MEDIATORTESTHELPER_H
#define  MEDIATORTESTHELPER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "MediatorCommandInitiator.h"
#include "MediatorCommandResponder.h"
#include "MediatorEventProvider.h"
#include "MediatorEventConsumer.h"
#include "MediatorNotifications.h"
#include "MediatorServiceDefs.h"

using namespace MediatorService;
// CONSTANTS

// MACROS

// DATA TYPES

enum TMediatorNotifyType
    {
    EClear,
    EMediatorEvent,
    EMediatorCommand,
    ECommandResponse,
    ECancelMediatorCommand,
    EMediatorEventsAdded,
    EMediatorCommandsAdded,
    EMediatorCategoryRemoved,
    EMediatorEventsRemoved,
    EMediatorCommandsRemoved 
    };

// CLASS DECLARATION

NONSHARABLE_CLASS(CTestCasePropertyWatch):public MMediatorEventObserver,
                    public MMediatorNotifications,
                    public MMediatorCommandObserver,
                    public MMediatorCommandResponseObserver
	{
public:
	static CTestCasePropertyWatch* NewL();
	
	~CTestCasePropertyWatch();
    void ClearAllNotifyData( );
	TMediatorNotifyType iNotificationReceived;	
	TUid iDomain;
    TUid iCategory; 
    TInt iEventId;                                                                                                                               
	TInt iCommandId;
	REventList iEvents;
	RCommandList iCommands;
		
private:
	CTestCasePropertyWatch();
	void ConstructL();
    
    void MediatorEventL( TUid aDomain,
                         TUid aCategory, 
                         TInt aEventId, 
                         const TDesC8& aData );	
                         
    void MediatorCommandL( TUid aDomain,
                           TUid aCategory, 
                           TInt aCommandId,
                           TVersion aVersion, 
                           const TDesC8& aData );

    void CommandResponseL( TUid aDomain,
                           TUid aCategory, 
                           TInt aCommandId,
                           TInt aStatus, 
                           const TDesC8& aData );

    void CancelMediatorCommand( TUid aDomain,
                                TUid aCategory, 
                                TInt aCommandId );    

    void MediatorEventsAddedL( TUid aDomain, 
                               TUid aCategory, 
                               const REventList& aEvents );
     
    void MediatorCommandsAddedL( TUid aDomain, 
                                 TUid aCategory,
                                 const RCommandList& aCommands );
                                             
    void MediatorCategoryRemovedL( TUid aDomain, TUid aCategory );
                                             
    void MediatorEventsRemovedL( TUid aDomain, 
                                 TUid aCategory,
                                 const REventList& aEvents );
                                             
    void MediatorCommandsRemovedL( TUid aDomain, 
                                   TUid aCategory,
                                   const RCommandList& aCommands );                                             
                                             
    	

	};
	

NONSHARABLE_CLASS(CTimeOutTimer) : public CActive
{
	public:
		static CTimeOutTimer* NewL();
		void RequestTimeOut(TInt aSeconds);
		void RunL();
		void DoCancel();
		TBool iTimeOutOccured;
		
	protected:
		CTimeOutTimer();		
		void ConstructL();
	private:
		RTimer iTimer;
		
	
};

#endif
