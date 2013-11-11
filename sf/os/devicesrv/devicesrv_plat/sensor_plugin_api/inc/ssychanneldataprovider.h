/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel data provider interface for SSY plug-ins.
*
*/



#ifndef SSYCHANNELDATAPROVIDER_H
#define SSYCHANNELDATAPROVIDER_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h>

/** 
The UID of the ssy channel data provider API.
*/
static const TUid KSsyChannelDataProviderInterface1 = {0x200159EF};

// CLASS DECLARATION

/**
* Channel data provider implementation
 */
class MSsyChannelDataProvider
    {
public:
    /**
    * Starts asynchronous data listening. Multiple OpenChannel()-requests
    * can be active for different channels at the same time.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this start channel 
    *            data message
    * @param[in] aBuffer Pointer to a data buffer 
    * @param[in] aCount Indicates data buffer size as a count of the data 
    *            objects.
    */  
    virtual void StartChannelDataL( const TSensrvChannelId aChannelId, 
                                    TUint8* aBuffer, TInt aCount ) = 0;  

    /**
    * Stops asynchronous data listening. The data buffer is not valid after 
    * call of this function.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this stop channel 
    *            data message
    */  
    virtual void StopChannelDataL( const TSensrvChannelId aChannelId ) = 0;  
    
    /**
    * Forces SSY to call BufferFilled() regardless of how many data items 
    * have been written to buffer. Even if no data items have yet been written, 
    * BufferFilled() must be called.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this force buffer filled 
    *            message
    */  
    virtual void ForceBufferFilledL( const TSensrvChannelId aChannelId ) = 0;      

    /** 
    * Returns a pointer to a specified interface - to allow future extension
    * of this class without breaking binary compatibility
    *
    * @since S60 5.0
    * @param aInterfaceUid Identifier of the interface to be retrieved
    * @param aInterface A reference to a pointer that retrieves the specified interface.
    */
    virtual void GetChannelDataProviderInterfaceL( TUid aInterfaceUid, 
                                                   TAny*& aInterface ) = 0;
    };


#endif //SSYCHANNELDATAPROVIDER_H

// End of File


