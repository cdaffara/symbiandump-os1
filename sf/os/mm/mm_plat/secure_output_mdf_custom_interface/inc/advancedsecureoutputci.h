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
* Description:  Advanced Secure Output Custom Interface
*
*/






#ifndef ADVANCEDSECUREOUTPUTCI_H
#define ADVANCEDSECUREOUTPUTCI_H

//  INCLUDES
#include <e32std.h>


// CONSTANTS

/** Allowed Secure Output Custom Interface UID */
const TUid KMmfVideoAdvancedSecureOutputUid = { 0x2002120E };


// CLASS DECLARATION

/**
* Advanced Secure Output Custom Interface main interface class.
*
* This interface is an extension of Secure Output Custom Interface.
* It allows a bitmask to be passed to implementor of this interface.
* The bitmask defines what outputs are allowed.
*
* The interface is typically implemented in video decoding or
* post-processing media devices with support for insecure video
* outputs.
*/

class MMmfAdvancedVideoSecureOutput
{
public:
	enum TMmVideollowedOutputs
	{
		/**
		* Allow all video outputs
		*/    	
		EVideoAllowAll                      = 0xFFFFFFFF,    

		/**
		* Not allowed on external outputs
		*/    	 
		EVideoAllowInternalOnly             = 0x00000000,               			

		/**
		* Allow passing content over analog outputs,
		* e.g. composite and S-video
		*/
		EVideoAllowAnalog              = 0x00000010,

		/**
		* Allow video output over an anlog output channel which has MacroVision
		* video protection. It can also be used for other analog output channel 
		* which has a protection mechanism.
		*/     
		EVideoAllowMacroVision         = 0x00000020,

		/**
		* Allow the passing the video data to devices using HDMI, eg DVD players and HDTV
		*/			
		EVideoAllowHDMI                = 0x00000200,

		/**
		* Licensed product must attempt to engage HDCP to protect the video. 
		* However it should be passed through to HDMI even if HDCP is not engaged or fails to engage.
		*/
		EVideoAllowHdmiHdcpRequested   = 0x00000400,						    

		/**
		* Licensed product is required to engage HDCP to protect the video.
		* If HDCP is not engaged or can not be engaged the video must not be passed through to HDMI.
		*/
		EVideoAllowHdmiHdcpRequired     = 0x00000800        

	};

	/**
	* Notifies the media device about the allowed outputs. Video stream is allowed only to the
	* selected outputs. The device display is typically considered a
	* secure output, but a high-quality unprotected analog or digital
	* video output connector is not. By default all content can be
	* sent to both secure and insecure outputs.
	*
	* @param aAllowedOutputMask bitMask to decide what outputs are allowed.
	*        This API doesn't dictact the interpretation of bitMask.
	*
	* @leave KErrNotSupported Insecure outputs cannot be
	* disabled. The client should not play protected content.
	*/    
	virtual void MmavsoSetAllowedOutputL(TUint aAllowedOutputMask) = 0;
};

#endif      // ADVANCEDSECUREOUTPUTCI_H

// End of File

