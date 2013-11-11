/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __MCT_LBS_REQUEST_OBSERVER_H__ 
#define __MCT_LBS_REQUEST_OBSERVER_H__

// CLASS DECLARATION

/**
*  Interface for notification when a CRequester request is complete.
*
*/
class MCT_LbsRequestObserver
    {
    public:
        
        /**
        * Called when a CRequester request has completed.
        */
		virtual void RequestCompleteL() = 0;

    };

#endif      // __MCT_LBS_REQUEST_OBSERVER_H__   
            
// End of File
