/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file NetConLog.h
 @internalComponent
*/

#if !defined (__NETCONLOG_H__)
#define __NETCONLOG_H__

#if defined (_DEBUG)

	#include <comms-infras/commsdebugutility.h>
	#define LOG(MSG) MSG

	#if defined (DETAILED_LOG)
		#define LOG_DETAILED(MSG) MSG
	#else
		#define LOG_DETAILED(MSG)
	#endif

	_LIT(KNetConLogFolder,"NetCon");		///< KNetConLogFolder holds name of the Network Controller Log Folder
	_LIT(KNetConLogFile,"NetCon.txt");		///< KNetConLogFile holds name of the Network Controller Log File
	_LIT(KEndOfLine,"\r\n");				///< KEndOfLine holds the End Of Line format specifier.

	const TInt KNetConLogHexDumpWidth = 16;

	class NetConLog
	/**
	The class is used to write into the Network Controller Log.

	@internalComponent
	*/
		{

	public:
	    static void Printf(TRefByValue<const TDesC> aFmt, ...);
		
		/* The next two functions are not used at present
		
		static void Write(const TDesC& aDes);
		static void HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen, TInt aWidth = KNetConLogHexDumpWidth);
		
		*/
		
		};

#else
	#define LOG(MSG)
	#define LOG_DETAILED(MSG)
#endif

#endif // __NETCONLOG_H__

