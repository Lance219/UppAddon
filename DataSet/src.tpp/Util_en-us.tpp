topic "";
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
[s4;%- &]
[s5;:ParseBool`(const char`*`,bool`&`):%- [@(0.0.255) bool]_[* ParseBool]([@(0.0.255) const
]_[@(0.0.255) char]_`*_[*@3 input], [@(0.0.255) bool`&]_[*@3 output])&]
[s2;  [%-*@3 input]  can be Yes/No/Y/N/True/False/T/F/0/1, case insensitive&]
[s2; [%-*@3 output] .to store the parsed boolean value&]
[s2; Returns true on sucessful parsing; false otherwise.&]
[s3; &]
[s4;%- &]
[s5;:TypeAlign`(FieldTypeId`):%- [@(0.0.255) int]_[* TypeAlign](FieldTypeId_[*@3 fid])&]
[s2;  [%-*@3 fid] .a field type id&]
[s2; Returns the alignment of corresponding type, eg 1 for byte alignment, 
2 for word alignment, etc&]
[s3; &]
[s4;%- &]
[s5;:TypeSize`(FieldTypeId`):%- [@(0.0.255) int]_[* TypeSize](FieldTypeId_[*@3 fid])&]
[s2;  [%-*@3 fid] .a field type id&]
[s2; Returns the size of the corresponding type, eg 1 for FT`_INT1, 
4 for FT`_INT4&]
[s3; &]
[s4;%- &]
[s5;:TypeName`(FieldTypeId`):%- [@(0.0.255) const]_[@(0.0.255) char`*]_[* TypeName](FieldTy
peId_[*@3 fid])&]
[s2;  [%-*@3 fid] a field type id.&]
[s3; [3 -|Return the name of the type associated with fid. eg. int2 
for FT`_INT2]&]
[s0; ]]