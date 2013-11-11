// usampler_cia.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <platform.h>
#include <kern_priv.h>

#ifdef __GCCE__
// GCCE.h on many platforms has completely wrong definitions
#undef __NAKED__
#define __NAKED__ __attribute__((__naked__))
#endif

#include "usampler.cia"
