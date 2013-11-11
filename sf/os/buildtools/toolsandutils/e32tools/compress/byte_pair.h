// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BYTE_PAIR_H
#define BYTE_PAIR_H

#ifdef __VC32__
#ifdef __MSVCDOTNET__
#include <strstream>
#include <iomanip>
#else //!__MSVCDOTNET__
#include <strstrea.h>
#include <iomanip.h>
#endif //__MSVCDOTNET__
#else // !__VC32__
#ifdef __TOOLS2__ 
#include <sstream>
#include <iomanip>
using namespace std;
#else
#include <strstream.h>
#include <iomanip.h>
#endif
#endif // __VC32__



#include <e32cmn.h>
const TInt MaxBlockSize = 0x1000;
class CBytePair {
    private:
        TBool iFastCompress;
        TUint16 PairCount[0x10000];
        TUint16 PairBuffer[MaxBlockSize*2];
        TInt PairsFound;

        TUint16 GlobalPairs[0x10000];
        TUint16 GlobalTokenCounts[0x100];

        TUint16 ByteCount[0x100+4];

        void CountBytes(TUint8* data, TInt size) {
            memset(ByteCount,0,sizeof(ByteCount));
            TUint8* dataEnd = data+size;
            while(data<dataEnd)
                ++ByteCount[*data++];
        }
        inline void ByteUsed(TInt b) {
            ByteCount[b] = 0xffff;
        }
        int TieBreak(int b1,int b2) {
            return -ByteCount[b1]-ByteCount[b2];
        }

        void InitPairCount(TUint8* data, TInt size, TInt minFrequency, TInt marker);
        TInt MostCommonPair(TInt& pair);
        TInt LeastCommonByte(TInt& byte);
    public:
        CBytePair(TBool fastCompress) {
            iFastCompress = fastCompress;
            memset(GlobalPairs,0,sizeof(GlobalPairs));
            memset(GlobalTokenCounts,0,sizeof(GlobalTokenCounts));
        }
        TInt Compress(TUint8* dst, TUint8* src, TInt size);
        TInt Decompress(TUint8* dst, TInt dstSize, TUint8* src, TInt srcSize, TUint8*& srcNext);
};
TInt BytePairCompress(TUint8* dst, TUint8* src, TInt size, CBytePair *aBPE);

#endif

