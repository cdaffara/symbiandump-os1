@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem
@echo off

@set generic_source=%CleanSourceDir%
@set techview_source=%CleanSourceDir%
@set platform_source=%CleanSourceDir%\%Platform%
@set KitToolsCommonDir=%CleanSourceDir%\common\%ProductDir%\tools
@set KitToolsPlatformDir=%CleanSourceDir%\%Platform%\%ProductDir%\tools

@set results=%ProductPath%\generated\logs

@set tool=%KitToolsCommonDir%\exportipr.pl
@set etel_script=%KitToolsPlatformDir%\etel.bld
@set ipv6_script=%KitToolsPlatformDir%\ipv6.bld
@set java_script=%KitToolsPlatformDir%\java.bld
@set midp81_script=%KitToolsPlatformDir%\midp81.bld
@set midp91_script=%KitToolsPlatformDir%\midp91.bld
@set midp92_script=%KitToolsPlatformDir%\midp92.bld
@set midp93_script=%KitToolsPlatformDir%\midp93.bld
@set midpFuture_script=%KitToolsPlatformDir%\midpFuture.bld
@set midp94_script=%KitToolsPlatformDir%\midp94.bld
@set midp95_script=%KitToolsPlatformDir%\midp95.bld
@set midp96_script=%KitToolsPlatformDir%\midp96.bld
@set midptb92_script=%KitToolsPlatformDir%\midptb92.bld
@set midptb101sf_script=%KitToolsPlatformDir%\midptb101sf.bld
@set crypto_script=%KitToolsPlatformDir%\crypto.bld
@set systemtest_script=%KitToolsPlatformDir%\systemtest.bld

@set path=%path%;%OutputDir%\generic\epoc32\gcc\bin\
@set EPOCROOT=\
@mkdir %results% 2> nul

@set gt_script=%OutputDir%\generic\epoc32\data\GT.txt
@set techview_script=%OutputDir%\generic\epoc32\data\TV.txt

perl %tool% -export -bldinf -src %generic_source% -genbuild %gt_script% > %results%\GTipr.log 2> %results%\GTerr.log
perl %tool% -restricted -cABCDEFGT -src %generic_source% -genbuild %gt_script% > %results%\GTrestrict.log 2> nul
perl %tool% -export -CA -src %generic_source% -genbuild %gt_script% > %results%\GTAexports.log 2> nul
perl %tool% -export -CB -src %generic_source% -genbuild %gt_script% > %results%\GTBexports.log 2> nul
perl %tool% -export -CI -src %generic_source% -genbuild %gt_script% > %results%\GTIexports.log 2> nul
perl %tool% -export -CO -src %generic_source% -genbuild %gt_script% > %results%\GTOexports.log 2> nul
perl %tool% -export -CC -src %generic_source% -genbuild %gt_script% > %results%\GTCexports.log 2> nul
perl %tool% -export -CDET -src %generic_source% -genbuild %gt_script% > %results%\GTDETexports.log 2> nul
perl %tool% -export -CFG -src %generic_source% -genbuild %gt_script% > %results%\GTFGexports.log 2> nul
perl %tool% -export -CX -src %generic_source% -genbuild %gt_script% > %results%\GTXexports.log 2> nul
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %etel_script% > %results%\Etel.log 2> %results%\Etelerr.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %crypto_script% > %results%\crypto.log 2> %results%\cryptoerr.log
perl %tool% -export -cABI -src %generic_source% -genbuild %ipv6_script% > %results%\ipv6ABIexports.log 2> %results%\ipv6err.log
perl %tool% -export -cABCDEFGT -src %platform_source% -genbuild %java_script% > %results%\javaexports.log 2> %results%\javaerr.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp81_script% > %results%\midp81exports.log 2> %results%\midp81err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp91_script% > %results%\midp91exports.log 2> %results%\midp91err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp92_script% > %results%\midp92exports.log 2> %results%\midp92err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp93_script% > %results%\midp93exports.log 2> %results%\midp93err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midpFuture_script% > %results%\midpFutureexports.log 2> %results%\midpFutureerr.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp94_script% > %results%\midp94exports.log 2> %results%\midp94err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp95_script% > %results%\midp95exports.log 2> %results%\midp95err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midp96_script% > %results%\midp96exports.log 2> %results%\midp96err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midptb92_script% > %results%\midptb92exports.log 2> %results%\midptb92err.log
perl %tool% -export -cABCDEFGT -src %generic_source% -genbuild %midptb101sf_script% > %results%\midptb101sfexports.log 2> %results%\midptb101sferr.log
perl %tool% -export -bldinf -src %techview_source% -genbuild %techview_script% > %results%\TVipr.log 2> %results%\TVerr.log
perl %tool% -restricted -cABCDEFGT -src %techview_source% -genbuild %techview_script% > %results%\TVrestrict.log 2> nul
perl %tool% -export -CA -src %techview_source% -genbuild %techview_script% > %results%\TVAexports.log 2> nul
perl %tool% -export -CB -src %techview_source% -genbuild %techview_script% > %results%\TVBexports.log 2> nul
perl %tool% -export -CI -src %techview_source% -genbuild %techview_script% > %results%\TVIexports.log 2> nul
perl %tool% -export -CO -src %techview_source% -genbuild %techview_script% > %results%\TVOexports.log 2> nul
perl %tool% -export -CC -src %techview_source% -genbuild %techview_script% > %results%\TVCexports.log 2> nul
perl %tool% -export -CDET -src %techview_source% -genbuild %techview_script% > %results%\TVDETexports.log 2> nul
perl %tool% -export -CDET -src %techview_source% -genbuild %systemtest_script% > %results%\systemtestDETexports.log 2> nul
perl %tool% -export -CFG -src %techview_source% -genbuild %techview_script% > %results%\TVFGexports.log 2> nul
perl %tool% -export -CX -src %techview_source% -genbuild %techview_script% > %results%\TVXexports.log 2> nul
