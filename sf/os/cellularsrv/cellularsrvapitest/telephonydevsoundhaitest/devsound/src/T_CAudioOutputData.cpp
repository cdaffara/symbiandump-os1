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

#include "T_CAudioOutputData.h"

/*@{*/
// Output options Id's
const TUid OUTPUT_NOPREFERENCE					= TUid::Uid(0x00);
const TUid OUTPUT_ALL 							= TUid::Uid(0x01);
const TUid OUTPUT_NOOUTPUT 						= TUid::Uid(0x02);
const TUid OUTPUT_PRIVATE 						= TUid::Uid(0x03);
const TUid OUTPUT_PUBLIC  						= TUid::Uid(0x04);
/*@}*/


/*@{*/
//Section name literals 
_LIT(KDevSoundKey, 								"DevSoundInstanceName");
_LIT(KAudioOutputPar, 							"AudioOutput");
_LIT(KSecureOutputPar,							"SecureOutput");
/*@}*/

/*@{*/
//Command literals 
_LIT(KCmdNewL, 									"NewL");
_LIT(KCmdDestructor, 							"~");
_LIT(KCmdAudioOutput,							"AudioOutput");
_LIT(KCmdDefaultAudioOutput, 					"DefaultAudioOutput");
_LIT(KCmdRegisterObserver, 						"RegisterObserver");
_LIT(KCmdSecureOutput, 							"SecureOutput");
_LIT(KCmdSetAudioOutput, 						"SetAudioOutput");
_LIT(KCmdSetSecureOutput, 						"SetSecureOutput");
_LIT(KCmdUnregisterObserver, 					"UnregisterObserver");
/*@}*/


/*@{*/
// Output options literal
_LIT(KOutput_NoPreference, 						"Output_NoPreference");
_LIT(KOutput_All, 								"Output_All");
_LIT(KOutput_NoOutput, 							"Output_NoOutput");
_LIT(KOutput_Private, 							"Output_Private");
_LIT(KOutput_Public, 							"Output_Public");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CAudioOutputData::iEnumOutputOptions[] = 
	{ 
	{ KOutput_NoPreference,		OUTPUT_NOPREFERENCE.iUid },	
	{ KOutput_All,				OUTPUT_ALL.iUid },
	{ KOutput_NoOutput,			OUTPUT_NOOUTPUT.iUid },	
	{ KOutput_Private,			OUTPUT_PRIVATE.iUid },
	{ KOutput_Public,			OUTPUT_PUBLIC.iUid }
	};



/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CAudioOutputData* CT_CAudioOutputData::NewL()
	{
	CT_CAudioOutputData* ret = new (ELeave) CT_CAudioOutputData();
	return ret;
	}

/**
 * Private constructor. First phase construction
 */
CT_CAudioOutputData::CT_CAudioOutputData()
	:
	iAudioOutput(NULL),
	iSecureOutput(EFalse)
	{
	}


/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CAudioOutputData::GetObject()
	{
	return iAudioOutput;
	}

/**
 * Helper method for DoCmdDestructor
 */
void CT_CAudioOutputData::DestroyData()
	{
	if (iAudioOutput)
		{ 
		delete iAudioOutput;
		iAudioOutput = NULL;
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
TBool CT_CAudioOutputData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
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
	else if (aCommand == KCmdAudioOutput)
		{
		DoCmdAudioOutput();
		}
	else if (aCommand == KCmdDefaultAudioOutput)
		{
		DoCmdDefaultAudioOutput();
		}
	else if (aCommand == KCmdRegisterObserver)
		{
		DoCmdRegisterObserverL();
		}
	else if (aCommand == KCmdSecureOutput)
		{
		DoCmdSecureOutput();
		}
	else if (aCommand == KCmdSetAudioOutput)
		{
		DoCmdSetAudioOutputL(aSection);
		}
	else if (aCommand == KCmdSetSecureOutput)
		{
		DoCmdSetSecureOutputL(aSection);
		}
	else if (aCommand == KCmdUnregisterObserver)
		{
		DoCmdUnregisterObserver();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command."));
		ret=EFalse;
		}
	return ret;
	}

/**
 * Create an instance of CAudioOutput
 * @param aSection  - Section to read param from the ini file
 * @return none
 */
void CT_CAudioOutputData::DoCmdNewL(const TTEFSectionName& aSection) //quite return
	{
	DestroyData();
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdNewL"));
	TPtrC devSoundObject;
    if( !GetStringFromConfig(aSection, KDevSoundKey, devSoundObject) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDevSoundKey);
    	SetBlockResult(EFail);
    	}
    else
    	{
	    CMMFDevSound* DevSoundObject = static_cast<CMMFDevSound*>(GetDataObjectL(devSoundObject));
	    iAudioOutput = (CAudioOutput*)DevSoundObject->CustomInterface(KUidAudioOutput);
	    INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdNewL"));
    	}
    }

/**
 * Destroy an instance of CAudioOutput
 * @param none
 * @return none
 */
void CT_CAudioOutputData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdDestroyData"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdDestroyData"));
	}

/**
 * Get the audio Output pref
 * @param none
 * @return none
 */
void CT_CAudioOutputData::DoCmdAudioOutput()
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdAudioOutput"));
	audioOutputPref = iAudioOutput->AudioOutput();
	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdAudioOutput"));
	}

/**
 * Get the default audio output pref
 * @param none
 * @return none
 */
void CT_CAudioOutputData::DoCmdDefaultAudioOutput()
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdDefaultAudioOutput"));
	audioOutputPref = iAudioOutput->DefaultAudioOutput();
	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdDefaultAudioOutput"));
	}

/**
 * Register the observer
 * @param none
 * @return none
 */
void CT_CAudioOutputData::DoCmdRegisterObserverL() //quite return
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdRegisterObserverL"));
    TRAPD( error, iAudioOutput->RegisterObserverL(*this) );    	    
    if( error != KErrNone ) 
        {
        ERR_PRINTF2(_L("Register observer failed with error %d"), error);
        SetError(error);
        }
    else
    	{
    	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdRegisterObserverL"));
    	}
    }

/**
 * Secure the output
 * @param none
 * @return none
 */
void CT_CAudioOutputData::DoCmdSecureOutput()
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdSecureOutput"));
	iSecureOutput = iAudioOutput->SecureOutput();
	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdSecureOutput"));
	}

/**
 * Set the audio Output
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CAudioOutputData::DoCmdSetAudioOutputL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdSetAudioOutputL"));
	TInt parAudioOutput;
	if(!GetEnumFromConfig(aSection, KAudioOutputPar, iEnumOutputOptions, parAudioOutput))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAudioOutputPar);
    	SetBlockResult(EFail); 		
		}
	else{
	    TRAPD(error, iAudioOutput->SetAudioOutputL((CAudioOutput::TAudioOutputPreference)parAudioOutput));
	    if( error != KErrNone )
	        {
	        ERR_PRINTF2(_L("Setting Audio output failed with error %d"), error);
	        SetError(error);
	        }    
	    else
	    	{
	    	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdSetAudioOutputL"));
	    	}
		}
	}

/**
 * Set the secure output
 * @param aSection - Section to read param from the ini file
 * @return none
 */
void CT_CAudioOutputData::DoCmdSetSecureOutputL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdSetSecureOutputL"));
    TBool parSecureOutput;    
	if(!GetBoolFromConfig(aSection, KSecureOutputPar, parSecureOutput))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSecureOutputPar);
    	SetBlockResult(EFail);
    	    		
		}
	else
		{
		TRAPD( error, iAudioOutput->SetSecureOutputL(parSecureOutput) );
	    if( error != KErrNone ) 
	        {
	        ERR_PRINTF2(_L("Setting secure output failed with error %d"), error);
	        SetError(error);
	        }
	    else
	    	{
	    	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdSetSecureOutputL"));
	    	}
		}
	}

/**
 * Unregister the observer
 * @param none
 * @return none
 */
void CT_CAudioOutputData::DoCmdUnregisterObserver() 
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DoCmdUnregisterObserver"));
	iAudioOutput->UnregisterObserver(*this);
	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DoCmdUnregisterObserver"));
	}

/**
 * Shows that the audio output was change
 * @param aAudioOutput
 * @param aNewDefault  - The new default value
 */
void CT_CAudioOutputData::DefaultAudioOutputChanged(CAudioOutput& /*aAudioOutput*/, CAudioOutput::TAudioOutputPreference aNewDefault)
	{
	INFO_PRINTF1(_L("*START*CT_CAudioOutputData::DefaultAudioOutputChanged"));
	INFO_PRINTF2(_L("Audio Output changed to %d"), aNewDefault);
	INFO_PRINTF1(_L("*END*CT_CAudioOutputData::DefaultAudioOutputChanged"));
	}
