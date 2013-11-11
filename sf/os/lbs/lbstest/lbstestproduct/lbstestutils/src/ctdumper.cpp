// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <flogger.h>
#include <lbs/test/ctdumper.h>

const TInt KLbsDevLogMaxBufSize = 150;
_LIT(KLbsDevLogFolder, "refFiles");
_LIT(KLbsDevLogFile,"gpsUpdates.txt");


/** Static function, overwrites de log file if it already exists.

*/
EXPORT_C void CT_Dumper::OverWrite()
	{
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeOverwrite);
		}
		
	logger.Close();
	}


/** Static function, one parameter
@param aFmt TDes string reference 

*/
EXPORT_C void CT_Dumper::Write(TRefByValue<const TDesC16> aFmt, ...)
	{
	RFileLogger logger;
	TInt err = logger.Connect();
	
	if (err == KErrNone)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf16<KLbsDevLogMaxBufSize> strList;
		strList.FormatList(aFmt, list);
		VA_END(list);
		TBuf16<KLbsDevLogMaxBufSize> txt;
		
		txt.Append(strList);

		logger.CreateLog(KLbsDevLogFolder, KLbsDevLogFile, EFileLoggingModeAppend);
		logger.SetDateAndTime(EFalse, EFalse);
		logger.Write(txt);		
		}
		
	logger.Close();
	}





