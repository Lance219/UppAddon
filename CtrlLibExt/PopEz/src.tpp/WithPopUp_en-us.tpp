topic "template <class T>";
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
[{_}%EN-US 
[s1;:noref:%- [@(0.0.255)3 template][3 _<][@(0.0.255)3 class][3 _][*@3;3 T][3 >]&]
[s1;:WithPopUp`:`:class:%- [@(0.0.255) class]_[* WithPopUp]_:_[@(0.0.255) public]_[*@3 T]&]
[s2; Add PopUp functionality to Ctrl(or its derivative) T. Unlike 
DropTree, DropList or DropGrid, the PopUp will not take away 
keyboard focus from the owner. This way, functionality such as 
auto completion can be implemented more naturally.&]
[s2; &]
[s2; T should be Ctrl or a class derived from Ctrl, typically EditString, 
LineEdit, RichEdit..&]
[s2;  &]
[s3; &]
[s1;:EditKeyValue`:`:class:%- [@(0.0.255)3 class][3 _][*3 EditKeyValue][3 _:_][@(0.0.255)3 publi
c][3 _][*@3;3 EditField][3 , ][@(0.0.255)3 public][3 _][*@3;3 Convert]&]
[s2; PopEasy doesn`'t work easily with stock [* EditField] derivatives. 
[* EditKeyValue] is designed to work with PopEasy in a typical 
situation to retrieve Key:Value pair.&]
[s2;  `'Key`' is the internal representation; `'Value`' is what`'s 
shown to &]
[s2;  the world.&]
[s2; &]
[s2;  eg, in a database application, `'key`' may be the primary key, 
&]
[s2;   while `'value`' is some combination of attributes that make 
sense&]
[s2;  to users&]
[s2; &]
[s3;%- &]
[s0; ]]