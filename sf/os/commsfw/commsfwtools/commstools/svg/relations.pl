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
#

require 'getopts.pl';
use strict;
my $version = "2.1 (28/11/07)";
my $animate = 1;
my $debug;
my $maxRelationsPopupHeight = 980;	# maximum height of relations popup window
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


#
# Flags
my $displayObjectAddressFlag = 0;		# display object hex address alongside name
my $displayObjectExecutable = 1;		# display object executable alongside name

my $group = 1;							# next free group number (monotonically incremented after allocation)

my @colToSubColIndex;
my @rowToSubRowIndex;
my @rowToSubGridHeight;
my @colToSubGridWidth;
my @objects;
my @subGridRow;
my @subGridPlacement;
my %relationships;
my %groupRanges;

my @animateSequence;
my %animateVisible;

my %nodeToExe;			# map subsession nodes (e.g. Socket) to their executable
my %directionTranslate = ( n => { x =>  0, y => -1 },
						   s => { x =>  0, y => +1 },
						   w => { x => -1, y =>  0 },
						   e => { x => +1, y =>  0 } );

my %initRange = ( minCol => 9999, minRow => 9999, maxCol => -9999, maxRow => -9999 );
my %overallRange = %initRange;

# tabLabel is a monotonically increasing value used to derive a unique Tab Label.  It is incremented
# everytime a new Tab is drawn.

my $tabLabel = "a";

# minor aesthetic adjustments to the spacing between the end of a Tab line and its Labels
my $tabToFontAdjustTopY = 2;
my $tabToFontAdjustBottomY = 1;
my $tabLineLengthY = 5;
my $outputPath = "";		# output path as specified by "-o <path>" argument
our($opt_x,$opt_v,$opt_V, $opt_o);
Getopts("x:vVo:");

$outputPath = processPathArgument($opt_o);

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

my $fontHeight = 9;								# assumed font height
my $groupHighlightFill = "rgb(235,235,235)";
my $groupHighlightMargin = 5;					# margin of the group background rectangle

my $horizMargin = 20;
my $vertMargin = $horizMargin;

my $gridXSpacing = $horizMargin;
my $gridYSpacing = $gridXSpacing;

my $rectangleMargin = 10;
my $rectangleWidth = 110;
my $rectangleHeight = 27;
my $rectangleWidthWithMargin = $rectangleWidth + $rectangleMargin;
my $rectangleHeightWithMargin = $rectangleHeight + $rectangleMargin;

my $rectangleOpacity = 0.80;
my $rectangleColour1 = "lightgreen";
my $rectangleColour2 = "lightskyblue";
my $rectHighlightColour = "salmon";

# X position of last grid column + size of last grid column + horizontal margin
my $width = gridX($#colToSubColIndex) + $colToSubGridWidth[$#colToSubColIndex] * $rectangleWidthWithMargin + $horizMargin;
my $height = gridY($#rowToSubRowIndex) + $rowToSubGridHeight[$#subGridRow] * $rectangleHeightWithMargin + $vertMargin;

createOutputFile();
outputDocHeader($width,$height);
outputGroupHighlight();
print "Generating links\n" if ($opt_V);
outputLinks();
print "Generating nodes\n" if ($opt_V);
outputNodes();
outputDocFooter();
closeOutputFile();
outputRelationsHTMLEmbedder($width, $height);

sub createOutputFile()
{
	# Have already created the directory $outputPath, just create the "html" subdirectory
	my $path = $outputPath . "html";
	if (! -d $path) {
		mkdir $path;
	}
	$path .= "/relations.svg";
	open SVG, ">$path" || die "Cannot open $path for writing\n";
}

sub closeOutputFile()
	{
		close SVG;
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
				s/^"([^"]+)"\s+//;	# deal with quoted strings: "..."
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

		elsif ($action eq "t" && $displayObjectExecutable)
			{
			# Text
			# We are trying to to create a map of subsessions to the executables that are using them,
			# (e.g. Socket1 -> dnd).  To do this, we need to search for the special "t" entries of the form:
			#
			#	t !tester ESoSendToNoLength(Socket)
			#
			m/(\S+) (\S+)/;
			my $exe = $1;
			my $ipc = $2;
			if ($exe =~ s/^!//)
				{
				if ($ipc =~ m/E\w+\(([A-Z]\w+)\)/)
					{
					if (! defined $nodeToExe{$1})
						{
						$nodeToExe{$1} = $exe;
						}
					elsif ($nodeToExe{$1} ne $exe)
						{
						# error - we have more than one node that seems to be using the same subsession.
						# Null out the hash entry - it won't be overwritten.
						$nodeToExe{$1} = "";
						}
					}
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
			$obj->{group} = $newGroup;
			$obj->{col} += $adjustCol;
			$obj->{row} += $adjustRow;
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
	my ($width,$height) = @_;
	print SVG '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',"\n";
	print SVG '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">',"\n";
	print SVG "<svg id=\"svgroot\" height=\"$height\" width=\"$width\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
	# place popup rectangle before all other text, otherwise it will be on top of rather than
	# underneath the popup text
	}

sub outputGroupStart($)
{
	my $id = $_[0];
	print SVG qq{<g id="${id}">\n};
}

sub outputGroupEnd()
{
	print SVG "</g>\n";
}

sub outputText($$$$$$)
{
	my ($x,$text,$y,$anchor,$otherSvgAttr,$colour) = @_;

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
			outputRect($x - 1, $y - $fontHeight, 3, $fontHeight, "white", "white", "");
			}
		if ($colour)
			{
			$attrs .= qq { fill="$colour" };
			}
		}

	print SVG "<text $attrs>$text</text>\n";
}

sub outputLine($$$$$$)
{
	my ($x1,$y1,$x2,$y2,$colour,$otherAttr) = @_;

	if (! $colour) {
		$colour = "black";
	}

	print SVG qq{<line stroke="$colour" x1="$x1" y1="$y1" x2="$x2" y2="$y2" $otherAttr};

	my $id = getAttr("id", $otherAttr);
	print SVG qq{ />\n};
}


sub getAttr($$)
{
	my ($attr,$attrList) = @_;
	if ($attrList =~ m/${attr}="([^"]+)"/) {
		return $1;
	} else {
		return;
	}
}

sub outputRect($$$$$$$)
{
	my ($x,$y,$width,$height,$fill,$stroke,$otherAttr) = @_;
	print SVG qq {<rect fill="$fill" stroke="$stroke" x="$x" y="$y" width="$width" height="$height" $otherAttr};

	my $id = getAttr("id", $otherAttr);
	print SVG qq{ />\n};
}

sub outputDocFooter()
	{
	print SVG "<script><![CDATA[\n";
	AnimateDo() if ($animate);
###################################
#  Begin Interpolated Text...     #
###################################
	print SVG <<'EOT'
var highlightedNodes = new Array();
var highlightedNodesAttr = new Array();
bindToParent();

initAnimate();

function bindToParent()
	{
	parent.relationsMessageHighlightPtr = messageHighlight;
	parent.relationsObjectHighlightPtr = objectHighlight;

	// Allow for animation.html to call into us when one of its buttons is pressed.
	parent.relationsCommandPtr = animateCommand;
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
	sequenceMessageHighlight(evt.currentTarget.id);
	messageHighlight(evt.currentTarget.id);
	}

function NodeEvent(evt)
	{
	sequenceObjectHighlight(evt.currentTarget.id);
	objectHighlight(evt.currentTarget.id);
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
		var nodeList = document.getElementsByTagName("*");
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
	var nodeList = document.getElementsByTagName("*");
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
#  ...End Interpolated Text     #
#################################
	}

sub outputRelationsHTMLEmbedder($$)
	{
	my ($width,$height) = @_;
	# Should have already created $outputPath/html directory.
	my $path = $outputPath . "html/relations.html";
	open HTML, ">$path" || die "Cannot open $path for writing\n";
	print HTML qq{<html>\n<body onLoad="resize()">\n};
	print HTML qq{<embed src="relations.svg" width="$width" height="$height" type="image/svg+xml" pluginspage="http://www.adobe.com/svg/viewer/install/" name="embedder">\n};
	print HTML qq{<input type="button" value="Animate..." onclick="popupAnimate()">\n};
	print HTML qq{<script type="text/javascript">\n};
	print HTML "var relationsWidth = $width;\n";
	print HTML "var relationsHeight = $height;\n";
	print HTML "var maxPopupHeight = $maxRelationsPopupHeight;\n";
#################################
#  Begin Interpolated Text...   #
#################################
	print HTML<<'EOT'
function resize() {
	// Try to resize so that relations diagram is fully visible without scrollbars.
	var height = relationsHeight + ((opener != null) ? 130 : 220);
	if (height > maxPopupHeight) {
		height = maxPopupHeight;
	}
	window.resizeTo(relationsWidth + 55, height);
}

function popupAnimate() {
	animate = window.open("animate.html", "_blank", "resizable=yes,scrollbars=yes");
}

</script>
</body></html>
EOT
;
#################################
#  ...End Interpolated Text     #
#################################
	close HTML;
	}

#
# Output the "animate.html" file that creates the animate "control panel" popup
#
sub outputAnimateHTML()
{
	# Should have already created $outputPath directory, create html/ subdirectory.
	my $path = $outputPath . "html";
	if (! -d $path) {
		mkdir $path;
	}
	$path .= "/animate.html";
	open HTML, ">$path" || die "Cannot open $path for writing\n";
#################################
#  Begin Interpolated Text...   #
#################################
	print HTML<<'EOT'
<html>
<body onLoad="load()">
<input type="button" value="Rewind" onclick="rewind()">
<input type="button" value="Play" onclick="play()">
<input type="button" value="Stop" onclick="stop()">
<input type="button" value="Step +" onclick="stepForward()">
<input type="button" value="Step -" onclick="stepBackward()">
<input type="button" value="Restore" onclick="restore()">
<script type="text/javascript">
function load() {
	window.resizeTo(400, 130);
}

function play()
{
	if (!opener.closed) {
		// Call into relations.svg via the relationsCommandPtr property that relations.svg has set in
		// its parent relations.html.
		opener.relationsCommandPtr("p");
	}
}

function rewind()
{
	if (!opener.closed) {
		opener.relationsCommandPtr("rw");
	}
}

function stop()
{
	if (!opener.closed) {
		opener.relationsCommandPtr("s");
	}
}

function stepForward()
{
	if (!opener.closed) {
		opener.relationsCommandPtr("+");
	}
}

function stepBackward()
{
	if (!opener.closed) {
		opener.relationsCommandPtr("-");
	}
}

function restore()
{
	if (!opener.closed) {
		opener.relationsCommandPtr("rs");
	}
}

</script>
</body></html>
EOT
;
#################################
#  ...End Interpolated Text     #
#################################
	close HTML;
}


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
			outputRect(objectX($obj), objectY($obj),
					   $rectangleWidth, $rectangleHeight,
					   $colour, "black", qq{ onclick="NodeEvent(evt)" opacity="$rectangleOpacity" id="$obj->{Name}" } );

#			my $debug="-" . $obj->{group} . "(" . $obj->{col} . "," . $obj->{row} . ")(" . $obj->{subCol} . "," . $obj->{subRow} . ")";
			my $midPointX = objectX($obj) + ($rectangleWidth/2);

			my $objName = $obj->{Name};
			my $secondLine = "";	# if non-null, second line of text below the object name

			if ($displayObjectAddressFlag) {
				$secondLine = $obj->{Addr};
			}

			if ($displayObjectExecutable && defined $nodeToExe{$objName}) {
				my $exe = $nodeToExe{$objName};
				if ($exe ne "") {
					$secondLine .= "($exe)";
				}
			}

			my $spacingY = ($rectangleHeight + $fontHeight);
			if ($secondLine ne "") {
				$spacingY /= 3;
			} else {
				$spacingY /= 2;
			}

			#
			# If outputting multiple text elements in a rectangle, group ("<g>") them so that we can animate them
			# together via the group.  If using the group, it gets the id name "<object>_t".  If using a
			# single text element, the group isn't used and the single  text element carries the id name instead.
			# From the perspective of the animate, it will just animate object with tag "<object>_t", which
			# will automatically either be single text element or the group of several text elements.
			#

			my $id = $obj->{Name} . "_t";
			if ($animate && $secondLine) {
				outputGroupStart($id);
				# don't need to assign id to any text elements below as it is assigned to the group
				$id = "";
			} else {
				# no group - assign id to the (single) text element below
				$id = qq{id="$id"};
			}

			outputText($midPointX,
					   $objName . $debug,
					   objectY($obj) + $spacingY,
					   "middle", qq{$id onclick="NodeEvent(evt)"}, "black");

			if ($secondLine ne "") {
				outputText($midPointX,
						   $secondLine,
						   objectY($obj) + 2*$spacingY,
						"middle", qq{onclick="NodeEvent(evt)"}, "black");
			}
			if ($animate && $secondLine) {
				outputGroupEnd();
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
					my $relationshipName = RelationshipName($obj->{Name},$obj2->{Name});
					outputLine(objectSideX($obj, "n", 0), objectSideY($obj, "n", 0),
							   objectSideX($obj2, "s", 0), objectSideY($obj2, "s", 0),
							   "", qq{ id="$relationshipName" onclick="MessageEvent(evt)" } );
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
					my $relationshipName = RelationshipName($obj->{Name},$obj2->{Name});
					outputLine(objectSideX($obj, "w", 0), objectSideY($obj, "w", 0),
							   objectSideX($obj2, "e", 0), objectSideY($obj2, "e", 0),
							   "", qq{ id="$relationshipName" onclick="MessageEvent(evt)" });
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

	my $obj2X = objectSideX($obj2, $dir2, $attachPoint2);
	my $obj2Y = objectSideY($obj2, $dir2, $attachPoint2);

	if ($dir eq "n")
		{
		if ($animate) {
			# If animating, group together the two tabs and their accompanying
			# text labels using a "<g>" element.  We can then set the id of the group
			# to the relationship name (e.g. "IPCPr_Conn") and animate
			# all the pieces together.  Ordinarily, a direct line is used to represent
			# relationships and has the same id.
			my $relationshipName = RelationshipName($obj->{Name},$obj2->{Name});
			outputGroupStart($relationshipName);
		}

		# output Tabs at the top of objects - de-emphasise the tab labels with semi-condensed font option
		outputLine($objX, $objY, $objX, $objY - $tabLineLengthY, "", "");
		outputText($objX, getCurrentTabLabel(),
				   $objY - $tabLineLengthY - $tabToFontAdjustTopY,
				   "middle", qq{font-stretch="semi-condensed"}, "");

		# output Tabs at the bottom of objects
		outputLine($obj2X, $obj2Y + $tabLineLengthY, $obj2X, $obj2Y, "", "");
		outputText($obj2X, getCurrentTabLabel(), $obj2Y + $tabLineLengthY + $fontHeight + $tabToFontAdjustBottomY, "middle", qq{font-stretch="semi-condensed"}, "");

		if ($animate) {
			outputGroupEnd();
		}

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
			return objectX($obj) + ($attachPoint * 10);
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

sub normaliseCoords()
	{
	print "normaliseCoords():\n";
	foreach $group (keys %groupRanges) {
		my $rangeRef = $groupRanges{$group};

		my $x = $rangeRef->{minCol};
		my $y = $rangeRef->{minRow};

		print "\tgroup $group:\tmin ($rangeRef->{minCol},$rangeRef->{minRow}), max ($rangeRef->{maxCol},$rangeRef->{maxRow}) => ";

		$rangeRef->{minCol} = 0;
		$rangeRef->{minRow} = 0;
		$rangeRef->{maxCol} -= $x;
		$rangeRef->{maxRow} -= $y;

		print "min ($rangeRef->{minCol},$rangeRef->{minRow}), max ($rangeRef->{maxCol},$rangeRef->{maxRow})\n";

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
#				print "subGridPlacement $obj->{name} [ $col, $row ] zeroed\n";
				}
			else {
				if ($w_e_Relations) {
					++$subGridPlacement[$col][$row]{subRow};
#					print "subGridPlacement $obj->{name} [ $col, $row ] subRow = ", $subGridPlacement[$col][$row]{subRow},"\n";
					}
				if ($n_s_Relations) {
					++$subGridPlacement[$col][$row]{subCol};
#					print "subGridPlacement $obj->{name} [ $col, $row ] subCol = ", $subGridPlacement[$col][$row]{subCol},"\n";
					}
				}

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
	if ($debug) { print "\nseperateGroups():\n"; }
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
		$row += $ref->{maxRow} - $ref->{minRow} + 1;
		}
	if ($debug) { print "\n"; }
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

#
# Given two strings, "X" and "Y", return a string of the form "X_Y" where
# "X" is the lexically lesser in rank.
#
# For example: "A1" and "B2" yields "A1_B2".  The idea is to form a predictable
# and unique relationship name between two named nodes, where there are normally
# two possibilities (i.e. "A1_B2" and "B2_A1").

sub RelationshipName($$)
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
				my $relationshipName = RelationshipName($objRef->{Name}, $destObj->{Name});
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
		my $relationshipName = RelationshipName($srcName, $destName);
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
	AnimatePrologue();
	AnimateSequencerPart1();

	# Translate the animations we have built up in @animateSequence array into a string array in the SVG file:
	# Each entry in the array is a string which describes a step in the animation.  The string is of the form:
	# "<action>,<name>,<type>"
	# Where:
	# <action> = "0" for a destroy and "1" for a create
	# <name> = name of node or relationship
	# <type> = "n" to identify <name> as a node and "r" to identify it as a relationship.
	# For example "1,Conn1,n", "0,IPCPr1_Conn2,r" means Node "Conn1" is created then relationship
	# "IPCPr1_Conn2" is destroyed.  It is possible to infer <type> from the presence of an "_" in the
	# <name>, but this is considered too implicit and error prone to future modifications.
	my $name;
	my $first = 1;
	foreach my $seq (@animateSequence) {
		if (exists $seq->{object}) {
			# animate/de-animate a node
			if (hasRelationships($seq->{object})) {
				DoComma(\$first);
				print SVG qq{"$seq->{action},$seq->{object}{Name},n"};	# Rectangle & Text
			}
		} else {
			# animate/de-animate a relationship between nodes
			if (isRelated($seq->{srcObj}, $seq->{destObj})) {
				DoComma(\$first);
				print SVG qq{"$seq->{action},$seq->{name},r"};		# Line
			}
		}
	}
	AnimateSequencerPart2();

	# output "animate.html" defining the animate control panel popup
	outputAnimateHTML();
}

# tedium of ensuring that last entry in array doesn't have a trailing comma
sub DoComma($)
{
	my $firstRef = $_[0];
	if ($$firstRef) {
		$$firstRef = 0;
	} else {
		print SVG ",\n";
	}
}

sub AnimatePrologue()
	{
###########################
# Begin Interpolated Text #
###########################
	print SVG<<END

// TODO:
// - 0.8 opacity where required

var sequenceIndex = 0;

var firsttime = 1;
var animsMax = 2;
var anims = new Array();
var animsNextFree = 0;
var eventListener = true;

// The step period must be higher than the fade period !
var animationStepPeriod = 500;
var animationFadePeriod = "0.2s";

var timerHandle;
var timerActive;

var debugString = "";

var animationMode = false;

function debug(string)
{
	if (debugString.length > 100) {
		debugString = "";
	}
	debugString += string;
	window.status = debugString;
}

function initAnimate()
{
	var i;
	for (i = 0 ; i < animsMax ; ++i) {
		anims[i] = new Object();
		anims[i].element = document.createElementNS("http://www.w3.org/2000/svg", "animate");
		anims[i].active = false;
		with (anims[i].element) {
			setAttribute("attributeName", "opacity");
			setAttribute("attributeType", "XML");
			setAttribute("begin", "indefinite");
			setAttribute("dur", animationFadePeriod);
			setAttribute("fill", "freeze");
			if (eventListener) {
				addEventListener("end", eventEnd, false);
			}
		}
	}

	// HACK
	// The first animation of each dynamically created SVG animate element doesn't seem to work (at least
	// not in the Adobe SVG viewer), so we have to "kick start" them by performing dummy animate of an
	// invisible rectangle.

	var svg = document.getElementById("svgroot");
	for (i = 0 ; i < animsMax ; ++i) {
		var rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
		svg.appendChild(rect);
		animateElement(rect, false);
	}
}

// Called when an animation element finishes its animation
function eventEnd(evt)
{
	var animObj = findAnimateObjectByElement(evt.currentTarget);
	if (animObj) {
		releaseAnimateObject(animObj);
	} else {
		alert("eventEnd: cannot find associated animate object");
	}
}

function startTimer()
{
	timerHandle = setTimeout("animate(+1, true)", animationStepPeriod);
	timerActive = true;
}

function stopTimer()
{
	if (timerActive) {
		clearTimeout(timerHandle);
		timerActive = false;
	}
}

function setAllVisibility(disposition)
{
	setVisibility("rect", disposition);
	setVisibility("line", disposition);
	setVisibility("text", disposition);
	setVisibility("g", disposition);
}

function setVisibility(nodeName, disposition)
{
	var els = document.getElementsByTagNameNS("http://www.w3.org/2000/svg", nodeName);
	for (var i = 0 ; i < els.length ; ++i) {
		var node = els.item(i);
		// Only consider nodes with an id, as these represent the objects we are interested in -
		// relationship lines between nodes, node rectangles and node textual names.  This leaves
		// the group highlight rectangles intact.
		if (node.id) {
			if (disposition) {
				node.setAttribute("opacity", "1");	// what about 0.8??
			} else {
				node.setAttribute("opacity", "0");
			}
		}
	}
}

//
// Given an SVG animate element, find the associated anims[] entry that owns that element.
//
function findAnimateObjectByElement(element)
{
	for (var i = 0 ; i < animsMax ; ++i) {
		if (anims[i].element == element) {
			return anims[i];
		}
	}
}

function grabAnimateObject()
{
	if (animsNextFree < animsMax) {
		releaseAnimateObject(anims[animsNextFree]);

		anims[animsNextFree].active = true;
		return anims[animsNextFree++];
	} else {
		return null;
	}
}

function releaseAnimateObject(animObject)
{
	if (animObject.active) {
		releaseAnimateElement(animObject.element);
		animObject.active = false;
	}
}

function releaseAnimateElement(animElement)
{
	var parent = animElement.parentNode;
	if (parent) {
		// Once an animate element has finished animating its parent element, we need to remove it as
		// a child of that element to be re-used as a child of the next element to animate.  However, removing
		// it seems to reset the animated attribute of the parent back to its original pre-animated state.
		// Consequently, we have to explicitly set the parent attribute to the target value after removing
		// the child.
		var opacity = animElement.getAttribute("to");
		parent.removeChild(animElement);
		parent.setAttribute("opacity", opacity);
	}
}

// Once all animate elements have done their job, remove them from the parent nodes that they are animating
// so that they can be re-used.
function releaseAnimateObjects()
{
	if (!eventListener) {
		for (var i = 0 ; i < animsMax ; ++i) {
			releaseAnimateObject(anims[i]);
		}
	}
	animsNextFree = 0;
}

function animateElement(elementToAnimate, action)
{
	var from;
	var to;
	if (action) {
		// Make visible
		from = "0";
		to = "1";		// incomplete - need to go to 0.8 for rectangles
	} else {
		// Make invisible
		from = "1";		// incomplete - need to go from 0.8 for rectangles
		to = "0";
	}
	var animObject = grabAnimateObject();
	var animElement = animObject.element;
	animElement.setAttribute("from", from);
	animElement.setAttribute("to", to);
	elementToAnimate.appendChild(animElement);	// make the animation element a child of the parent element to animate
	animElement.beginElement();			// start animation
}

function animate(direction, continuous)
{
	// In an ideal world, the "end" animation event can be used to release the objects immediately
	// their animations have finished.  However, we do not seem to get "end" animate events at the end
	// of the first animation of an animation element.
	releaseAnimateObjects();

	var extendedStatus = false;
	if (direction == +1) {
		if (sequenceIndex < sequences.length) {
			var sequence = sequences[sequenceIndex];
			var fields = sequence.split(',', 3);
	
			animatePieces(fields, direction);
			if (++sequenceIndex < sequences.length && continuous) {
				startTimer();
			}
			extendedStatus = true;
		}
	} else
	if (direction == -1) {
		if (sequenceIndex > 0) {
			var sequence = sequences[--sequenceIndex];
			var fields = sequence.split(',', 3);

			animatePieces(fields, direction);
			extendedStatus = true;
		}
	}
	else
		alert("Bad direction " + direction + " in animate()");

	var text = sequenceIndex + " / " + (sequences.length - 1);
	if (extendedStatus) {
		text = FormStatusString(text, fields, direction);
	}
	SetStatusLine(text);
}

function FormStatusString(prefix, fields, direction)
{
	var text = prefix + ":  ";
	if (direction == -1) {
		text += "! ";
	}
	if (fields[0] == "0") {
		if (fields[2] == "n") {
			text += "Destroy ";
		} else {
			text += "Unrelate ";
		}
	} else {
		if (fields[2] == "n") {
			text += "Create ";
		} else {
			text += "Relate ";
		}
	}
	return text + fields[1];
}

function SetStatusLine(text)
{
	window.status = text;
}

function animatePieces(fields, direction)
{
	var action = (fields[0] == "0") ? false : true;
	if (direction == -1) {
		action = !action;
	}
	var el;
	if (fields[2] == "n") {
		// Object to animate is a Node.  Animate the rectangle and text.
		el = document.getElementById(fields[1]);
		animateElement(el, action);

		el = document.getElementById(fields[1] + "_t");
		animateElement(el, action);
	} else {
		// Object to animate is a Relationship between Nodes.  Animate the line.
		el = document.getElementById(fields[1]);
		animateElement(el, action);
	}
}


function setAnimationMode()
{
	if (!animationMode) {
		setAllVisibility(false);
		animationMode = true;
	}
}

function resetAnimationMode()
{
	if (animationMode) {
		setAllVisibility(true);
		animationMode = false;
		sequenceIndex = 0;
	}
}

function animateCommand(cmd)
{
	switch (cmd) {
	case "p":				// Play
		setAnimationMode();
		startTimer();
		break;
	case "rw":				// Rewind
		stopTimer();
		setAllVisibility(false);
		animationMode = true;
		sequenceIndex = 0;
		break;
	case "s":				// Stop
		stopTimer();
		releaseAnimateObjects();
		break;
	case "+":				// Step Forward
		stopTimer();
		setAnimationMode();
		animate(+1, false);
		break;
	case "-":				// Step Backward
		stopTimer();
		setAnimationMode();
		animate(-1, false);
		break;
	case "rs":				// Restore
		stopTimer();
		resetAnimationMode();
		break;
	default:
		alert("animateCommand: Invalid command: " + cmd);
		break;
	}
}
END
############################
# ...End Interpolated Text #
############################
}

sub AnimateSequencerPart1()
{
##############################
# Begin Interpolated Text... #
##############################
	print SVG<<END
// sequences[] is an array of strings defining the animation sequence, where each string is of the form:
// "<action>,<name>,<type>"
// <action> = "0" for destroy, "1" for create
// <name> = name of node or relationship between nodes
// <type> = "0" if <name> refers to a node, "1" if a relationship between nodes

 var sequences = new Array(
END
############################
# ...End Interpolated Text #
############################
}

sub AnimateSequencerPart2()
{
	print SVG ");\n\n";
}

sub processPathArgument($)
{
	my $path = $_[0];
	if ($path) {
		# ensure "/" at the end
		if ($path !~ /\/$/) {
			$path .= "/";
		}
		mkdirp($path);
		return $path;
	} else {
		return "";
	}
}

sub mkdirp($) 
{ 
    my $dirName = @_[0]; 
    if ($dirName =~ m/^(.*)\//i) { 
        if ($1 ne "") { 
            mkdirp($1); 
        } 
    }
	if (! -d $dirName) {
		mkdir($dirName); 
	}
}
