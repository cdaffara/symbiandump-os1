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


#include "T_CTelephonyAudioRoutingData.h"

const TUid EEarPiece		 				= TUid::Uid(0x02);
const TUid EHeadSet			 				= TUid::Uid(0x04);
/*@{*/
//Command literals 
_LIT(KCmdNewL, 								"NewL");
_LIT(KCmdDestructor, 							"~");
_LIT(KCmdAvailableOutputs, 					"AvailableOutputs");
_LIT(KCmdOutput, 								"Output");
_LIT(KCmdPreviousOutput, 						"PreviousOutput");
_LIT(KCmdSetShowNote, 							"SetShowNote");
_LIT(KCmdGetShowNote,							"GetShowNote");
_LIT(KCmdSetOutputL, 							"SetOutputL");
/*@}*/

/*@{*/
//INI Section name literals 
_LIT(KMode, 								"Mode");
/*@}*/

/*@{*/
//Enumeration Literals 
_LIT(KENotActive, 							"ENotActive");
_LIT(KENone, 								"ENone");
_LIT(KEHandset, 							"EHandSet");
_LIT(KELoudSpeaker, 						"ELoudSpeaker");
_LIT(KEWiredAudioAccessory, 				"EWiredAudioAccessory");
_LIT(KEBTAudioAccessory, 					"EBTAudioAccessory");
_LIT(KETTY, 								"ETTY");
_LIT(KEEarPiece,							"EEarPiece");
_LIT(KEHeadSet,								"EHeadSet");
/*@}*/

const CDataWrapperBase::TEnumEntryTable CT_CTelephonyAudioRoutingData::iEnumOutput[] = 
	{ 
	{ KENotActive,	CTelephonyAudioRouting::ENotActive},
	{ KENone, CTelephonyAudioRouting::ENone},
	{ KEHandset, CTelephonyAudioRouting::EHandset},
	{ KEEarPiece, EEarPiece.iUid},
	{ KELoudSpeaker, CTelephonyAudioRouting::ELoudspeaker},
	{ KEWiredAudioAccessory, CTelephonyAudioRouting::EWiredAudioAccessory},
	{ KEHeadSet,	EHeadSet.iUid},
	{ KEBTAudioAccessory, CTelephonyAudioRouting::EBTAudioAccessory},
	{ KETTY, CTelephonyAudioRouting::ETTY}
	};

/**
 * Public destructor
 */
CT_CTelephonyAudioRoutingData::~CT_CTelephonyAudioRoutingData()
	{
	DestroyData();
	}

/**
 * Two phase constructor
 * @leave	system wide error
 */

CT_CTelephonyAudioRoutingData* CT_CTelephonyAudioRoutingData::NewL()
	{
	CT_CTelephonyAudioRoutingData* ret = new (ELeave) CT_CTelephonyAudioRoutingData();
	return ret;
	}

/**
 * Private constructor. First phase construction
 */

CT_CTelephonyAudioRoutingData::CT_CTelephonyAudioRoutingData()
	:
	iTelephonyAudioRouting (NULL)
	{
	}


void CT_CTelephonyAudioRoutingData::DestroyData()
{
if(iTelephonyAudioRouting)
	{
	delete iTelephonyAudioRouting;
	iTelephonyAudioRouting = NULL;
	}
}


/**
 * Return a pointer to the object that the data wraps
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CTelephonyAudioRoutingData::GetObject()
	{
	return iTelephonyAudioRouting;
	}


/**
 * Process a command read from the Ini file
 * @param aCommand 			The command to process
 * @param aSection			The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex	Command index dor async calls to returns errors to
 * @return TBool			ETrue if the command is process
 * @leave					system wide error
 */

TBool CT_CTelephonyAudioRoutingData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
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
	else if (aCommand == KCmdAvailableOutputs)
		{
		DoCmdAvailableOutputs();
		}
	else if (aCommand == KCmdOutput)
		{
		DoCmdOutput();
		}
	else if (aCommand == KCmdPreviousOutput)
		{
		DoCmdPreviousOutput();
		}
	else if (aCommand == KCmdSetShowNote)
		{
		DoCmdSetShowNote(aSection);
		}
	else if (aCommand == KCmdGetShowNote)
		{
		DoCmdGetShowNote();
		}
	else if (aCommand == KCmdSetOutputL)
		{
		DoCmdSetOutputL(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret=EFalse;
		}
	
	return ret;	
	}

/**
 * Signals that available audio outputs have changed. 
 * @param CTelephonyAudioRouting& aTelephonyAudioRouting reference.
 * @return void
 */
void CT_CTelephonyAudioRoutingData::AvailableOutputsChanged( CTelephonyAudioRouting& /*aTelephonyAudioRouting*/)
	{
	INFO_PRINTF1(_L("CT_CTelephonyAudioRoutingData::AvailableOutputsChanged"));
	}

/**
 * Signals that current output has changed. 
 * @param CTelephonyAudioRouting& aTelephonyAudioRouting reference.
 * @return void
 */
void CT_CTelephonyAudioRoutingData::OutputChanged( CTelephonyAudioRouting& /*aTelephonyAudioRouting*/)
	{
	INFO_PRINTF1(_L("CT_CTelephonyAudioRoutingData::OutputChanged"));
	}

/**
 * Signals that CTelephonyAudioRouting::SetOutputL request issued by this
 * application (on CTelephonyAudioRouting instance) was serviced by the
 * Adaptation.
 * Only when the observer receives this callback, the application should consider
 * CTelephonyAudioRouting::SetOutputL message to be completed.
 *
 * @param CTelephonyAudioRouting& aTelephonyAudioRouting reference.
 * @param aError KErrNone if output requested by application is set, else
 *        system-wide error code.
 * @return void
 */
void CT_CTelephonyAudioRoutingData::SetOutputComplete( CTelephonyAudioRouting& /*aTelephonyAudioRouting*/, TInt aError)
	{
	DecOutstanding();
	if(aError != KErrNone)
		{
		ERR_PRINTF2(_L("Could not set output complete: %d"), aError);
		}
	else
		{
		INFO_PRINTF1(_L("CT_CTelephonyAudioRoutingData::SetOutputComplete"));
		}
	}

/**
 * Create an instance of CTelephonyAudioRouting
 * @param 
 * @return
 */
void CT_CTelephonyAudioRoutingData::DoCmdNewL()
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdNewL"));
	DestroyData();
	TRAPD(error,iTelephonyAudioRouting = CTelephonyAudioRouting::NewL(*this));
	if(error!=KErrNone)
		{
		ERR_PRINTF2(_L("> Could not create CTelephonyAudioRouting: %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF1(_L("CTelephonyAudioRouting created successfully."));
		INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdNewL"));
		}	
	}

/**
 * Delete the instance of CTelephonyAudioRouting
 * @param
 * @return
 */
void CT_CTelephonyAudioRoutingData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdDestructor"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CTelephonyAudioRouting destroyed successfully."));
	}


/**
* Shows the array of currently available outputs.
* The array contains list of following outputs depending on whether the
* output is available or not at the time of query:
*  - EHandset
*  - ELoudspeaker
*  - EWiredAudioAccessory,
*  - EWiredAccessory,
*  - EBTAudioAccessory
*  - ETTY
* Audio can be routed to any of the available outputs in the list.
* @param 
* @return 
*/

void CT_CTelephonyAudioRoutingData::DoCmdAvailableOutputs()
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdAvailableOutputs"));	
	TInt availableOutput;
	const TArray<CTelephonyAudioRouting::TAudioOutput>& availableOutputsArray(iTelephonyAudioRouting->AvailableOutputs());
	
	if (availableOutputsArray.Count()==0)
		{
		INFO_PRINTF1(_L("None available output"));
		}
	else
		{
		TBuf<80> msg;
		for (TInt i=0;i<availableOutputsArray.Count();i++)
			{
			availableOutput = availableOutputsArray[i];
			switch (availableOutput)
				{
				case CTelephonyAudioRouting::ENotActive:
					msg.Append(_L("not active"));
				break;
				case CTelephonyAudioRouting::ENone:
					msg.Append(_L("none"));
				break;
				case CTelephonyAudioRouting::EHandset:
					msg.Append(_L("handset"));
				break;
				case CTelephonyAudioRouting::ELoudspeaker:
					msg.Append(_L("loudspeaker"));
				break;
				case CTelephonyAudioRouting::EWiredAudioAccessory:
					msg.Append(_L("wired acc"));
				break;
				case CTelephonyAudioRouting::EBTAudioAccessory:
					msg.Append(_L("BT audio acc"));
				break;
				case CTelephonyAudioRouting::ETTY:
					msg.Append(_L("ETTY"));
				break;
				default:
					msg.AppendFormat(_L("0x%x"), availableOutput);
				}
			if(i+1<availableOutputsArray.Count())
				{
				msg.Append(_L(","));
				}
			}
			INFO_PRINTF2(_L("Available Outputs: %S"), &msg);
		}
	INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdAvailableOutputs"));
	}


/**
 * Shows the current output configured by the client application to play
 * telephony audio.
 * @param none.
 * @return void.
 */

void CT_CTelephonyAudioRoutingData::DoCmdOutput()
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdOutput"));
	iOutput = iTelephonyAudioRouting->Output();
    INFO_PRINTF2(_L("Output: 0x%x"),iOutput);
    switch(iOutput)
        {
        case CTelephonyAudioRouting::ENotActive :
        	INFO_PRINTF1(_L("Not active"));
        break;
        case CTelephonyAudioRouting::ENone :
        	INFO_PRINTF1(_L("None"));
        break;
        case CTelephonyAudioRouting::EHandset :
        	INFO_PRINTF1(_L("Handset"));
        break;
        case CTelephonyAudioRouting::ELoudspeaker :
        	INFO_PRINTF1(_L("Loudspeaker"));
        break;
        case CTelephonyAudioRouting::EWiredAudioAccessory :
        	INFO_PRINTF1(_L("Wired acc"));
        break;
        case CTelephonyAudioRouting::EBTAudioAccessory :
        	INFO_PRINTF1(_L("BT audio acc"));
        break;
        case CTelephonyAudioRouting::ETTY :
        	INFO_PRINTF1(_L("ETTY"));
        break;
        default:
        	INFO_PRINTF2(_L("0x%x"), iOutput);
        }
    INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdOutput"));
	}


/**
 * Show the second last output that was successfully configured to
 * play telephony audio.
 * @param none.
 * @return void
 */

void CT_CTelephonyAudioRoutingData::DoCmdPreviousOutput()
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdPreviousOutput"));	
    CTelephonyAudioRouting::TAudioOutput previousOutput;
	previousOutput = iTelephonyAudioRouting->PreviousOutput();
	INFO_PRINTF2(_L("Previous audio output: 0x%x"), previousOutput);
    switch (previousOutput)
        {
        case CTelephonyAudioRouting::ENotActive:
        	INFO_PRINTF1(_L("Not active"));
        break;
        case CTelephonyAudioRouting::ENone:
        	INFO_PRINTF1(_L("None"));
        break;
        case CTelephonyAudioRouting::EHandset:
        	INFO_PRINTF1(_L("Handset"));
        break;
        case CTelephonyAudioRouting::ELoudspeaker:
        	INFO_PRINTF1(_L("Loudspeaker"));
        break;
        case CTelephonyAudioRouting::EWiredAudioAccessory:
        	INFO_PRINTF1(_L("Wired acc"));
        break;
        case CTelephonyAudioRouting::EBTAudioAccessory:
        	INFO_PRINTF1(_L("BT audio acc"));
        break;
        case CTelephonyAudioRouting::ETTY:
        	INFO_PRINTF1(_L("ETTY"));
        break;
        default:
        	INFO_PRINTF2(_L("0x%x"), previousOutput);
        }
    INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdPreviousOutput"));	
	}


/*
 * Sends the Show Note mode to server along with the next SetOutputL()
 * request.
 * After SetOutputL() request from the Application is succesfully
 * processed by the server and application receives SetOutputComplete()
 * callback with KErrNone, it can query Show Note mode sent to server by
 * calling GetShowNote().
 * Other observers wanting to query Show Note mode can call GetShowNote()
 * after receiving OutputChanged() callback.
 *
 * @param aSection the Section to read from the ini file.
 * @return 
 */

void CT_CTelephonyAudioRoutingData::DoCmdSetShowNote(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdSetShowNote"));
	TBool mode;
	if(!GetBoolFromConfig(aSection, KMode, mode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMode);
    	SetBlockResult(EFail);
		}
	else
		{
		TInt error = iTelephonyAudioRouting->SetShowNote(mode);
	    if (error != KErrNone)
	        {
	    	ERR_PRINTF2(_L("Could not set show note. Error: %d"), error);
	    	SetError(error);
	        }
	    else
	    	{
	    	INFO_PRINTF2(_L("Mode set to: %d"), mode);
			INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdSetShowNote"));
	    	}		
		}    
	}


/**
 * Shows the  Show Note mode for the last succesfully completed
 * SetOutputL() request.
 * @param 
 * @return 
 */


void CT_CTelephonyAudioRoutingData::DoCmdGetShowNote()
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdGetShowNote"));
	TBool mode;
	TInt error = iTelephonyAudioRouting->GetShowNote(mode);
    if(error != KErrNone) 
        {
    	ERR_PRINTF2(_L("Could not get show note. Error: %d"), error);
    	SetError(error);
        }
    else
    	{
    	INFO_PRINTF2(_L("Mode: %d"), mode);
        INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdGetShowNote"));
    	}    
	}

/**
 * Sends an asynchronous request to Adaptation to set the output
 * where telephony audio will be routed. This request is completed only
 * when the application receives
 * MTelephonyAudioRoutingObserver::SetOutputComplete callback.
 * @param aSection - The section to read from the ini file
 * @return none.
 */

void CT_CTelephonyAudioRoutingData::DoCmdSetOutputL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_CTelephonyAudioRoutingData::DoCmdSetOutputL"));
	TInt output;
	if (!GetEnumFromConfig(aSection, KCmdOutput, iEnumOutput, output))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCmdOutput);
    	SetBlockResult(EFail);
    	}
	else
		{
		if(iOutput!=output)
			{
			TRAPD(error, iTelephonyAudioRouting->SetOutputL((CTelephonyAudioRouting::TAudioOutput)output));
		    IncOutstanding();
		    if (error != KErrNone)
		    	{
		    	ERR_PRINTF2(_L("Could not set output. Error: %d"), error);
		    	SetError(error);
		        }
		    else
		    	{
		    	INFO_PRINTF1(_L("Output set to: "));
			    switch (output)
				    {
				    case CTelephonyAudioRouting::ENotActive:
				    	INFO_PRINTF1(_L("Not active"));
				    break;
				    case CTelephonyAudioRouting::ENone:
				    	INFO_PRINTF1(_L("None"));
				    break;
				    case CTelephonyAudioRouting::EHandset:
				    	INFO_PRINTF1(_L("Handset"));
				    break;
				    case CTelephonyAudioRouting::ELoudspeaker:
				    	INFO_PRINTF1(_L("Loudspeaker"));
				    break;
				    case CTelephonyAudioRouting::EWiredAudioAccessory:
				    	INFO_PRINTF1(_L("Wired acc"));
				    break;
				    case CTelephonyAudioRouting::EBTAudioAccessory:
				    	INFO_PRINTF1(_L("BT audio acc"));
				    break;
				    case CTelephonyAudioRouting::ETTY:
				    	INFO_PRINTF1(_L("ETTY"));
			        break;
				    default:
				    	INFO_PRINTF2(_L("%d"), output);
				    }
		    	}
	    	}
		}
	INFO_PRINTF1(_L("*END*CT_CTelephonyAudioRoutingData::DoCmdSetOutputL"));
	}
