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
* Description:  	Receives the Last Known Position, Last Known Position Area requests and Empty
* 				Position Store requests from Subsessions and Sessions in the Location Server and 
*				forwards them to the respective active objects.
*
*/




#ifndef CPOSLOCMONITORREQHANDLERHUB_H
#define CPOSLOCMONITORREQHANDLERHUB_H

//--------------------------------------------------------------------------------------
//  INCLUDES
#include <e32err.h> 						
#include <lbspositioninfo.h>
#include <lbserrors.h>
#include "EPos_Global.h"

#include "rlbslocmonitorsession.h"
#include "rlbslocmonitorareapositioner.h"

// TODO - Do we need in header ? forward declaration + #include in cpp file ?
#include "EPos_CPosLastKnownPosHandler.h"
#include "EPos_CPosLastKnownPosAreaHandler.h"
#include "EPos_CPosEmptyLastKnownPosStoreHandler.h"                  

//TODO Check if necessary
#include <f32file.h>
#include <e32property.h>

//--------------------------------------------------------------------------------------
// CONSTANT DECLARATIONS
// The enum used to identify the respective requests queues
enum TRequestQId
	{
	ELastKnownPosition = 0,
	ELastKnownPositionArea,
	EEmptyLastKnownPositionStore,
	};


//--------------------------------------------------------------------------------------
// CLASS DECLARATION
/**
  This class forwards the request for Last Known ( Position / Position Area ) and
  empty last known position store requests to the respective active objects. 
 */ 

class CPosLocMonitorReqHandlerHub : public CBase
    {
    public:  // Constructors and destructor
        static CPosLocMonitorReqHandlerHub* NewL();
        virtual ~CPosLocMonitorReqHandlerHub();

    public: // New functions
        void SetPositionInfo(const TPositionInfo& aPositionInfo);
        
        void GetLastKnownPosReqL(const RMessage2& aMessage);
        void GetLastKnownPosAreaReqL(const RMessage2& aMessage);
        
        void CancelGetLastKnownPosReqL(const RMessage2& aMessage);
        void CancelGetLastKnownPosAreaReqL(const RMessage2& aMessage);
        
        void EmptyLastKnownPosStoreReqL(const RMessage2& aMessage);
        void CancelEmptyLastKnownPosStoreReqL(const RMessage2& aMessage);
        
        void NotifySubSessionClosed(const RMessage2& aMessage);
        void NotifyServerShutDown();
        void NotifySessionClosed(const CSession2* aSessionPtr);
        
    private:
        void SetLastKnownPosition(TPositionInfoBase& aPosInfo) const;
        CPosLocMonitorReqHandlerHub( );
        void ConstructL();
        
        CPosLocMonitorReqHandlerHub( const CPosLocMonitorReqHandlerHub& );
        CPosLocMonitorReqHandlerHub& operator=( const CPosLocMonitorReqHandlerHub& );
        
    private:    // Data

    	// Session with the location monitor
        RLbsLocMonitorSession 				iLocMonSession; 	
        // Subsession with the location monitor - used only for SetPositionInfoL()
        RLbsLocMonitorAreaPositioner		iLocMonSubSession; 	
        
        // Last Known Position Request
        CPosLastKnownPosHandler* 	iLastKnownPosHandler;	
    	// Last Known Position Area Request
        CPosLastKnownPosAreaHandler* iLastKnownPosAreaHandler;
        // Empty Last Known Position Store request
        CPosEmptyLastKnownPosStoreHandler* iEmptyLastKnownPosStoreHandler;

    };

#endif      // CPOSLOCMONITORREQHANDLERHUB_H
            
// End of File
