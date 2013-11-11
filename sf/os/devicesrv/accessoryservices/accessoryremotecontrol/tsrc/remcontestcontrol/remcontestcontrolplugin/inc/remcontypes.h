/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the ?classname class.
*
*/


#ifndef __TFREMCONTYPES_H__
#define __TFREMCONTYPES_H__

#include <e32base.h>
#include <RemConKeyEventData.h>
#include <remconcoreapi.h>
#include <RemConExtensionApi.h>
#include <ctsydomainpskeys.h>
 		
enum TTFRemConFunction
	{
	ETFRemCon_Subscribe						   				= 1,	
	ETFRemCon_PocButtonDownResponse							= 4,
	ETFRemCon_PocButtonUpResponse							= 5,
	ETFRemCon_SyncButtonResponse							= 6,
	ETFRemCon_AddressOutgoingCommand					    = 7,
	ETFRemCon_PermitOutgoingCommand						    = 8,
	ETFRemCon_CancelOutgoingCommand						    = 9,
	ETFRemConEvent_AnswerCallResponse						= 10,
	ETFRemConEvent_EndCallResponse							= 11,
	ETFRemConEvent_LastNumberRedialResponse					= 12,
	ETFRemConEvent_VoiceDialResponse						= 13,
	ETFRemConEvent_DialCallResponse							= 14,
	//ETFRemConEvent_DisableNRECFromTerminalResponse		= 15,
	ETFRemConEvent_MultipartyCallingResponse				= 16,
	ETFRemConEvent_GenerateDTMFResponse						= 17,
	ETFRemConEvent_SpeedDialResponse						= 18,
	ETFRemConEvent_CheckEvent  						        = 19,
	ETFRemConEvent_CheckResponse                            = 20,
	ETFRemConEvent_SetCallState                             = 21,
	ETFRemConEvent_CmdHandler                               = 22,
	ETFRemConEvent_NormalHandler                            = 23,
	ETFRemConEvent_DeleteInstances                          = 24,
	ETFRemConEvent_LoadPCH_NoAccFW                          = 25,
	ETFRemConEvent_DeletePCH_NoAccFW                        = 26,
	ETFRemConEvent_CheckOutgoingCommandResponse             = 27,
	ETFRemCon_NotifyCommandSubscribe                        = 28,
    ETFRemCon_AddressIncomingNotify                         = 29,
	ETFRemCon_LaunchAudioClient                             = 30,
	ETFRemCon_CloseAudioClient                              = 31,
	ETFRemCon_CheckClientKey                                = 32,
    ETFRemConEvent_CheckCmdSource                           = 33
	};
			
enum TTFRemConkeyEvent
    {
    ETFRemConEvent_None = 0,
    ERemConEvent_CodeSendEnd,						
    ETFRemConEvent_VolumeUp,	                        
    ETFRemConEvent_VolumeDown,                       
    ETFRemConEvent_CodePlayPause,					
	ETFRemConEvent_CodeStop,							
	ETFRemConEvent_CodeBack,	 						
	ETFRemConEvent_CodeForward, 						
    ETFRemConEvent_PocUp,  		              	    
    ETFRemConEvent_PocDown,		              	    
    ETFRemConEvent_Sync,		                  	    
    ETFRemConEvent_CoreApiPlaybackSpeedX1,			
	ETFRemConEvent_AnswerCall,						
	ETFRemConEvent_EndCall,
	ETFRemConEvent_AnswerEnd,							
	ETFRemConEvent_VoiceDial,						
	ETFRemConEvent_LastNumberRedial,					
	ETFRemConEvent_DialCall,							
	ETFRemConEvent_DisableNRECFromTerminal,			
	ETFRemConEvent_MultiPartyCalling,				
	ETFRemConEvent_GenerateDTMF,						
	ETFRemConEvent_SpeedDial,
	ETFRemConEvent_Mute						
    };


// Defines test case state for stub
struct TRemConControlTestCaseState
    {
    TTFRemConFunction		  	iFunction;       
    TRemConKeyEventData 		iKeyEvents;
    TInt						iArg1;           
    TText8*		            	iArg2; 
    TBool                    	iUseConverter; 
    TTFRemConkeyEvent			iExpectedKeyEvent;
    };    
    
const TInt KTFErrRemConServiceMissing    	    	      = -4001;
const TInt KTFErrRemConEventMissing	  	    	   	  	  = -5000;
const TInt KTFErrRemConUnexpectedEvent      	   	  	  = -6000;
const TInt KTFErrRemConUnexpectedEventParameter  		  = -7000;
const TInt KTFErrRemConUnexpectedResult           		  = -8000;

#endif //__TFREMCONTYPES_H__
