// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _BASICOP_H
#define _BASICOP_H

#include "types.h"

int2 add( int2 var1, int2 var2 );
int2 sub( int2 var1, int2 var2 );
int2 mult( int2 var1, int2 var2 );
int2 abs_s( int2 var1 );

#ifdef L_MULTF
/*
** define L_MULTF controls if L_mult is implemented
** as a macro or as a function
** See functional description in file basicop.c
*/
int4 L_mult( int2 var1, int2 var2 );
#else
#define L_mult( var1, var2 ) \
  ( ((int4)(var1) * (int2)(var2)) << 1 )
#endif

int2 shl( int2 var1, int2 var2 );
int2 shr( int2 var1, int2 var2 );
int2 negate( int2 var1 );
int2 extract_h( int4 L_var1 );
int2 extract_l( int4 L_var2 );
int4 L_mac( int4 L_var3, int2 var1, int2 var2 );

int4 L_add( int4 L_var1, int4 L_var2 );
int4 L_sub( int4 L_var1, int4 L_var2 );
int2 mult_r( int2 var1, int2 var2 );
int4 L_shl( int4 L_var1, int2 var2 );
int4 L_shr( int4 L_var1, int2 var2 );

int4 L_deposit_h( int2 var1 );
int4 L_deposit_l( int2 var1 );

int2 norm_s( int2 var1 );

int2 div_s( int2 var1, int2 var2 );

int2 norm_l( int4 L_var1 );


#endif
