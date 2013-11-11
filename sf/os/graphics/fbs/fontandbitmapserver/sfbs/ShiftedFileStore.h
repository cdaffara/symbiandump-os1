// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SHIFTEDFILESTORE_H__
#define __SHIFTEDFILESTORE_H__

#include <s32file.h>

/**
@internalComponent
class CShiftedFileStore - this is highly specialized class, derived from
CDirectFileStore. It has only one purpose - to make possible creating 
direct file store instance beginning at the middle of some physical file. It is expected
to be used with the new RSC files format: Header + RSC file section + MBM file section.
*/
class CShiftedFileStore: public CDirectFileStore
	{
public:
	static CShiftedFileStore* OpenLC(RFs& aFs, const TDesC& aName, TUint aFileMode, TUint aFileOffset);
	static CShiftedFileStore* FromL(RFile& aFile, TUint aFileOffset);
private:
	CShiftedFileStore(RFile& aFile);

	};

#endif //__SHIFTEDFILESTORE_H__

