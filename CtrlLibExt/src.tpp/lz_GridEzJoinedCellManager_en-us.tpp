topic "JoinedCellManager";
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:codeitem]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s1;:lz`:`:GridEz`:`:JoinedCellManager: [* JoinedCellManager]&]
[s0;l288;%% manages regions of joined cells. A Rect is used to store 
the rows and columns of the region, specifically, `[left, right) 
for columns and `[top, bottom) for rows.&]
[s2;%% &]
[s0;%% &]
[s3; &]
[s4; &]
[s5;:lz`:`:GridEz`:`:JoinedCellManager`:`:Add`(int`,int`,int`,int`): [@(0.0.255) voidl] 
[* Add]([@(0.0.255) int] [*@3 leftCol], [@(0.0.255) int] [*@3 topRow], 
[@(0.0.255) int] [*@3 rightCol], [@(0.0.255) int] [*@3 btmRow])&]
[s2;%% add a region [*@3 r]([%-*@3 leftCol,] [%-*@3 topRow,] [%-*@3 rightCol, 
btmRow]) [* Note ]that the action is linear. It will check each 
existing region [*@3 b]. If [*@3 r].intersects([*@3 b]), [*@3 b] will 
be removed from [*@3 joinedCells].&]
[s2;%% &]
[s0;l288;%% [*2 Add][2 () will mainted ][*@3;2 joinedCells][2  in ascending 
order of (left, top). Note no two regions shall have the same 
(left, top). ]&]
[s3; &]
[s4; &]
[s5;:lz`:`:GridEz`:`:JoinedCellManager`:`:RemoveRegionContainsCell`(int`,int`): [@(0.0.255) b
ool] [* RemoveRegionContainsCell]( [@(0.0.255) int] [*@3 col], [@(0.0.255) int] 
[*@3 row])&]
[s2;%% Remove from the joinedCells database the region that contains 
the cell at column [%-*@3 col ][%- and row] [%-*@3 row] . Returns [@(0.0.255) true] 
if a region is deleted, [@(0.0.255) false] otherwise.&]
[s3; &]
[s0;%% ]]