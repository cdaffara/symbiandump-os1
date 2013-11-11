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
// interfacetable.h
// THIS WHOLE FILE IS INTERNAL TECHNOLOGY - DO NOT ADD NON-INTERNAL ITEMS
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_INTERFACETABLE_INTERNAL_H
#define SYMBIAN_INTERFACETABLE_INTERNAL_H

#include <e32std.h>

const TInt KErrInterfaceNotSupported = -17204;	// Requested interface is not supported by this object

namespace NetInterfaces
{

template<class TINTERFACE> struct TIfStaticFetcherFirstInHierarchy;
class TInterfaceIter;

class TInterfaceControl
/**
This class provides a base for an API extension implementer.
It implements an interface towards the ESock server.

*/
	{
	friend class TIfStaticFetcherFirstInHierarchy<TInterfaceControl>;
	friend class TInterfaceIter;

public:
	struct TFIEntry
		{
		typedef TAny* (*TStaticFetch)(TInterfaceControl*);
		typedef const TFIEntry* (*TStaticNext)();
		TInt iIfId;
		TStaticFetch iFetchFnL;
		TStaticNext iNextFn;
		};

public:
	IMPORT_C TAny* FetchInterfaceL(TInt aInterfaceId);
	IMPORT_C TAny* FetchInterface(TInt aInterfaceId);

protected:
	inline TInterfaceControl();

private:
	TAny* DoFetchInterface(TInt aInterfaceId);

private:
	const TFIEntry* iHead;
	};

TInterfaceControl::TInterfaceControl()
:	iHead(NULL)
	{
	}

/**
	Simple iterator for all the interfaces available via a specified
	TInterfaceControl.

*/
class TInterfaceIter
	{
public:
	IMPORT_C TInterfaceIter(const TInterfaceControl& aItfControl);

	inline TInt ItfId() const
		{
		return iCurrent? iCurrent->iIfId : 0;
		}

	inline TAny* operator++(TInt /*aInd*/)	// postfix
	    {
	    return Next();
		}

	IMPORT_C TAny* operator[](TInt aInd);

private:
	IMPORT_C TAny* Next();

private:
    const TInterfaceControl& iItfControl;
    const TInterfaceControl::TFIEntry* iCurrent;
	};

template <class TINTERFACE>
class TIfStaticFetcherFirstInHierarchy
/**
This class provides a base for the templated hierarchy of
interface fetchers.

*/
	{
public:
    inline static const TInterfaceControl::TFIEntry* IFEntry()
		{
		return NULL;
		}

protected:
	inline explicit TIfStaticFetcherFirstInHierarchy(TInterfaceControl* aInterfaceControl, const TInterfaceControl::TFIEntry* aHead)
		{
		aInterfaceControl->iHead = aHead; //This class is a friend of TInterfaceControl so we can do this
		}
	};

template <class TBASE, class TCLIENT, class TINTERFACE, class TLINK = TBASE>
class TIfStaticFetcher : protected TBASE
/**
Helper templated class used to build a static const linked list of
supported calls to ReturnInterfacePtrL().

*/
	{
public:
	inline static const TInterfaceControl::TFIEntry* IFEntry()
		{
		return &iFIEntry;
		}

protected:
	inline explicit TIfStaticFetcher(TInterfaceControl* aInterfaceControl, const TInterfaceControl::TFIEntry* aHead = NULL)
	:	TBASE(aInterfaceControl,(aHead)?aHead:&iFIEntry)
		{
		}

private:
	inline static TAny* FetchL(TInterfaceControl* aInterfaceControl)
		{
		TINTERFACE* interface;
		TCLIENT* cl = (TCLIENT*)aInterfaceControl;
		cl->ReturnInterfacePtrL(interface);
		return interface;
		}
	static const TInterfaceControl::TFIEntry iFIEntry;
	};

template <class TBASE, class TCLIENT, class TINTERFACE, class TLINK>
const TInterfaceControl::TFIEntry TIfStaticFetcher<TBASE,TCLIENT,TINTERFACE,TLINK>::iFIEntry =
	{
	TINTERFACE::KInterfaceId,
	&TIfStaticFetcher<TBASE,TCLIENT,TINTERFACE,TLINK>::FetchL,
	&TLINK::IFEntry
	};

template <class TBASE, class TCLIENT, class TINTERFACE>
class TIfStaticFetcherLinkBase : protected TBASE
/**
This template works like TIfStaticFetcher, but doesn't use a TLINK. This means that in the
template name TLINK will not be expanded twice. The double expansion was causing linking problems
with armlink. By using this class in ITFHIERARCHY_LINK_1, the final expansion will be shortened,
and the linker bug will be avoided.
This code is only here because of this bug.
-IBK

*/
	{
public:
	inline static const TInterfaceControl::TFIEntry* IFEntry()
		{
		return &iFIEntry;
		}

protected:
	inline explicit TIfStaticFetcherLinkBase(TInterfaceControl* aInterfaceControl, const TInterfaceControl::TFIEntry* aHead = NULL)
	:	TBASE(aInterfaceControl,(aHead)?aHead:&iFIEntry)
		{
		}

private:
	inline static TAny* FetchL(TInterfaceControl* aInterfaceControl)
		{
		TINTERFACE* interface;
		TCLIENT* cl = (TCLIENT*)aInterfaceControl;
		cl->ReturnInterfacePtrL(interface);
		return interface;
		}
	static const TInterfaceControl::TFIEntry iFIEntry;
	};

template <class TBASE, class TCLIENT, class TINTERFACE>
const TInterfaceControl::TFIEntry TIfStaticFetcherLinkBase<TBASE,TCLIENT,TINTERFACE>::iFIEntry =
	{
	TINTERFACE::KInterfaceId,
	&TIfStaticFetcherLinkBase<TBASE,TCLIENT,TINTERFACE>::FetchL,
	&TBASE::IFEntry
	};



class AApiExtBase
/**
*/
	{
public:
	IMPORT_C NetInterfaces::TInterfaceControl* FetchExtInterfaceControlL(TInt aInterfaceId);
	IMPORT_C TAny* FetchExtInterfaceL(TInt aInterfaceId); //Never returns NULL
	IMPORT_C TAny* FetchExtInterface(TInt aInterfaceId); //May return NULL
	IMPORT_C TBool SupportsExtInterface(TInt aInterfaceId) const;
	
protected:
	IMPORT_C virtual NetInterfaces::TInterfaceControl* DoFetchInterfaceControlL(TInt aInterfaceId);
	};

}	// namespace NetInterfaces

#endif
	//SYMBIAN_INTERFACETABLE_INTERNAL_H



