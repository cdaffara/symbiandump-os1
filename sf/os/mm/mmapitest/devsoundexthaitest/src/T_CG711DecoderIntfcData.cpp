/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include "t_cg711decoderintfcdata.h"

/*@{*/
//Enumeration Literals 
_LIT(KEDecALaw, 					"EDecALaw");
_LIT(KEDecULaw,						"EDecULaw" );
/*@}*/

/*@{*/
//Command literals 
_LIT(KCmdNewL, 						"NewL");
_LIT(KCmdDestructor, 					"~");
_LIT(KCmdSetDecoderMode, 				"SetDecoderMode");
_LIT(KCmdSetCng, 						"KCmdSetCng");
/*@}*/

/*@{*/
//INI Section name literals 
_LIT(KDecoder, 						"Decoder");
_LIT(KCng, 							"CNG");
/*@}*/

const CDataWrapperBase::TEnumEntryTable iEnumDecoders[] = 
	{ 
	{ KEDecALaw,	0/*EDecALaw */},
	{ KEDecULaw,	1/*EDecULaw*/ }
	};


/**
 * Private constructor. First phase construction
 */
CT_CG711DecoderIntfcData::CT_CG711DecoderIntfcData()
	:
	iG711Decoder(NULL),
	iCng(EFalse)
    {
    }

/**
 * Second phase construction
 *
 * @internalComponent
 * @return	N/A
 * @pre		None
 * @post	None
 * @leave	system wide error
 */
//void CT_CG711DecoderIntfcData::ConstructL()
//    {
//   
//    }
    
/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CG711DecoderIntfcData* CT_CG711DecoderIntfcData::NewL()
    {
    CT_CG711DecoderIntfcData* self = new (ELeave)CT_CG711DecoderIntfcData();
    return self;    
    }
    
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CG711DecoderIntfcData::GetObject()
    {
    return iG711Decoder;
    }

/**
 * Public destructor
 */
CT_CG711DecoderIntfcData::~CT_CG711DecoderIntfcData()
    {     
    DestroyData();
    }

/**
 * Helper method for DoCmdDestructor
 */
void CT_CG711DecoderIntfcData::DestroyData()
	{
    if (iG711Decoder != NULL)
    	{
    	delete iG711Decoder;
        iG711Decoder = NULL;
    	}	
	}

/**
 * Process a command read from the Ini file
 * @param aCommand 			- The command to process
 * @param aSection			- The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex	- Command index dor async calls to returns errors to
 * @return TBool			- ETrue if the command is process
 * @leave					- system wide error
 */
TBool CT_CG711DecoderIntfcData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	
	if (aCommand == KCmdNewL)
		{
		DoCmdNewL();
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else if (aCommand == KCmdSetDecoderMode)
		{
		DoCmdSetDecoderMode(aSection);
		}
	else if (aCommand == KCmdSetCng)
		{
		DoCmdSetCng(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret=EFalse;
		}
	
	return ret;
	}

/**
 * Create an Instance of CG711DecoderIntfc
 * @param none
 * @return none
 */
void CT_CG711DecoderIntfcData::DoCmdNewL()
	{
	INFO_PRINTF1(_L("*START*CT_CG711DecoderIntfcData::DoCmdNewL()"));

	TRAPD(error, CT_CG711DecoderIntfcData::NewL());
	if(KErrNone!=error)
		{
		ERR_PRINTF2(_L("> Could not create CG711HwDevice: %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF1(_L("*END*CT_CG711DecoderIntfcData::DoCmdNewL()"));
		}	
	}

/**
 * Destroy an instance of CG711DecoderIntfc
 * @param none
 * @return none
 */
void CT_CG711DecoderIntfcData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CG711DecoderIntfcData::DoCmdDestructor()"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CG711DecoderIntfcData::DoCmdDestructor()"));
	}

/**
 * Set decoder mode
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CG711DecoderIntfcData::DoCmdSetDecoderMode(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("*START* CT_CG711DecoderIntfcData::DoCmdSetDecoderMode()"));
    TInt expectedValue;
    if (!GetEnumFromConfig(aSection, KDecoder, iEnumDecoders, expectedValue))
    	{
		INFO_PRINTF2(_L("Parameter %S was not found in INI file."), &KDecoder);
		SetBlockResult(EFail);
    	}
    else
    	{
    	TInt error = iG711Decoder->SetDecoderMode((CG711DecoderIntfc::TDecodeMode)expectedValue);
        if ( KErrNone != error )    	
            {
            ERR_PRINTF2(_L("[%d] SetDecoderMode()"), error);        
            SetError(error);
            }
        else
        	{
        	INFO_PRINTF1(_L("*END*CT_CG711DecoderIntfcData::DoCmdSetDecoderMode()"));
        	}        
    	}    
    }

/** 
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CG711DecoderIntfcData::DoCmdSetCng(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("*START*CT_CG711DecoderIntfcData::DoCmdSetCng()"));
    
	if(!GetBoolFromConfig(aSection, KCng, iCng))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCng);
		SetBlockResult(EFail);
		return;
		}
	else
		{
		TInt error = iG711Decoder->SetCng(iCng);
	    if ( KErrNone != error )    	
	        {
	        ERR_PRINTF2(_L("[%d] SetCng() error "), error);
	        SetError(error);
	        return;
	        }
	    else
	    	{
	    	INFO_PRINTF1(_L("*END*CT_CG711DecoderIntfcData::DoCmdSetCng()"));    
	    	}	    
		}    
    }

