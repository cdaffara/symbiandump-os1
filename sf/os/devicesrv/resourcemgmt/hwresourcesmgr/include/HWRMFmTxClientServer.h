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
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMFMTXCLIENTSERVER_H
#define HWRMFMTXCLIENTSERVER_H

// INCLUDES
#include <hwrm/hwrmfmtxdata.h>

// CONSTANTS
const TInt KHWRMFmTxSubResourceCount(1);   


// MACROS
// None

// DATA TYPES

// Data packages for commands
typedef TPckgBuf<TRtPlusTag>   THWRMRtPlusTagCmdPackage;

// Opcodes used in message passing between client and server
// FM Tx opcodes start at 4000
enum THWRMFmTxServRequest
    {	
	/**
	* Enables FM Tx with specified frequency 
	* 
	* parameters:
	* 0: TInt - Tx frequency in kHz
	*/
	EHWRMFmTxOn = 4000,

	/**
	* Disables FM Tx immediately.
	* 
	* parameters:
	* None
	*/
	EHWRMFmTxOff,

	/**
	* Sets FM Tx frequency.
	* 
	* parameters:
	* 0: TInt - Tx frequency in kHz
	*/
	EHWRMFmTxSetFreq,

	/**
	* Sets Tx frequency to next suggested clear frequency
	* 
	* parameters:
	* 0: TInt - clear Tx frequency in kHz
	*/
	EHWRMFmTxSetNextClearFreq,
	
	/**
	* Enables FM Tx automatic frequency mode
	* 
	* parameters:
	* None
	*/	
	EHWRMFmTxAutoFreqOn,

	/**
	* Sets FM Tx RDS PTY
	* 
	* parameters:
	* 0: TFmTxRdsProgrammeType - PTY
	*/	
	EHWRMFmTxSetRdsPty,
	
	/**
	* Sets FM Tx RDS PTYN
	* 
	* parameters:
	* 0: TFmTxRdsProgrammeTypeName - PTYN
	*/	
	EHWRMFmTxSetRdsPtyn,
	
	/**
	* Sets FM Tx RDS MS
	* 
	* parameters:
	* 0: TBool - Music/Speech flag
	*/	
	EHWRMFmTxSetRdsMs,
	
	/**
	* Sets FM Tx RDS language ID
	* 
	* parameters:
	* 0: TRdsLanguageIdType - Language ID
	*/	
	EHWRMFmTxSetRdsLanguageId,
	
    /**
  	* Resets RDS data to defaults.
    *
	* parameters:
	* None
  	*/
    EHWRMFmTxClearRdsData,
    
	/**
	* Sets FM Tx RDS Radiotext
	* 
	* parameters:
	* 0: TDes8 - Radiotext
	*/	
	EHWRMFmTxSetRadiotext,
	
	/**
	* Sets FM Tx RDS Radiotext+
	* 
	* parameters:
	* 0: TDes8 - Radiotext
	* 1: TRtPlusContentType - Radiotext content type
	*/		
	EHWRMFmTxSetRadiotextPlus1,
	
	/**
	* Sets FM Tx RDS Radiotext+
	* 
	* parameters:
	* 0: TDes8 - Radiotext
	* 1: TRtPlusTag - Radiotext tag
	*/		
	EHWRMFmTxSetRadiotextPlus2,
	
	/**
	* Sets FM Tx RDS Radiotext+
	* 
	* parameters:
	* 0: TDes8 - Radiotext
	* 1: TRtPlusTag - Radiotext tag
	* 2: TRtPlusTag - Radiotext tag	
	*/		
	EHWRMFmTxSetRadiotextPlus3,  
    
    /**
  	* Clears Radiotext data
    *
	* parameters:
	* None
  	*/    
    EHWRMFmTxClearRtData,
	
	/**
  	* Reserves FM Tx.
  	*
  	* parameters:
  	* 0: TBool - ForceNoCCoeEnv flag
  	*/
  	EHWRMFmTxReserve,
  	
  	/**
  	* Releases FM Tx.
    *
	* parameters:
	* None
  	*/
    EHWRMFmTxRelease,
	
	/**
	* Get FM Tx frequency range
	*
	* parameters:
	* 0: TPckgBuf<TFmTxFrequencyRange> On return, the frequency range
	*/
	EHWRMFmTxGetFreqRange,

	/**
	* Gets next suggested clear frequency
	* 
	* parameters:
	* 0: TInt - clear Tx frequency in kHz
	*/
	EHWRMFmTxGetNextClearFreq,

	/**
	* Gets next suggested clear frequency
	* 
	* parameters:
	* 0: TClearFrequencyArray - fixed size array of 10 clear Tx frequencies in kHz
	*/
	EHWRMFmTxGetNextClearFreqArray,

	/**
	* Cancels previous EHWRMFmTxGetNextClearFreq or EHWRMFmTxGetNextClearFreqArray
	*/	
	EHWRMFmTxCancelGetNextClearFreq,
	
	/**
	* Cancels previous EHWRMFmTxSetNextClearFreq
	*/	
	EHWRMFmTxCancelSetNextClearFreq,   
	
    /**
	* Cleans up the FM Tx service so that session can be closed.
    *
   	* parameters:
   	* None
	*/
   	EHWRMFmTxCleanup,

   	/**
   	* Disables FM Tx automatic frequency mode
   	* 
   	* parameters:
   	* None
   	*/	
   	EHWRMFmTxAutoFreqOff,
    
    EHWRMEndOfFmTxOpCodes
    };


// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

#endif  // HWRMFMTXCLIENTSERVER_H
            
// End of File
