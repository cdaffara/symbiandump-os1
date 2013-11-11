/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the following sections:
* - Command definitions for HWRM Vibra plugin API
* - Command parameter definitions for HWRM Vibra plugin API
* - Response parameter definitions for HWRM Vibra plugin API
*
*/




/**
 @file
 @publishedPartner
 @released
*/


#ifndef HWRMVIBRACOMMANDS_H
#define HWRMVIBRACOMMANDS_H

// Include plugin service so that including only this header is necessary to use Vibra plugin API.
#include <hwrm/hwrmpluginservice.h>

// CONSTANTS

// Identifies the ECom implementation to be used.
_LIT8(KHWRMVibraApiEComMatch, "HWRMVibra");


/**
*  Plugin interface class just encapsulates the Vibra command enumerations.
*  Responsible for defining the command IDs for each message.
*/
namespace HWRMVibraCommand
    {
    /**
    * Data structure used to pass vibra intensity and duration.
    * Used in EPulseVibraCmdId command.
	*
	* @publishedPartner
	* @released
    */ 
    struct TVibraData
   	    {
   	    // Duration of vibration in milliseconds. 
        TUint iDuration;
   	    // Intensity of vibration. 
   	    TInt8 iIntensity;
   	    };

    // typedef'd package of TInt containing the vibra command*/
    typedef TPckgBuf<TInt> TStartVibraCmdDataPackage;
    
       
    // Data packages for pulse command
    typedef TPckgBuf<TVibraData> TPulseVibraCmdDataPackage;

   
    /** typedef'd package of TInt containing the error code response. */
    typedef TPckgBuf<TInt> TErrorCodeResponsePackage;

    /**
    * Command definitions
    * All commands contain a command id and a set of parameters. The command ids
    * are defined first and after that parameters for each command.
    *
    * This interface is asynchronous. A command is interpreted of being completed
    * after a response for that command is received.
    *
    * The field input means the parameter for the command.
    * The field output means the parameter for the response.
	*
	* @publishedPartner
	* @released
    */
    enum TVibraCmd
        {
        /**
        * No command. May be used for sanity checks, but 
        * never as an actual command ID.
        *
        * input    None   
        * output   None   
        */
        ENoCommandId = 2000,
        
        /**
        * Start vibra. 
        *
        *
        * input    TStartVibraCmdDataPackage   Intensity  
        *    Intensity of the vibra in decimal is -100 to 100,
        *    which shows the percentage of the vibra motor full
        *    safe rotation speed. When intensity is negative, 
        *    the vibra motor rotates in the negative direction.
        *    When intensity is positive, the vibra motor rotates
        *    in the positive direction. 
        *    Value 0 effectively stops the vibra.
        *    Plugin should map intensity to such values that high or 
        *    low intensities never damage the device even
        *    if used for extended periods of time.
        * output   TErrorCodeResponsePackage   Error code
        *    KErrNone if operation succeeds.  
        *    KErrNotSupported if device doesn't support user-defined vibra intensity. 
        *    KErrArgument if intensity is outside allowed range.
        *    KErrGeneral if there is any problem.
        */
        EStartVibraCmdId,
        
        /**
        * Start vibra with device default intensity.
        *
        * input    None   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStartVibraWithDefaultSettingsCmdId,

        /**
        * Stop vibra. 
        *
        * input    None   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStopVibraCmdId,
        
        /**
        * Pulse vibra. Start and stop of vibration must be carried
        * out internally within plugin or underlying implementation.
        * If there is vibration ongoing when pulse command is sent,
        * intensity must be set according to pulse command intensity 
        * and when specified pulse duration has expired, interrupted
        * intensity must be restored. Ideally vibrations with varying 
        * intensity are observable althoug they would overlap.
        *
        * input    TPulseVibraCmdDataPackage   iIntensity
        *    Intensity of the vibra in decimal is 1 to 100,
        *    which shows the percentage of the vibra motor full
        *    safe rotation speed.
        *    Plugin should map intensity to such values that high or 
        *    low intensities never damage the device even
        *    if used for extended periods of time.
        * input    TPulseVibraCmdDataPackage   iDuration
        *    Duration of the vibra in milliseconds
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrNotSupported if device doesn't support user-defined vibra intensity.
        *    KErrArgument if intensity or duration is outside allowed range.
        *    KErrGeneral if there is any problem.
        */
        EPulseVibraCmdId
        };
    }

#endif      // HWRMVIBRACOMMANDS_H   
            
// End of File
