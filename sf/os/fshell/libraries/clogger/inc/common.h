// common.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef COMMON_H
#define COMMON_H

class TOverflow16 : public TDes16Overflow
	{
public:
	TOverflow16() : iOverflow(EFalse) {}
	void Overflow(TDes16 &) { iOverflow = ETrue; }
	TBool iOverflow;
	};
class TOverflow8 : public TDes8Overflow
	{
public:
	TOverflow8() : iOverflow(EFalse) {}
	void Overflow(TDes8 &) { iOverflow = ETrue; }
	TBool iOverflow;
	};


#endif
