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
* Description: Defines a custom interface for Decoders to indicate 
*              the support for Rotation     
*
*/





#ifndef VIDEODECODERROTATIONCI_H
#define VIDEODECODERROTATIONCI_H

//  INCLUDES
#include <mmf/devvideo/devvideobase.h>


// CONSTANTS

/** Video Decoder Rotation Custom Interface UID */
const TUid KMmfVideoDecoderRotationUid = { 0x200195B1 };


// CLASS DECLARATIONS



/**
 * Video Decoder Rotation Custom Interface main interface class. 
 *
 * The Video Decoder Rotation Custom Interface allows the MDF video
 * decoders to signal the capability to do the rotation. This enables
 * the PostProcessor to delegate the responsibility of rotation to the
 * decoders. 
 * 
 * 
 * The PostProcessor will provide the rotation parameters via
 * CMMFVideoPlayHwDevice::SetRotateOptionsL
 */

class MMmfVideoDecoderRotation
    {
    };
#endif      // VIDEODECODERROTATIONCI_H
            
// End of File
