topic "struct PopMan";
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
[s1;:lz`:`:PopSlave: [@(0.0.255)3 struct][3 _][*3 PopSlave]&]
[s2;%% Popup Manager class. A [/ PopEz] Popup is expected to have this 
physical layout:&]
[s2;l448;%% [*C@(0.0.255) class][C  MyPopUp : ][*C@(0.0.255) public][C  PopSlave,]&]
[s2;l448;%% [C -| ][*C@(0.0.255) public][C  ArrayCtrl /`* or any widget 
derived from Ctrl `*/]&]
[s2;l448;%% [C `{]&]
[s2;l448;%% [C -|//.....]&]
[s2;l448;%% [C `};]&]
[s2;l320;%% with PopSlave helping to communicate between the main 
Ctrl and the Popup. [C  ]&]
[s3;l448; &]
[s0;*@7;4%% &]
[ {{10000@(113.42.0) [s0;%% [*@7;4 Title]]}}&]
[s0;i448;a25;kKO9;@(0.0.255) &]
[ {{10000F(128)G(128)@1 [s0;%% [* Function List]]}}&]
[s0;i448;a25;kKO9;:noref:@(0.0.255) &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s1;:lz`:`:PopSlave`:`:PopUpHint: [@(0.0.255)3 struct][3 _][*3 PopUpHint]&]
[s2;%% a struct used to communicate context, cursor location, mouse 
location, selection, etc from owner Ctrl to PopMan. Current implementation 
use non`-extensible definition. It might be desirable to design 
PopUpHint as a virtual base class so that it can be customized.&]
[s3; &]
[s4; &]
[s5;:lz`:`:PopSlave`:`:Show: [* HintReason`::Show]&]
[s2;%% a HintReason that explicitly request the Popup be popped up. 
.&]
[s3; &]
[s4; &]
[s5;:lz`:`:PopSlave`:`:Changed: [* HintReason`::Changed]&]
[s2;%%  .a HintReason that indicates changes at cursor, change in 
cursor position, change in selecction, etc.&]
[s3; &]
[s4; &]
[s5;:lz`:`:PopSlave`:`:DoKey`(dword`,int`): [@(0.0.255) virtual] [@(0.0.255) bool]_[* DoKey
]([_^dword^ dword]_[*@3 key], [@(0.0.255) int]_[*@3 repcount])&]
[s0;l288; In the small group consists of main and popup, the main 
will receive keyboard event; after filtering out a few keystrokes, 
the main will forword the key event to PopMan, ie, this function. 
PopMan should only consume the ones of interests to it and for 
the others, it should return false so that the main gets a chance 
to reexamine the key event.&]
[s2;i224; [*@3 key][%%   same as in Ctrl`::Key(][_^dword^ dword]_[*@3 key], 
[@(0.0.255) int]_[*@3 repcount)]&]
[s2;i224;%%  [%-*@3 repcount] . ditto&]
[s4;i224; &]
[s5;:lz`:`:PopSlave`:`:PrepareData`(const PopupHint`&`,String`*`): [@(0.0.255) virtual] 
[@(0.0.255) int]_[* PrepareData]([@(0.0.255) const]_[_^PopMan`:`:PopUpHint^ PopUpHint][@(0.0.255) `&
]_[*@3 hint, ]String`* [*@3 suffix ])&]
[s2;%%  [%-*@3 hint] PopUp use information in it to prepare data to 
be popped up.&]
[s2;%% [%-*@3 suffix ]the common suffix items in prepared data share. 
This might to served to main for auto completion. Implementation 
is expected to calculate it if this parameter is not NULL.&]
[s2;%% returns number of record prepared, no need to be exact, most 
likely we only need to differentiate 3 types, 0 record, 1 record, 
and 2 or more records .&]
[s3;%% &]
[s4; &]
[s5;:lz`:`:PopSlave`:`:GetPreferredSize`(`)const: [@(0.0.255) virtual] 
Size_[* GetPreferredSizes]()[@(0.0.255) const]&]
[s0;l320; PopUp tell the PopMan what`'s its preferred size. Minimum 
doesn`'t seem to serve any practical purpose. I may opt to remove 
it in the future.&]
[s3;%% &]
[s4; &]
[s5;:PopMan`:`:NegotiateSizePosition`(Rect`&`): [@(0.0.255) void]_[* NegotiateSizePositio
n]([_^Rect^ Rect][@(0.0.255) `&]_[*@3 rect])&]
[s2;%%  [%-*@3 rect] .&]
[s3;%% &]
[s4; &]
[s5;:PopMan`:`:Select`(`): [@(0.0.255) virtual] [@(0.0.255) bool]_[* Select]()&]
[s0; -|This virtual method is designed to &]
[s0;l288;i96;O0; feed user selection to main, for example, set main`'s 
data, or add the selection to main`'s list, etc;&]
[s0;l288;i96;O0;%% [%- return true to allow the pop up be closed; false 
otherwise. For example, a TreeCtrl pop up can tailor this function 
so that only leaf nodes can be selected.] &]
[s3; &]
[s4; &]
[s5;:PopMan`:`:SetAutoPopUp`(bool`): [@(0.0.255) virtual] [@(0.0.255) void]_[* SetAutoPopUp
]([@(0.0.255) bool]_[*@3 popup][@(0.0.255) `=true])&]
[s2;%%  Set if the PopMan can decide to pop up without GUI user`'s 
explicit request to open it by way of key stroke (ALT`-DOWN/ALT`-UP) 
or mouse(eg, a drop down button). ProcessHints uses this flag.&]
[s3;%% &]
[s4; &]
[s5;:lz`:`:PopMainEx`:`:SetAutoComplete`(bool`): [@(0.0.255) virtual] 
PopMainEx`& [* SetAutoComplete]([@(0.0.255) bool]_[*@3 v][@(0.0.255) `=true])&]
[s2;%% Autocompletion is the action to insert common suffix/prefix 
automatically to minimize key stroke. For example, user entered 
`"el`", possible candidates are .`"election`", `"electric`",`"electronics`", 
then the common suffix is `"ect`", means we can attach `"ect`" 
to user input to form `"elect`" without disqualifying any candidates 
but can possible save the user 3 key strokes. Automatically add 
common suffix/prefix is called Autocompletion in PopEz jargon.&]
[s3;%% &]
[s4; &]
[s5;:lz`:`:PopMainEx`:`:GetAutoComplete`(`)const: [@(0.0.255) bool] 
[* GetAutoComplete]() [@(0.0.255) const]&]
[s2;%% see  .[%-*^topic`:`/`/PopEz`/src`/PopSlave`_en`-us`#lz`:`:PopMainEx`:`:SetAutoComplete`(bool`)^ S
etAutoComplete].&]
[s3; &]
[s4; &]
[s5;:lz`:`:PopMainEx`:`:SetAutoSelect`(bool`): [@(0.0.255) virtual] 
PopMainEx`& [* SetAutoSelect]([@(0.0.255) bool]_[*@3 v][@(0.0.255) `=true])&]
[s2;%% Automatic selection is the action of calling Select() as if 
user hit Enter key when there is one and only one qualifying 
candidate to user input.&]
[s3;%% &]
[s0;%% ]]