/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Secure Output Custom Interface
*
*/






#ifndef SECUREOUTPUTCI_H
#define SECUREOUTPUTCI_H

//  INCLUDES
#include <e32std.h>


// CONSTANTS

/** Secure Output Custom Interface UID */
const TUid KMmfVideoSecureOutputUid = { 0x10204be9 };


// CLASS DECLARATION

/**
 * Secure Output Custom Interface main interface class.
 *
 * The Secure Output Custom Interface can be used to notify a video
 * rendering media device that the video can only be sent to a secure
 * output. It is typically used for DRM-protected content.
 *
 * The interface is typically implemented in video decoding or
 * post-processing media devices with support for insecure video
 * outputs.
*/
class MMmfVideoSecureOutput
    {
public:
    /**
     * Notifies the media device if the content can only be sent to a
     * secure output. The device display is typically considered a
     * secure output, but a high-quality unprotected analog or digital
     * video output connector is not. By default all content can be
     * sent to both secure and insecure outputs.
     *
     * This method can only be called before the media device has been
     * initialised with InitializeL().
     *
     * @param aSecure Set to ETrue if the content can only be sent to
     *     a secure output, EFalse if both secure and unsecure outputs can
     *     be used.
     *
     * @leave KErrNotSupported Insecure outputs cannot be
     * disabled. The client should not play protected content.
     */
    virtual void MmvsoSetSecureOutputL(TBool aSecure) = 0;
    };

#endif      // SECUREOUTPUTCI_H

// End of File
