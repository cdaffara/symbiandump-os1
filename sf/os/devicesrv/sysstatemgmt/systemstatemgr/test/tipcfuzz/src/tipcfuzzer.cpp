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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tipcfuzzer.h"

CIpcFuzzer* CIpcFuzzer::NewL()
	{
	CIpcFuzzer* self = CIpcFuzzer::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

CIpcFuzzer* CIpcFuzzer::NewLC()
	{
	CIpcFuzzer* self = new CIpcFuzzer();
	CleanupStack::PushL(self); // self;
	self->ConstructL();
	return self;
	}

// Constructor
CIpcFuzzer::CIpcFuzzer()
	{
	}

void CIpcFuzzer::ConstructL()
	{
	}

//Destructor
CIpcFuzzer::~CIpcFuzzer()
	{
	}

// Prepares 8 bit junk descriptor data.
HBufC8* CIpcFuzzer::GetDescData8L(TUint aIdx)
	{
	HBufC8* buf8 = NULL;
	switch(aIdx)
		{
			case 1:
				{
				buf8 = HBufC8::NewLC(1);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0x61),1);	
				}
				break;
			case 2:
				{
				buf8 = HBufC8::NewLC(255);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0x61),255);	
				}
				break;
			case 3:
				{
				buf8 = HBufC8::NewLC(55000);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0x61),55000);	
				}
				break;
			case 4:
				{
				buf8 = HBufC8::NewLC(255);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0xffff),255);	
				}
				break;
			case 5:
				{
				buf8 = HBufC8::NewLC(255);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0x0000),255);	
				}
				break;
			case 6:
				{
				buf8 = HBufC8::NewLC(255);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0x6100),255);
				}
				break;
			case 7:
				{
				buf8 = HBufC8::NewLC(55000);
				TPtr8 ptr = buf8->Des();
				ptr.Fill(TChar(0x6100),55000);
				}
				break;
			case 8:
				{
				buf8 = HBufC8::NewLC(0);	
				}
				break;
			default:
				break;
		}
	if(buf8)
		CleanupStack::Pop();
	return buf8;
	}

// Prepares 16 bit junk descriptor data.
HBufC* CIpcFuzzer::GetDescData16L(TUint aIdx)
	{
	HBufC* buf16 = NULL;
	switch(aIdx)
		{
			case 1:
				{
				buf16 = HBufC::NewLC(1);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0x61),1);	
				}
				break;
			case 2:
				{
				buf16 = HBufC::NewLC(255);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0x61),255);	
				}
				break;
			case 3:
				{
				buf16 = HBufC::NewLC(55000);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0x61),55000);	
				}
				break;
			case 4:
				{
				buf16 = HBufC::NewLC(255);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0xffff),255);	
				}
				break;
			case 5:
				{
				buf16 = HBufC::NewLC(255);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0x0000),255);	
				}
				break;
			case 6:
				{
				buf16 = HBufC::NewLC(255);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0x6100),255);
				}
				break;
			case 7:
				{
				buf16 = HBufC::NewLC(55000);
				TPtr ptr = buf16->Des();
				ptr.Fill(TChar(0x6100),55000);
				}
				break;
			case 8:
				{
				buf16 = HBufC::NewLC(0);				
				}
				break;
			default:
				break;
		}
	if(buf16)
		CleanupStack::Pop();
	return buf16;
	}

// Prepares an integer value.
TInt CIpcFuzzer::GetIntData(TUint aIdx)
	{
	TInt data = 0;
	switch(aIdx)
		{
			case 1:
				{
				data = 1;	
				}
				break;
			case 2:
				{
				data = 0xffffffff;	
				}
				break;
			case 3:
				{
				data = 0x7fffffff;	
				}
				break;
			case 4:
				{
				data = 0x80000000;	
				}
				break;
			case 5:
				{
				data = 0x0fffffff;	
				}
				break;
			case 6:
				{
				data = 55000;	
				}
				break;
			case 7:
				{
				data = 255;	
				}
				break;
			case 8:
				{
				data = 0x77777777;	
				}
				break;
			case 9:
				{
				data = 12345678;	
				}
				break;
			default:
				break;
		}
	return data;
	}
	




