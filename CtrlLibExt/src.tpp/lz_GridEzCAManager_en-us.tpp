topic "VAManager";
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,2#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_} 
[ {{10000@(113.42.0) [s0;%% [*@7;4 CAManager]]}}&]
[s1;:lz`:`:GridEz`:`:CAManager: [*3 CAManager]&]
[s2;%% It`'s the abbreviation for Client Area manager. This class 
is entrusted with the responsibility to allocate the client area 
of [* GridEz], considering [* ScrollBar] positions, deciding which 
rows and columns get to be displayed on the screen and how much 
height/width they each have. It is also responsible for coordinate 
translation between different perspectives.&]
[s2;%% &]
[s2;%% Rows and Columns are managed similarly. So we talk about columns 
only. For a [* GridEz] object [@3 g], if [@3 g].[* IsFitColumns()] is 
true, all columns will be scaled and crammed onto the screen. 
In this case, [* GridEz] perspective is identical to FullPicture 
perspective. Here FullPicture is used without a definition, but 
you should get it. In the more general case when [@3 g].[* IsFitColumns()] 
is false, certain columns will have to be left out. The [* ScrollBar] 
position, and whether a column is fixed will decide which ones 
get to be chosen to stay, or be [* visible].&]
[s2;%% &]
[s2;%% Let`'s say we have an array of visible columns like this:&]
[s2;%% [@(0.0.255) F1, F2, P10, S11, S12, S13, P14, F79, F80,]&]
[s2;%% with [@(0.0.255) F1], [@(0.0.255) F2] fixed columns on the left, 
[@(0.0.255) F79], [@(0.0.255) F80] fixed columns on the right, [@(0.0.255) P10] 
a partial visible column with part of its left covered by left 
fixed columns, [@(0.0.255) P14] a partially visible column with 
its right part covered or trimmed, and [@(0.0.255) S11] to [@(0.0.255) S13] 
are fully visible columns in scrollable area. We can imagine, 
with other conditions fixed, if we change the value of the horizontal 
[* ScrollBar], [@(0.0.255) F1], [@(0.0.255) F2], [@(0.0.255) F79], and 
[@(0.0.255) F80] will remain unchanged, but a new set of columns, 
for example, [@(0.0.255) P50], [@(0.0.255) S51], [@(0.0.255) S52], 
[@(0.0.255) F53], [@(0.0.255) F54] will take [@(0.0.255) P10 ]through 
[@(0.0.255) P14]`'s place. Notice the Full Picture columns index 
numbers are in sequence for each of the groups of fixed left 
columns, scrollable area, fixed right columns. These are hard 
coded rules. So we have [@(0.0.255) visibles`[0`]]`=[@(0.0.255) F1],..., 
[@(0.0.255) visibles`[8`]]`=[@(0.0.255) F80]. Now the two partially 
visible columns, [@(0.0.255) visibles`[2`]]`=[@(0.0.255) P10] and 
[@(0.0.255) visibles`[6`]]`=[@(0.0.255) P14] are of special interest 
to us. To draw them properly, we need to know the width of them 
that are not visible, respectively. Of course, both or either 
value can be 0, but never negative.&]
[s0;i448;a25;kKO9;@(0.0.255) &]
[ {{10000F(128)G(128)@1 [s0;%% [* Function List]]}}&]
[s0;i448;a25;kKO9;:noref:@(0.0.255) &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s2;%% &]
[s3; &]
[s4; &]
[s5;:lz`:`:GridEz`:`:CAManager`:`:Accum`:`:start`(int`)const: [@(0.0.255) int] 
[* start]([@(0.0.255) int] [*@3 rc]) [@(0.0.255) const]&]
[s2;%% returns the start position on the full picture for full picture 
row or column index [%-*@3 rc] .&]
[s3; &]
[s4; &]
[s5;:lz`:`:GridEz`:`:CAManager`:`:Accum`:`:IndexFullPictureToGrid`(int`)const: [@(0.0.255) i
nt] [* IndexFullPictureToGrid]([@(0.0.255) int] [*@3 fp`_index]) [@(0.0.255) const]&]
[s2;%% Take column for example. Given a column index [%-*@3 fp`_index] 
in the full picture, return the column index of it in GridEz. 
If it`'s not in the GridEz screen, returns `-1.&]
[s3; &]
[s4; &]
[s4; &]
[s5;:lz`:`:GridEz`:`:CAManager`:`:Accum`:`:alloc`(int`,int`,int`,int`&`,int`&`,int`&`,int`&`,int`&`,int`&`): [@(0.0.255) v
oid] [* alloc]([@(0.0.255) int] [*@3 fixedstart], [@(0.0.255) int] [*@3 fixedend], 
[@(0.0.255) int] [*@3 cx], [@(0.0.255) int`&] [*@3 lcx], [@(0.0.255) int`&] 
[*@3 ccx], [@(0.0.255) int`&] [*@3 rcx], [@(0.0.255) int`&] [*@3 lcxf], 
[@(0.0.255) int`&] [*@3 ccxf], [@(0.0.255) int`&] [*@3 rcxf])&]
[s0;l256; Allocate space. Take columns for example, given number 
of fixed left columns [*@3 fixedstart], number of fixed right columns 
[*@3 fixedend], total width [*@3 cx], output total width of fixed 
left columns in the full picture [*@3 lcxf][%%  ], total center columns 
width in full picture [*@3 ccxf], total fixed right columnes in 
the full picture [*@3 rcxf], total width of fixed left columns 
in the GridEz [*@3 lcx], total width of center columns in the GridEz 
[*@3 ccx], and total width of right fixed columns in the GridEZ 
[*@3 rcx].&]
[s2;%% &]
[s5;:lz`:`:GridEz`:`:CAManager`:`:Accum`:`:UpdateVisibles`(int`,int`,int`): [@(0.0.255) v
oid] [* UpdateVisibles]([@(0.0.255) int] [*@3 fixedstart], [@(0.0.255) int] 
[*@3 fixedend], [@(0.0.255) int] [*@3 cx])&]
[s2;%% Update Accum`::visibles vector. Take column for examples, this 
function starts with a width of [%-*@3 cx] . It first allocate 
space for the left fixed columns whose count is [%-*@3 fixedstart] 
. If there is space left, it next allocate for the right fixed 
colmns whose count is given by [%-*@3 fixedend]. If there are more 
space left, it`'s allocated to scrollable columns based on the 
offset specified by the scroll bar..&]
[s3; &]
[s0;%% ]]