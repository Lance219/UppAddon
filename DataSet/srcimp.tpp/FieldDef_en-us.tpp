topic "class FieldDef";
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:codeitem]
[ $$0,0#00000000000000000000000000000000:Default]
[{_}%EN-US 
[s1;:FieldDef`:`:class:%- [@(0.0.255) class]_[* FieldDef]&]
[s2; 2011`-04`-28 To allow a field to be either nullable or non`-nullable.&]
[s0; -|To rewrite the Layout logic. bits should be taken care after 
normal fields have been laid`-out so that the paddings can be 
made use of.&]
[s0; -|a FieldDef have a nullbit property, if it`'s `-1, the field 
is not nullable (its IsNull information is not maintained and 
IsNull() always returns false. Otherwise it`'s the bit where 
the IsNull information is stored.&]
[s0; &]
[s0; JOB TO DO:&]
[s0; allow char`[n`] to be stored, probably multimorphic with a String 
object!!&]
[s0; how about bit`[n`]?/byte`[n`]??&]
[s3;%- &]
[s4;%- &]
[s5;:RecordDef`:`:PhysicalLayout`(`):%- [_^RecordDef^ RecordDef][@(0.0.255) `&]_[* Physical
Layout]()&]
[s2; Use to layout the record so that fields are properly aligned 
and record size is minimized.&]
[s0; &]
[s2; -|Non`-bit fields are laid out first.&]
[s2; -|Logic: the fields with largest align should be laid out first.When 
calculating size, the aggregate size(TypeSize `* count) should 
be used. &]
[s3;%- &]
[s0; ]]