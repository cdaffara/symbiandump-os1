// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * @internalAll 
*/


#if !defined (WAPPSTOR_H__)
#define WAPPSTOR_H__

#include <e32base.h>
#include "gsmubuf.h"
#include "gsmustor.h"
#include "WAPDGRM.H"

//
// class CWapReassemblyStore
//

/**
 *  @internalComponent
 */
class CWapReassemblyStore : public CSARStore
	{
public:
	static CWapReassemblyStore* NewL(RFs& aFs);
	~CWapReassemblyStore();

        // Note !
        // if aDatagram.IsComplete() returns true, calling this method is not needed !
	TBool AddMessageL(  TInt&                   aIndex,
                                const CWapDatagram&     aDatagram);

	void GetDatagramL(  TInt            aIndex,
                                CWapDatagram&   aDatagram);
	TBool FindAndDeleteDatagramL( CWapDatagram& aDatagram);
private:
    void ConstructL();

	void InternalizeEntryL( TStreamId                   aStreamId,
                            CWapDatagram&               aDatagram,
                            CArrayPtr<CWapDatagram::TSegmentData>&  aSmsBufferArray);

    void ExternalizeEntryL( TStreamId&                          aStreamId,
                            const CWapDatagram&                 aDatagram,
                            const CArrayPtr<CWapDatagram::TSegmentData>& aSmsBufferArray);

	void PopulateEntry( TWapReassemblyEntry& aEntry,
                        const CWapDatagram& aDatagram,
                        TInt                aNumSmsBuffers);

	void CreateEntryL(  const CWapDatagram&                 aDatagram,
                        const CArrayPtr<CWapDatagram::TSegmentData>& aSmsBufferArray);

    void OpenStoreL();
private:
	CWapReassemblyStore(RFs& aFs);
private:
	TFileName iFullPathBuf;
	};


#endif // !defined WAPPSTOR_H__

