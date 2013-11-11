# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# use strict;
# 
#

use Math::Complex;
use Math::Trig;
require 'getopts.pl';

my $version = "0.2 VRML (28/12/07)";
my $animate = 0;

#
# Notes for future merging with parseseq.pl:
# - usage of %symtab with "oc" and "od"

#
# Abstract principles:
#
# Objects are conceptually placed within a variable sized rectangular "super" grid.  Each position of this
# grid represents a particular "type" of object (e.g. IP SubConnection Provider).  More than one object
# (of the same type) can occupy the same position on this grid.  Consequently, each grid position has
# within it a variable sized rectangular "sub-grid" that is used to define the placement of each of the
# individual objects that occupy the same super grid position.  The object placement within the sub-grid
# is calculated to avoid connecting lines to neighbouring objects from passing through other objects
# (this is, in fact, the raison d'etre for the sub-grid concept).  Note that the sub-grid can have
# larger dimensions than the number of objects within it - i.e. it can have "empty" spaces.  This
# is a consequence of the simple placement algorithm:
#
# Each sub-grid in the super grid has a (subRow,subCol) variable that starts with the value (0,0).  The
# first object is placed at (0,0).  For every subsequent object needs to be placed, if there are has
# any horizontal connections ("west" or "east") to neighbouring objects, then (subRow) variable is
# incremented.  if there are any vertical connections, then (subCol) variable is incremented.  The object
# is then placed at the final (subRow,subCol) position within the sub-grid, and process repeats.
#
# Each set of inter-related objects forms a "group".  The relationship is based around the exchange of
# messages.  There may be several such distinct and seperate groups.  By definition, no object in one
# group has any message exchange with an object in another group.
#
# Example of two groups:
#
#	group 1:
#
#	A -- B -- C
#		 |
#		 D
#
#	group 2:
#
#	E -- F -- G
#	|
#	H
#
# Two groups are merged into one if, whilst reading the message sequence file, a message is found to flow from
# an object in one group is sent to an object in another group.  For example, if object D sends a message to
# object E, then a larger group is formed:
#
#
#	group 1:
#
#	A -- B -- C
#		 |
#		 D
#		 |
#		 E -- F -- G
#		 |
#		 H



#
# Each object is a hash:
#	n => array of object references to objects that are "north" of this object
#	s => array of object references to objects that are "south" of this object
#	w => array of object references to objects that are "west" of this object
#	e => array of object references to objects that are "east" of this object
#	group => group number.  Each group is a set of interconnected objects.
#	col => column number of object within super grid
#	row => row number of object within super grid
#	subCol => column number of object within sub-grid (within supergrid col,row position)
#	subRow => row number of objects within sub-grid (within supergrid col,row position)
#
# subGridPlacement[<col>][<row>] is a hash containing the (subRow,subCol) variable pair for a sub-grid at
# a particular <col> and <row> in the overall grid:
#	subCol => <subCol>
#	subRow => <subRow>
#
# colToSubColIndex[<col>] and rowToSubRowIndex[<row>] are arrays containing the cumulative total number of sub-grid
# columns and rows, respectively, at a particular <col> and <row> position in the super grid.
# For example, if the first, second and third columns of the super grid contain sub-grids of
# 2, 1 and 3 columns respectively, then colToSubColIndex[] contains 0, 2, 3, 6, ...).
#
# colToSubGridWidth[<col>] and rowToSubGridHeight[<row>] are arrays containing the width and height respectively
# of the sub-grid at a particular <col> and <row> position in the super grid, respectively.
#
# gridVerticalLineCount[<col>] and gridHorizontalLineCount[<row>] are arrays containing the number
# of "long" lines that run vertically and horizontally at certain <col> and <row> positions in the
# grid for connecting objects that are not neighbours.  Objects connected to immediate neighbours
# do not have "long" lines and are not counted here.
#
# groupRanges{<group>} is a hash which contains
#	minCol => minimum column number of any object within the group
#	maxCol => maximum column number of any object within the group
#	minRow => minimum row number of any object within the group
#	maxRow => maximum row number of any object within the group
#	row => row number that the group starts at within the super grid
#

# VRML line array elements
use constant X => 0;
use constant Y => 1;
use constant Z => 2;
# fonts
use constant FONT_FAMILY => "SANS";
use constant Z_POSITION => 0;

#
# Flags
my $displayObjectAddressFlag = 0;		# display object hex address alongside name

my $group = 1;							# next free group number (monotonically incremented after allocation)
my $groupVerticalSeperation = 3;

my %directionTranslate = ( n => { x =>  0, y => -1 },
						   s => { x =>  0, y => +1 },
						   w => { x => -1, y =>  0 },
						   e => { x => +1, y =>  0 } );

my %initRange = ( minCol => 9999, minRow => 9999, maxCol => -9999, maxRow => -9999 );
my %overallRange = %initRange;

# tabLabel is a monotonically increasing value used to derive a unique Tab Label.  It is incremented
# everytime a new Tab is drawn.

my $tabLabel = "a";

my @colToSubColIndex;
my @rowToSubRowIndex;
my @rowToSubGridHeight;
my @colToSubGridWidth;
my %relationships;
my %groupRanges;
my @objects;
my @subGridPlacement;

Getopts("x:vV");

readRelationshipFile();

print "Reading sequences\n" if ($opt_V);
readSequences();
#printGroupRanges();

#printRelationships();
print "Normalising coordinates\n" if ($opt_V);
normaliseCoords();
print "Seperating groups\n" if ($opt_V);
seperateGroups();
print "Seperating overlaps\n" if ($opt_V);
seperateOverlaps();

# print2DArray("subGridPlacement", \@subGridPlacement);

print "Calculating grid positions\n" if ($opt_V);
calculateGridPositions();

if ($opt_v) {
	printArray("colToSubColIndex", \@colToSubColIndex);
	printArray("rowToSubRowIndex", \@rowToSubRowIndex);

	printArray("rowToSubGridHeight", \@rowToSubGridHeight);
	printArray("colToSubGridWidth", \@colToSubGridWidth);

	printLoners();
	printRelationships();
	printGroupRanges();
}

#my $outputFormat = "svg";
my $outputFormat = "vrml";
my $adjustFactor;
if ($outputFormat eq "vrml") {
	$adjustFactor = 0.1;
} else {
	$adjustFactor = 1;
}

#VRML specific
my $vrmlCycleInterval = 0.2;
my $boxDepth = 20 * $adjustFactor;
my $lineThickness = 2 * $adjustFactor;
my $lineExtensionFactor = 1.01;			# *** REMOVE THIS ***
my $initialTransparency = $animate ? 1 : 0;
my $boxDepthMargin = 120 * $adjustFactor;
my $boxDepthWithMargin = $boxDepth + $boxDepthMargin;
my $boxInternalTextColour = colourString(0,100,100);

my $tabSpacing = 10 * $adjustFactor;
# minor aesthetic adjustments to the spacing between the end of a Tab line and its Labels
my $tabToFontAdjustTopY = 2 * $adjustFactor;
my $tabToFontAdjustBottomY = 1 * $adjustFactor;
my $tabLineLengthY = 5 * $adjustFactor;

my $fontHeight = 9 * $adjustFactor;				# assumed font height
my $boxInternalFontHeight = $fontHeight / 3;
my $groupHighlightFill = colourString(235,235,235);
my $groupHighlightMargin = 5 * $adjustFactor;			# margin of the group background rectangle

my $horizMargin = 20 * $adjustFactor;
my $rectangleMargin = 15 * $adjustFactor;

#Sphere
#my $rectangleWidth = 50 * $adjustFactor;
#my $rectangleHeight = 50 * $adjustFactor;
my $rectangleWidth = 110 * $adjustFactor;
my $rectangleHeight = 27 * $adjustFactor;

my $rectangleWidthWithMargin = $rectangleWidth + $rectangleMargin;
my $rectangleHeightWithMargin = $rectangleHeight + $rectangleMargin;
my $vertMargin = $horizMargin;

my $gridXSpacing = $horizMargin;
my $gridYSpacing = $gridXSpacing;

my $rectangleOpacity = 0.80;
my $rectangleColour1 = colourString(144,238,144);		# "lightgreen"
my $rectangleColour2 = colourString(135,206,250);		# "lightskyblue"
my $rectHighlightColour = colourString(250,128,114);		# "salmon"

# X position of last grid column + size of last grid column + horizontal margin
my $width = gridX($#colToSubColIndex) + $colToSubGridWidth[$#colToSubColIndex] * $rectangleWidthWithMargin + $horizMargin;
my $height = gridY($#rowToSubRowIndex) + $rowToSubGridHeight[$#subGridRow] * $rectangleHeightWithMargin + $vertMargin;

createOutputFile();
outputDocHeader($width,$height);
# Not for VRML (yet)
#outputGroupHighlight();
print "Generating links\n" if ($opt_V);
outputLinks();
print "Generating nodes\n" if ($opt_V);
outputNodes();
outputDocFooter();
if ($animate) {
	print "Generating animation\n" if ($opt_V);
	AnimateDo();
}
closeOutputFile();
if ($outputFormat eq "svg")
	{
	outputRelationsHTMLEmbedder($width, $height);
	}

sub colourString($$$)
	{
	my ($r,$g,$b) = @_;
	if ($outputFormat eq "svg") {
		return "rgb($r,$g,$b)";
		}
	else {
		my $str = sprintf("%.3f %.3f %.3f", $r/255, $g/255, $b/255);
		return $str;
		}
	}

sub createOutputFile()
	{
        if ($outputFormat eq "svg")
        	{
		open SVG, ">relations.svg" || die "Cannot open relations.svg for writing\n";
		}
	else
	    	{
	    	open WRL, ">relations.wrl" || die "Cannot open relations.wrl for writing\n";
		}
	}

sub closeOutputFile()
	{
	if ($outputFormat eq "svg")
		{
		close SVG;
		}
	else
		{
		close WRL;
		}
	}

sub readSequences()
	{
	while (<>) {
		die unless s/^(\w+)\s+//;
		my $action = $1;
		if ($action eq "p" || $action eq "r" || $action eq "sc")
			{
			# Post/Receive
			# [P|R] <message> <source object> <destination object>
			my $msg;
			if (/^"/)
				{
				s/^"([^"]+)"\s+//;
				$msg = $1;
				}
			else
				{
				s/^(\S+)\s+//;
				$msg = $1;
				}
			$msg =~ s/ \(.*\)//;		# remove message arguments in parentheses: "<message>(<arguments>)" => "<message>"
			split;
			my $srcRef = findOrCreateObject(shift @_);
			my $destRef = findOrCreateObject(shift @_);
			my $direction = $relationships{$msg};
			if ($direction) {
				if (isDirection($direction)) {
					AddRelation($srcRef, $destRef, $direction);
				}
				}
			elsif ($srcRef != $destRef) {
#				print "Unknown relationship: $srcRef->{Name} -- $msg --> $destRef->{Name}\n";
				}
			AnimateObjectMessage($srcRef, $destRef, $direction) if $animate;
			}
		elsif ($action eq "oc")
			{
			# Object Create
			# oc <object name> <order> <addr>
			split;
			my $objRef = findOrCreateObject(shift @_);
			shift @_;							# order - can we use this to define object column placement ?
			$objRef->{Addr} = shift @_;
			AnimateObjectCreate($objRef) if $animate;
			}
		elsif ($action eq "od")
			{
			# Object Destroy
			# od <object name> <addr>
			split;
			my $objRef = findObject(shift @_);
			if ($objRef != 0) {
				AnimateObjectDestroy($objRef) if $animate;
				}
			}
		}
	}

sub AddRelation($$$)
	{
	my ($srcRef,$destRef,$dir) = @_;

	my $rdir = reverseDirection($dir);

	if ($srcRef != $destRef && ! RelationExists($srcRef->{$dir}, $destRef))
		{
		push @{$srcRef->{$dir}}, $destRef;
		push @{$destRef->{$rdir}}, $srcRef;

		# my %range = %initRange;

		# we are about to merge groups, so delete the group about to be assimilated from the group ranges hash
		delete $groupRanges{$destRef->{group}};

		my $rangeRef = \%{$groupRanges{$srcRef->{group}}};

		MergeGroups($destRef->{group}, $srcRef->{group},
					$srcRef->{col} + $directionTranslate{$dir}{x} - $destRef->{col},
					$srcRef->{row} + $directionTranslate{$dir}{y} - $destRef->{row}, $rangeRef );

		# MergeGroups() above will automatically adjust the min/max range values in range for all the "incoming"
		# objects.  However, we still have to take into account the effect that the original src object has on
		# the min/max range values, so call updateRange() specially here for it.
		updateRange($rangeRef, $srcRef->{col}, $srcRef->{row});

		# Ultimately we need to place objects on the super grid according to what type they are - for example,
		# flows in col 0, SCPRs in col 1 etc.  At the moment, they are mixed up in the same super grid, as
		# some groups may have flow, some may not, but all groups are left aligned.  This explains why
		# the display is messed up with MCPRs and CPRs occupying the same column in different groups.

		updateRange(\%overallRange, $rangeRef->{minCol}, $rangeRef->{minRow});
		updateRange(\%overallRange, $rangeRef->{maxCol}, $rangeRef->{maxRow});
		}
	}

#
# Indicate whether the string passed is a (compass) direction.
#
sub isDirection($)
{
	my $dir = $_[0];
	return $dir eq "n" || $dir eq "s" || $dir eq "w" || $dir eq "e";
}

sub MergeGroups($$$$)
	{
	my ($targetGroup, $newGroup, $adjustCol, $adjustRow, $rangeRef) = @_;
	
	foreach my $obj (@objects) {
		if ($obj->{group} == $targetGroup) {
#			print "Merge $obj->{Name}, group $obj->{group} => $newGroup, ($obj->{col},$obj->{row}) => ";
			$obj->{group} = $newGroup;
			$obj->{col} += $adjustCol;
			$obj->{row} += $adjustRow;
#			print "($obj->{col},$obj->{row})\n";
			# determine minimum and maximum row,col extent of group
			updateRange($rangeRef, $obj->{col}, $obj->{row});
			}
		}
	}

sub updateRange($range, $col, $row)
	{
	my ($range, $col, $row) = @_;

	if (!defined($range->{maxCol}) || $col > $range->{maxCol}) {
		$range->{maxCol} = $col;
		}
	if (!defined($range->{maxRow}) || $row > $range->{maxRow}) {
		$range->{maxRow} = $row;
		}
	if (!defined($range->{minCol}) || $col < $range->{minCol}) {
		$range->{minCol} = $col;
		}
	if (!defined($range->{minRow}) || $row < $range->{minRow}) {
		$range->{minRow} = $row;
		}
	}

sub RelationExists($$)
	{
	my ($relationArrayRef, $objRef) = @_;

	if (defined ($relationArrayRef))
		{
		foreach my $obj (@{$relationArrayRef})
			{
			if ($obj == $objRef)
				{
				return 1;
				}
			}
		}
	else
		{
		return 0;
		}
	}

sub reverseDirection($)
	{
	my ($dir) = @_;
	my %reverseMap = ( "n" => "s", "s" => "n", "w" => "e", "e" => "w" );
	return $reverseMap{$dir};
	}

#
# Print objects that exchange no messages with anyone else.
#
sub printLoners()
	{
	my $wrapCount = 8;
	my $wrapIndex = 1;
	print "Loners:\t";
	my $commaRequired = 0;
	for (my $i = 0 ; $i <= $#objects ; ++$i)
		{
		my $obj = $objects[$i];
		if (!defined($obj->{subCol}) && !defined($obj->{subRow}))
			{
			if ($commaRequired != 0)
				{
				print ", ";
				}
			$commaRequired = 1;
			print $obj->{Name};
			if (($wrapIndex++ % $wrapCount) == 0)
				{
				$commaRequired = 0;
				print "\n\t";
				}
			}
		}
	print "\n\n";
	}

sub printRelationships()
	{
	print "Relationships - overallRange: ($overallRange{minCol}, $overallRange{minRow}) to ($overallRange{maxCol}, $overallRange{maxRow}):\n";
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ++$i)
		{
		printObjectRelationships($objects[$i]);
		}
	print "\n";
	}

sub printObjectRelationships($)
	{
	my ($obj) = @_;

	# Object
	print $obj->{Name}, ": group $obj->{group}, ($obj->{col},$obj->{row}), sub ($obj->{subCol},$obj->{subRow})\n";

	# North
	if (defined($obj->{n}))
		{
		print "\tN: ";
		printObjects($obj->{n});
		print "\n";
		}
	
	# West
	if (defined($obj->{w}))
		{
		print "\tW: ";
		printObjects($obj->{w});
		print "\n";
		}


	# East
	if (defined($obj->{e}))
		{
		print "\tE: ";
		printObjects($obj->{e});
		print "\n";
		}

	# South
	if (defined($obj->{s}))
		{
		print "\tS: ";
		printObjects($obj->{s});
		print "\n";
		}

	print "\n";
	}

sub printObjects($)
	{
	my ($relationArray) = @_;

	my $commaRequired = 0;
	foreach my $obj (@{$relationArray})
		{
		if ($commaRequired == 1)
			{
			print ", ";
			}
		print $obj->{Name};
		$commaRequired = 1;
		}
	}

sub findObject($)
	{
	my $objName = $_[0];
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ++$i) {
		if ($objects[$i]{Name} eq $objName) {
			return $objects[$i];
			}
		}
	return 0;
	}

sub findOrCreateObject($)
	{
	my $objName = $_[0];
	my $objRef = findObject($objName);
	if ($objRef != 0) {
		return $objRef;
		}
	else {
		my $objectCount = scalar(@objects);
		$objects[$objectCount] = { Name => $objName, row => 0, col => 0, group => $group++ };
#		print "findOrCreateObject: $objects[$i]->{Name}, group $objects[$i]->{group}, ($objects[$i]->{col},$objects[$i]->{row})\n";
		return $objects[$objectCount];
		}
	}

sub readRelationshipFile()
	{
	if ($opt_x) {
		open (REL, $opt_x) || die "Cannot open exclude file $opt_x\n";
		while (<REL>) {
			chomp;
			# remove leading and trailing blanks and ignore blank lines
			s/^\s+//;
			s/\s+$//;
			if (! $_)
				{ next;	};

			my ($message,$direction) = split;
			$relationships{$message} = $direction;
			}
		close REL;
		}
	}

#######################
# SVG output routines
#######################

sub outputDocHeader()
	{
	if ($outputFormat eq "svg")
		{
		outputSVGDocHeader();
		}
	else
		{
		outputVRMLDocHeader();
		}
	}

sub outputSVGDocHeader()
	{
	my ($width,$height) = @_;
	print SVG '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',"\n";
	print SVG '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">',"\n";
	print SVG "<svg height=\"$height\" width=\"$width\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
	# place popup rectangle before all other text, otherwise it will be on top of rather than
	# underneath the popup text
	}

sub outputText($$$$$$$)
	{
	if ($outputFormat eq "svg")
		{
		outputSVGText(@_);
		}
	else
		{
		outputVRMLText(@_);
		}
	}

# Not working
sub outputTextAlongLine($$$$$$$)
	{
	if ($outputFormat eq "svg") {
		return;
		}
	my ($x1,$y1,$z1,$x2,$y2,$z2,$text) = @_;

	my $rotation = "";
		{
		my $rotateRef = calcAxialRotationAndLength([$x1, -$y1, $z1] , [$x2, -$y2, $z2]);
		printHash("Text Rotation", $rotateRef);
		print "\n";
	        if (exists $rotateRef->{angle})
	        	{
			$rotation = "vrml: rotation $rotateRef->{x} $rotateRef->{y} $rotateRef->{z} " . ($rotateRef->{angle} - (pi/2));
		        }
		}
	outputText(($x1 + $x2) / 2, $text, ($y1 + $y2) / 2, ($z1 + $z2) / 2, "middle", $rotation, "");
	}


sub outputSVGText($$$$$$$)
	{
	my ($x,$text,$y,$z,$anchor,$otherSvgAttr,$colour) = @_;

	my $attrs = qq{ x="$x" y="$y" };

	if ($otherSvgAttr)
		{
		$attrs .= qq { $otherSvgAttr  };
		}

	if ($anchor)
		{
		$attrs .= qq { text-anchor="$anchor" };
		}

	if ($colour)
		{
		if ($colour =~ s/^!//)
			{
			outputRect($x - 1, $y - $fontHeight, $z, 3, $fontHeight, "white", "white", "");
			}
		if ($colour)
			{
			$attrs .= qq { fill="$colour" };
			}
		}

	print SVG "<text $attrs>$text</text>\n";
	}

sub outputLine($$$$$$$$)
	{
	if ($outputFormat eq "svg")
		{
		outputSVGLine(@_);
		}
	else
		{
		outputVRMLLine(@_);
		}
	}

sub outputSVGLine($$$$$$$$)
	{
	my ($x1,$y1,$z1,$x2,$y2,$z2,$colour,$otherAttr) = @_;

	if (! $colour) {
		$colour = "black";
	}

	print SVG qq{<line stroke="$colour" x1="$x1" y1="$y1" x2="$x2" y2="$y2" $otherAttr />\n};
	}

sub outputVerticalLine()
	{
	if ($outputFormat eq "svg")
		{
		outputSVGVerticalLine(@_);
		}
	else
		{
		outputVRMLVerticalLine(@_);
		}
	}

sub outputSVGVerticalLine()
	{
	my ($x,$y1,$y2,$colour) = @_;
	if (! $colour) { $colour = "black"; }
	print SVG qq{<line stroke="$colour" x1="$x" y1="$y1" x2="$x" y2="$y2" />\n};
	}

sub outputRect($$$$$$$$)
	{
	if ($outputFormat eq "svg")
		{
		outputSVGRect(@_);
		}
	else
		{
		outputVRMLRect(@_);
		}
	}

sub outputSVGRect($$$$$$$)
	{
	my ($x,$y,$z,$width,$height,$fill,$stroke,$otherAttr) = @_;
	print SVG qq {<rect fill="$fill" stroke="$stroke" x="$x" y="$y" width="$width" height="$height" $otherAttr />\n};
	}

sub outputDocFooter()
	{
	if ($outputFormat eq "svg")
		{
		outputSVGDocFooter();
		}
	}

sub outputSVGDocFooter()
	{
###################################
#  Begin Interpolated Text        #
###################################
	print SVG <<'EOT'
<script><![CDATA[

var highlightedNodes = new Array();
var highlightedNodesAttr = new Array();
bindToParent();

function bindToParent()
	{
	parent.relationsMessageHighlightPtr = messageHighlight;
	parent.relationsObjectHighlightPtr = objectHighlight;
	}

function sequenceMessageHighlight(id) {
	if (!opener.closed) {
		opener.sequenceMessageHighlightPtr(id);
	}
}

function sequenceObjectHighlight(id) {
	if (!opener.closed) {
		opener.sequenceObjectHighlightPtr(id);
	}
}

function MessageEvent(evt)
	{
	sequenceMessageHighlight(evt.currentNode.id);
	messageHighlight(evt.currentNode.id);
	}

function NodeEvent(evt)
	{
	sequenceObjectHighlight(evt.currentNode.id);
	objectHighlight(evt.currentNode.id);
	}

function display(obj)
	{
	var s = "";
	var t = "";
	for (i in obj)
		{
		t = typeof obj[i];
		s += i + " = ";
		if (t != "unknown") {
			var u = "";
			u += obj[i];
			if (u.indexOf("function") < 0)
				{
				s += u;
				}
			else
				{
				s += "(function)";
				}
			}
		else
			{
			s += "(unknown)";
			}
		s += ",   ";
		}
	alert(s);
	}

function messageHighlight(nodeNamesString)
	{
	// given a string "x_y" find all nodes with tags "x", "y", "x_y" or "y_x"
	var nodeNames = nodeNamesString.split("_", 2);

	if (nodeNames.length == 2)
		{	
		UnhighlightCurrentNodes();
		var nodeList = svgDocument.getElementsByTagName("*");
		var count = nodeList.length;
		var el;
		var reverseNodeNamesString = nodeNames[1] + "_" + nodeNames[0];
		highlightIndex = 0;
		for (i = 0 ; i < count ; i++)
			{
			el = nodeList.item(i);
			if (el.id == nodeNames[0] || el.id == nodeNames[1] || el.id == nodeNamesString || el.id == reverseNodeNamesString)
				{
				highlightNode(el);
				}
			}
		}
	}

function objectHighlight(nodeNameString)
	{
	// given a string "x" find all nodes with tag "x"
	UnhighlightCurrentNodes();

	// OPTIMISE nodeList
	var nodeList = svgDocument.getElementsByTagName("*");
	var count = nodeList.length;
	var el;
	for (i = 0 ; i < count ; i++)
		{
		el = nodeList.item(i);
		if (el.id == nodeNameString)
			{
			highlightNode(el);
			}
		}
	}

function highlightNode(el)
	{
	if (el.tagName == "rect")
		{
		highlightedNodes.push(el);
		highlightedNodesAttr.push(el.getAttribute("fill"));
		el.setAttribute("fill", "lightsalmon");
		}
	}

function UnhighlightCurrentNodes()
	{
	while(highlightedNodes.length > 0)
		{
		highlightedNodes.pop().setAttribute("fill", highlightedNodesAttr.pop());
		}
	}

function matchName(name, targetName)
	{
	var nameLen = name.length;
	var targetNameLen = targetName.length;
	var pos, underlinePos;

	pos = targetName.indexOf(name);
	if (pos != -1)
		{
		// check for exact match
		if (pos == 0 && nameLen == targetNameLen)
			{
			return true;
			}
		else
			{
			// match "x" against "x_y" or "y_x"
			underlinePos = targetName.indexOf("_");
			if ( underlinePos != -1 && ((pos == 0 && underlinePos == nameLen) || (pos == targetNameLen - nameLen)) )
				{
				return true;
				}
			}
		}
	return false;
	}
]]></script>
</svg>
EOT
#################################
#  End Interpolated Text        #
#################################
	}

sub outputRelationsHTMLEmbedder($$)
	{
	my ($width,$height) = @_;
	open HTML, ">relations.html" || die "Cannot open relations.html for writing\n";
	print HTML qq{<html>\n<body onLoad="resize()">\n};
	print HTML qq{<embed src="relations.svg" width="$width" height="$height" type="image/svg+xml" pluginspage="http://www.adobe.com/svg/viewer/install/" name="embedder">\n};
	print HTML qq{<script type="text/javascript">\n};
	print HTML "var relationsWidth = $width;\n";
	print HTML "var relationsHeight = $height;\n";
	print HTML<<'EOT'
function resize() {
	if (opener != null) {
		window.resizeTo(relationsWidth + 55, relationsHeight + 70);
	} else {
		window.resizeTo(relationsWidth + 55, relationsHeight + 180);
	}
}
</script>
</body></html>
EOT
;
	close HTML;
	}


###############################################
#
# VRML Routines
#
###############################################

sub outputVRMLDocHeader()
	{
	###########################
	# Begin Interpolated Text #
	###########################
	print WRL<<END
#VRML V2.0 utf8

WorldInfo {
   title "Node Relationships"
   info ["Nodes"]
}
Viewpoint {
   position 30 -40 120
   orientation 1 0 0 0
   fieldOfView 0.78
   description "Camera 1"
}
END
	#########################
	# End Interpolated Text #
	#########################
	}

sub formatVrmlCoord($$) {
        my $coordP = $_[0];
        my $sep = $_[1];
	return $coordP->[X] . $sep . $coordP->[Y] . $sep . $coordP->[Z];
}

sub outputVRMLText($$$$$$$)
	{
	my ($x,$text,$y,$z,$anchor,$otherAttr,$colour) = @_;
#	print "outputVRMLText(x=$x, text=$text, y=$y, z=$z, anchor=$anchor, attr=$otherAttr, colour=$colour)\n";
	my $coordRef = [ $x, -$y, $z ];

	if ($colour !~ /^\d/) {
		$colour = "1 1 1";
		}
	my $rotation = "";
	my $def = "";
	my $size = $fontHeight;			# default
	if (hasVrmlTokens(\$otherAttr) == 1) {
		my $s = "";
		while ($s = getTokens(\$otherAttr)) {
			if ($s =~ /^rotation /) {
				$rotation = $s;
			} elsif ($s =~ /^size\s*(.*)/) {
				$size = $1;
			} elsif ($s =~ /DEF\s*(\S+)/) {
				$def = $1;
			}
		}
	}
	print WRL "Transform {\n";
	if ($rotation ne "") {
		print WRL "	$rotation\n";
	}
	print WRL "	translation ", formatVrmlCoord($coordRef, " "), "\n";
	print WRL "	children Shape {\n";
	print WRL "		appearance Appearance {\n";
	if ($def) {
		print WRL "			material DEF $def Material {\n";
	} else {
		print WRL "			material Material {\n";
	}
	print WRL "				diffuseColor $colour\n";
	print WRL "				emissiveColor $colour\n";
	print WRL "				transparency $initialTransparency\n";
	print WRL "			}\n";
	print WRL "		}\n";
	print WRL "		geometry Text {\n";
	print WRL "			string \"$text\"\n";
	print WRL "			fontStyle FontStyle {\n";
	print WRL "				size $size\n";
	print WRL qq{				justify "MIDDLE"\n };
	print WRL "				family \"", FONT_FAMILY, "\"\n";
#	print WRL qq{				style "BOLD"\n };
	print WRL "			}\n";
	print WRL "		}\n";
	print WRL "	}\n";
	print WRL "}\n";

	}

sub outputVRMLLine($$$$$$$$)
	{
	my ($x1,$y1,$z1,$x2,$y2,$z2,$colour,$otherAttr) = @_;
#	print "outputVRMLLine(x1=$x1, y1=$y1, z1=$z1, x2=$x2, y2=$y2, z2=$z2, colour=$colour, attr=$otherAttr)\n";

	my $def;
	if (hasVrmlTokens(\$otherAttr) == 1) {
		my $s = "";
		while ($s = getTokens(\$otherAttr)) {
			if ($s =~ /^DEF\s*(\S+)/) {
				$def = $1;
			}
		}
	}


        my $p1Ref = [ $x1, -$y1, $z1 ];
	my $p2Ref = [ $x2, -$y2, $z2 ];
        $colour = "1 1 1";
	my $rotateRef = calcAxialRotationAndLength($p1Ref, $p2Ref);
#	printHash("Rotation", $rotateRef);
#	print "\n";
	my @lineCentre = (($p1Ref->[X] + $p2Ref->[X]) / 2, ($p1Ref->[Y] + $p2Ref->[Y]) / 2, ($p1Ref->[Z] + $p2Ref->[Z]) / 2);
        print WRL "Transform {\n";
        if (exists $rotateRef->{angle} && $rotateRef->{angle} != 0)
        	{
	        print WRL "	rotation $rotateRef->{x} $rotateRef->{y} $rotateRef->{z} $rotateRef->{angle}\n";
	        }
        print WRL "	translation @lineCentre\n";
        print WRL "	children Shape {\n";
        print WRL "		appearance Appearance {\n";
        if ($def) {
	        print WRL "			material DEF $def Material {\n";
	} else {
	        print WRL "			material Material {\n";
	}
        print WRL "				diffuseColor $colour\n";
#        print WRL "				emissiveColor 1 0 0\n";
        print WRL "				transparency $initialTransparency\n";
        print WRL "			}\n";
        print WRL "		}\n";
        print WRL "		geometry Cylinder {\n";
        print WRL "			bottom FALSE\n";
        print WRL "			top FALSE\n";
        print WRL "			radius $lineThickness\n";
        print WRL "			height $rotateRef->{length3D}\n";
        print WRL "		}\n";
        print WRL "	}\n";
        print WRL "}\n";
	}

sub outputVRMLVerticalLine()
	{
	my ($x,$y1,$y2,$colour) = @_;
#	print "outputVRMLVerticalLine(x=$x, y1=$y1, y2=$y2, colour=$colour)\n";
	outputVRMLLine($x, $y1, 0, $x, $y2, 0, $colour, "");
	}

sub outputVRMLRect($$$$$$$$)
	{
	my ($x,$y,$z,$width,$height,$fill,$stroke,$otherAttr) = @_;
#	print "outputVRMLRect(x=$x, y=$y, z=$z, width=$width, height=$height, fill=$fill, stroke=$stroke, attr=$otherAttr)\n";

	my $def;
	if (hasVrmlTokens(\$otherAttr) == 1) {
		my $s = "";
		while ($s = getTokens(\$otherAttr)) {
			if ($s =~ /^DEF\s*(\S+)/) {
				$def = $1;
			}
		}
	}

        my $posRef = [ $x + ($width/2), -($y + $height/2), $z ];
	my $sizeRef = [ $width, $height, $boxDepth ];
	print WRL "Transform {\n";
        print WRL "	translation @{$posRef}\n";
        print WRL "	children Shape {\n";
        print WRL "		appearance Appearance {\n";
        if ($def) {
	        print WRL "			material DEF $def Material {\n";
	} else {
	        print WRL "			material Material {\n";
	}
        print WRL "				diffuseColor $fill\n";
#        print WRL "				emissiveColor $fill\n";
        print WRL "				transparency $initialTransparency\n";
        print WRL "			}\n";
        print WRL "		}\n";
        print WRL "		geometry Box {\n";
        print WRL "			size @{$sizeRef}\n";
#Sphere
#        print WRL "		geometry Sphere {\n";
#        print WRL "			radius ", $width/2, "\n";
        print WRL "		}\n";
        print WRL "	}\n";
        print WRL "}\n";
	}

sub calcAxialRotationAndLength($$)
        {
        my ($p1, $p2) = @_;             # references
	if ($p1->[Y] >= $p2->[Y]) {
		my $temp = $p1;
		$p1 = $p2;
		$p2 = $temp;
		}
        my $x = ($p2->[X] - $p1->[X]);
        my $y = ($p2->[Y] - $p1->[Y]);
        my $z = ($p2->[Z] - $p1->[Z]);
        my $lengthXYZ = sqrt($x*$x + $y*$y + $z*$z);
        my $rotationAngle = asin($y / $lengthXYZ);
	$rotationAngle = (pi/2) - $rotationAngle;
        # degenerate cases
        $lengthXYZ *= $lineExtensionFactor;
        if ($z == 0)
        	{
        	return { x => 0, y => 0, z => -1, angle => $rotationAngle, length3D => $lengthXYZ };
        	}
        elsif ($x == 0)
        	{
		if ($z < 0) {
			$rotationAngle = -$rotationAngle;
			}
        	return { x => 1, y => 0, z => 0, angle => $rotationAngle, length3D => $lengthXYZ };
		}
        my $perpendicularGradientXZ = -1 / ($z / $x);
        if ($p2->[X] > $p1->[X]) {
		if ($perpendicularGradientXZ > 0) {
			$rotationAngle = -$rotationAngle;
			}
		}
	else {
		if ($perpendicularGradientXZ < 0) {
			$rotationAngle = -$rotationAngle;
			}
		}
        return { x => 1, y => 0, z => $perpendicularGradientXZ, angle => $rotationAngle, length3D => $lengthXYZ };
        }

################
#
#
#
################

sub outputNodes()
{
	my $obj;
	foreach $obj (@objects) {
		if (hasRelationships($obj)) {
			#
			# figure out the colours to use, based on a chequer board pattern
			#
			my $colour;
			if ($obj->{col} & 1) {
				$colour = $rectangleColour1;
			} else {
				$colour = $rectangleColour2;
			}
			my $x = objectX($obj);
			my $y = objectY($obj);
			my $z = objectZ($obj);
			if ($outputFormat eq "svg") {
				outputRect($x, $y, $z,
					   $rectangleWidth, $rectangleHeight,
					   $colour, "black", qq{ onclick="NodeEvent(evt)" opacity="$rectangleOpacity" id="$obj->{Name}" } );
			} else {
				outputRect($x, $y, $z,
					   $rectangleWidth, $rectangleHeight,
					   $colour, "black", "vrml: DEF $obj->{Name}");
			}

			my $debug;
#			$debug="-" . $obj->{group} . "(" . $obj->{col} . "," . $obj->{row} . ")(" . $obj->{subCol} . "," . $obj->{subRow} . ")";
			my $midPointX = $x + ($rectangleWidth/2);

			my $spacingY = ($rectangleHeight + $fontHeight);
			if ($displayObjectAddressFlag) {
				$spacingY /= 3;
			} else {
				$spacingY /= 2;
			}
			my $textZOffset = $boxDepth/2 + $adjustFactor;

			my $otherAttr;
			if ($outputFormat ne "svg") {
				$otherAttr = "vrml: DEF $obj->{Name}_TF";
			} else {
				$otherAttr = qq{id="$obj->{Name}" onclick="NodeEvent(evt)"};
			}

			outputText($midPointX,
   				   $obj->{Name} . $debug,
				   $y + $spacingY,
				   $z + $textZOffset,
				   "middle", $otherAttr, "black");

			if ($outputFormat ne "svg") {
				# Object name at rear side of box
				outputText($midPointX,
	   				   $obj->{Name} . $debug,
					   $y + $spacingY,
					   $z - $textZOffset,
					   "middle", "vrml: rotation 0 1 0 " . pi . " ; DEF $obj->{Name}_TB", "black");
			}

			if ($displayObjectAddressFlag) {
				if ($outputFormat ne "svg") {
					outputText($midPointX, $obj->{Addr}, $y + $spacingY, $z, "middle", "vrml: size " . $boxInternalFontHeight, $boxInternalTextColour);
				} else {
					outputText($midPointX,
						   $obj->{Addr},
						   $y + 2*$spacingY,
						   $z + $textZOffset,
						   "middle", "", "black");
				}
			}
		}
	}
}

sub outputLinks()
	{
	my $obj;
	foreach $obj (@objects) {
		# relationships between objects are symmetrical in both directions, so only need to display
		# one each of west/east and north/south.
		if ($obj->{n}) {
			foreach my $obj2 (@{$obj->{n}}) {
				if (checkOverlap($obj, $obj2) == 1)
					{
#					++$gridVerticalLineCount[$obj->{col}];
					my $attachPoint2 = ++$obj2->{n_AttachPointCount};
					my $attachPoint = ++$obj->{n_AttachPointCount};
					outputTabConnectors($obj, "n", $attachPoint, $obj2, "s", $attachPoint2);
					}
				else
					{
					my $x1 = objectSideX($obj, "n", 0);
					my $y1 = objectSideY($obj, "n", 0);
					my $z1 = objectZ($obj);
					my $x2 = objectSideX($obj2, "s", 0);
					my $y2 = objectSideY($obj2, "s", 0);
					my $z2 = objectZ($obj2);
					my $otherAttr;
					if ($outputFormat eq "svg") {
						$otherAttr = qq{ id="$obj->{Name}_$obj2->{Name}" onclick="MessageEvent(evt)" };
					} else {
						$otherAttr = "vrml: DEF " . vrmlRelationshipName($obj->{Name}, $obj2->{Name});
					}
					outputLine($x1, $y1, $z1, $x2, $y2, $z2, "", $otherAttr);

#					outputTextAlongLine($x1,$y1,$z1,$x2,$y2,$z2,"message");
					}
				}
			}
		if ($obj->{w}) {
			foreach my $obj2 (@{$obj->{w}}) {
				if (checkOverlap($obj, $obj2) == 1)
					{
#					++$gridHorizontalLineCount[$obj->{row}];
					my $attachPoint2 = ++$obj2->{w_AttachPointCount};
					my $attachPoint = ++$obj->{w_AttachPointCount};
					outputTabConnectors($obj, "w", $attachPoint, $obj2, "e", $attachPoint2);
					}
				else
					{
					my $x1 = objectSideX($obj, "w", 0);
					my $y1 = objectSideY($obj, "w", 0);
					my $z1 = objectZ($obj);
					my $x2 = objectSideX($obj2, "e", 0);
					my $y2 = objectSideY($obj2, "e", 0);
					my $z2 = objectZ($obj2);

					if ($outputFormat eq "svg") {
						$otherAttr = qq{ id="$obj->{Name}_$obj2->{Name}" onclick="MessageEvent(evt)" };
					} else {
						$otherAttr = "vrml: DEF " . vrmlRelationshipName($obj->{Name}, $obj2->{Name});
					}
					outputLine($x1, $y1, $z1, $x2, $y2, $z2, "", $otherAttr);
#					outputTextAlongLine($x1,$y1,$z1,$x2,$y2,$z2,"message");
					}
				}
			}
		}
	}


#
# Output a unique background rectangle around each group in order to highlight each group seperately
#
sub outputGroupHighlight()
	{
	my $x = gridX(0) - $groupHighlightMargin;
	for my $i (keys %groupRanges)
		{
		my $ref = $groupRanges{$i};
		die "outputGroupHighlight(): minRow for group $i is non-zero ($ref->{minRow})\n" if $ref->{minRow} != 0;
		outputRect($x,
				   gridY($ref->{row}) - $groupHighlightMargin,
				   0,
				   $width,
				   gridHeight($ref->{row}, $ref->{maxRow} + 1) + $groupHighlightMargin * 2,
#				   $groupHighlightFill, "", qq { onclick="debugEvent(evt)" } );
				   $groupHighlightFill, "", "");
		}
	}

#
# Check if a line drawn between two objects will cross through another object
#

sub checkOverlap($$)
	{
	my ($obj, $obj2) = @_;
	if (abs($obj->{col} - $obj2->{col}) > 1 || abs($obj->{row} - $obj2->{row}) > 1)
		{
		return 1;
		print "Overlap $obj->{Name} and $obj2->{Name}\n";
		}
	return 0;
	}

#
# Output labelled tabs representing the connection between two nodes which have intervening nodes (so
# a direct line is not possible)
#

sub outputTabConnectors($$$$$$)
	{
	my ($obj, $dir, $attachPoint, $obj2, $dir2, $attachPoint2) = @_;

	my $objX = objectSideX($obj, $dir, $attachPoint);
	my $objY = objectSideY($obj, $dir, $attachPoint);
	my $objZ = objectZ($obj);

	my $obj2X = objectSideX($obj2, $dir2, $attachPoint2);
	my $obj2Y = objectSideY($obj2, $dir2, $attachPoint2);
	my $obj2Z = objectZ($obj2);

	if ($dir eq "n")
		{
		# output Tabs at the top of objects - de-emphasise the tab labels with semi-condensed font option
		my $otherAttrs = "";
		if ($outputFormat ne "svg") {
			$otherAttrs = "vrml: DEF " . vrmlRelationshipName($obj->{Name}, $obj2->{Name});
		}
		outputLine($objX, $objY, $objZ, $objX, $objY - $tabLineLengthY, $objZ, "", $otherAttrs);
		outputText($objX, getCurrentTabLabel(),
				   $objY - $tabLineLengthY - $tabToFontAdjustTopY,
				   $objZ,
				   "middle", qq{font-stretch="semi-condensed"}, "");

		# output Tabs at the bottom of objects
		outputLine($obj2X, $obj2Y + $tabLineLengthY, $obj2Z, $obj2X, $obj2Y, $obj2Z, "", "");
		outputText($obj2X, getCurrentTabLabel(), $obj2Y + $tabLineLengthY + $fontHeight + $tabToFontAdjustBottomY,
				$obj2Z,
				"middle", qq{font-stretch="semi-condensed"}, "");

		incrementCurrentTabLabel();
		}
	elsif ($dir eq "w")
		{
		}
	else
		{
		die "outputTabConnectors: incorrect direction $dir\n";
		}
	}

#
# Return the current Tab Label string
#
sub getCurrentTabLabel()
	{
	return $tabLabel;
	}

#
# Increment the current Tab Label.
#
# Cycle through a-z, A-Z, 0-9 (...then funny characters I guess!)
#
sub incrementCurrentTabLabel()
	{
	if ($tabLabel eq "z")
		{
		$tabLabel = "A";
		}
	elsif ($tabLabel eq "Z")
		{
		$tabLabel = "0";
		}
	else
		{
		$tabLabel++;
		}
	}

#
# Return the X position of a particular column in the super grid
#

sub gridX($)
	{
	my ($col) = @_;
	return $horizMargin + $colToSubColIndex[$col] * ($rectangleWidthWithMargin) + ($col * $gridXSpacing);
	}

#
# Return the Y position of a particular row in the super grid
#

sub gridY($)
	{
	my ($row) = @_;
	return $vertMargin + $rowToSubRowIndex[$row] * $rectangleHeightWithMargin + ($row * $gridYSpacing);
	}

#
# Return the height (pixels) of an area starting at a specified row and a number of rows high.
#
sub gridHeight($$)
	{
	my ($startRow,$heightInRows) = @_;
	my $endRow = $startRow + $heightInRows - 1;
	die "gridHeight: invalid row height $heightInRows\n" if $heightInRows <= 0;

	# calculate the number of sub rows within the area concerned
	my $heightInSubRows = $rowToSubRowIndex[$endRow] -  $rowToSubRowIndex[$startRow] + $rowToSubGridHeight[$endRow];
	
	# height is: height of rectangles + height of spacing between rectangles + height of spacing between rows
	# The "-1"s are to exclude the spacing after the last row.
	return ($heightInSubRows * $rectangleHeight) +
		   (($heightInSubRows - 1) * $rectangleMargin) +
		   (($heightInRows - 1) * $gridYSpacing);
	}


#
# Return the X position of one of the object's sides
#
# Arguments:
# obj			object in question
# dir			the side in question expressed as a compass point - i.e. "n", "s", "w", "e"
# attachPoint	Used for "n" and "s" sides only.  Zero means return X position of the centre position of the side.
#				A number above zero means return the X position of a point along the side starting from its leftmost
#				edge and increasing equi-distantly as the number increases.  Used for drawing starting and ending
#				points of a "long" line.
#

sub objectSideX($$$)
	{
	my ($obj,$dir,$attachPoint) = @_;
	if ($dir eq "n" || $dir eq "s") {
		if ($attachPoint == 0)
			{
			return objectX($obj) + ($rectangleWidth/2);
			}
		else
			{
			# BUG - can overlap with attachPoint 0 !!
			return objectX($obj) + ($attachPoint * $tabSpacing);
			}
		}
	elsif ($dir eq "w") {
		return objectX($obj);
		}
	elsif ($dir eq "e") {
		return objectX($obj) + $rectangleWidth;
		}
	else {
		die "objectSideX: Bad argument (dir=$dir)\n";
		}
	
	}
	
sub objectSideY($$$)
	{
	my ($obj,$dir,$attachPoint) = @_;
	if ($dir eq "w" || $dir eq "e") {
		if ($attachPoint == 0)
			{
			return objectY($obj) + ($rectangleHeight/2);
			}
		else
			{
			# BUG - can overlap with attachPoint 0 !!
			return objectY($obj) + ($attachPoint * 10);
			}
		}
	elsif ($dir eq "n") {
		return objectY($obj);
		}
	elsif ($dir eq "s") {
		return objectY($obj) + $rectangleHeight;
		}
	else {
		die "objectSideY: Bad argument (dir=$dir)\n";
		}
	
	}

#
# Return the X position of the leftmost side of an object.
# Calculated by taking the X position of the supergrid column that the object is within
# and adding this to the column within the subgrid that the object occupies, and then
# multiplying by the subgrid spacing.
#
#
# Arguments:
# obj		object in question
#

sub objectX($)
	{
	my $obj = $_[0];
	return gridX($obj->{col}) + $obj->{subCol} * $rectangleWidthWithMargin;
	}

sub objectY($)
	{
	my $obj = $_[0];
	return gridY($obj->{row}) + $obj->{subRow} * $rectangleHeightWithMargin;
	}

sub objectZ($)
	{
	my $obj = $_[0];
	# Find number of objects at [col,row])
	my $objects = $subGridPlacement[$obj->{col}][$obj->{row}]{subZ} + 1;
	# Calculate total depth to be occupied by these objects and spacing betwen them
	my $depthZ = ($objects-1) * $boxDepthWithMargin + $boxDepth;

	return ($depthZ/2) - ($obj->{subZ} * $boxDepthWithMargin);
	}

sub normaliseCoords()
	{
	foreach $group (keys %groupRanges) {
		my $rangeRef = $groupRanges{$group};

		print "group $group, rangeRef $rangeRef\n";

		my $x = $rangeRef->{minCol};
		my $y = $rangeRef->{minRow};

		$rangeRef->{minCol} = 0;
		$rangeRef->{minRow} = 0;
		$rangeRef->{maxCol} -= $x;
		$rangeRef->{maxRow} -= $y;

		foreach my $obj (@objects) {
			if ($obj->{group} == $group) {
				$obj->{col} -= $x;
				$obj->{row} -= $y;
				}
			}
		}
	}

sub seperateOverlaps()
	{
	my $obj;
	foreach $obj (@objects) {
		my $w_e_Relations = defined($obj->{w}) || defined($obj->{e});
		my $n_s_Relations = defined($obj->{n}) || defined($obj->{s});
		my $col = $obj->{col};
		my $row = $obj->{row};

		# only deal with objects that have relationships to other objects
		if ($w_e_Relations || $n_s_Relations) {
			if (!defined($subGridPlacement[$col][$row]{subCol})) {
				$subGridPlacement[$col][$row]{subCol} = 0;
				$subGridPlacement[$col][$row]{subRow} = 0;
				$subGridPlacement[$col][$row]{subZ} = 0;	# VRML
#				print "subGridPlacement $obj->{name} [ $col, $row ] zeroed\n";
				}
			else {
				if ($outputFormat eq "svg") {
					if ($w_e_Relations) {
						++$subGridPlacement[$col][$row]{subRow};
#						print "subGridPlacement $obj->{name} [ $col, $row ] subRow = ", $subGridPlacement[$col][$row]{subRow},"\n";
						}
					if ($n_s_Relations) {
						++$subGridPlacement[$col][$row]{subCol};
#						print "subGridPlacement $obj->{name} [ $col, $row ] subCol = ", $subGridPlacement[$col][$row]{subCol},"\n";
						}
					}
				else {
					++$subGridPlacement[$col][$row]{subZ};
					}
				}

			$obj->{subZ} = $subGridPlacement[$col][$row]{subZ};	# VRML
			$obj->{subCol} = $subGridPlacement[$col][$row]{subCol};
			$obj->{subRow} = $subGridPlacement[$col][$row]{subRow};

			$rowToSubGridHeight[$row] = max($rowToSubGridHeight[$row], $obj->{subRow} + 1);
			$colToSubGridWidth[$col] = max($colToSubGridWidth[$col], $obj->{subCol} + 1);
			}
		}

	}

#
# Ensure that each distinct group is displayed in its own area of the canvas vertically.
# Go through and offset vertically each of the objects in the group.  Also, initialise
# the {row} for each group in the groupRange to contain the row that the group starts at.
#
sub seperateGroups()
	{
	my $debug = 1;
	if ($debug) { print "\nseperateGroups(): g = $g\n"; }
	my $row = 0;
	foreach $group (keys %groupRanges)
		{
		if ($row != 0)
			{
			foreach my $obj (@objects)
				{
				if ($obj->{group} == $group)
					{
					$obj->{row} += $row;
					}
				}
			}

		my $ref = $groupRanges{$group};
		if ($debug) { print "$group:\trow $row, height: ",$ref->{maxRow} - $ref->{minRow}, "\n"; }
		$ref->{row} = $row;
		$row += $ref->{maxRow} - $ref->{minRow} + $groupVerticalSeperation;
		}
	}

sub calculateGridPositions()
	{
	my $total = 0;
	my $last;
	for (my $i = 0 ; $i < scalar(@rowToSubGridHeight) ; ++$i) {
		$last = $rowToSubGridHeight[$i];
		$rowToSubRowIndex[$i] = $total;
		$total += $last;
		}

	$total = 0;
	for (my $i = 0 ; $i < scalar(@colToSubGridWidth) ; ++$i) {
		$last = $colToSubGridWidth[$i];
		$colToSubColIndex[$i] = $total;
		$total += $last;
		}

	}


sub hasRelationships($)
	{
	my $objRef = $_[0];
	if ($objRef->{n} || $objRef->{s} || $objRef->{w} || $objRef->{e}) {
		return 1;
		}
	else {
		return 0;
		}
	}

sub isRelated($$)
{
	my ($srcRef, $destRef) = @_;
	my $obj;
	foreach my $dir ( "n", "s", "e", "w" ) {
		if (exists $srcRef->{$dir}) {
			foreach $obj (@{$srcRef->{$dir}}) {
				if ($obj == $destRef) {
					return 1;
				}
			}
		}
	}
	return 0;
}


sub max($$)
	{
	my ($a,$b) = @_;
	return $a > $b ? $a : $b;
	}

sub printArray($$)
	{
	my ($name,$array) = @_;
	print $name, " (size ", scalar(@{$array}), "): ";
	foreach my $i (@{$array}) {
		print $i, ", ";
		}
	print "\n\n";
	}

sub printHash($$)
	{
	my ($name,$hash) = @_;
	if ($name ne "")
		{
		print $name, ":\t";
		}
	else
		{
		print "\t";
		}

	foreach my $i (keys %{$hash}) {
		print $i, " = ", $hash->{$i}, ", ";
		}
	}

sub printGroupRanges()
	{
	print "\n%groupRanges:\n";
	foreach my $i (keys %groupRanges) {
		printHash($i, $groupRanges{$i});
		print "\n";
		}
	print "\n";
	}

sub print2DArray($$)
	{
	my ($name,$array) = @_;
	print $name, ":\n";
	my $index1 = 0;
	my $index2 = 0;
	foreach my $i (@{$array}) {
		print "$index1:\n";
		$index2 = 0;
		foreach my $j (@{$i}) {
			print "\t";
			printHash($index2, $j);
			print "\n";
			++$index2;
			}
		++$index1;
		print "\n";
		}
	print "\n";
	}

sub getTokens($)
	{
	my $stringRef = $_[0];
	$$stringRef =~ s/^\s*//;
	$$stringRef =~ s/\s*$//;
	if ($$stringRef !~ /;/) {
		my $string = $$stringRef;
		$$stringRef = "";
		return $string;
		}
	$$stringRef =~ s/^(.*?)\s*;\s*//;
	return $1;
	}

#
# VRML Animation Routines
#

sub hasVrmlTokens($)
	{
	my $stringRef = $_[0];
	if ($$stringRef =~ s/^vrml: //) {
		return 1;
		}
	else {
		return 0;
		}
	}

#
# Given two strings, "X" and "Y", return a string of the form "X_Y" where
# "X" is the lexically lesser in rank.
#
# For example: "A1" and "B2" yields "A1_B2".  The idea is to form a predictable
# and unique relationship name between two named nodes, where there are normally
# two possibilities (i.e. "A1_B2" and "B2_A1").

sub vrmlRelationshipName($$)
	{
	my ($name1, $name2) = @_;
	return $name1 lt $name2 ? $name1 . "_" . $name2 : $name2 . "_" . $name1;
	}
#
# Animation Glossary:
#
# "Node"		A mesh node (e.g. "IPCPr").  Graphically represented as a box with text on front and back.
# "Relationship"	A relationship between two nodes (e.g. "IPCPr_NetMCPr").  Graphically represented as a thin cylinder connecting the nodes.
# "Animate"		To make a node or relationship visible.
# "DeAnimate"		To make a node or relationship invisible.
#

#
# Two object associated with animation:
# @animateSequence
#	Array indicating the animation sequence.  Each entry is a hash:
#		Hash keys present for nodes and relationships:
#			{action} => (integer) 0 to make node/relationship invisible, 1 to make it visible.
#
#		Hash keys present only for nodes:
#			{object} => (ref) reference into @objects entry for the node.
#
#		Hash keys present only for relationships:
#			{name} => (string) name of the relationship.
#			{srcObj} => (ref) reference into @objects for source node.
#			{destObj} => (ref) reference into @objects for destination node.
#
# %animateVisible
#	Hash indicating which nodes or relationships are currently on display at the current point in the
#	animation sequence.
#
#	{<name>} = name of node or relationship, which is a hash:
#
#		Keys present for nodes and relationships:
#		{visible} => (integer).  1 if node or relationship <name> is displayed, 0 if node or relationship <name> is no longer displayed.
#
#		Hash keys present only for nodes:
#		{related} => (hash).  Keys within {related} are hash are names of nodes that are
#			   related to this node and whose relationships to this node are visible.

sub Animate($$$$)
	{
	my ($name, $object,$srcObject,$destObject) = @_;
	die "Animate: null name" if (!$name);

	if ($object) {
		# animate a node
		$animateVisible{$name} = { visible => 1, related => {} };	# "related" = hash of names of other objects this object has a relationship with.
		push @animateSequence, { object => $object, action => 1 };
		}
	else {
		# animate a relationship between nodes
		$animateVisible{$name} = { visible => 1 };
		push @animateSequence, { srcObj => $srcObject, destObj => $destObject, name => $name, action => 1 };
		# for the two nodes, store the fact that each node has a relationship to the other
		my $destName = $destObject->{Name};
		my $srcName = $srcObject->{Name};
		$animateVisible{$srcName}{related}{$destName} = 1;
		$animateVisible{$destName}{related}{$srcName} = 1;
		}
	}

sub CheckAndAnimate($$$$)
	{
	my ($name,$object,$srcObject,$destObject) = @_;

	# We only want to animate if the object is not already visible.  Note that this is not the same the hash entry
	# existing with $animateVisible{$name}{visible} set to 0, which means that it was animated and then de-animated.
	# In this case, we would not want to re-animate the object, as it shouldn't be possible for the same node to
	# be re-created (although relationships are assumed to be terminated once and never re-created between the same
	# two nodes).
	if (! exists($animateVisible{$name})) {
		Animate($name, $object, $srcObject, $destObject);
		return 1;
		}
	return 0;
	}

sub CheckAndDeAnimate($$$$)
{
	my ($name, $objRef, $srcObject, $destObject) = @_;
	# Only de-animate if the object is visible (i.e. $animateVisible{$name} exists and $animateVisible{$name}{visible} is non-zero).
	if ($animateVisible{$name}{visible}) {
		if ($objRef) {
			# de-animate out relationships to this nodes that are still visible
			# (i.e. because the appropriate "ClientLeaving" etc message has not been seen)
			foreach my $relatedNode (keys %{$animateVisible{$name}{related}}) {
				die "CheckAndDeAnimate: overwriting destObject" if $destObject != 0;
				my $destObj = findObject($relatedNode);
				my $relationshipName = vrmlRelationshipName($objRef->{Name}, $destObj->{Name});
				if ($animateVisible{$relationshipName}{visible}) {
					push @animateSequence, { srcObj => $objRef, destObj => $destObj, name => $relationshipName, action => 0 };
					$animateVisible{$relationshipName}{visible} = 0;
				}
			}
			push @animateSequence, { object => $objRef, action => 0 };
		} else {
			push @animateSequence, { srcObj => $srcObject, destObj => $destObject, name => $name, action => 0 };
			my $destName = $destObject->{Name};
			my $srcName = $srcObject->{Name};
			delete $animateVisible{$srcName}{related}{$destName};
			delete $animateVisible{$destName}{related}{$srcName};
		}
		$animateVisible{$name}{visible} = 0;
	}
}

#
# Called when a message is exchanged between two nodes.
#
sub AnimateObjectMessage($$$)
{
	my ($srcRef, $destRef, $direction) = @_;
	if ($srcRef != $destRef) {
		my $srcName = $srcRef->{Name};
		my $destName = $destRef->{Name};
		my $relationshipName = vrmlRelationshipName($srcName, $destName);
		if ($direction && $direction eq "~") {
			# Relationship terminated
			CheckAndDeAnimate($relationshipName, 0, $srcRef, $destRef);
		} else {
			CheckAndAnimate($relationshipName, 0, $srcRef, $destRef);
		}
	}
}

#
# Called when an node is created
#
sub AnimateObjectCreate($)
{
	my ($objRef) = @_;
	my $objectName = $objRef->{Name};
	CheckAndAnimate($objectName, $objRef, 0, 0);
}

#
# Called when a node is destroyed
#
sub AnimateObjectDestroy($)
	{
	my ($objRef) = @_;
	my $objectName = $objRef->{Name};
	CheckAndDeAnimate($objectName, $objRef, 0, 0);
	}

sub AnimateDo()
{
	AnimateVrmlPrologue();
	AnimateVrmlSequencerPart1();

	# Translate the animations we have built up in @animateSequence array into two integer arrays in the VRML file:
	#
	# action[]	Action to perform.  Each entry is an integer encoded as:
	# 			(<number of VRML elements to animate> << 8) | <action>
	# 		Where: <action> = 1 to make VRML elements visible or 0 to make them invisible.
	#		The VRML elements themselves are contained in the VRML "nodes" array.
	#
	# nodes[]	VRML elements to animate.  Each entry is a VRML SFNode reference.
	#
	# The action[] array is executed in sequence.  Each entry in the action[] array corresponds to one or more
	# entries in the nodes[] array.  Each array has an iterator - the action[] iterator always increments one
	# at a time.  The nodes[] iterator increments by the number of VRML elements that the corresponding action[]
	# entry needs to iterate.  Thus, for each action[] entry there are one or more nodes[] entries:
	# For example:
	#	action[0] = 1<<8 | ... :	nodes[0]
	#	action[1] = 3<<8 | ...		nodes[1] nodes[2] nodes[3]
	#	action[2] = 1<<8 | ...		nodes[4]
	#	action[3] = 1<<8 | ...		nodes[5]
	#	action[4] = 2<<8 | ...		nodes[6] nodes[7]
	# etc
	my $name;
	foreach my $seq (@animateSequence) {
		if (exists $seq->{object}) {
			# animate/de-animate a node
			if (hasRelationships($seq->{object})) {
				print WRL "USE $seq->{object}{Name},\n";	# VRML box
				print WRL "USE $seq->{object}{Name}_TF,\n";	# VRML text at front
				print WRL "USE $seq->{object}{Name}_TB,\n";	# VRML text at back
			}
		} else {
			# animate/de-animate a relationship between nodes
			if (isRelated($seq->{srcObj}, $seq->{destObj})) {
				print WRL "USE $seq->{name},\n";		# VRML thin cylinder
			}
		}
	}
	AnimateVrmlSequencerPart2();
	foreach my $seq (@animateSequence) {
		if (exists $seq->{object}) {
			if (hasRelationships($seq->{object})) {
				print "Action $seq->{action}, $seq->{object}{Name}\n";
				print WRL $seq->{action} + (3 << 8), ",\n";
			}
		} else {
			if (isRelated($seq->{srcObj}, $seq->{destObj})) {
				print "Action $seq->{action}, $seq->{name}\n";
				print WRL $seq->{action} + (1 << 8), ",\n";
			}
		}
	}
	AnimateVrmlSequencerPart3();
}

sub AnimateVrmlPrologue()
	{
	###########################
	# Begin Interpolated Text #
	###########################
	print WRL<<END

DEF TIME1 TimeSensor {
   cycleInterval     $vrmlCycleInterval
   enabled           TRUE
   loop              FALSE
}

DEF TIME2 TimeSensor {
   cycleInterval     $vrmlCycleInterval
   enabled           TRUE
   loop              FALSE
}

DEF TIME3 TimeSensor {
   cycleInterval     $vrmlCycleInterval
   enabled           TRUE
   loop              FALSE
}

DEF TIME4 TimeSensor {
   cycleInterval     $vrmlCycleInterval
   enabled           TRUE
   loop              FALSE
}

DEF TIME5 TimeSensor {
   cycleInterval     $vrmlCycleInterval
   enabled           TRUE
   loop              FALSE
}

DEF TIME6 TimeSensor {
   cycleInterval     $vrmlCycleInterval
   enabled           TRUE
   loop              FALSE
}

DEF INVERT1 ScalarInterpolator {
	key                  [ 0 1 ]
	keyValue             [ 1 0 ]
}

DEF INVERT2 ScalarInterpolator {
	key                  [ 0 1 ]
	keyValue             [ 1 0 ]
}

DEF INVERT3 ScalarInterpolator {
	key                  [ 0 1 ]
	keyValue             [ 1 0 ]
}

DEF START TimeSensor {
   cycleInterval     1000000
   enabled           TRUE
   loop              TRUE
}

END
	#########################
	# End Interpolated Text #
	#########################
	}

sub AnimateVrmlSequencerPart1()
	{
	###########################
	# Begin Interpolated Text #
	###########################
	print WRL<<END
DEF SEQ Script {
	eventIn SFTime startTime
	eventIn SFTime timeCycleTime
	eventIn SFBool timeIsActive1
	eventIn SFBool timeIsActive2
	eventIn SFBool timeIsActive3
	field SFInt32 nodeIndex 0
	field SFInt32 actionIndex 0
	field SFInt32 pending 0
	directOutput TRUE
	mustEvaluate TRUE
	field MFNode timeNode [ USE TIME1, USE TIME2, USE TIME3 ]
	field MFNode timeNode2 [ USE TIME4, USE TIME5, USE TIME6 ]
	field MFNode invertNode [ USE INVERT1, USE INVERT2, USE INVERT3 ]
	field MFNode nodes [
END
	#########################
	# End Interpolated Text #
	#########################
	}

sub AnimateVrmlSequencerPart2()
	{
	print WRL<<END
		]
	field MFInt32 action [
END
	}

sub AnimateVrmlSequencerPart3()
{
	###########################
	# Begin Interpolated Text #
	###########################
	print WRL<<END
		]
	url "javascript:
		function startTime(value, time) {
			startNextNode(time);
		}

		function startNextNode(time) {
			var count = (action[actionIndex] >> 8) & 0xff;
			var act = action[actionIndex] & 0xff;
			if (act == 1) {
				for (var i = 0 ; i < count ; ++i) {
					Browser.addRoute(invertNode[i], 'value_changed', nodes[nodeIndex++], 'set_transparency');
				}
				for (var i = 0 ; i < count ; ++i) {
					timeNode[i].startTime = time;
				}
			} else {
				for (var i = 0 ; i < count ; ++i) {
					Browser.addRoute(timeNode2[i], 'fraction_changed', nodes[nodeIndex++], 'set_transparency');
				}
				for (var i = 0 ; i < count ; ++i) {
					timeNode2[i].startTime = time;
				}
			}
			pending = count;
			actionIndex++;
		}

		function timeCycleTime(value, time) {
		}

		function timeIsActive1(value, time) {
			timeIsActive(value, time, 0);
		}

		function timeIsActive2(value, time) {
			timeIsActive(value, time, 1);
		}

		function timeIsActive3(value, time) {
			timeIsActive(value, time, 2);
		}

		function timeIsActive(value, time, which) {
			if (value == false) {
				if (actionIndex > 0) {
					var count = (action[actionIndex-1] >> 8) & 0xff;
					var act = action[actionIndex-1] & 0xff;
					var j = nodeIndex - count + which;

					if (act == 1) {
						Browser.deleteRoute(invertNode[which], 'value_changed', nodes[j], 'set_transparency');
						nodes[j].transparency = 0;
					} else {
						Browser.deleteRoute(timeNode2[which], 'fraction_changed', nodes[j], 'set_transparency');
						nodes[j].transparency = 1;
					}
				}
				if (--pending == 0 && actionIndex < action.length) {
					startNextNode(time);
				}
			}
		}
	"
}

ROUTE START.cycleTime TO SEQ.startTime
#ROUTE TIME.fraction TO INVERT.fraction

ROUTE TIME1.fraction TO INVERT1.fraction
ROUTE TIME2.fraction TO INVERT2.fraction
ROUTE TIME3.fraction TO INVERT3.fraction

#ROUTE TIME.cycleTime TO SEQ.timeCycleTime

ROUTE TIME1.isActive TO SEQ.timeIsActive1
ROUTE TIME2.isActive TO SEQ.timeIsActive2
ROUTE TIME3.isActive TO SEQ.timeIsActive3

ROUTE TIME4.isActive TO SEQ.timeIsActive1
ROUTE TIME5.isActive TO SEQ.timeIsActive2
ROUTE TIME6.isActive TO SEQ.timeIsActive3

END
	#########################
	# End Interpolated Text #
	#########################
	}
