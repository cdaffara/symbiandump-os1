#!perl

# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

package KitStandardLocations;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw($SourceDir $BuildDir $OutputDir $Platform $ToolsDir $LogsDir $ZipDir $PublishLocation $ProductPath $BuildBaseName $ProductDir $CustKitDir $DevKitDir $SuppKitDir $SrcDefDir $PkgDefDir $CustKitPath $DevKitPath $SuppKitPath $pkgDefDevKitDir $pkgDefCustKitDir $outputDevKitDir $outputCustKitDir);


# Standard locations from environment variables
$SourceDir       = $ENV{'CleanSourceDir'};      # path to clean source 
$BuildDir        = $ENV{'BuildDir'};            # path to build directory ([substed drive]\)
$OutputDir       = $ENV{'OutputDir'};           # path to where the binaries are put ([substed drive]\bin\platform)
$Platform        = $ENV{'Platform'};            # beech, cedar etc. 
$ToolsDir        = $ENV{'ToolsDir'};            # path to main tools (IPR tool)
$LogsDir         = $ENV{'LogsDir'};             # path to main build logs direrctory 
$ZipDir          = $ENV{'ZipDir'};              # path to ZIPs directory (for lockit)
$PublishLocation = $ENV{'PublishLocation'};     # path to DevBuilds publish location
$ProductPath     = $ENV{'ProductPath'};         # path to output directory for Kits	
$BuildBaseName   = $ENV{'BuildBaseName'};       # name of the build (e.g. "Symbian_OS_v8.0a")    


# Kit-specific locations
$ProductDir   = "Product";
$CustKitDir   = "CustKit";
$DevKitDir    = "DevKit";
$SuppKitDir   = "SuppKit";
$SrcDefDir    = "SourceDefinitions";
$PkgDefDir    = "PackageDefinitions";
$CustKitPath  = "$ProductPath\\$CustKitDir";
$DevKitPath   = "$ProductPath\\$DevKitDir";
$SuppKitPath  = "$ProductPath\\$SuppKitDir";
$pkgDefDevKitDir  = "$DevKitPath\\$PkgDefDir";
$pkgDefCustKitDir = "$CustKitPath\\$PkgDefDir";
$outputDevKitDir  = "$ProductPath\\_PACKAGES\\DevKit_CD";
$outputCustKitDir = "$ProductPath\\_PACKAGES\\CustKit_Extra_CD";



# set environment variables for non source rebaseline batch files
$ENV{'ProductDir'} = $ProductDir ;



