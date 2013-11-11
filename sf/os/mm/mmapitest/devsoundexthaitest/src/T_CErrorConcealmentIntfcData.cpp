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


#include "t_cerrorconcealmentintfcdata.h"

/*@{*/
//Command literals 
_LIT(KCmdNewL, 							"NewL");
_LIT(KCmdDestructor, 					"~");
_LIT(KCmdConcealErrorForNextBuffer, 	"ConcealErrorForNextBuffer");
_LIT(KCmdSetFrameMode, 					"SetFrameMode");
_LIT(KCmdFrameModeRqrdForEC, 			"FrameModeRqrdForEC");
/*@}*/

/*@{*/
//INI Section name literals
_LIT(KFrameMode, 						"FrameMode");
_LIT(KFrameModeRqrd, 					"FrameModeRqrd");
_LIT(KDevSoundInstanceName,				"DevSoundInstanceName");
/*@}*/




/**
 * Two phase constructor
 * @leave	system wide error
 */
CT_CErrorConcealmentIntfcData* CT_CErrorConcealmentIntfcData::NewL()
    {
    CT_CErrorConcealmentIntfcData* self = new (ELeave)CT_CErrorConcealmentIntfcData();    
    return self;    
    }

/**
 * Private constructor. First phase construction
 */
CT_CErrorConcealmentIntfcData::CT_CErrorConcealmentIntfcData()
	:
	iErrorConcealmentIntfc(NULL)
    {
    }


/**
 * Public destructor
 */
CT_CErrorConcealmentIntfcData::~CT_CErrorConcealmentIntfcData()
    {     
    DestroyData();
    }


/**
 * Helper method for DoCmdDestructor
 */
void CT_CErrorConcealmentIntfcData::DestroyData()
	{
	if (iErrorConcealmentIntfc)
		{ 
		delete iErrorConcealmentIntfc;
		iErrorConcealmentIntfc = NULL;
		}	 
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CErrorConcealmentIntfcData::GetObject()
    {
    return iErrorConcealmentIntfc;
    }

/**
 * Process a command read from the Ini file
 * @param aCommand 			- The command to process
 * @param aSection			- The section get from the *.ini file of the project T_Wlan
 * @param aAsyncErrorIndex	- Command index dor async calls to returns errors to
 * @return TBool			- ETrue if the command is process
 * @leave					- System wide error
 */
TBool CT_CErrorConcealmentIntfcData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
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
	else if (aCommand == KCmdConcealErrorForNextBuffer)
		{
		DoCmdConcealErrorForNextBuffer();
		}
	else if (aCommand == KCmdSetFrameMode)
		{
		DoCmdSetFrameMode(aSection);
		}
	else if (aCommand == KCmdFrameModeRqrdForEC)
		{
		DoCmdFrameModeRqrdForEC(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret=EFalse;
		}	
	return ret;
	}

/**
 * Create an instance of CErrorConcealmentIntfc
 * @param none
 * @return none
 */
void CT_CErrorConcealmentIntfcData::DoCmdNewL()
	{
	DestroyData();
	INFO_PRINTF1(_L("*START*CT_CErrorConcealmentIntfcData::DoCmdNewL()"));
	if (!iErrorConcealmentIntfc)
		{
		TPtrC devSoundInstanceName;
		if (!GetStringFromConfig(aSection, KDevSoundInstanceName, devSoundInstanceName))
			{
			ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDevSoundInstanceName);
	    	SetBlockResult(EFail);
			}
		else
			{
			CMMFDevSound* devSoundInstance = static_cast<CMMFDevSound*>(GetDataObjectL(devSoundInstanceName));		
			if(devSoundInstance)
				{
				iErrorConcelmentIntfc == (CErrorConcealmentIntfc*)devSoundInstance->CustomInterface(KUidErrorConcealmentIntfc);
				if (iErrorConcealmentIntfc == NULL)
					{
					ERR_PRINTF1(_L("Could not get ErrConcealment CI"));
					SetError(KErrNotFound);
					}
				else
					{
					INFO_PRINTF1(_L("ErrConcealment created successfully"));
					INFO_PRINTF1(_L("*END*CT_CErrorConcealmentIntfcData::DoCmdNewL()"));
					}
				}
			else
				{
				ERR_PRINTF1(_L("DevSound not ready"));
				SetError(KErrNotReady);
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("*END*CT_CErrorConcealmentIntfcData::DoCmdNewL()"));
		}
	}

/**
 * Destroy an instance of CErrorConcealmentIntfc
 * @param
 * @none
 */
void CT_CErrorConcealmentIntfcData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_CErrorConcealmentIntfcData::DoCmdDestroyData()"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_CErrorConcealmentIntfcData::DoCmdDestroyData()"));
	}

/**
 * Command for Conceal and error for next buffer
 * @param none
 * @return none
 */
void CT_CErrorConcealmentIntfcData::DoCmdConcealErrorForNextBuffer()
	{
	INFO_PRINTF1(_L("*START*CT_CErrorConcealmentIntfcData::DoCmdConcealErrorForNextBuffer()"));
	TInt error = iErrorConcealmentIntfc->ConcealErrorForNextBuffer());
	if(error != KErrNone)
		{
		ERR_PRINTF2(_L("> Could not execute DoCmdConcealErrorForNextBuffer: %d"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF1(_L("*END*CT_CErrorConcealmentIntfcData::DoCmdConcealErrorForNextBuffer()"));
		}
	}

/**
 * Puts the hwdevice into a frame-based interface.
 * The alternate mode of interface would be buffer-based.
 * @param aSection - The section to read param from the ini file
 * @return none
 */
void CT_CErrorConcealmentIntfcData::DoCmdSetFrameMode(const TTEFSectionName& aSection) 
    {   
    INFO_PRINTF1(_L("*START*CT_CErrorConcealmentIntfcData::DoCmdSetFrameMode()"));
    TBool aFrameMode = EFalse;
    if (!GetBoolFromConfig(aSection, KFrameMode, aFrameMode))
    	{
		INFO_PRINTF2(_L("Parameter %S was not found in INI file."), &KFrameMode);
		SetBlockResult(EFail);
    	}
    else
    	{
	    TInt error = iErrorConcealmentIntfc->SetFrameMode(aFrameMode);    
	    if ( KErrNone != error )    	
	        {
	        ERR_PRINTF2(_L("> Could not execute DoCmdSetFrameMode: %d"), error);   
	        SetError(error);
	        }
	    else
	    	{
	    	INFO_PRINTF1(_L("*END*CT_CErrorConcealmentIntfcData::DoCmdSetFrameMode()")); 
	    	}
    	}
    }

/**
 * Query to find out if frame-based interface is required by hwdevice
 * for supporting error concealment.
 * @param aSection
 * @return none
 */
void CT_CErrorConcealmentIntfcData::DoCmdFrameModeRqrdForEC(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("*START*CT_CErrorConcealmentIntfcData::DoCmdFrameModeRqrdForEC()"));
    TBool aFrameModeRqrd = EFalse;
    if (!GetBoolFromConfig(aSection, KFrameModeRqrd, aFrameModeRqrd))
    	{
		INFO_PRINTF2(_L("Parameter %S was not found in INI file."), &KFrameModeRqrd);
		SetBlockResult(EFail);
    	}
    else
    	{
	    TInt error = iErrorConcealmentIntfc->FrameModeRqrdForEC(aFrameModeRqrd);
	    
	    if ( KErrNone != error )    	
	        {
	        ERR_PRINTF2(_L("> Could not execute DoCmdSetFrameMode: %d"), error);   
	        SetError(error);
	        }
	    else
	    	{
	    	INFO_PRINTF1(_L("*END*CT_CErrorConcealmentIntfcData::DoCmdFrameModeRqrdForEC()")); 
	    	}
    	}
    }
