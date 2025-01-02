topic "";
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[i448;a25;kKO9; $$5,0#37138531426314131252341829483380:structitem]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s1; &]
[s2;:lz`:`:Mapper`:`:y2: [@(0.0.255) int]_[* y2]&]
[s3;%% x1 is the width of left fixed columns, x2 is the total width 
of left fixed columns and center scrollable columns.&]
[s0;l288;%% -|y1 is the height of top fixed rows, y2 is the total 
height of top fixed rows and center scrollable rows.&]
[s4; &]
[s5;:lz`:`:Mapper`:`:struct: [@(0.0.255) struct]_[* Mapper]&]
[s3;%% Mapper is responsible for determining a cell`'s position both 
in the full picture and its mapping into the screen. Mapper is 
also entrusted the job of joined cells management for painting 
purpose, eg, quickly determining if a cell is the leading cell 
in a joined region (hence need to be painted) or if it belong 
to a region that`'s already painted(hence skip further painting).&]
[s4; &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:start`(int`)const: [@(0.0.255) int]_[* start]([@(0.0.255) int
]_[*@3 rc])[@(0.0.255) const]&]
[s3; Returns the startig position (x`-coordinate if [*@3 rc] is a column 
index, y`-coorid if [*@3 rc] is a row index) in the Full Picture.&]
[s3; &]
[s3; [*@3 rc: ]the index of the row or column for whom the starting 
position in the Full Picture is required. Valid range are from 
0 to the number of rows(or columns)`-1.&]
[s4;%% &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:end`(int`)const: [@(0.0.255) int]_[* end]([@(0.0.255) int]_[*@3 r
c])[@(0.0.255) const]&]
[s3; [%%  ][*@3 rc:][%%  ]the index of the row or column for whom the ending 
position in the Full Picture is required. Valid range are from 
0 to the number of rows(or columns)`-1.Note start/end are in 
left`-close`-right`-open way.So returned value is 1 past the 
actual end position of row/column [@3 rc.]In the function[*  MakeVisible], 
&]
[s3; [C `_end`=end(index)`-1;]&]
[s0;l288; [2 The subtraction of 1 is for this reason.]&]
[s0;@3 &]
[s4;%% &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:GridToFullPicture`(int`)const: [@(0.0.255) int]_[* GridToFu
llPicture]([@(0.0.255) int]_[*@3 position])[@(0.0.255) const]&]
[s3;%%  [%-*@3 position] the x`- or y`- coordinate of a point in the 
Grid..&]
[s3;%% &]
[s0;l288;%% Returns the x`- or y`- coordinate of the corresponding 
point in Full Picture. &]
[s4;%% &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:FullPictureToGrid`(int`,int`&`)const: [@(0.0.255) int]_[* F
ullPictureToGrid]([@(0.0.255) int]_[*@3 position], [@(0.0.255) int`&]_[*@3 section])[@(0.0.255) c
onst]&]
[s3;%%  [%-*@3 position] a coordinate in full picture to be translated 
into GridEz coordinate;&]
[s3;%% [%-*@3 section] .output the section (0 for section A, 1 for 
section E, 2 for section I) that position is in&]
[s0;l288;%% &]
[s0;l288;%% Returns the GridEz coordinate.&]
[s4;%% &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:OffsetI`(`)const: [@(0.0.255) int]_[* OffsetI]()[@(0.0.255) c
onst]&]
[s3;%% return the difference in x/y between Full Picture and Grid 
view for a point in Section I. It`'s used internally when mapping 
a coordination in Grid coordinate to Full Picture coordinate&]
[s4; &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:IsVisible`(int`)const: [@(0.0.255) bool]_[* IsVisible]([@(0.0.255) i
nt]_[*@3 index])[@(0.0.255) const]&]
[s3;%%  [%-*@3 index] the index of the row or column for whom visibility 
is checked.&]
[s3;%% if a row/column`'s[@3  index] is in the [@3 visible`_rc] vector, 
it`'s visible.&]
[s4;%% &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:MakeVisible`(int`): [@(0.0.255) int]_[* MakeVisible]([@(0.0.255) i
nt]_[*@3 index])&]
[s0;l288;%%  [%-*@3 index:] the row or column number that we will attempt 
to make fully visible by changing scroll.&]
[s0;l288;%% &]
[s0;l288;%% return the proposed new value of scroll.&]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:ends: [_^Upp`:`:Vector^ Vector]<[@(0.0.255) int]>_[* ends]&]
[s3;%% Holds the accumulated widths/heights in full picture.Indexed 
by column/row number starting from 0.&]
[s4; &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:dimA: [@(0.0.255) int]_[* dimA]&]
[s3;%% the [@(0.0.255) cx] or [@(0.0.255) cy] of section A in the Grid. 
Note the cx/cy should be the same on Grid or in full picture 
except when the Grid is too small to hold even section A (then 
no room will be left for Section I, not to mention Section E).&]
[s4; &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:dimE: [@(0.0.255) int]_[* dimE]&]
[s3;%% the [@(0.0.255) cx] or [@(0.0.255) cy] of section E in the Grid.&]
[s4; &]
[s1; &]
[s2;:lz`:`:Mapper`:`:Accum`:`:dimFE: [@(0.0.255) int]_[* dimFE]&]
[s3;%% the [@(0.0.255) cx] or [@(0.0.255) cy] of section E in the Full 
Picture. &]
[s4; &]
[s1; &]
[s2;:lz`:`:Mapper`:`:joined: [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:Vector^ Vector]<[@(0.0.255) i
nt]>>_[* joined]&]
[s3;%% [* joined] is a matrix. indexed by row index /col index from 
screen perspective.&]
[s3;%% &]
[s3;%% In particular, if joined`[screenRow`]`[screenCol`] is&]
[s0;l352;O0;%% [2 `-    ][*@3;2 `-1][2 , the screen cell at (screenRow, 
screenCol) is an independant cell]&]
[s0;l352;O0;%% [2       ][*@3;2 v][2  (v!`=`-1), the screen cell at (screenRow, 
screenCol) is part of a region identified by ][*@3;2 v][2  (index 
in the JoinedCellManager`::joindedCells vector) of the region.]&]
[s3;%% &]
[s0;l288;%% [2 For example, a ][*2 joined][2  table like]&]
[s0;l288;%% &]
[s0;l288;%% &]
[ {{999:1223:1111:1111:1111:1111:1111:1111:1112<288;>128;@(229) [s0;l288;$(229)2%% ]
:: [s0;l32;%% [$(229)2 0(0)]]
:: [s0;%% [$(229)2 1(1)]]
:: [s0;%% [$(229)2 2(10)]]
:: [s0;%% [$(229)2 3(11)]]
:: [s0;%% [$(229)2 4(12)]]
:: [s0;%% [$(229)2 5(13)]]
:: [s0;%% [$(229)2 6(14)]]
:: [s0;%% [$(229)2 7(20)]]
:: [s0;%% [2 0(0)]]
::@2 [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 0]]
:: [s0;%% [2 0]]
:: [s0;%% [2 0]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
::@(229) [s0;%% [2 1(100)]]
::@2 [s0;%% [2 1]]
:: [s0;%% [2 1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
::@(229) [s0;%% [2 2(101)]]
::@2 [s0;%% [2 1]]
:: [s0;%% [2 1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 2]]
:: [s0;%% [2 2]]
:: [s0;%% [2 `-1]]
::@(229) [s0;%% [2 3(102)]]
::@2 [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 2]]
:: [s0;%% [2 2]]
:: [s0;%% [2 `-1]]
::@(229) [s0;%% [2 4(103)]]
::@2 [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;~~~160;%% [2 `-1]]
:: [s0;%% [2 2]]
:: [s0;%% [2 2]]
:: [s0;%% [2 `-1]]
::@(229) [s0;%% [2 5(900)]]
::@2 [s0;%% [2 `-1]]
:: [s0;%% [2 3]]
:: [s0;%% [2 3]]
:: [s0;%% [2 3]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]
:: [s0;%% [2 `-1]]}}&]
[s1;l256;%% [2 in the above matrix, `-1 indicates the cell is independent, 
 cell (0,2)`-(0,4) belong to region 0, cell (2,5)`-(4,6) belong 
to region 2, etc. The painting process will go through this matrix, 
if it sees a `-1, it draws a cell; if it sees a `-2, it skip 
the cells, if it sees a number >`=0, it draws the region and 
set all cells in the region `-2. So the drawing process is destructive 
to the above matrix. User can access these information through 
WhenPaintingCell callback, from it one can mark cells `-2 to 
skip its painting (if it has not been painted yet).]&]
[s0;l288;%% ]]