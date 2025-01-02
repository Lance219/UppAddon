topic "";
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s0;*@7;4%% &]
[ {{10000@(113.42.0) [s0;%% [*@7;4 GridEz]]}}&]
[s1; &]
[s4; &]
[s0;i448;a25;kKO9;:lz`:`:GridEz: [@(0.0.255)2 class ][2 GridEz][@(0.0.255)2  
: public ][2 Ctrl]&]
[s0;l256;a25;kKO9;%% [2 A GridCtrl/ArrayCtrl like Ctrl who actually 
supports joining cells.]&]
[ {{10000F(128)G(128)@1 [s0;%% [* Function List]]}}&]
[s2;:lz`:`:GridEz`:`:GridToCell`(Point`,CellPoint`&`): CellPoint[@(0.0.255) `&] 
[* GridToCell](Point [*@3 p], CellPoint[@(0.0.255) `&] [*@3 cp])&]
[s0;l256; [2 Find the corresponding row,column and relative postion 
of a Point ][*@3;2 p ][2 in the GridEz coordinated.]&]
[s3;%% [%-*@3 cp] .the buffer to store the translation result which 
is returned by the function.&]
[s4; &]
[s2;:lz`:`:GridEz`:`:GridToCell`(Point`): CellPoint [* GridToCell](Point 
[*@3 p])&]
[s3; Find the corresponding row,column and relative postion of a 
Point [*@3 p ]in the GridEz coordinated.&]
[s0;%% &]
[s2;:lz`:`:GridEz`:`:GridToFP`(Point`)const: Point [* GridToFP](Point 
[*@3 p]) [@(0.0.255) const]&]
[s3;%%  Translate a point from GridEz coordinates to the full picture 
coordinates. This will take into consideration of ScrollBar positions.&]
[s3;%% [%-*@3 p] .a point in the GridEz coordinates for whom a translation 
is needed.&]
[s4; &]
[s2;:lz`:`:GridEz`:`:GridToFPx`(int`)const: [@(0.0.255) int] [* GridToFPx]([@(0.0.255) int] 
[*@3 x]) [@(0.0.255) const]&]
[s3;%% Translate the [%-*@3 x`-]coordinate of a point from GridEz coordinates 
to the corresponding full picture coordinates. This will take 
into consideration of ScrollBar positions.&]
[s3;%% [%-*@3 x] .the x`-coordinate in the GridEz coordinates for whom 
a translation is needed.&]
[s2;:lz`:`:GridEz`:`:GridToFPy`(int`)const: [@(0.0.255) int] [* GridToFPy]([@(0.0.255) int] 
[*@3 y]) [@(0.0.255) const]&]
[s3;%% Translate the y[%-*@3 `-]coordinate of a point from GridEz coordinates 
to the corresponding full picture coordinates. This will take 
into consideration of ScrollBar positions.&]
[s3;%% [%-*@3 y] .the x`-coordinate in the GridEz coordinates for whom 
a translation is needed.&]
[s2;@(0.0.255) &]
[s2;:lz`:`:GridEz`:`:GetColumnIndexAt`(int`)const: [@(0.0.255) int] 
[* GetColumnIndexAt]([@(0.0.255) int] [*@3 x]) [@(0.0.255) const]&]
[s3;%%  Get the column index at [%-*@3 x] in GridEz coordinates..&]
[s2;@(0.0.255) &]
[s2;:lz`:`:GridEz`:`:GetRowIndexAt`(int`)const: [@(0.0.255) int] [* GetRowIndexAt]([@(0.0.255) i
nt] [*@3 y]) [@(0.0.255) const]&]
[s3;%%  Get the row index at [%-*@3 y] in GridEz coordinates..&]
[s3;%%   .&]
[s1; &]
[s2;:lz`:`:GridEz`:`:DoColWidthNSbs`(Size`&`): [@(0.0.255) void] [* DoColWidthNSbs](Size[@(0.0.255) `&
] [*@3 sz])&]
[s3;%% Do column width and ScrollBars. [%-*@3 sz] is modified accordingly..&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:IsRowInI`(int`)const: [@(0.0.255) bool] [* IsRowInI]([@(0.0.255) int] 
[*@3 r]) [@(0.0.255) const]&]
[ {{3333:3333:3334<448;>2176; [s3;l192; [*@3 A]]
:: [s3;l192; [*@3 B]]
:: [s3;l224; [*@3 C]]
:: [s3;l192; [*@3 D]]
:: [s3;l192; [*@3 E]]
:: [s3;l224; [*@3 F]]
:: [s3;l192; [*@3 G]]
:: [s3;l192; [*@3 H]]
:: [s3;l224; [*@3 I]]}}&]
[s3;%% Same as [* IsFixedBtmRow]([%-@(0.0.255) int][%-  ][%-*@3 r]) [%-@(0.0.255) const]. 
Note areas [*@3 B, E, H ]are affected by horizontal scrolling, 
while areas [*@3 D, E, F] are affected by vertically scrolling.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:ScrollInto`(int`): GridEz[@(0.0.255) `&] [* ScrollInto]([@(0.0.255) in
t] [*@3 row])&]
[s3;%% make row of index [%-*@3 row] visible..&]
[s4; &]
[s2;:lz`:`:GridEz`:`:joinedcells: [_^Upp`:`:Vector^ Vector]<[_^Upp`:`:Rect^ Rect]>_[* joine
dcells]&]
[s3;%% There is good reason to assume that there will not be frequent 
inserting and removing of joined cells information, hence Vector 
is chosen instead of [@(141.42.0) set], [@(170.42.0) map], or even 
[@(170.42.0) InVecto][@(198.42.0) r]. [* joinedcells] shall be in sorted 
order, insertion and deletion will maintain this. It`'s wise 
to add joined cells from top left to bottom right. Eg, a joined 
region with top left (1,5) will be stored before region with 
top left (1,10). etc, etc. While in the more frequently used 
and performance demanding painting process, joined cells information 
are accessed in almost constant time.&]
[s1; &]
[s2;:lz`:`:GridEz`:`:resizing`_rc: int16 [* resizing`_rc]&]
[s3;%% This variable is updated in CursorImage(). If it`'s 0, press 
left mouse button and move will not result in a resizing. If 
it`'s positive,  [%-* resizing`_rc]`-1 is the index of the screen 
row (can be mapped to the index of the corresponding full picture 
row) that`'s being resized (if HasCapture()) or the candidator 
for resizing. If it`'s negative, `-[%-* resizing`_rc]`-1is the 
index of the screen column (can be mapped to the index of the 
corresponding full picture column that`'s being resized (if HasCapture()) 
or the candidator for resizing.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:autohidesbs: [@(0.0.255) bool] [* autohidesbs] [@(0.0.255) :] 
[@3 1]&]
[s3;%% automatically hide ScrollBars. .&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:fitcolumns: [@(0.0.255) bool] [* fitcolumns] [@(0.0.255) :] 
[@3 1]&]
[s3;%% Scale column widths so that all columns fit in the currently 
available [* GridEz] client area.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:fitrows: [@(0.0.255) bool] [* fitrows] [@(0.0.255) :] 
[@3 1]&]
[s3;%% Scale row heights so that all rows fit in the currently available 
[* GridEz] client area. .&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:colinnaturalorder: [@(0.0.255) bool] [* colinnaturalorder] 
[@(0.0.255) :] [@3 1]&]
[s3;%% flag to show if columns are in natural order. this flag is 
meant for column reordering which hasn`'t been implemented yet. 
.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:rowinnaturalorder: [@(0.0.255) bool] [* rowinnaturalorder] 
[@(0.0.255) :] [@3 1]&]
[s3;%% flag to show if rows are in natural order. this flag is meant 
for row reordering which hasn`'t been implemented yet.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:colcanresize: [@(0.0.255) bool] [* colcanresize] 
[@(0.0.255) :] [@3 1]&]
[s3;%% Indicates if column resizing allowed. A column is allowed 
to be resized through GUI if the GridEz`'s [%-* colcanresize].and 
the column`'s own [* canresize]are both true.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:rowcanresize: [@(0.0.255) bool] [* rowcanresize] 
[@(0.0.255) :] [@3 1]&]
[s3;%% Indicates if row resizing allowed. A row is allowed to be 
resized through GUI if the GridEz`'s [%-* rowcanresize].and the 
row`'s own [* canresize] are both true.&]
[s3;%%  .&]
[s0;%% ]]