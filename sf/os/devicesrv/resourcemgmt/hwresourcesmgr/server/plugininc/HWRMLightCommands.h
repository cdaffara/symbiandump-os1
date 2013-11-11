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
* - Command definitions for HWRM Light plugin API
* - Command parameter definitions for HWRM Light plugin API
* - Response parameter definitions for HWRM Light plugin API
*
*/




/**
 @file
 @publishedPartner
 @released
*/

#ifndef HWRMLIGHTCOMMANDS_H
#define HWRMLIGHTCOMMANDS_H

// Include plugin service so that including only this header is necessary to use Light plugin API.
#include <hwrm/hwrmpluginservice.h>

// CONSTANTS

// Identifies the ECom implementation to be used.
_LIT8(KHWRMLightApiEComMatch, "HWRMLight");

// CLASS DECLARATION

/**
*  Plugin interface class just encapsulates the Light command enumerations.
*  Responsible for defining the command IDs for each message.
*/
namespace HWRMLightCommand
    {
    /**
    * Enumeration for light targets.
    * If certain target is not supported by device,
    * plugin can either return KErrNotSupported or interpret
    * specified target in some device specific way.
    *
    * Light targets can be used as bitmask.
	*
	* @publishedPartner
	* @released
    */
    enum TLightTarget
        {
        /**
        No target. Not a valid target value, used only for error checking.
        */
        ENoTarget                    = 0x0,      
        /**
        Primary display of the device.
        */
        EPrimaryDisplay              = 0x1,     
        /**
        Primary keyboard of the device. 
        */
        EPrimaryKeyboard             = 0x2,     
        /**
        Secondary display of the device.
        */
        ESecondaryDisplay            = 0x4,   
        /**
        Secondary keyboard of the device. 
        */
        ESecondaryKeyboard           = 0x8,  
        /**
        Device specific custom target 1.
        */                                    
        ECustomTarget1               = 0x10,   
        /**
        Device specific custom target 2.
        */
        ECustomTarget2               = 0x20,    
        /**
        Device specific custom target 3.
        */
        ECustomTarget3               = 0x40,    
        /**
        Device specific custom target 4.
        */
        ECustomTarget4               = 0x80,    
        /**
        Device specific custom target 5.
        */
        ECustomTarget5               = 0x100,   
        /**
        Device specific custom target 6.
        */
        ECustomTarget6               = 0x200,   
        /**
        Device specific custom target 7.
        */
        ECustomTarget7               = 0x400,   
        /**
        Device specific custom target 8.
        */
        ECustomTarget8               = 0x800,   
        /**
        Device specific custom target 9.
        */
        ECustomTarget9               = 0x1000, 
        /**
        Device specific custom target 10.
        */
        ECustomTarget10              = 0x2000,  
        /**
        Device specific custom target 11.
        */
        ECustomTarget11              = 0x4000,  
        /**
        Device specific custom target 12.
        */
        ECustomTarget12              = 0x8000,
        /**
        Device specific custom target 13.
        */
        ECustomTarget13              = 0x10000,
        /**
        Device specific custom target 14.
        */
        ECustomTarget14              = 0x20000,
        /**
        Device specific custom target 15.
        */
        ECustomTarget15              = 0x40000,
        /**
        Device specific custom target 16.
        */
        ECustomTarget16              = 0x80000,
        /**
        Device specific custom target 17.
        */
        ECustomTarget17              = 0x100000,
        /**
        Device specific custom target 18.
        */
        ECustomTarget18              = 0x200000,
        /**
        Device specific custom target 19.
        */
        ECustomTarget19              = 0x400000,
        /**
        Device specific custom target 20.
        */
        ECustomTarget20              = 0x800000,
        /**
        Device specific custom target 21.
        */
        ECustomTarget21              = 0x1000000,
        /**
        Device specific custom target 22.
        */
        ECustomTarget22              = 0x2000000,
        /**
        Device specific custom target 23.
        */
        ECustomTarget23              = 0x4000000,
        /**
        Device specific custom target 24.
        */
        ECustomTarget24              = 0x8000000,
        /**
        Device specific custom target 25.
        */
        ECustomTarget25              = 0x10000000,
        /**
        Device specific custom target 26.
        */
        ECustomTarget26              = 0x20000000,
        /**
        Device specific custom target 27.
        */
        ECustomTarget27              = 0x40000000,
        };

    /**
    * Data structure used to pass data with ELightsOnCmdId.
	*
	* @publishedPartner
	* @released
    */ 
    struct TLightsOnData
        {
        /**
        Light target(s). TLightTargets can be used as bitmask.
        Command only affects selected targets.
        */
        TInt iTarget;
    
    	/**
        Intensity of light as percentage (1-100).
        If intensity is 0, default intensity is used.
		Note: Some devices will not support user defined intensity,
		in which case the device will behave in its default fashion.
		*/
        TInt iIntensity;
        
        /**
        Is fade-in used or not when turning lights on.
		Note: Some devices will not support fade-in, in which case 
		       the device will behave in its default fashion.
		*/
        TBool iFadeIn;
        };

    /**
    * Data structure used to pass data with ELightsOnSensorCmdId.
	*
	* @publishedPartner
	* @released
    */ 
    struct TLightsOnSensorData
        {
        
        /**
        Light target(s). TLightTargets can be used as bitmask.
        Command only affects selected targets.
        */
        TInt iTarget;
    
        /**
        Sensitivity of light sensor as percentage (1-100)
        If sensitivity is 0, default sensitivity is used.
		Note: Some devices will not support user defined sensitivity,
		in which case the device will behave in its default fashion.
		*/
        TInt iSensitivity;
        
        /**
        Is fade-in used or not when turning lights on.
		Note: Some devices will not support fade-in, in which case 
		the device will behave in its default fashion.
		*/
        TBool iFadeIn;
        };

    /**
    * Data structure used to pass data with ELightsBlinkCmdId
	*
	* @publishedPartner
	* @released
    */ 
    struct TLightsBlinkData
        {
        /**
        Light target(s). TLightTargets can be used as bitmask.
        Command only affects selected targets.
        If any of the selected targets doesn't support lights blink,
        none are affected and KErrNotSupported is returned.
        */
        TInt iTarget;
    
    	/**
        Intensity of light as percentage (1-100). 
        If intensity is 0, default intensity is used.
		Note: Some devices will not support user defined intensity,
		in which case the device will behave in its default fashion.
		*/
        TInt iIntensity;
    
    	/**
        Light on cycle duration in milliseconds.
        If value is 0, use default cycle duration.
        In that case also iOffCycleDuration must be 0.
		Note: Some devices will not support variable blink cycle times,
		       in which case a device default blink cycle duration is used.
		*/
        TInt iOnCycleDuration;

        /**
        Light off cycle duration in milliseconds.
        If value is 0, use default cycle duration.
        In that case also iOnCycleDuration must be 0.
		Note: Some devices will not support variable blink cycle times,
		       in which case a device default blink cycle duration is use.     
		*/
        TInt iOffCycleDuration;
        };


    /**
    * Data structure used to pass data with ELightsOffCmdId.
	*
	* @publishedPartner
	* @released
    */
    struct TLightsOffData
        {
        /**
        Light target(s). TLightTargets can be used as bitmask.
        Command only affects selected targets.
        */
        TInt iTarget;
    
        /**
        Is fade-out used or not when turning lights off.
		Note: Some devices will not support fade-out, in which case 
		       the device will behave in its default fashion.
		*/
        TBool iFadeOut;
        };
        
    /**
    * Data structure used to pass data with ESetLightsIntensityCmdId.
	*
	* @publishedPartner
	* @released
    */
    struct TLightsIntensityData
        {
        
        /**
        Light target(s). TLightTargets can be used as bitmask.
        Command only affects selected targets.
        */
        TInt iTarget;
    
        /**
        Intensity to be set for specifies targets.
		Note: Some devices will not support user defined intensity,
		       in which case the device will behave in its default fashion.
		*/
        TInt iIntensity;
        };

    struct TLightsColorData 
        {
        /**
        Light target(s). TLightTargets can be used as bitmask.
        Command only affects selected targets.
        */
        TInt iTarget;
        
        TUint8 iRed;
        
        TUint8 iGreen;
        
        TUint8 iBlue;
        
        /**
        * Set default color if True. R G B values are ignored.
        */
        TBool  iDefaultColor; 
        };


    // Data packages for commands
    
    /** A typedef'd packaged TLightsOnData */ 
    typedef TPckgBuf<TLightsOnData>        TLightsOnCmdPackage;
    /** A typedef'd packaged TLightsOnSensorData */
    typedef TPckgBuf<TLightsOnSensorData>  TLightsOnSensorCmdPackage;
    /** A typedef'd packaged TLightsBlinkData */
    typedef TPckgBuf<TLightsBlinkData>     TLightsBlinkCmdPackage;
    /** A typedef'd packaged TLightsOffData */
    typedef TPckgBuf<TLightsOffData>       TLightsOffCmdPackage;
    /** A typedef'd packaged TLightsIntensityData */
    typedef TPckgBuf<TLightsIntensityData> TLightsIntensityCmdPackage;
    /** A typedef'd packaged TInt */
    typedef TPckgBuf<TInt>                 TLightsSensorSensitivityCmdPackage;
    /** A typedef'd packaged TLightsColorData */
    typedef TPckgBuf<TLightsColorData>     TLightsColorCmdPackage;

    /** A typedef'd packaged TInt */
    typedef TPckgBuf<TInt>                TErrorCodeResponsePackage;


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
    enum TLightCmd
        {
        /**
        * No command. May be used for sanity checks, but 
        * never as an actual command ID.
        *
        * input    None   
        * output   None   
        */
        ENoCommandId = 1000,
        
        /**
        * Turns lights on.
        *
        * input    TLightsOnCmdPackage         See description above 
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrNotSupported if any target doesn't support lights.
        *    KErrGeneral if there is any problem.
        */
        ELightsOnCmdId,

        /**
        * Turns lights on using sensor for intensity control
        *
        * input    TLightsOnSensorCmdPackage   See description above 
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrNotSupported if any target doesn't support lights.
        *    KErrGeneral if there is any problem.
        */
        ELightsOnSensorCmdId,

        /**
        * Turns lights on in blinking mode
        *
        * input    TLightsBlinkCmdPackage      See description above 
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrNotSupported if any target doesn't support lights
        *        in general or just doesn't support blink.
        *    KErrGeneral if there is any problem.
        */
        ELightsBlinkCmdId,

        /**
        * Turns Lights off.
        *
        * input    TLightsOffCmdPackage        See description above 
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrNotSupported if any target doesn't support lights.
        *    KErrGeneral if there is any problem.
        */
        ELightsOffCmdId,

        /**
        * Set default lights intensity.
        *
        * input    TLightsIntensityCmdPackage  Lights intensity percentage
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        ESetLightsIntensityCmdId,
        
        /**
        * Set default lights sensor sensitivity.
        *
        * input    TLightsSensorSensitivityCmdPackage  Sensor sensitivity 
        *                                               percentage
        * output   TErrorCodeResponsePackage           Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        ESetLightsSensorSensitivityCmdId,
        
        
        /**
        * Set the color attribute of the light target.
        *
        * input    TLightsColorCmdPackage      See description above 
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrNotSupported if any target doesn't support lights.
        *    KErrGeneral if there is any problem.
        */
        ELightsSetColorCmdId

        };
    }

#endif      // HWRMLIGHTCOMMANDS_H   
            
// End of File
