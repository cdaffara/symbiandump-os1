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
#define __PLACEMENT_NEW_INLINE
#define __PLACEMENT_VEC_NEW_INLINE
/*
#ifdef __VC32__
 #ifdef __MSVCDOTNET__
  #include <strstream>
  #include <iomanip>
//  #include <ostream.h>
 #else //!__MSVCDOTNET__
  #include <strstrea.h>
  #include <iomanip.h>
 #endif //__MSVCDOTNET__
#else // !__VC32__
 #include <strstream.h>
 #include <iomanip.h>
#endif // __VC32__
*/
#ifdef __TOOLS2__
#include <sstream>
#include <ostream>
using namespace std;
#else
#include <strstream>
#include <ostream.h>
#endif


#include <e32cmn.h>

TInt BytePairCompress(TUint8* dst, TUint8* src, TInt size);
TInt Pak(TUint8* dst, TUint8* src, TInt size);
TInt Unpak(TUint8* dst, TInt dstSize, TUint8* src, TInt srcSize, TUint8*& srcNext);
void BytePairCompress(char * bytes, TInt size, ostream &os);

#endif