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


#include "T_CRestrictedAudioOutputData.h"

/*@{*/
//Command literals 
_LIT(KCmdNewL, 									"NewL");
_LIT(KCmdDestructor, 								"~");
_LIT(KCmdAppendAllowedOutput, 						"AppendAllowedOutput");
_LIT(KCmdRemoveAllowedOutput, 						"RemoveAllowedOutput");
_LIT(KCmdGetAllowedOutput, 						"GetAllowedOutput");
_LIT(KCmdGetAllowedOutputCount, 					"GetAllowedOutputCount");
_LIT(KCmdReset, 									"Reset");
_LIT(KCmdCommit, 									"Commit");
/*@}*/

/*@{*/
//INI Section name literals 
_LIT(KOutput, 									"Output");
_LIT(KIndex, 									"Index");
_LIT(KDevSoundInstanceName, 					"DevSoundInstanceName");
/*@}*/

/*@{*/
//Enumeration Literals 
_LIT(KEAllowAllAudio, 							"EAllowAllAudio");
_LIT(KEAllowAudioAnalog, 						"EAllowAudioAnalog");
_LIT(KEAllowAudioFMTransmitter, 				"EAllowAudioFMTransmitter");
_LIT(KEAllowAudioBtHFPHSP, 						"EAllowAudioBtHFPHSP");
_LIT(KEAllowAudioBtA2DP, 						"EAllowAudioBtA2DP");
_LIT(KEAllowAudioUplink, 						"EAllowAudioUplink");
_LIT(KEAllowAudioUsb, 							"EAllowAudioUsb");
_LIT(KEAllowAudioRecording, 					"EAllowAudioRecording");
_LIT(KEAllowVisualization, 						"EAllowVisualization");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CRestrictedAudioOutputData::iEnumAllowedOutputPreference[]=
	{
	{ KEAllowAllAudio, CRestrictedAudioOutput::EAllowAllAudio},
	{ KEAllowAudioAnalog, CRestrictedAudioOutput::EAllowAudioAnalog},
	{ KEAllowAudioFMTransmitter, CRestrictedAudioOutput::EAllowAudioFMTransmitter},
	{ KEAllowAudioBtHFPHSP, CRestrictedAudioOutput::EAllowAudioBtHFPHSP},
	{ KEAllowAudioBtA2DP, CRestrictedAudioOutput::EAllowAudioBtA2DP},
	{ KEAllowAudioUplink, CRestrictedAudioOutput::EAllowAudioUplink},
	{ KEAllowAudioUsb, CRestrictedAudioOutput::EAllowAudioUsb},
	{ KEAllowAudioRecording, CRestrictedAudioOutput::EAllowAudioRecording},
	{ KEAllowVisualization, CRestrictedAudioOutput::EAllowVisualization}
	};

/**
 * Public destructor
 */
CT_CRestrictedAudioOutputData::~CT_CRestrictedAudioOutputData()
	{
	DestroyData();
	}

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_CRestrictedAudioOutputData* CT_CRestrictedAudioOutputData::NewL()
	{
	CT_CRestrictedAudioOutputData* ret = new (ELeave) CT_CRestrictedAudioOutputData();
	return ret;
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CRestrictedAudioOutputData::GetObject()
	{
	return iRestrictedAudioOutput;
	}

/**
 * Helper destructor
 */
void CT_CRestrictedAudioOutputData::DestroyData()
	{		
	if (iRestrictedAudioOutput)
		{
		delete iRestrictedAudioOutput;
		iRestrictedAudioOutput = NULL;
		}
	}
/**
 * Process a command read from the Ini file
 * @param aCommand 			The command to process
 * @param aSection			The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex	Command index dor async calls to returns errors to
 * @return TBool			ETrue if the command is process
 * @leave					system wide error
 */
TBool CT_CRestrictedAudioOutputData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
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
	else if (aCommand == KCmdAppendAllowedOutput)
		{
		DoCmdAppendAllowedOutput(aSection);
		}
	else if (aCommand == KCmdRemoveAllowedOutput)
		{
		DoCmdRemoveAllowedOutput(aSection);
		}
	else if (aCommand == KCmdGetAllowedOutput)
		{
		DoCmdGetAllowedOutput(aSection);
		}
	else if (aCommand == KCmdGetAllowedOutputCount)
		{
		DoCmdGetAllowedOutputCount();
		}
	else if (aCommand == KCmdReset)
		{
		DoCmdReset();
		}
	else if (aCommand == KCmdCommit)
		{
		DoCmdCommit();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret=EFalse;
		}

	return ret;
	}

/**
 * Create a new instance of CRestrictedAudioOutput
 * @param aSection - Section to read from the ini file
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdNewL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdNewL()"));
	DestroyData();
	TPtrC devSoundInstanceName;
	if (!GetStringFromConfig(aSection, KDevSoundInstanceName, devSoundInstanceName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDevSoundInstanceName);
    	SetBlockResult(EFail);
		}
	else
		{
		CMMFDevSound* mMFDevSound = static_cast<CMMFDevSound*>(GetDataObjectL(devSoundInstanceName));		
		TInt error;
		if ( !iRestrictedAudioOutput )
	        {
	    	if ( mMFDevSound )
	    		{
	    		CleanupStack::PushL(mMFDevSound);
	            iRestrictedAudioOutput = (CRestrictedAudioOutput*)mMFDevSound->CustomInterface(KUidRestrictedAudioOutput);
	    	    if ( iRestrictedAudioOutput == NULL )
	    	        {
	    			ERR_PRINTF1(_L("Failed to create RestrictedAudioOutput"));
	    			error = KErrNotFound;
	    			SetError(error);
	    	        }
	    	    else
	    	    	{
	    	    	INFO_PRINTF1(_L("RestrictedAudioOutput created"));
		    		CleanupStack::Pop(mMFDevSound);
	    	    	}	    		
	    	    }
	    	else 
	    	    {
				ERR_PRINTF1(_L("DevSound not ready"));
				error = KErrNotReady;
				SetError(error);
	    	    }    	
	        }
		INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdNewL()"));
		}	
	}

/**
 * Delete an instance of CRestrictedAudioOutput
 * @param
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdDestructor()"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdDestructor()"));
	}

/**
 * Appends an allowed output to the existing list.
 * @param aSection - The section to read from the ini file
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdAppendAllowedOutput(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdAppendAllowedOutput()"));	
	TInt output;
	if ( !GetEnumFromConfig(aSection, KOutput, iEnumAllowedOutputPreference, output) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutput);
    	SetBlockResult(EFail);
		}
	else
		{
		TInt error;
		error = iRestrictedAudioOutput->AppendAllowedOutput((CRestrictedAudioOutput::TAllowedOutputPreference) output);
		
		if ( (error!=KErrNone) && (error!=KErrAlreadyExists) )
			{
			ERR_PRINTF2(_L("Error in appending allowed output. Error: %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF2(_L("Allowed output 0x%x appended"), (CRestrictedAudioOutput::TAllowedOutputPreference) output);
			INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdAppendAllowedOutput()"));
			}		
		}
	}

/**
 * Removes an allowed output from the existing list.
 * @param aSection - The section to read from the ini file
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdRemoveAllowedOutput(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdRemoveAllowedOutput()"));
    TInt output;
	if(!GetEnumFromConfig(aSection, KOutput, iEnumAllowedOutputPreference, output))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOutput);
    	SetBlockResult(EFail);
		}
	else
		{
		TInt error;
		error = iRestrictedAudioOutput->RemoveAllowedOutput((CRestrictedAudioOutput::TAllowedOutputPreference) output);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Error in removing allowed output. Error: %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF2(_L("Allowed output 0x%x removed"), (CRestrictedAudioOutput::TAllowedOutputPreference) output);
			INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdRemoveAllowedOutput()"));
			}	
		}
	}

/**
 * Gets a reference to an allowed output located at a specified position
 * within the list.
 * @param aSection - The section to read from the ini file
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdGetAllowedOutput(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdGetAllowedOutput()"));
	TInt index;
	if ( !GetIntFromConfig(aSection, KIndex, index) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KIndex);
    	SetBlockResult(EFail);
		}
	else
		{
		TInt error;
	    CRestrictedAudioOutput::TAllowedOutputPreference allowedOutput;
		error = iRestrictedAudioOutput->GetAllowedOutput(index, allowedOutput);
		if ( error != KErrNone ) 
			{
			ERR_PRINTF2(_L("Error in getting allowed output. Error: %d"), error);
			SetError(error);
			}
		else
			{
			switch (allowedOutput)	
				{
				case CRestrictedAudioOutput::EAllowAllAudio:
					INFO_PRINTF1(_L("AllowAllAudio"));
				break;
				case CRestrictedAudioOutput::EAllowAudioAnalog:
					INFO_PRINTF1(_L("AllowAudioAnalog"));
				break;
				case CRestrictedAudioOutput::EAllowAudioFMTransmitter:
					INFO_PRINTF1(_L("AllowAudioFMTransmitter"));
				break;
				case CRestrictedAudioOutput::EAllowAudioBtHFPHSP:
					INFO_PRINTF1(_L("AllowAudioBtHFPHSP"));
				break;
				case CRestrictedAudioOutput::EAllowAudioBtA2DP:
					INFO_PRINTF1(_L("AllowAudioBtA2DP"));
				break;
				case CRestrictedAudioOutput::EAllowAudioUplink:
					INFO_PRINTF1(_L("AllowAudioUplink"));
				break;
				case CRestrictedAudioOutput::EAllowAudioUsb:
					INFO_PRINTF1(_L("AllowAudioUsb"));
				break;
				case CRestrictedAudioOutput::EAllowAudioRecording:
					INFO_PRINTF1(_L("AllowAudioRecording"));
				break;
				case CRestrictedAudioOutput::EAllowVisualization:
					INFO_PRINTF1(_L("AllowVisualization"));
				break;
				default:
					INFO_PRINTF2(_L("Unknown output 0x%x"), allowedOutput);
				}
			INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdGetAllowedOutput()"));
			}		
		}	
	}
	

/**
 * Gets the number of allowed objects in the array. *
 * @param 
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdGetAllowedOutputCount()
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdGetAllowedOutputCount()"));
	TInt error, size;
	error = iRestrictedAudioOutput->GetAllowedOutputCount(size);
	if(error != KErrNone)
		{
		ERR_PRINTF2(_L("Error in getting allowed output count. Error: %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF2(_L("Allowed output count: %d"), size);	
		INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdGetAllowedOutputCount()"));
		}	
	}

/**
 * Empties the list, so that it is ready to be reused.
 *
 * @param
 * @return void
 */
void CT_CRestrictedAudioOutputData::DoCmdReset()
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdReset()"));
	TInt error;
    error = iRestrictedAudioOutput->Reset();
    if (error!=KErrNone)
        {
		ERR_PRINTF2(_L("Error in reset. Error: %d"), error);
		SetError(error);
        }
    else
    	{
    	INFO_PRINTF1(_L("Reset successfully"));
        INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdReset()"));
    	}    
	}


/**
 * Applies settings cached locally and commits.
 *
 * This function can only be called when the DevSound instance is in Idle OR
 * Stopped state.
 * @param
 * @return
 */

void CT_CRestrictedAudioOutputData::DoCmdCommit()
	{
	INFO_PRINTF1(_L("*START*CT_CRestrictedAudioOutputData::DoCmdCommit()"));
    TInt error;
    error = iRestrictedAudioOutput->Commit();
    if (error != KErrNone)
        {
		ERR_PRINTF2(_L("Error in Commit. Error: %d"), error);
		SetError(error);
        }
    else
    	{
    	INFO_PRINTF1(_L("Commit successfully"));
    	INFO_PRINTF1(_L("*END*CT_CRestrictedAudioOutputData::DoCmdCommit()"));
    	}
	}
