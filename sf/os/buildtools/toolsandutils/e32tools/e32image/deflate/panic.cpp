// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// e32tools\petran\Szip\panic.cpp
// 
//

#include "panic.h"
#include "h_utl.h"
#include <stdlib.h>

char* HuffmanError[]=	{
						"Huffman: Too many codes\n",
						"Huffman: Invalid coding\n",
						"Huffman: Buffer overflow\n",
						"Huffman: Out Of Memory\n",
						"Huffman: Corrupt File\n",
						};

const TInt KHuffmanErrorBase=700;

void Panic(TPanic aPanic)
	{
  	Print(ESevereError,HuffmanError[TInt(aPanic)]);
	exit(KHuffmanErrorBase+TInt(aPanic));
	}
