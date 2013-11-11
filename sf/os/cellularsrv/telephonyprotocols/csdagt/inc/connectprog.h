/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CONNECTPROG.H
*/

#ifndef __CONNECTPROG_H__
#define __CONNECTPROG_H__

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <csdprog.h>
#endif

// Stages in establishing a connection with NetDial

 /**
 * Defines the stages in establishing a connection with dial-up networking.
 *
 * The stages which may be reported depend on the type of connection being made. 
 * Stage information is usually read from a TNifProgress::iStage member.
 *
 * @publishedAll
 * @since		v5.0  
 * @deprecated	7.0s - maintained for compatibility with 6.1\n
 *				This enumeration is still used by the new RGenericAgent API.\n\n
 *				New software should use the CSD progress values defined in csdprog.h
 * 
 * \n
 * For a \b Dial \b Up \b Connection (outgoing call), the following stages are reported 
 * in this order \n 
 * (the meaning of each enumerator is defined in the enumeration values / member list, below):
 * -# EStartingSelection 
 * -# EFinishedSelection 
 * -# EStartingDialling 
 * -# EFinishedDialling 
 * -# EScanningScript 
 * -# EScannedScript 
 * -# EGettingLoginInfo 
 * -# EGotLoginInfo 
 * -# EStartingLogIn 
 * -# EFinishedLogIn 
 * -# EConnectionOpen 
 * -# EStartingHangUp 
 * -# EFinishedHangUp 
 *		   
 * For a \b Dial \b In \b Connection (incoming call), the following stages are
 * reported in this order: 
 * -# EStartingAnswer 
 * -# EAnswered 
 * -# EConnectionOpen 
 * -# EStartingHangUp 
 * -# EFinishedHangUp 
 *
 * For a \b Direct \b Connection, the following stages are reported in this order:
 * -# EStartingSelection 
 * -# EFinishedSelection 
 * -# EScanningScript 
 * -# EScannedScript 
 * -# EStartingConnect 
 * -# EFinishedConnect 
 * -# EConnectionOpen 
 * -# EStartingHangUp 
 * -# EFinishedHangUp 	
 */
 
enum TNetDialProgress
	{
	/** Start selection of service and location. */
	EStartingSelection=2,
	/** Selection of service and location is complete. */
	EFinishedSelection,	
	/** Starting the dialling phase. */
	EStartingDialling,
	/** Dialling phase complete. */
	EFinishedDialling,
	/** Start scanning the script. */
	EScanningScript,
	/** Scanning the script is complete. */
	EScannedScript,
	/** Get login information from database or user */
	EGettingLoginInfo,
	/** The fetching of login information is complete. */
	EGotLoginInfo,
	/** Start the connection process; executes the script, if there is one */
	EStartingConnect,				// 10
	/** Connection process is complete */
	EFinishedConnect,
	/** Start the login process; executes the script, if there is one. */
	EStartingLogIn,
	/** Login process is complete */
	EFinishedLogIn,
	/** The connection is now open */
	EConnectionOpen,
	/** Starting the hang-up process */
	EStartingHangUp,
	/** Hang up process is complete */
	EFinishedHangUp,
	/** Start the answering phase. */
	EStartingAnswer,
	/** Answering is complete */
	EAnswered,						// 18
	/** Error setting overrides (note that this is reported only if an
	* error occurs, in which case it will be the first stage of the connection). */
	ESettingOverrides
	};



#endif
