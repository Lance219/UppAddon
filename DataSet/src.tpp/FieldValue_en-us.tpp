topic "";
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s1; &]
[s2;:FieldValue`:`:DoSet`(const void`*`,FieldTypeId`): [@(0.0.255) void]_[* DoSet]([@(0.0.255) c
onst]_[@(0.0.255) void]_`*_[*@3 data], FieldTypeId_[*@3 inTid])&]
[s3;%%  [%-*@3 data]  points to source data;&]
[s3;%% [%-*@3 inTid] .Field Type of data pointed by data&]
[s4;%% &]
[s1; &]
[s2;:FieldValue`:`:location`(`)const: [@(0.0.255) const]_[@(0.0.255) char]_`*_[* location](
)[@(0.0.255) const]&]
[s3;%% The memory address of actual data storage for date type other 
than FT`_BIT and IsNull flags.&]
[s4; &]
[s1; &]
[s2;:RecordSet`:`:Append`(`): [@(0.0.255) void]_[* Append]()&]
[s3; -|Alias for AppendRecord().&]
[s0; &]
[s3;%% Append a blank record. The cursor will be positioned to the 
newly appended record for immediately modifications on it.&]
[s4; &]
[s1; &]
[s2;:RecordSet`:`:AppendRecord`(`): [@(0.0.255) void]_[* AppendRecord]()&]
[s3;%% Append a blank record. The cursor will be positioned to the 
newly appended record for immediately modifications on it.&]
[s3;%% &]
[s4; &]
[s1; &]
[s2;:RecordSet`:`:Erase`(int`): [_^RecordSet^ RecordSet][@(0.0.255) `&]_[* Erase]([@(0.0.255) i
nt]_[*@3 i])&]
[s3;%%  [%-*@3 i] index of the Record to be Erase`'d.&]
[s4;%% &]
[s1; &]
[s2;:RecordSet`:`:Erase`(const Vector`<int`>`&`): [_^RecordSet^ RecordSet][@(0.0.255) `&]_
[* Erase]([@(0.0.255) const]_[_^Vector^ Vector]<[@(0.0.255) int]>`&_[*@3 v])&]
[s3;%%  [%-*@3 v] .should be in ascending order and no duplicate indices 
and no invalid indeces&]
[s4;%% &]
[s0;%% ]]