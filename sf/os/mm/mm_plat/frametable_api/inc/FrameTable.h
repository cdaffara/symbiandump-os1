/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class definition for the frame table functions.
 *
*/


#ifndef CFRAMETABLE_H
#define CFRAMETABLE_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MFrameTableEventObserver;

// CLASS DEFINITIONS

/**
 *  This class provides AAC utility functions.
 *
 *  @lib AACAudioControllerUtility.lib
 *  @since 3.0
 */
class CFrameTable : public CBase
    {
public:
    // Constructors and destructor

    enum TFrameTableEvent
        {
        EPosReached,
        EDecodeInterval,
        EPlayWindowEndPosReached
        };

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CFrameTable* NewL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CFrameTable();

public:
    // New functions

    IMPORT_C TInt InitFrameTable(TInt aSampleRate, TInt aSamplesPerFrame);
    IMPORT_C TInt SubmitTableEntry(TUint aPos);

    IMPORT_C TInt FindFramePosFromTime(TUint& aTimeMs, TUint& aPos);
    IMPORT_C TInt FindFrameTimeFromPos(TUint& aTimeMs, TUint& aPos);
    IMPORT_C TInt LastFramePos(TUint& aPos);
    IMPORT_C TInt LastFrameTime(TUint& aTimeMs);

    IMPORT_C void ShrinkTable();
    IMPORT_C void ResetTable();

    IMPORT_C TInt SetSourceReference(TUint aTimeMs, TUint aPos);

    IMPORT_C TInt RegisterForEvent(TFrameTableEvent aEvent,
            MFrameTableEventObserver* aObserver, TUint aParam);
    IMPORT_C TInt UnRegisterForEvent(TFrameTableEvent aEvent,
            MFrameTableEventObserver* aObserver);
    IMPORT_C TInt Bitrate();
    IMPORT_C TInt IsSeeking(TBool& aIsSeeking);
    IMPORT_C TInt GetLastPosEvent(TUint& aPos);

private:

    class TBufRefPosEntry
        {
public:
        TUint iPos;
        TUint iFrameNum;
        };

    /**
     * C++ default constructor.
     */
    CFrameTable();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    TUint CalcFrameFromTimeMs(TUint aTimeMs);
    TInt SubmitLowResTableEntry(TBufRefPosEntry& aEntry);

private:
    // Data

    RArray<TBufRefPosEntry> iFrameTable;
    RArray<TBufRefPosEntry> iLowResFrameTable;
    TUint iSampleRate;
    TUint iSamplesPerFrame;
    TUint iMsecPerFrame;
    TUint iLastBytePos;
    TUint iAvgBitrate;
    TUint iOffset;
    TUint iTimeRefMs;
    TUint iBufCnt;
    TUint iPosEventMs;
    TUint iPlayWindowEndPosEventMs;
    TInt iIntervalRef;
    TUint iIntervalEvent;
    MFrameTableEventObserver* iObserver;
    TUint iEventPos;
    TUint iCurrentFrameCount;
    TUint iFramesPerLowResInterval;
    TUint iLowResIntervalMs;
    TUint iFrameNum;
    TBool iOkToShrink;
    };

class MFrameTableEventObserver
    {
public:
    virtual TInt
            HandleFrameTableEvent(CFrameTable::TFrameTableEvent aEvent) = 0;
    };

#endif 		// CFRAMETABLE_H
// End of File
