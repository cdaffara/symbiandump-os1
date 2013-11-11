/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptic plugin request data header file.
*
*/


#ifndef C_HWRMHAPTICSPLUGINREQUESTDATA_H
#define C_HWRMHAPTICSPLUGINREQUESTDATA_H

#include "hwrmhapticsplugintransactionlist.h"

/**
 * Data storage class for plugin requests.
 */
class CHWRMHapticsPluginRequestData : public CHWRMHapticsPluginTransactionListItem
    {
public:

    /**
     * C++ constructor.
     */
    CHWRMHapticsPluginRequestData( const RMessage2& aRequestMessage,
                                   TUint8 aTransId, 
                                   TInt aCommandId, 
                                   TBool aSplitCommand);

    /**
     * Virtual destructor.
     */
    virtual ~CHWRMHapticsPluginRequestData();


    /**
     * Method to get plugin request message.
     *
     * @return Plugin request message
     */
    RMessage2 RequestMessage();

    /**
     * Method to get information is command split to two adaptation calls.
     *
     * @return Is command split or not.
     */
    TBool CommandSplit();
    
    /**
     * Method to get information is command split to two adaptation calls.
     *
     * @return Is command split or not.
     */
    void SetRequestData( HBufC8* aRequestData );
    
private:

    /**
     * Can have NULL handle for timer triggered requests.
     */
    RMessage2 iRequestMessage;

    /**
     * Indicates if command was split to two adaptation calls.
     */
    TBool iSplitCommand;

    /**
     * Request data buffer to be deleted, when the request is completed.
     * Owned.
     */
    HBufC8* iReqData;
    };


#endif  // C_HWRMHAPTICSPLUGINREQUESTDATA_H
            
// End of File
