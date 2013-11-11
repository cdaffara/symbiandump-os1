/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Cut down version of NetMeta.lib to allow Commsdat to build on the TOOLS2 platform
* 
*
*/



/**
 @file NetMetaLib.h
 @internalTechnology
*/
 
 
#ifndef NET_META_LIB_H
#define NET_META_LIB_H


#include <e32base.h>
#include <e32std.h>





namespace Meta
{
class MMetaType;
typedef MMetaType* (*TMetaTypeNewL)(const TAny*, const TAny*);


struct SVDataTableEntry
    {
    TInt iOffset;
    TMetaTypeNewL iMetaNewL;
    };
    

class MMetaData
	{
public:
	virtual SVDataTableEntry const* GetVDataTable() const = 0;
	virtual TUint8* GetAttribPtr(const TInt aOffset) const = 0;
	};

	
class TMetaVTableIterator
	{
public:
	IMPORT_C explicit TMetaVTableIterator(MMetaData const* aMetaData);
	IMPORT_C ~TMetaVTableIterator();
	IMPORT_C SVDataTableEntry const* operator++(TInt aDummy);
protected:
    TBool IsNextEntryPresent();
private:
	SVDataTableEntry const* iEntry;
	};
	
}


#endif
