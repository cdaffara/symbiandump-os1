/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Interface proxy for WMA Decoder CI.
*
*/



// INCLUDE FILES
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>
#include "WmaDecoderIntfcMsgs.h"
#include "WmaDecoderIntfcProxy.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================= MEMBER FUNCTIONS ==============================

//------------------------------------------------------------------------------
// CWmaDecoderIntfcProxy::CWmaDecoderIntfcProxy
// C++ default constructor can NOT contain any code, that might leave.
//------------------------------------------------------------------------------
CWmaDecoderIntfcProxy::CWmaDecoderIntfcProxy(
                            TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility) :
    iCustomCommand(aCustomCommand),
    iMessageHandler(aMessageHandler),
    iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }

//------------------------------------------------------------------------------
// CWmaDecoderIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcProxy::ConstructL()
    {
    iToolOutPut32Bit = EFalse;
    iToolDownMixToStereo = ETrue;
    iToolLostDataConcealment = EFalse;
	iParametersSameAsAdaptation = EFalse;
    }

//------------------------------------------------------------------------------
// CWmaDecoderIntfcProxy::NewL
// Two-phased constructor.
//------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfcProxy* CWmaDecoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler,
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CWmaDecoderIntfcProxy* self = new(ELeave) CWmaDecoderIntfcProxy(
                                                   aMessageHandler,
                                                   aCustomCommand,
                                                   aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfcProxy::~CWmaDecoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
/*
    iSupportedFormats.Close();
    iSupportedTools.Close();
    iControllableTools.Close();
    iCurrentSupportedFormats.Close();
    iCurrentSupportedTools.Close();
    iCurrentControllableTools.Close(); 
*/
    }

// From CWmaDecoderIntfc
//------------------------------------------------------------------------------
// Configures format.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetFormat(TFormat aFormat)
    {
    iFormat = aFormat;
    }

//------------------------------------------------------------------------------
// Configures decoder's bits per sample.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetBitsPerSampleIn(TUint aBitsPerSample)
    {
    iBitsPerSample = aBitsPerSample;
    }

//------------------------------------------------------------------------------
// Configures decoder's number of channels.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetNumChannelsIn (TUint aNumChannelsIn)
    {
    iNumChannelsIn = aNumChannelsIn;
    }

//------------------------------------------------------------------------------
// Configures decoder's sample rate.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetSamplesPerSec (TUint aSamplesPerSec)
    {
    iSamplesPerSec = aSamplesPerSec;
    }

//------------------------------------------------------------------------------
// Configures decoder's average bytes per second.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetAvgBytesPerSec (TUint aAvgBytesPerSec)
    {
    iAvgBytesPerSec = aAvgBytesPerSec;
    }
    
//------------------------------------------------------------------------------
// Configures decoder's block align.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetBlockAlign (TUint aBlockAlign)
    {
    iBlockAlign = aBlockAlign;
    }

//------------------------------------------------------------------------------
// Configures decoder's Encode Options.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetEncodeOptions (TUint aEncodeOptions)
    {
    iEncodeOptions = aEncodeOptions;
    }
    
//------------------------------------------------------------------------------
// Configures decoder's Encode Options1.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetEncodeOptions1 (TUint aEncodeOptions1)
    {
    iEncodeOptions1 = aEncodeOptions1;
    }

//------------------------------------------------------------------------------
// Configures decoder's Encode Options2.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetEncodeOptions2 (TUint aEncodeOptions2)
    {
    iEncodeOptions2 = aEncodeOptions2;
    }
    
//------------------------------------------------------------------------------
//Configures decoder's Channel Mask.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::SetChannelMaskIn (TUint aChannelMask)
    {
    iChannelMask = aChannelMask;
    }

//------------------------------------------------------------------------------
// Returns decoder's current configured format.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetFormat (TFormat& aFormat)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		}
	if (status == KErrNone)
		{
	   	aFormat = iCurrentFormat;
   		status = iCurrentFormatStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}
    return status;
	}

//------------------------------------------------------------------------------
// Returns configured bits per sample .
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetBitsPerSampleIn(TUint& aBitsPerSample) 
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		}
	if (status == KErrNone)
		{
   	    aBitsPerSample = iCurrentBitsPerSample;
   	    status = iCurrentBitsPerSampleStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}        
    	
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured number of channels.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetNumOfChannelsIn (TUint& aNumOfChannels)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		aNumOfChannels = iCurrentNumChannelsIn;
   		status = iCurrentNumChannelsInStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}        
    	
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured sample rate.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetSamplesPerSec (TUint& aSamplesPerSec)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		aSamplesPerSec = iCurrentSamplesPerSec;
   		status = iCurrentSamplesPerSecStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}        
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured average bytes per second.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetAvgBytesPerSec (TUint& aAvgBytesPerSec)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		aAvgBytesPerSec = iCurrentAvgBytesPerSec;
   		status = iCurrentAvgBytesPerSecStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}        
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured block align.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetBlockAlign (TUint& aBlockAlign)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		aBlockAlign = iCurrentBlockAlign;
   		status = iCurrentBlockAlignStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}        
    	    	
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured encode options.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetEncodeOptions (TUint& aEncodeOpts)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		aEncodeOpts = iCurrentEncodeOptions;
   		status = iCurrentEncodeOptionsStatus;
		}
   	if (!ValidGetStatus(status))
        {
    	status = KErrGeneral;	
    	}        
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured advanced encode options.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetEncodeOptions1 (TUint& EncodeOpts1)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		EncodeOpts1 = iCurrentEncodeOptions1;
   		status = iCurrentEncodeOptions1Status;
		}
	if (!ValidGetStatus(status) &&
        status != KErrNotSupported)
        {
    	status = KErrGeneral;	
    	}        
    return status;
    }

//------------------------------------------------------------------------------
// Returns configured advanced encode options.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetEncodeOptions2 (TUint& EncodeOpts2)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		EncodeOpts2 = iCurrentEncodeOptions2;
   		status = iCurrentEncodeOptions2Status;
		}
	if (!ValidGetStatus(status) &&
        status != KErrNotSupported)
        {
    	status = KErrGeneral;	
    	}        
    return status;
    }
    
//------------------------------------------------------------------------------
// Returns configured channel mask.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetChannelMaskIn (TUint& aChannelMask)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	if (status == KErrNone)
		{
   		aChannelMask = iCurrentChannelMask;
   		status = iCurrentChannelMaskStatus;
		}
	if (!ValidGetStatus(status) &&
        status != KErrNotSupported)
        {
    	status = KErrGeneral;	
    	}        

    return status;
    }
    
//------------------------------------------------------------------------------
// Returns supported formats.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetSupportedFormats(RArray<TFormat>& aSupportedFormats)
    {

    TInt status = KErrNone;
    TRAPD(err, status = GetSupportedFormatsL(aSupportedFormats))
    if (err != KErrNone)
    	{
        if (err != KErrNone  && err != KErrNotFound)
        	{
    		status = KErrGeneral;	    		
    		}
    	}
    return status;
    }

//------------------------------------------------------------------------------
// Returns supported tools.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetSupportedTools(RArray<TTool>& aSupportedTools)
    {
    TInt status = KErrNone;
    TRAPD(err, status = GetSupportedToolsL(aSupportedTools))
    if (err != KErrNone)
    	{
        if (err != KErrNone  && err != KErrNotFound)
        	{
    		status = KErrGeneral;	    		
    		}
    	}

    return status;
    }

//------------------------------------------------------------------------------
// Returns supported max number of channels.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetSupportedMaxChannelsIn(TUint& aSupportedMaxChannelsIn)
    {
    TInt status = KErrNone;
    
	TPckgBuf<TUint> pckgBuf;
    status = iCustomCommand.CustomCommandSync(
                                               iMessageHandler, 
                                               EWmaGetSupportedMaxChannels, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               pckgBuf);
                                               
	aSupportedMaxChannelsIn = pckgBuf();	
    if (status != KErrNone  && status != KErrNotFound)
        {
    	status = KErrGeneral;	    		
    	}
    return status;
    }

//------------------------------------------------------------------------------
// Returns supported max bit rate.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetSupportedMaxBitrate(TUint& aSupportedMaxBitrate)
    {
    TInt status = KErrNone;

	TPckgBuf<TUint> pckgBuf;
    status = iCustomCommand.CustomCommandSync(
                                               iMessageHandler, 
                                               EWmaGetSupportedMaxBitrate, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               pckgBuf);
                                               
	aSupportedMaxBitrate = pckgBuf();	
	

    if (status != KErrNone  && status != KErrNotFound)
        {
    	status = KErrGeneral;	    		
    	}

    return status;
    }

//------------------------------------------------------------------------------
// Get Supported Max Sample Rate
//------------------------------------------------------------------------------
TInt CWmaDecoderIntfcProxy::GetSupportedMaxSampleRate(TUint& aSupportedMaxSampleRate)
	{
    TInt status = KErrNone;
    
	TPckgBuf<TUint> pckgBuf;
    status = iCustomCommand.CustomCommandSync(
                                               iMessageHandler, 
                                               EWmaGetSupportedMaxSampleRate, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               pckgBuf);
                                               
	aSupportedMaxSampleRate = pckgBuf();	
    if (status != KErrNone  && status != KErrNotFound)
        {
    	status = KErrGeneral;	    		
    	}

    return status;
	}

//------------------------------------------------------------------------------
// Returns controllable tools.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetControllableTools(RArray<TTool>& aControllableTools)
    {
    TInt status = KErrNone;
    TRAPD(err, status = GetControllableToolsL(aControllableTools))
    if (err != KErrNone)
    	{
        if (err != KErrNone  && err != KErrNotFound)
        	{
    		status = KErrGeneral;	    		
    		}
    	}

    return status;
    }

//------------------------------------------------------------------------------
// Enable tool.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::EnableTool(TTool aTool) 
    {
	switch (aTool)
		{
    	case EToolOutput32Bit:
    	     iToolOutPut32Bit = ETrue;
    	     break;
    	case EDownMixToStereo:
             iToolDownMixToStereo = ETrue;
             break;
    	case ELostDataConcealment:
    	     iToolLostDataConcealment = ETrue;
    	     break;
    	default:
    	     break;     			
		}

    }

//------------------------------------------------------------------------------
// disable tool.
//------------------------------------------------------------------------------
EXPORT_C void CWmaDecoderIntfcProxy::DisableTool(TTool aTool) 
    {
	switch (aTool)
		{
    	case EToolOutput32Bit:
    	     iToolOutPut32Bit = EFalse;
    	     break;
    	case EDownMixToStereo:
             iToolDownMixToStereo = EFalse;
             break;
    	case ELostDataConcealment:
    	     iToolLostDataConcealment = EFalse;
    	     break;
    	default:
    	     break;     			
		}
    }


//------------------------------------------------------------------------------
// Returns tool state.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::GetTool(TTool aTool, TBool& aEnabled)
    {
    TInt status = KErrNone;

	if (!iParametersSameAsAdaptation)
		{
		status = GetAllMessage();	
		} 
	switch (aTool)
		{
    	case EToolOutput32Bit:
    	     if (iCurrentToolOutPut32Bit)
    	     	{
    	     	aEnabled = ETrue;	
    	     	}
    	     else
    	     	{
     	     	aEnabled = EFalse;	   	     		
    	     	}
    	     status = iCurrentToolOutPut32BitStatus;
    	     break;
    	     
    	case EDownMixToStereo:
    	     if (iCurrentToolDownMixToStereo)
    	     	{
    	     	aEnabled = ETrue;	
    	     	}
    	     else
    	     	{
     	     	aEnabled = EFalse;	   	     		
    	     	}
    	     status = iCurrentToolDownMixToStereoStatus;
			 break;

    	case ELostDataConcealment:
    	     if (iCurrentToolLostDataConcealment)
    	     	{
    	     	aEnabled = ETrue;	
    	     	}
    	     else
    	     	{
     	     	aEnabled = EFalse;	   	     		
    	     	}
    	     status = iCurrentToolLostDataConcealmentStatus;
    	     break;
    	default:
    	     break;     			
		}

	if (!ValidGetStatus(status) &&
        status != KErrNotSupported)
        {
    	status = KErrGeneral;	
    	}        
    	
    return status;
 

    }

//------------------------------------------------------------------------------
// Applies configuration settings to the decoder.
//------------------------------------------------------------------------------
EXPORT_C TInt CWmaDecoderIntfcProxy::ApplyConfig()
    {
    TInt status = KErrNone;
    
    
       TWmaDecoderConfig configParams;        
       
        configParams.iFormat = iFormat;
        configParams.iBitsPerSample = iBitsPerSample;
        configParams.iNumChannelsIn = iNumChannelsIn;
        configParams.iSamplesPerSec = iSamplesPerSec;
        configParams.iAvgBytesPerSec = iAvgBytesPerSec;
    	configParams.iBlockAlign = iBlockAlign;
    	configParams.iEncodeOptions = iEncodeOptions;
    	configParams.iEncodeOptions1 = iEncodeOptions1;
     	configParams.iEncodeOptions2 = iEncodeOptions2;   	
    	configParams.iChannelMask = iChannelMask;
        configParams.iToolOutPut32Bit = iToolOutPut32Bit;
        configParams.iToolDownMixToStereo = iToolDownMixToStereo;
        configParams.iToolLostDataConcealment = iToolLostDataConcealment;


    	TPckgBuf<TWmaDecoderConfig> pckgBuf(configParams);
    	status = iCustomCommand.CustomCommandSync(iMessageHandler,
    	                                               EWmaApplyConfig,
    	                                               pckgBuf,
    	                                               KNullDesC8);
    	if (status == KErrNone)
    		{
			iCurrentFormat = iFormat;
        	iCurrentBitsPerSample = iBitsPerSample;
        	iCurrentNumChannelsIn = iNumChannelsIn;
        	iCurrentSamplesPerSec = iSamplesPerSec;
        	iCurrentAvgBytesPerSec = iAvgBytesPerSec;
        	iCurrentBlockAlign = iBlockAlign;
        	iCurrentEncodeOptions = iEncodeOptions;
			iCurrentEncodeOptions1 = iEncodeOptions1;
			iCurrentEncodeOptions2 = iEncodeOptions2;			
        	iCurrentChannelMask = iChannelMask;
            iCurrentToolOutPut32Bit = iToolOutPut32Bit;
            iCurrentToolDownMixToStereo = iToolDownMixToStereo;
            iCurrentToolLostDataConcealment = iToolLostDataConcealment;

  		    iCurrentFormatStatus = status;
          	iCurrentBitsPerSampleStatus = status;       
         	iCurrentNumChannelsInStatus = status ;        
         	iCurrentSamplesPerSecStatus = status;
         	iCurrentAvgBytesPerSecStatus = status ;
         	iCurrentBlockAlignStatus = status;
         	iCurrentEncodeOptionsStatus = status;
			iCurrentEncodeOptions1Status = status;
			iCurrentEncodeOptions2Status = status;		
        	iCurrentChannelMaskStatus = status;
        	iCurrentToolOutPut32BitStatus = status;
        	iCurrentToolDownMixToStereoStatus = status;
        	iCurrentToolLostDataConcealmentStatus = status;              

			iParametersSameAsAdaptation = ETrue;
       		}
        else
        	{
			iParametersSameAsAdaptation = EFalse;        		
        	} 
	return status;
    }

	    
//------------------------------------------------------------------------------
// Receive all configure paramemters
//------------------------------------------------------------------------------
TInt CWmaDecoderIntfcProxy::GetAllMessage()
	{
	TPckgBuf<TAllParams> pckg;
    TInt status = iCustomCommand.CustomCommandSync(
                                               iMessageHandler, 
                                               EWmaGetAllMsg, 
                                               KNullDesC8,
                                               KNullDesC8,pckg);
   	if (status == KErrNone)
   		{		
   		iCurrentFormat = pckg().iCurrentFormat;
		iCurrentFormatStatus = pckg().iCurrentFormatStatus;    	

    	iCurrentBitsPerSample = pckg().iCurrentBitsPerSample;
		iCurrentBitsPerSampleStatus =pckg().iCurrentBitsPerSampleStatus;    	

   		iCurrentNumChannelsIn = pckg().iCurrentNumChannelsIn;
		iCurrentNumChannelsInStatus = pckg().iCurrentNumChannelsInStatus;    	

    	iCurrentSamplesPerSec =	pckg().iCurrentSamplesPerSec ;
		iCurrentSamplesPerSecStatus = pckg().iCurrentSamplesPerSecStatus;    	

   		iCurrentAvgBytesPerSec = pckg().iCurrentAvgBytesPerSec;
		iCurrentAvgBytesPerSecStatus = pckg().iCurrentAvgBytesPerSecStatus;    	

   		iCurrentBlockAlign = pckg().iCurrentBlockAlign;
		iCurrentBlockAlignStatus = pckg().iCurrentBlockAlignStatus;    	

   		iCurrentEncodeOptions = pckg().iCurrentEncodeOptions;
		iCurrentEncodeOptionsStatus = pckg().iCurrentEncodeOptionsStatus ;    	

   		iCurrentEncodeOptions1 = pckg().iCurrentEncodeOptions1;
		iCurrentEncodeOptions1Status = pckg().iCurrentEncodeOptions1Status;    
		
   		iCurrentEncodeOptions2 =  pckg().iCurrentEncodeOptions2;
		iCurrentEncodeOptions2Status = pckg().iCurrentEncodeOptions2Status;    	
   	
   		iCurrentChannelMask = pckg().iCurrentChannelMask ;
		iCurrentChannelMaskStatus = pckg().iCurrentChannelMaskStatus;    	
   	
   		iCurrentToolOutPut32Bit = pckg().iCurrentToolOutPut32Bit;
		iCurrentToolOutPut32BitStatus = pckg().iCurrentToolOutPut32BitStatus;    	
   	
   		iCurrentToolDownMixToStereo = pckg().iCurrentToolDownMixToStereo;
		iCurrentToolDownMixToStereoStatus =  pckg().iCurrentToolDownMixToStereoStatus;    	
	   	
   		iCurrentToolLostDataConcealment = pckg().iCurrentToolLostDataConcealment;
		iCurrentToolLostDataConcealmentStatus = pckg().iCurrentToolLostDataConcealmentStatus ;
		iParametersSameAsAdaptation = ETrue;    	
	}
	return status;
}

//------------------------------------------------------------------------------
// check return status
//------------------------------------------------------------------------------
TBool CWmaDecoderIntfcProxy::ValidGetStatus(TInt aStatus)
	{
    if (aStatus != KErrNone  &&  aStatus != KErrCompletion &&
        aStatus != KErrUnknown &&  aStatus != KErrArgument &&
        aStatus != KErrNotReady && aStatus != KErrNotFound &&
        aStatus != KErrGeneral)
    	{
    	return EFalse;	
    	}
    else
    	{
    	return ETrue;
    	}
	}


//------------------------------------------------------------------------------
// GetSupportedFormatsL
//------------------------------------------------------------------------------
TInt CWmaDecoderIntfcProxy::GetSupportedFormatsL(RArray<TFormat>& aSupportedFormats)
	{
    TInt status = KErrNone;

	aSupportedFormats.Reset();

	TPckgBuf<TUint> supportedFormatsPkg;
	status = iCustomCommand.CustomCommandSync(
	                                           iMessageHandler, 
                                               EWmaGetNumOfSupportedFormats, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               supportedFormatsPkg);
    HBufC8* buf = HBufC8::NewLC(supportedFormatsPkg()*sizeof(TUint));
   	// buf is left on cleanup 
   	TPtr8 ptr = buf->Des();
    iCustomCommand.CustomCommandSync(
                                    iMessageHandler, 
                                    EWmaGetSupportedFormats, 
                                    KNullDesC8,
                                    KNullDesC8,
                                    ptr);
  	RDesReadStream stream(ptr);
    CleanupClosePushL(stream); // stream on cleanup
   	for (TInt i=0; i<supportedFormatsPkg(); i++)
    	{
   		aSupportedFormats.AppendL(static_cast<TFormat>(stream.ReadUint32L()));
   		}
   	CleanupStack::PopAndDestroy(&stream);
   	CleanupStack::PopAndDestroy(buf);    			
    if (status != KErrNone  && status != KErrNotFound)
        {
    	status = KErrGeneral;	    		
    	}

    return status;		
	}

//------------------------------------------------------------------------------
// GetSupportedToolsL
//------------------------------------------------------------------------------
TInt CWmaDecoderIntfcProxy::GetSupportedToolsL(RArray<TTool>& aSupportedTools)
	{
	
	TInt status = KErrNone;
	aSupportedTools.Reset();

	TPckgBuf<TUint> supportedToolsPkg;
	status = iCustomCommand.CustomCommandSync(
	                                           iMessageHandler, 
                                               EWmaGetNumOfSupportedTools, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               supportedToolsPkg);
                                               
   	HBufC8* buf = HBufC8::NewLC(supportedToolsPkg()*sizeof(TUint));
   	// buf is left on cleanup 
   	TPtr8 ptr = buf->Des();

    iCustomCommand.CustomCommandSync(
                                    iMessageHandler, 
                                    EWmaGetSupportedTools, 
                                    KNullDesC8,
                                    KNullDesC8,
                                    ptr);
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream); // stream on cleanup
	for (TInt i=0; i<supportedToolsPkg(); i++)
		{
		aSupportedTools.AppendL(static_cast<TTool>(stream.ReadUint32L()));
		}
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
   		
   
   if (status != KErrNone  && status != KErrNotFound)
        {
    	status = KErrGeneral;	    		
    	}
    	
   return status;		
	}
       
//------------------------------------------------------------------------------
// GetControllableToolsL
//------------------------------------------------------------------------------
TInt CWmaDecoderIntfcProxy::GetControllableToolsL(RArray<TTool>& aControllableTools)
	{
	TInt status = KErrNone;
	aControllableTools.Reset();

	TPckgBuf<TUint> numPckg;
	status = iCustomCommand.CustomCommandSync(
	                                           iMessageHandler, 
                                               EWmaGetNumOfControllableTools, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               numPckg);
 
    HBufC8* buf = HBufC8::NewLC(numPckg()*sizeof(TUint));
    // buf is left on cleanup 
    TPtr8 ptr = buf->Des();

    iCustomCommand.CustomCommandSync(
                                               iMessageHandler, 
                                               EWmaGetControllableTools, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               ptr);
   	RDesReadStream stream(ptr);
    CleanupClosePushL(stream); // stream on cleanup
   	for (TInt i=0; i < numPckg(); i++)
    	{
   		aControllableTools.AppendL(static_cast<TTool>(stream.ReadUint32L()));
   		}
   	CleanupStack::PopAndDestroy(&stream);
   	CleanupStack::PopAndDestroy(buf);
    if (status != KErrNone  && status != KErrNotFound)
        {
    	status = KErrGeneral;	    		
    	}

    return status;    	
	}



// End of File
