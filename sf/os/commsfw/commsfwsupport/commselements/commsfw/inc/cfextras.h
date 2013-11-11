// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __CFEXTRAS_H__
#define __CFEXTRAS_H__

#include <e32base.h>
#include <f32file.h>
#include "elements/cftransportmacro.h"

// This namespace is split between this file and cfutil.h. You can include this file to use COwnEntryList
// etc without having to include the other - including cfutil.h requires that you link against commsfw and
// so requires any component using anything that includes cfutil.h to link against commsfw (nasty).

namespace CommsFW
//CommsFW namespace doxygen is supplied in cfmsgs.h. The namespace is publishedPartner, released
//The items in this header are all internal technology.
{
#ifdef SYMBIAN_TRACE_ENABLE	
	const TInt KMaxLogTextLength = 250;
	const TInt KPrimaryFilter = 194;
#endif

enum { KMaxIniDataSectionNameLength = 32 };
IMPORT_C TBool GetVarFromIniData(const TDesC8& aIniData, const TDesC8& aSection, const TDesC8& aVarName, TPtrC8& aResult);
IMPORT_C TBool GetVarFromIniData(const TDesC8& aIniData, const TDesC8 &aSection, const TDesC8 &aVarName, TInt &aResult);

class COwnEntryList : public CBase
	{
public:
	class TOwnEntry : public TEntry
		{
	public:
		IMPORT_C TOwnEntry(const TParse& aPath, const TEntry& aEntry);
		};
	IMPORT_C explicit COwnEntryList(TInt aGranularity);
	IMPORT_C ~COwnEntryList();
	IMPORT_C void WildScanAcrossDrivesL(const TDesC& aDir, const TDesC& aFileMask);
	IMPORT_C void UniqueWildScanAcrossDrivesL(const TDesC& aDir, const TDesC& aFileMask);
	IMPORT_C void AddL(const COwnEntryList& aDir);
	IMPORT_C void AddL(const TOwnEntry& aEntry);
	IMPORT_C TInt Count() const;
	IMPORT_C const TOwnEntry& operator[](TInt anIndex) const;
protected:
private:
	RArray<TOwnEntry> iArray;
	};

// Small extension of platform RHeap, exposing the access count and a means
// of logging all remaining allocated cells
class RCFSharedHeap : public RHeap
	{
public:
	IMPORT_C TInt AccessCount() const;
	IMPORT_C void LogAllocatedCells(const TDesC8& aSubSystem, const TDesC8& aComponent);
private:
#if defined(__FLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)

#ifdef SYMBIAN_TRACE_ENABLE	
	class TLogTextBuf : public TBuf8<KMaxLogTextLength>
	    {
	public:
	    class TLogIgnoreOverflow8 : public TDes8Overflow
	        {
	        public:
	            void Overflow(TDes8& /*aDes*/) { }
	        };

	    void AppendFormatIgnoreOverflow(TRefByValue<const TDesC8> aFmt, ...)
	        {
	    	TLogIgnoreOverflow8 overflowHandler;
	    	VA_LIST list;
	    	VA_START(list, aFmt);
	    	AppendFormatList(aFmt, list, &overflowHandler);
	    	VA_END(list);
	        }	    
	    };
#endif	   
 
	class TLogInfo
		{
	public:
		TLogInfo(const TDesC8& aSubSystem, const TDesC8& aComponent)
		: iSubSystem(aSubSystem), iComponent(aComponent)
			{ }
	public:
		const TDesC8& iSubSystem;
		const TDesC8& iComponent;
		};
	static void WalkFunc(TAny* aContext, TCellType aType, TAny* aCell, TInt aSize);
#endif
	};


} // CommsFW

#endif

