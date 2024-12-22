topic "struct PopUpEx";
[2 $$0,0#00000000000000000000000000000000:Default]
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,2#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[{_} 
[s1;:PopUpEx`:`:struct: [@(0.0.255)3 struct][3 _][*3 PopUpEx]&]
[s2;%% Primary for some virtual functions that can be reimplemented 
to provided customized behavior on the main Ctrl`'s side.&]
[s3; &]
[s0;*@7;4%% &]
[ {{10000@(113.42.0) [s0;%% [*@7;4 Title]]}}&]
[s0;i448;a25;kKO9;@(0.0.255) &]
[ {{10000F(128)G(128)@1 [s0;%% [* Function List]]}}&]
[s0;i448;a25;kKO9;:noref:@(0.0.255) &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s4; &]
[s5;:PopUpEx`:`:GetSizePosition`(Rect`&`): [@(0.0.255) virtual] [@(0.0.255) void]_[* GetSiz
ePosition]([_^Rect^ Rect][@(0.0.255) `&]_[*@3 rt])&]
[s2;%% Main has precedence over where it want the popup be displayed 
and at what size. However it can allow the PopMan to decide the 
size and location of the popup by setting rt to Null, which is 
default behavior. .&]
[s3;%% &]
[s4; &]
[s5;:PopUpEx`:`:AcceptData`(const Value`&`,const Value`&`): [@(0.0.255) virtual] 
[@(0.0.255) void]_[* AcceptData]([@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&]_[*@3 key],
 [@(0.0.255) const]_[_^Value^ Value][@(0.0.255) `&]_[*@3 text])&]
[s0; -|PopUp is closed and data with (key, text) is selected. It`'s 
up to the main ctrl to decide how to use it. Eg. it may use SetData 
to update to key, or it may add it to a list of items.&]
[s3;%% &]
[s4; &]
[s5;:PopUpEx`:`:Cancel`(`): [@(0.0.255) virtual] [@(0.0.255) void]_[* Cancel]()&]
[s2;%% PopUp is closed without a selection being made. Main may take 
this chance to restore its original data.&]
[s3; &]
[s0;%% ]]