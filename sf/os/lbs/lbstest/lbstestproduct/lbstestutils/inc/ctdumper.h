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

/**
 @file
 @internalTechnology
 @test
*/

#ifndef CTDUMPER_H
#define CTDUMPER_H

#include <e32base.h>

#define LBSDUMPNEWLOG() 				CT_Dumper::OverWrite()
#define LBSDUMP(L1) 					CT_Dumper::Write(_L(L1))
#define LBSDUMP2(L1,L2) 				CT_Dumper::Write(_L(L1), L2)

	
/** Dumper used by the GPS module to dump the update array in a file.
*/
class CT_Dumper : public CBase
	{
public:
	IMPORT_C static void OverWrite();
	IMPORT_C static void Write(TRefByValue<const TDesC16> aFmt, ...);
	}; 

#endif // CTDUMPER_H
