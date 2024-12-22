topic "";
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[ $$0,0#00000000000000000000000000000000:Default]
[{_}%EN-US 
[s1;%- &]
[s2;:Upp`:`:Cell`:`:MaskAsCtrlPointer`(`):%- [@(0.0.255) void]_[* MaskAsCtrlPointer]()&]
[s3; Cell`::CellData`* pCellData isn`'t necessarily a simple pointer 
to a Cell`::CellData object. Sometimes we want it to point to 
a const ASCIIZ string, sometimes it point to a Ctrl that need 
to be deleted when Removed, and yet sometimes it can point to 
a Ctrl that belongs somewhere else.&]
[s3; &]
[s3; This internal function is used to mark the pointer that points 
to a non`-owned Ctrl &]
[s4;%- &]
[s0; ]]