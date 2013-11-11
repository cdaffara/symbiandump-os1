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

#include "T_CAudioInputData.h"


/*@{*/
// Output options Id's
const TUid INPUT_DEFAULTMIC						= TUid::Uid(0x00);
const TUid INPUT_OUTPUTTOSPEAKER				= TUid::Uid(0x01);
const TUid INPUT_FMRADIO 						= TUid::Uid(0x02);
const TUid INPUT_VOICECALL 						= TUid::Uid(0x03);
const TUid INPUT_LINEIN  						= TUid::Uid(0x04);
/*@}*/


/*@{*/
//Section name literals 
_LIT(KDevSoundKey, 								"DevSoundInstanceName");
_LIT(KAudioInputPar, 							"AudioInput");
/*@}*/

/*@{*/
//Command literals 
_LIT(KCmdNewL, 									"NewL");
_LIT(KCmdDestructor, 							"~");
_LIT(KCmdAudioInput,							"AudioInput");
_LIT(KCmdSetAudioInput, 						"SetAudioInput");
/*@}*/



/*@{*/
// Output options literal
_LIT(KInput_DefaultMic, 						"Input_DefaultMic");
_LIT(KInput_OutputtoSpeaker,					"Input_OutputtoSpeaker");
_LIT(KInput_FMRadio,	 						"Input_FMRadio");
_LIT(KInput_VoiceCall,	 						"Input_VoiceCall");
_LIT(KInput_LineIn,	 							"Input_LineIn");
/*@}*/


const CDataWrapperBase::TEnumEntryTable CT_CAudioInputData::iEnumInputOptions[] = 
	{ 
	{ KInput_DefaultMic,		INPUT_DEFAULTMIC.iUid },	
	{ KInput_OutputtoSpeaker,	INPUT_OUTPUTTOSPEAKER.iUid },
	{ KInput_FMRadio,			INPUT_FMRADIO.iUid },	
	{ KInput_VoiceCall,			INPUT_VOICECALL.iUid },
	{ KInput_LineIn,			INPUT_LINEIN.iUid }
	};


/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CAudioInputData* CT_CAudioInputData::NewL()
	{
	CT_CAudioInputData* ret = new (ELeave) CT_CAudioInputData();
	return ret;
	}

/**
 * Private constructor. First phase construction
 */
CT_CAudioInputData::CT_CAudioInputData()
	:
	iAudioInput(NULL)
	{
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CAudioInputData::GetObject()
	{
	return iAudioInput;
	}
/**
 * Helper method for DoCmdDestructor
 */
void CT_CAudioInputData::DestroyData()
	{
	if (iAudioInput)
		{ 
		delete iAudioInput;
		iAudioInput = NULL;
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
TBool CT_CAudioInputData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;	
	if (aCommand == KCmdNewL)
		{
		DoCmdNewL(aSection);
		}
	else if (aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else if (aCommand == KCmdAudioInput)
		{
		DoCmdAudioInput();
		}
	else if (aCommand == KCmdSetAudioInput)
		{
		DoCmdSetAudioInputL(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret=EFalse;
		}
	return ret;
	}

/**
 * Create an instance of CAudioInput object
 * @param aSection - Section to read param from the ini file
 * @return none 
 */
void CT_CAudioInputData::DoCmdNewL(const TTEFSectionName& aSection)  
	{
	DestroyData();
	INFO_PRINTF1(_L("*START*CT_CAudioInputData::DoCmdNewL()"));
	TPtrC devSoundObject;
    if( !GetStringFromConfig(aSection, KDevSoundKey, devSoundObject) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDevSoundKey);
    	SetBlockResult(EFail);
    	}
    else
    	{
	    CMMFDevSound* DevSoundObject = static_cast<CMMFDevSound*>(GetDataObjectL(devSoundObject));
	    iAudioInput = (CAudioInput*)DevSoundObject->CustomInterface(KUidAudioInput);	
	    INFO_PRINTF1(_L("*END*CT_CAudioInputData::DoCmdNewL()"));
    	}
	}

/**
 * Destroy an instance of CAudioInput object
 * @param none
 * @param none
 */
void CT_CAudioInputData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CAudioInputData::DoCmdDestructor()"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CAudioInputData::DoCmdDestructor()"));
	}

/**
 * Get an array of Audio Input
 * @param none
 * @return none
 */
void CT_CAudioInputData::DoCmdAudioInput()
	{
	INFO_PRINTF1(_L("*START*CT_CAudioInputData::DoCmdAudioInput()"));
	CAudioInput::TAudioInputArray inputArray = iAudioInput->AudioInput();
	INFO_PRINTF1(_L("*END*CT_CAudioInputData::DoCmdAudioInput()"));
	}

/**
 * Set the audio input
 * @param aSection  - Section to read param from the ini file
 * @return none		
 */
void CT_CAudioInputData::DoCmdSetAudioInputL(const TTEFSectionName& aSection)
	{
	
	INFO_PRINTF1(_L("*START*CT_CAudioInputData::DoCmdSetAudioInputL()"));
    TInt parAudioInput;    
	if(!GetEnumFromConfig(aSection, KAudioInputPar, iEnumInputOptions, parAudioInput))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAudioInputPar);
    	SetBlockResult(EFail);    		
		}
	else
		{
		CArrayFixFlat<CAudioInput::TAudioInputPreference>* inputarray = new (ELeave) CArrayFixFlat<CAudioInput::TAudioInputPreference>(4);             
		CleanupStack::PushL(inputarray);
		inputarray->AppendL((CAudioInput::TAudioInputPreference)parAudioInput);    
		TRAPD( error, iAudioInput->SetAudioInputL( inputarray->Array() ) );
		if( error != KErrNone )
			{
			ERR_PRINTF2(_L("Setting audio input failed with error %d"), error);
	        SetError(error);
	        }
		else
			{
			CleanupStack::PopAndDestroy(&inputarray);
			INFO_PRINTF1(_L("*END*CT_CAudioInputData::DoCmdSetAudioInputL()"));
			}
		}
	}

