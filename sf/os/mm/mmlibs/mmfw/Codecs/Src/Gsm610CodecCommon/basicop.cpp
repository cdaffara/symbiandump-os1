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

#include "basicop.h"

/*
** int2 add( int2 var1, int2 var2 )
**
** Function performs the addition (var1+var2) with overflow control
** and saturation; the result is set at +32767 when overflow occurs
** or at -32768 when underflow occurs
**
** Input:
**   var1, var2
**     16-bit variables to be summed
**
** Output:
**   Sum of var and var2, see description above
**  
** Return value:
**   See above
*/
/*
** One way to implement saturation control to add and sub is to
** use temporary result that has enough bits to make overflow
** impossible and the limit the final result
*/
int2 add( int2 var1, int2 var2 )
{
  int4 L_temp;
  
  L_temp = (int4) var1 + var2;
  
  if ( L_temp < MININT2 )
    return MININT2;
  else if ( L_temp > MAXINT2 )
    return MAXINT2;
  else
    return (int2) L_temp;
}


/*
** int2 sub( int2 var1, int2 var2 )
**
** Function performs the subtraction (var1-var2) with overflow control
** and saturation; the result is set at +32767 when overflow occurs
** or at -32768 when underflow occurs
**
** Input:
**   var1, var2
**     16-bit variables to be summed
**
** Output:
**   Sum of var and var2, see description above
**  
** Return value:
**   See above
*/
int2 sub( int2 var1, int2 var2 )
{
  int4 L_temp;
  
  L_temp = (int4) var1 - var2;
  
  if ( L_temp < MININT2 )
    return MININT2;
  else if ( L_temp > MAXINT2 )
    return MAXINT2;
  else
    return (int2) L_temp;
}



/*
** int2 mult( int2 var1, int2 var2 )
**
** Function performs the multiplication of var1 by var2 and gives a
** 16-bit result which is scaled ie
**   mult( var1, var2 ) = (var1 times var2) >> 15
** and
**   mult( -32768, -32768 ) = 32767 
**
** Input:
**   var1, var2
**     16-bit variables to be multiplied
**
** Output:
**   Scaled result of multiplication
**  
** Return value:
**   See above
*/
int2 mult( int2 var1, int2 var2 )
{
  if ( ( var1 == MININT2 ) && ( var1 == var2 ) )
    return MAXINT2;
  else
    /* Note int4 cast of var1 to force 32-bit arithmetic */
   return( (int2) ( ( (int4) var1 * var2 ) >> 15 ) );
}


/*
** int2 abs_s( int2 var1 )
**
** Function returns absolute value of var1 with possible saturation:
**   abs( -32768 ) = 32767
**
** Input:
**   var1
**     16-bit variable
**
** Output:
**   absolute value of var1
**  
** Return value:
**   See above
*/
int2 abs_s( int2 var1 )
{
  if ( var1 == MININT2 )
    return MAXINT2;
  else
    return ( var1 > 0 ) ? var1 : int2 (-var1);
}


#ifdef L_MULTF
/* else implemented using macro (basicop.h) */

/*
** int4 L_mult( int2 var1, int2 var2 )
**
** Function performs the multiplication of var1 by var2 and gives a
** 32-bit result which is scaled by shifting result one bit left ie
**   L_mult( var1, var2 ) = (var1 times var2) << 1
**
**   L_mult( -32768, -32768 ) does not occur in algorithm
**
** Input:
**   var1, var2
**     16-bit variables to be multiplied
**
** Output:
**   Scaled result of multiplication
**  
** Return value:
**   See above
*/
int4 L_mult( int2 var1, int2 var2 )
{
  /* Note int4 cast of var1 to force 32-bit arithmetic */
  return ( L_shl( (int4) var1 * var2 ), 1 );
}
#endif /* L_MULTF */



/*
** int2 shr( int2 var1, int2 var2 )
**
** Function makes arithmetic var2-bit shift right of var1. If var2 is
** less than 0, this operation becomes arithmetic left shift of -var2
**
** Input:
**   var1
**     16-bit variable to be shifted
**   var2
**     amount of bits to be shifted
**
** Output:
**   16-bit value of shifted var1 is returned 
**  
** Return value:
**   See above
*/
int2 shr( int2 var1, int2 var2 )
{
  return shl( var1, int2 (-var2) );
}


/*
** int2 negate( int2 var1 )
**
** Function negates 16-bit variable var1. 
**   negate( -32768 ) = 32767
**
** Input:
**   var1
**     16-bit variable to be negated
**
** Output:
**   negated var1
**  
** Return value:
**   See above
*/
int2 negate( int2 var1 )
{
  if ( var1 == MININT2 )
    return MAXINT2;
  else
    return int2 (-var1);
}


/*
** int2 extract_h( int4 L_var1 )
**
** Function returns upper word (16 most significat bits) of the 
** 32-bit variable L_var1
**
** Input:
**   L_var1
**     32-bit variable
**
** Output:
**   upper word of the L_var1
**  
** Return value:
**   See above
*/
int2 extract_h( int4 L_var1 )
{
  return (int2) ( L_var1 >> 16 );
}


/*
** int2 extract_l( int4 L_var1 )
**
** Function returns lower word (16 least significat bits) of the
** 32-bit variable L_var1
**
** Input:
**   L_var1
**     32-bit variable
**
** Output:
**   lower word of L_var1
**  
** Return value:
**   See above
*/
int2 extract_l( int4 L_var1 )
{
  return (int2) (L_var1 & 0x0000ffff);
}


/*
** int4 L_mac( int4 L_var3, int2 var1, int2 var2 )
**
** Function multiplies var1 by var2 and shifts result left by one bit.
** Shifted result of multiplication is then added to L_var3 and result
** is returned. Summation is done with overflow control and saturation;
** the result is set at 2147483647 when overflow occurs and at
** -2147483648 when underflow occurs
**
** Input:
**   var1
**     16-bit multiplicant
**   var2
**     16-bit multiplier
**
**   L_var3
**     32-bit number that is summed with (var1*var2)<<1
**
** Output:
**   See description above
**  
** Return value:
**   See above
*/
int4 L_mac( int4 L_var3, int2 var1, int2 var2 )
{
  int4 L_temp;

  L_temp = ( (int4) var1 * var2 ) << 1;
  return L_add( L_var3, L_temp );
}


/*
** int4 L_add( int4 L_var1, int4 L_var2 )
**
** Function performs 32-bit addition of two 32-bit variables
** (L_var1 + L_var2) with overflow control and saturation; the
** result is set at 2147483647 when overflow occurs and at
** -2147483648 when underflow occurs
**
** Input:
**   L_var1, L_var2
**     32-bit variables to be summed
**
** Output:
**   32-bit result, see description above
**  
** Return value:
**   See above
*/
int4 L_add( int4 L_var1, int4 L_var2 )
{
  int4 L_temp1;
  int temp2; /* used for storing sign of L_var1 */
  
  L_temp1 = L_var1 + L_var2;

  /*
   * Overflow
   *   if sign(L_var1)==sign(L_var2) && sign(L_var1)!=sign(L_temp1).
   */

  if ( ( temp2 = (L_var1 < 0) ) == ( L_var2 < 0 ) 
    && ( temp2 != ( L_temp1 < 0 ) ) ) {
      L_temp1 = temp2 ? MININT4 : MAXINT4;
  }

  return L_temp1;
}


/*
** int4 L_sub( int4 L_var1, int4 L_var2 )
**
** Function performs 32-bit subtraction of two 32-bit variables
** (L_var1 - L_var2) with overflow control and saturation; the
** result is set at 2147483647 when overflow occurs and at
** -2147483648 when underflow occurs
**
** Input:
**   L_var1, L_var2
**     32-bit variables to be summed
**
** Output:
**   32-bit result, see description above
**  
** Return value:
**   See above
*/
int4 L_sub( int4 L_var1, int4 L_var2 )
{
  int4 L_temp1;
  int temp2;

  L_temp1 = L_var1 - L_var2;

  /*
   * Overflow
   *   if sign(L_var1)!=sign(L_var2) && sign(L_var1)!=sign(L_temp).
   */

  if ( ( temp2 = ( L_var1 < 0 ) ) != ( L_var2 < 0 ) 
    && ( temp2 != ( L_temp1 < 0 ) ) ) {
      L_temp1 = temp2 ? MININT4 : MAXINT4;
  }

  return L_temp1;
}


/*
** int2 mult_r( int2 var1, int2 var2 )
**
** Function performs the multiplication of var1 by var2 and gives a
** 16-bit result which is scaled with rounding ie
**   mult_r(var1, var2) = ((var1 times var2) + 16384) >> 15
** and
**   mult_r( -32768, -32768 ) = 32767 
**
** Input:
**   var1, var2
**     16-bit variables to be multiplied
**
** Output:
**   16-bit scaled result of multiplication
**  
** Return value:
**   See above
*/
int2 mult_r( int2 var1, int2 var2 )
{
  if ( ( var1 == MININT2 ) && ( var1 == var2 ) )
    return MAXINT2;
  else
    /* Note int4 cast of var1 to force 32-bit arithmetic */
    return( (int2) ( ( (int4) var1 * var2 + (int4) 16384 ) >> 15 ) );
}

/*
** int4 L_shr( int4 L_var1, int2 var2 )
**
** Function makes arithmetic var2-bit shift right of var1. If var2 is
** less than 0, this operation becomes arithmetic left shift of -var2
**
** Input:
**   L_var1
**     32-bit variable to be shifted
**   var2
**     amount of bits to be shifted
**
** Output:
**   16-bit value of shifted var1
**  
** Return value:
**   See above
*/
int4 L_shr( int4 L_var1, int2 var2 )
{
  return L_shl( L_var1, int2 (-var2) );
}


/*
** int4 L_deposit_h( int2 var1 )
**
** Function deposits the 16-bit variable var1 into the 16 most
** significant bits of the 32-bit word. The 16 least significant
** bits of the result are zeroed.
**
** Input:
**   var1
**     16-bit variable to be loaded to the upper 16 bits of
**     of the 32-bit variable
**
** Output:
**   32-bit number, see description above
**  
** Return value:
**   See above
*/
int4 L_deposit_h( int2 var1 )
{
  return ( (int4) var1 ) << 16;
}


/*
** int4 L_deposit_l( int2 var1 )
**
** Function deposits the 16-bit variable var1 into the 16 least
** significant bits of the 32-bit word. The 16 most significant bits
** of the result are sign extended.
**
** Input:
**   var1
**     16-bit variable to be converted to 32-bit variable
**
** Output:
**   32-bit variable that has same magnitude than var1
**  
** Return value:
**   See above
*/
int4 L_deposit_l( int2 var1 )
{
  return (int4) var1;
}


/*
** int2 norm_s( int2 var1 )
**
** Function produces number of left shifts needed to normalize the
** 16-bit variable var1 for positive values on the interval with
** minimum of 16384 and maximum of 32767 and for negative
** values on the interval with minimum of -32768 and maximum of
** -16384; in order to normalize the result, the following
** operation must be done:
**   norm_var1 = var1 << norm_s(var1)
**
** Input:
**   var1
**     16-bit variable which normalization factor is solved
**
** Output:
**   see description above
**  
** Return value:
**   See above
*/
int2 norm_s( int2 var1 )
{
  int2 cntr = 0;

  /* Special case when L_var1 == -32768: shift leads to underflow */
  if ( var1 == MININT2 )
    return 0;
  /* Special case when var1 == 0: shift does not change the value */
  else if ( var1 == 0 )
    return 0;
  else {
    if ( var1 < 0 ) {
      for ( ; var1 >= -16384; var1 *= 2 )
	cntr++;      
    }
    else {
      for ( ; var1 < 16384; var1 <<= 1 )
	cntr++;
    }

    return cntr;
  }
}


/*
** int2 norm_l( int4 L_var1 )
**
** Function produces number of left shifts needed to normalize the
** 32-bit variable L_var1 for positive values on the interval with
** minimum of 1073741824 and maximum of 2147483647 and for negative
** values on the interval with minimum of -2147483648 and maximum of
** -1073741824; in order to normalize the result, the following
** operation must be done:
**   L_norm_var1 = L_var1 << norm_l(L_var1)
**
** Input:
**   L_var1
**     32-bit variable which normalization factor is solved
**
** Output:
**   see description above
**  
** Return value:
**   See above
*/
int2 norm_l( int4 L_var1 )
{
  int2 cntr = 0;

  /* Special case when L_var1 == -2147483648: shift leads to underflow */
  if ( L_var1 == MININT4 )
    return 0;
  /* Special case when L_var1 == 0: shift does not change the value */
  else if ( L_var1 == 0 )
    return 0;
  else {
    if ( L_var1 < 0 ) {
      for ( ; L_var1 >= (int4) -1073741824L; L_var1 *= 2 )
	cntr++;
    }
    else {
      for ( ; L_var1 < (int4) 1073741824L; L_var1 <<= 1 )
	cntr++;
    }

    return cntr;
  }
}


/*
** int2 div_s( int2 num, int2 denum )
**
** Function produces a result which is the fractional integer division
** of var1 by var2; var1 and var2 must be positive and var2 must be
** greater or equal to var1; The result is positive (leading bit equal
** to 0) and truncated to 16 bits. If var1 == var2 then
**   div_s( var1, var2 ) = 32767
**
** Input:
**   L_var1
**     32-bit variable which normalization factor is solved
**
** Output:
**   16-bit result, see description above
**  
** Return value:
**   See above
*/
int2 div_s( int2 num, int2 denum )
{
  if ( num == denum )
    return MAXINT2;
  else
    return (int2) ( ( ( (int4) num ) << 15 ) / denum );
}


/*
** int2 shl( int2 var1, int2 var2 )
**
** Function makes arithmetic var2-bit shift left of var1. If var2 is
** less than 0, this operation becomes arithmetic right shift of -var2
**
** Input:
**   var1
**     16-bit variable to be shifted
**   var2
**     amount of bits to be shifted
**
** Output:
**   16-bit value of shifted var1 is retuned 
**  
** Return value:
**   See above
**
** Notes:
**   ANSI C does not guarantee that right shift is arithmetical
**   (that sign extension is done during shifting). That is why in this
**   routine negative values are complemented before shifting.
*/
int2 shl( int2 var1, int2 var2 )
{
  int2 result;

  if ( ( var1 == 0 ) || ( var2 == 0 ) ) {
    result = var1;
  }
  
  /* var2 > 0: Perform left shift */
  else if ( var2 > 0 ) {
    if ( var2 >= 15 ) {
      result = ( var1 < 0 ) ? int2(MININT2) : int2(MAXINT2);
    }
    else {

      int4 L_temp;
      
      L_temp = (int4) var1 << var2;
      if ( L_temp < MININT2 ) {
	result = MININT2;
      }
      else if ( L_temp > MAXINT2 ) {
	result = MAXINT2;
      }
      else {
	result = (int2) L_temp;
      }
    }
  }
  /* var2 < 0: Perform right shift */
  else {
    if ( -var2 >= 15 ) {
      result = ( var1 < 0 ) ? int2 (-1) : int2 (0);
    }
    else if ( var1 < 0 ) {
      result = int2 (~( (~var1) >> -var2 )); /* ~ used to ensure arith. shift */
    }
    else {
      result = int2 (var1 >> -var2);
    }
  }

  return result;
  
}


/*
** int4 L_shl( int4 L_var1, int2 var2 )
**
** Function makes arithmetic var2-bit shift left of var1. If var2 is
** less than 0, this operation becomes arithmetic right shift of -var2
**
** Input:
**   L_var1
**     32-bit variable to be shifted
**   var2
**     amount of bits to be shifted
**
** Output:
**   32-bit value of shifted var1 is retuned 
**  
** Return value:
**   See above
**
** Notes:
**   ANSI C does not guarantee that right shift is arithmetical
**   (that sign extension is done during shifting). That is why in this
**   routine negative values are complemented before shifting.
*/

int4 L_shl(int4 L_var1, int2 var2 )
{
  if ( ( L_var1 == 0L ) || ( var2 == 0 ) ) {
    return L_var1;
  }
  /* var2 > 0: Perform left shift */
  else if ( var2 > 0 ) {
    if ( var2 >= 31 ) {
      return ( L_var1 < 0 ) ? MININT4 : MAXINT4;
    }
    else {
      for( ; var2 > 0; var2-- ) {
	if ( L_var1 > (MAXINT4 >> 1) )
	  return MAXINT4;
	else if ( L_var1 < (MININT4 / 2) )
	  return MININT4;
	else
	  L_var1 *= 2;
      }
      return L_var1;
    }
  }
  /* var2 < 0: Perform right shift */
  else {
    if ( -var2 >= 31 ) {
      return ( L_var1 < 0 ) ? -1L : 0L;
    }
    else if ( L_var1 < 0 ) {
      return ~( (~L_var1) >> -var2 ); /* ~ used to ensure arith. shift */
    }
    else {
      return L_var1 >> -var2;
    }
  }

}

