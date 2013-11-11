/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSPSYLISTLISTENER_H
#define MPOSPSYLISTLISTENER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TPositionInfoBase;

/**
* Event type for PSY list change.
*/
enum TPosPsyListChangeEventType
    {
    //A PSY has been removed. Listener shall
    //stop using this PSY
    EPosPsyListChangeEventPsyDeleted = 1,  
    
    //The priority order is changed
    EPosPsyListChangeEventPriorityChanged,
    
    //The PSY list is changed. listener shall
    //rebuild the list in next location request. 
    EPosPsyListChangeEventListRebuild  
    };

/**
* PSY change event  
*/
struct TPosPsyListChangeEvent
    {
    //change event type
    TPosPsyListChangeEventType iType; 

    //Changed module ID. This field is valid only if
    //PSY list change event type is Deleted or Added
    TPositionModuleId iPsyId;
    };

// CLASS DECLARATION

/**
* The MPosPsyListListener is the interface to receive dynamic PSY list change
* event.
*/
class MPosPsyListListener
    {
    public: 
              
        /**
        * Notify the listener that PSY list is changed.
        *
        * @param aEvent Contains details of the changes of dynamic
        * PSY list.
        */
        virtual void PsyListChanged( 
            const TPosPsyListChangeEvent& aEvent ) = 0;
    };

#endif      // MPosPsyListListener_H  
            
// End of File
