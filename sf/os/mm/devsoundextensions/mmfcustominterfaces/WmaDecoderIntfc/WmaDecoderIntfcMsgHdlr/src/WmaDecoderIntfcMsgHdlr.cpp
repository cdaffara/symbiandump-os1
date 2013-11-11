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
* Description:   Message handler for WMA 10 Pro decoder custom interface.
*
*/



// INCLUDE FILES
#include    "WmaDecoderIntfcMsgs.h"
#include    "WmaDecoderIntfcMsgHdlr.h"
//#include "WmaDecoderIntfc.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::CWmaDecoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that might leave.
//------------------------------------------------------------------------------
CWmaDecoderIntfcMsgHdlr::CWmaDecoderIntfcMsgHdlr(
                              CWmaDecoderIntfc* aWmaDecoderConfigCI) :
	CMMFObject(KUidWmaDecoderIntfc)
    {
    iWmaDecoderIntfcCI = aWmaDecoderConfigCI;
    }

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::ConstructL()
    {
    }

//------------------------------------------------------------------------------
// WmaDecoderIntfcMsgHdlr::NewL
// Two-phased constructor.
//------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfcMsgHdlr* CWmaDecoderIntfcMsgHdlr::NewL(
                                       TAny* aWmaDecoderConfigCI)
    {
    CWmaDecoderIntfc* WmaDecoderConfigCI =
                (CWmaDecoderIntfc*)aWmaDecoderConfigCI;
    CWmaDecoderIntfcMsgHdlr* self =
                new (ELeave) CWmaDecoderIntfcMsgHdlr(WmaDecoderConfigCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfcMsgHdlr::~CWmaDecoderIntfcMsgHdlr()
	{
    iSupportedFormats.Close();
    iSupportedTools.Close();
    iControllableTools.Close();

	delete iDataCopyBuffer;
	delete iWmaDecoderIntfcCI;
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
//------------------------------------------------------------------------------ 
EXPORT_C void CWmaDecoderIntfcMsgHdlr::HandleRequest(
                                            TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidWmaDecoderIntfc);

	TRAPD(error, DoHandleRequestL(aMessage));

	if(error)
		{
		aMessage.Complete(error);
		}
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EWmaGetSupportedFormats:
			{
    		DoGetSupportedFormatsL(aMessage);
			break;
			}
		case EWmaGetSupportedTools:
			{
    		DoGetSupportedToolsL(aMessage);
			break;
			}
		case EWmaGetSupportedMaxChannels:
			{
    		DoGetSupportedMaxChannelsL(aMessage);
			break;
			}
		case EWmaGetSupportedMaxBitrate:
			{
    		DoGetSupportedMaxBitrateL(aMessage);
			break;
			}
		case EWmaGetSupportedMaxSampleRate:
			{
    		DoGetSupportedMaxSampleRateL(aMessage);
			break;
			}
		case EWmaGetControllableTools:
			{
    		DoGetControllableToolsL(aMessage);
			break;
			}
		case EWmaGetAllMsg:
			{
    		DoGetAllL(aMessage);
			break;
			}		
		case EWmaApplyConfig:
			{
    		DoApplyConfigL(aMessage);
			break;
			}
		case EWmaGetNumOfSupportedFormats:
 			{
    		DoGetNumOfSupportedFormatsL(aMessage);
			break;
			}
   		case EWmaGetNumOfSupportedTools:
			{
    		DoGetNumOfSupportedToolsL(aMessage);
			break;
			}		
		case EWmaGetNumOfControllableTools:
   			{
    		DoGetNumOfControllableToolsL(aMessage);
			break;
			}
 
		default:
			{
			aMessage.Complete(KErrNotSupported);
			break;
			}
		}
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetSupportedFormatsL
// Get supported formats.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoGetSupportedFormatsL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		{
		User::Leave(KErrNotReady);			
		}
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
    aMessage.Complete(KErrNone);
    }

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetSupportedToolsL
// Get supported tools.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoGetSupportedToolsL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		{
		User::Leave(KErrNotReady);			
		}
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
    aMessage.Complete(KErrNone);
	}
	
//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetSupportedMaxChannelsL
// Get max number of channels.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoGetSupportedMaxChannelsL(TMMFMessage& aMessage)
	{
	TUint maxChannels = 0;
	TInt status = iWmaDecoderIntfcCI->GetSupportedMaxChannelsIn(maxChannels);

    TPckgBuf<TUint> pckg;
    pckg() = maxChannels;
	aMessage.WriteDataToClientL(pckg);
    	
    aMessage.Complete(status);		
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetSupportedMaxBitrateL
// Get supported max bit rates.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoGetSupportedMaxBitrateL(TMMFMessage& aMessage)
	{
	TUint maxBitrate = 0;
	TInt status = iWmaDecoderIntfcCI->GetSupportedMaxBitrate(maxBitrate);

   	TPckgBuf<TUint> pckg;
   	pckg() = maxBitrate;
    aMessage.WriteDataToClientL(pckg);
   
    aMessage.Complete(status);			
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetSupportedMaxSampleRateL
// Get supported max sample rate.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoGetSupportedMaxSampleRateL(TMMFMessage& aMessage)
	{
	TUint maxSamplerate = 0;
	TInt status = iWmaDecoderIntfcCI->GetSupportedMaxSampleRate(maxSamplerate);

    TPckgBuf<TUint> pckg;
    pckg() = maxSamplerate;
	aMessage.WriteDataToClientL(pckg);
    
    aMessage.Complete(status);		
		
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetControllableToolsL
// Get supported controllable tools.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoGetControllableToolsL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		{
		User::Leave(KErrNotReady);			
		}
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
    aMessage.Complete(KErrNone);
	}

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetAllL
// Get all configure parameters.
//------------------------------------------------------------------------------	
void CWmaDecoderIntfcMsgHdlr::DoGetAllL(TMMFMessage& aMessage)
	{
   	TPckgBuf<TAllParams> pckg;
	CWmaDecoderIntfc::TFormat format;
	TInt status = iWmaDecoderIntfcCI->GetFormat(format);
   	pckg().iCurrentFormat = format;
	pckg().iCurrentFormatStatus = status;    	

    TUint bitsPerSample = 0;
    status = iWmaDecoderIntfcCI->GetBitsPerSampleIn(bitsPerSample);    	
    pckg().iCurrentBitsPerSample = bitsPerSample;
	pckg().iCurrentBitsPerSampleStatus = status;    	

	TUint numOfChannels = 0;
	status = iWmaDecoderIntfcCI->GetNumOfChannelsIn(numOfChannels);
   	pckg().iCurrentNumChannelsIn = numOfChannels;
	pckg().iCurrentNumChannelsInStatus = status;    	

	TUint samplesPerSec = 0;
	status = iWmaDecoderIntfcCI->GetSamplesPerSec (samplesPerSec);
   	pckg().iCurrentSamplesPerSec = samplesPerSec;
	pckg().iCurrentSamplesPerSecStatus = status;    	

    TUint avgBytesPerSec = 0;
    status = iWmaDecoderIntfcCI->GetAvgBytesPerSec (avgBytesPerSec);
   	pckg().iCurrentAvgBytesPerSec = avgBytesPerSec;
	pckg().iCurrentAvgBytesPerSecStatus = status;    	

	TUint blockAlign = 0;
	status = iWmaDecoderIntfcCI->GetBlockAlign (blockAlign) ;
   	pckg().iCurrentBlockAlign = blockAlign;
	pckg().iCurrentBlockAlignStatus = status;    	

	TUint encodeOpts = 0;
	status = iWmaDecoderIntfcCI->GetEncodeOptions (encodeOpts);
   	pckg().iCurrentEncodeOptions = encodeOpts;
	pckg().iCurrentEncodeOptionsStatus = status;    	

	TUint encodeOpts1 = 0;
	status = iWmaDecoderIntfcCI->GetEncodeOptions1 (encodeOpts1);
   	pckg().iCurrentEncodeOptions1 = encodeOpts1;
	pckg().iCurrentEncodeOptions1Status = status;    
		
	TUint encodeOpts2 = 0;
	status = iWmaDecoderIntfcCI->GetEncodeOptions2(encodeOpts2);
   	pckg().iCurrentEncodeOptions2 = encodeOpts2;
	pckg().iCurrentEncodeOptions2Status = status;    	
   	
	TUint channelMask = 0;
	status = iWmaDecoderIntfcCI->GetChannelMaskIn (channelMask);
   	pckg().iCurrentChannelMask = channelMask;
	pckg().iCurrentChannelMaskStatus = status;    	
   	
	CWmaDecoderIntfc::TTool tool = CWmaDecoderIntfc::EToolOutput32Bit;
	TBool able = EFalse;
	status = iWmaDecoderIntfcCI->GetTool(tool, able);
   	pckg().iCurrentToolOutPut32Bit = able;
	pckg().iCurrentToolOutPut32BitStatus = status;    	
   	
	tool = CWmaDecoderIntfc::EDownMixToStereo;
	status = iWmaDecoderIntfcCI->GetTool(tool, able);
   	pckg().iCurrentToolDownMixToStereo = able;
	pckg().iCurrentToolDownMixToStereoStatus = status;    	
	   	
	tool = CWmaDecoderIntfc::ELostDataConcealment;
	status = iWmaDecoderIntfcCI->GetTool(tool, able);
   	pckg().iCurrentToolLostDataConcealment = able;
	pckg().iCurrentToolLostDataConcealmentStatus = status;    	
 
    aMessage.WriteDataToClientL(pckg);
    
    aMessage.Complete(KErrNone);
	}


//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoApplyConfigL
// Handles the message from the proxy and calls the custom interface method
// to commit configuration settings to the decoder.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::DoApplyConfigL(TMMFMessage& aMessage)
    {
    TInt status = KErrNone;
	TPckgBuf<TWmaDecoderConfig> pckgBuf;
	
    aMessage.ReadData1FromClientL(pckgBuf);
	
	iWmaDecoderIntfcCI->SetFormat(pckgBuf().iFormat);
	iWmaDecoderIntfcCI->SetBitsPerSampleIn(pckgBuf().iBitsPerSample);
	iWmaDecoderIntfcCI->SetNumChannelsIn(pckgBuf().iNumChannelsIn);
	iWmaDecoderIntfcCI->SetSamplesPerSec(pckgBuf().iSamplesPerSec);
	iWmaDecoderIntfcCI->SetAvgBytesPerSec(pckgBuf().iAvgBytesPerSec);
	iWmaDecoderIntfcCI->SetBlockAlign(pckgBuf().iBlockAlign);
	iWmaDecoderIntfcCI->SetEncodeOptions(pckgBuf().iEncodeOptions);
	iWmaDecoderIntfcCI->SetEncodeOptions1(pckgBuf().iEncodeOptions1);
	iWmaDecoderIntfcCI->SetEncodeOptions2(pckgBuf().iEncodeOptions2);				
	iWmaDecoderIntfcCI->SetChannelMaskIn(pckgBuf().iChannelMask);

    TBool able = pckgBuf().iToolOutPut32Bit;
    if (able)
    	{
    	iWmaDecoderIntfcCI->EnableTool(CWmaDecoderIntfc::EToolOutput32Bit);	
    	}
    else
    	{
    	iWmaDecoderIntfcCI->DisableTool(CWmaDecoderIntfc::EToolOutput32Bit);	
    	}

    able = pckgBuf().iToolDownMixToStereo;
    if (able)
    	{
    	iWmaDecoderIntfcCI->EnableTool(CWmaDecoderIntfc::EDownMixToStereo);	
    	}
    else
    	{
    	iWmaDecoderIntfcCI->DisableTool(CWmaDecoderIntfc::EDownMixToStereo);	
    	}

    able = pckgBuf().iToolLostDataConcealment;
    if (able)
    	{
    	iWmaDecoderIntfcCI->EnableTool(CWmaDecoderIntfc::ELostDataConcealment);	
    	}
    else
    	{
    	iWmaDecoderIntfcCI->DisableTool(CWmaDecoderIntfc::ELostDataConcealment);	
    	}

	status = iWmaDecoderIntfcCI->ApplyConfig();
    aMessage.Complete(status);
    }

//------------------------------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::CreateBufFromUintArrayL
// Utility function used to create a buffer and fill it with data from the
// array passed in.
//------------------------------------------------------------------------------
void CWmaDecoderIntfcMsgHdlr::CreateBufFromUintArrayL(RArray<TUint>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;
	iDataCopyBuffer = CBufFlat::NewL(8);

	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);

	for (TInt i = 0; i < aArray.Count(); i++)
	    {	    	    
		stream.WriteUint32L(aArray[i]);
	    }
	    
	CleanupStack::PopAndDestroy(&stream);
	}	

// ---------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetNumOfSupportedFormatsL
//
// ---------------------------------------------------------
//
void CWmaDecoderIntfcMsgHdlr::DoGetNumOfSupportedFormatsL(TMMFMessage& aMessage)
	{
	iSupportedFormats.Reset();
	TInt status = iWmaDecoderIntfcCI->GetSupportedFormats(iSupportedFormats);

	CreateBufFromUintArrayL(reinterpret_cast<RArray<TUint>&>(iSupportedFormats));
    TPckgBuf<TUint> pckg;
   	pckg() = iSupportedFormats.Count();
    aMessage.WriteDataToClientL(pckg);
	
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetNumOfSupportedToolsL(
// ---------------------------------------------------------
//
void CWmaDecoderIntfcMsgHdlr::DoGetNumOfSupportedToolsL(TMMFMessage& aMessage)
	{
	iSupportedTools.Reset();
	TInt status = iWmaDecoderIntfcCI->GetSupportedTools(iSupportedTools);
	CreateBufFromUintArrayL(reinterpret_cast<RArray<TUint>&>(iSupportedTools));

	TPckgBuf<TUint> pckg;
    pckg() = iSupportedTools.Count();
	aMessage.WriteDataToClientL(pckg);
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CWmaDecoderIntfcMsgHdlr::DoGetNumOfControllableToolsL
// ---------------------------------------------------------
//
void CWmaDecoderIntfcMsgHdlr::DoGetNumOfControllableToolsL(TMMFMessage& aMessage)
	{
	iControllableTools.Reset();
	TInt status = iWmaDecoderIntfcCI->GetControllableTools(iControllableTools);
	CreateBufFromUintArrayL(reinterpret_cast<RArray<TUint>&>(iControllableTools));
	TPckgBuf<TUint> pckg;
    pckg() = iControllableTools.Count();
	aMessage.WriteDataToClientL(pckg); 
    aMessage.Complete(status);
	}



// End of File
