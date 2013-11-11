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



#ifndef HWRMFMTXCOMMANDS_H
#define HWRMFMTXCOMMANDS_H

/*****************************************************
*       Needs customer implementation
*****************************************************/

/*
* ==============================================================================
* This file contains the following sections:
*   - Event definition for HWRM FM TX plugin API
*   - Command definitions for HWRM FM Tx plugin API
*   - Command parameter definitions for HWRM FM Tx plugin API
*   - Response parameter definitions for HWRM FM Tx plugin API
* ==============================================================================
* @publishedPartner
* @released
*/

// INCLUDES
// Include plugin service so that including only this header is necessary to use FM Tx plugin API.
#include <hwrm/hwrmpluginservice.h>

// CONSTANTS

// Identifies the ECom implementation to be used for plugin controlling FM Tx.
_LIT8(KHWRMFmTxApiEComMatch, "HWRMFmTx");
// Identifies the ECom implementation to be used for plugin observing FM Tx related events.
// Default implementation monitoring accessory and USB connections.
_LIT8(KHWRMFmTxWatcherApiEComMatch, "HWRMFmTxWatcher");


// CLASS DECLARATION

/**
*  Plugin interface class just encapsulates the FM Tx command enumerations.
*  Responsible for defining the command IDs for each message.
*
*/
namespace HWRMFmTxCommand
    {
    //
    // Data packages for requests
    //
    /**
	*
	* @publishedPartner
	* @released
	*/
    const TUint8 KRdsPsLength = 8;
    typedef TBuf8<KRdsPsLength> TRdsPs;
    typedef TPckgBuf<TRdsPs> TRdsPsPackage;        // RDS PS ASCII String
    
    /**
	*
	* @publishedPartner
	* @released
	*/
    const TUint8 KMaxRdsPtynLength = 8;
    typedef TBuf8<KMaxRdsPtynLength> TRdsPtyn;
    typedef TPckgBuf<TRdsPtyn> TRdsPtynPackage;

    /**
	*
	* @publishedPartner
	* @released
	*/
	struct TRtPlusTag
    	{
        TUint8 iContentType;  // 0 to 63
        TUint8 iStartMarker;  // Start marker 0: means first character in the RT.
        TUint8 iLengthMarker; // The length marker is ranging from 1 to 63.
    	};
        /**
    	*
    	* @publishedPartner
    	* @released
    	*/
    const TUint8 KMaxRtDataLength = 64;
	struct TRtData
		{
		TBuf8<KMaxRtDataLength>  iText;
		TRtPlusTag iTag1;
		TRtPlusTag iTag2;
		};
    typedef TPckgBuf<TRtData>  TRtPackage;

    typedef TPckgBuf<TUint32> TSetFrequencyPackage;  // frequency units = kHz
    typedef TPckgBuf<TUint8> TScanRequestPackage;    // number of channels asked for (1 to 10)
	typedef TPckgBuf<TInt>     TRdsPtyPackage;
	typedef TPckgBuf<TBool>    TRdsMsPackage;
    typedef TPckgBuf<TUint32>  TRdsLangIdPackage;
	
    //
    // Data packages for responses
    //
    /**
	*
	* @publishedPartner
	* @released
	*/
    struct TScanResponseData
    	{
    	TInt iErrorCode; // KErrNone if 1+ frequencies were found
		TUint8 iFrequenciesFound;          // 0-10
		TFixedArray<TUint32, 10> iChannels; // kHz
    	};
	typedef TPckgBuf<TScanResponseData>   TScanResponsePackage;	

    /**
	*
	* @publishedPartner
	* @released
	*/
    struct TFrequencyRangeData
    	{
    	TInt iErrorCode; // KErrNone if other values are valid
    	TUint32 iMinFrequency; // kHz
    	TUint32 iMaxFrequency; // kHz
    	TUint32 iStepSize; // kHz
    	};
	typedef TPckgBuf<TFrequencyRangeData> TFrequencyRangePackage;    	
    	
    /**
	*
	* @publishedPartner
	* @released
	*/
    struct TConnStatusData
    	{
    	TInt iErrorCode; // KErrNone if connection observation successfully ongoing
    	TBool iConnected; // Truth value indicating wired connection status
    	};
    typedef TPckgBuf<TConnStatusData> TConnStatusPackage;

    /**
	*
	* @publishedPartner
	* @released
	*/
	enum TFmTxHwState
		{
		EFmTxHwStateOff,
		EFmTxHwStateOn,
		EFmTxHwStateScanning
		};
    /**
	*
	* @publishedPartner
	* @released
	*/
	struct TFmTxStatusData
		{
		TFmTxHwState state; // New Tx state
		TUint32 frequency;     // New Tx frequency
		};
	typedef TPckgBuf<TFmTxStatusData> TStatusIndicationPackage;
    
    typedef TPckgBuf<TInt> TErrorCodeResponsePackage;		


   /*
    * ==========================================================================
    * Event definitions
    * All events contain an event id and a set of parameters. The event ids
    * are defined first and after that parameters for each event.
    *
    * The field @param means the parameter for the event.
    *
    * ==========================================================================
    */
    /***
    * DOS(Domestic Operating System) originated events
	*
	* @publishedPartner
	* @released
    */
    enum TFmTxEvent
        {
        /**
        * No command. May be used for sanity checks, but
        * never as an actual command ID.
        *
        * @param None
        */
        ENoEventId = 0,

       /**
        * Status indication.
        *
        * @param    TStatusIndicationPackage:
        *               1. TFmTxStatusData
        */
        ETxStatusIndId,

        /**
        * Connection indication from watcher plugin.
        *
        * @param TConnStatusPackage
        */
        ETxWatcherConnIndId = 50
        };

    /*
    * ==========================================================================
    * Command definitions
    * All commands contain a command id and a set of parameters. The command ids
    * are defined first and after that parameters for each command.
    *
    * This interface is asynchronous. A command is interpreted of being completed
    * after a response for that command is received.
    *
    * The field input means the parameter for the command.
    * The field output means the parameter for the response.
    * ==========================================================================
	*
	* @publishedPartner
	* @released
    */
    enum TFmTxCmd
        {
        /**
        * No command. May be used for sanity checks, but 
        * never as an actual command ID.
        *
        * input    None   
        * output   None   
        */
        ENoCommandId = 4000,

        /**
        * Start FM Transmitter. 
        *
        * input    TSetFrequencyPackage   Initial frequency
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrArgument if frequency is outside allowed range.
        *                  - KErrInUse if Fm Tx is already enabled.
        *                  - KErrGeneral if there is any other problem.
        */
        ETxOnCmdId,

        /**
        * Stop FM Transmitter. 
        *
        * input    None   
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled.
        *                  - KErrGeneral if there is any other problem.
        */
        ETxOffCmdId,

        /**
        * Set FM Transmitter frequency
        *
        * input    TSetFrequencyPackage
        *               1. The new frequency in kHz.
        * output   TErrorCodeResponsePackage:
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrArgument if frequency is outside allowed range.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxFrequencyCmdId,

        /**
        * RSSI scan request
        *
        * input    TScanRequestPackage
        *               1. A request for clear frequencies.
        * output   TScanResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        *               2. A list of clear frequencies (if successful).
        */
        ETxScanRequestCmdId,

		/**
        * Tx frequency range request
        *
        * input    None
        * output   TFrequencyRangePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrGeneral if there is any other problem.
        *               2. The frequency range
        *                  - Max, Min, and step size (if successful).
        */
        EGetTxFrequencyRangeCmdId,
        
        /**
        * Set RDS PS data
        *
        * input    TRdsPsPackage:
        *               1. A package containing PS string.
        *                  The lenght of the string is always 8
        *                  character. If athe name of the RDS PS 
        *                  ASCII string is shorter than 8 character,
        *                  then the rest of the ASCII string must be
        *                  filled up with spaces(ASCII code 0x20).
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxRdsPsCmdId,
        
        /**
        * Set RDS PTY data
        *
        * input    TRdsPtyPackage:
        *               1. A package containing PTY number.
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxRdsPtyCmdId,        

        /**
        * Set RDS PTYN data
        *
        * input    TRdsPtynPackage:
        *               1. A package containing PTYN string
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxRdsPtynCmdId,

        /**
        * Set RDS MS data
        *
        * input    TRdsMsPackage:
        *               1. A package containing Music/Speech flag.
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxRdsMsCmdId,

        /**
        * Set RDS language ID data
        *
        * input    TRdsLangIdPackage:
        *               1. A package containing language ID.
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxRdsLangIdCmdId,

        /**
        * Set Radiotext data
        *
        * input    TRtPackage:
        *               1. A package containing Radiotext.
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        ESetTxRtCmdId,
        
        /**
        * Clears the previously set Radiotext data
        *
        * input    None
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrNotReady if Fm Tx is not enabled
        *                  - KErrGeneral if there is any other problem.
        */
        EClearTxRtCmdId,

        /**
        * Fetch connection status.
        *
        * input    None
        * output   TConnStatusPackage
        */
        EConnStatusCmdId = 4500,

        /**
        * Start observing fmtx state affecting connections.
        *
        * input    None
        * output   TConnStatusPackage
        */
        EStartObservingCmdId,

        /**
        * Stop observing fmtx state affecting connections.
        *
        * input    None
        * output   TErrorCodeResponsePackage
        *               1. Error code
        *                  - KErrNone if operation succeeds.
        *                  - KErrGeneral if there is any other problem.
        */
        EStopObservingCmdId
        };
    }

#endif      // HWRMFMTXCOMMANDS_H   
            
// End of File
