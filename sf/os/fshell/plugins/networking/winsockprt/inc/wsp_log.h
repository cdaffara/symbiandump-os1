// wsp_log.h
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __WSP_LOG_H__
#define __WSP_LOG_H__

#ifdef _DEBUG
//#define WSP_LOGGING
#endif

#ifndef WSP_LOGGING
#define WSP_LOG(a)
#define WSP_RAW_LOG(a)
#else
#define WSP_LOG(a) a 
#define WSP_RAW_LOG(a) a 
#define _WSP_LOG

#include <e32std.h>

class WspLog
	{
public:
	static TInt Open();
	static void Close();
	static void Write(const TDesC& aDes);
	static void Printf(TRefByValue<const TDesC> aFmt, ...);
	static void Printf(TRefByValue<const TDesC8> aFmt, ...);
	static void HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen);
	static void WriteRawInbound(const TDesC8& aDes);
	static void WriteRawOutbound(const TDesC8& aDes);
	};

#endif // WSP_LOGGING

#endif //__WSP_LOG_H__
