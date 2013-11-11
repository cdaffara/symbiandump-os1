// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __R_MROMIMAGE_H__
#define __R_MROMIMAGE_H__

class TRomNode;

/**
class MRomImage
MRofsImage is the interface used to access information held within an Core ROM image.
This interface used to remove the dependency between processing of 
extensions and kernel commands in the obey file

@internalComponent
@released
*/
class MRomImage
{
public:
	virtual TRomNode* RootDirectory() = 0;
	virtual TRomNode* CopyDirectory(TRomNode*& aSourceDirectory)=0;
	virtual TText* RomFileName() = 0;
	virtual TUint32 RomBase() = 0;
	virtual TUint32 RomSize() = 0;
	virtual TVersion Version() = 0;
	virtual TInt64 Time() = 0;
	virtual TUint32 CheckSum() = 0;
	virtual TUint32 DataRunAddress() = 0;
	virtual TUint32 RomAlign() = 0;

	virtual ~MRomImage() { };
};

#endif //__R_MROMIMAGE_H__
