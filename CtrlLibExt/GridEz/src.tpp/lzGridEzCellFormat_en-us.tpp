topic "CellFormat";
[i448;a25;kKO9; $$1,0#37138531426314131252341829483380:structitem]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:codeitem]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s1;:lz`:`:CellFormat: [* CellFormat]&]
[s2;%% the information about how to draw a cell, like font, paper 
color, ink color, padding, border, alignment, etc.&]
[s3; &]
[s4; &]
[s5;:lz`:`:CellFormat`:`:CellFormat`(nullptr`_t`): [* constexpr CellFormat](nullptr`_t)
&]
[s0;l288;%% [2 constructs a Null ][*2 CellFormat.]&]
[s4; &]
[s5;:lz`:`:CellFormat`:`:Padding`(int`): CellFormat[@(0.0.255) `&] 
[* Padding]([@(0.0.255) int] [*@3 value])&]
[s2;%% Set padding`-left, padding`-right, padding`-top, padding`-bottom 
all to [%-*@3 value.] Negative value is treated as [*@3 Null]. Current 
implementation also limit [%-*@3 value] to 65534.&]
[s3; &]
[s4; &]
[s5;:lz`:`:CellFormat`:`:FaceName`(`)const: String [* FaceName]() [@(0.0.255) const]&]
[s2;%% returns the Face name of associated font. [*@3 Null ]if there 
is none..&]
[s3; &]
[s4; &]
[s5;:lz`:`:CellFormat`:`:FaceName`(String`): CellFormat[@(0.0.255) `&] 
[* FaceName](String [*@3 face])&]
[s2;%% Set the face name of the Font to [%-*@3 face] If [%-*@3 face] 
is [*@3 Null], or it doesn`'t represent a valid/know font face 
name, face name will be set to [*@3 Null].&]
[s3; &]
[s4; &]
[s5;:lz`:`:CellFormat`:`:FontHeight`(`)const: [@(0.0.255) int] [* FontHeight]() 
[@(0.0.255) const]&]
[s2;%% return the associated font height, possibly [*@3 Null.]&]
[s3; &]
[s4; &]
[s5;:lz`:`:CellFormat`:`:FontWidth`(`)const: [@(0.0.255) int] [* FontWidth]() 
[@(0.0.255) const]&]
[s2;%% return the width of the associated font possibly [*@3 Null.]&]
[s2;%%  .&]
[s3; &]
[s4; &]
[s5;:lz`:`:CellFormat`:`:AllNotNull`(`)const: [@(0.0.255) bool] [* AllNotNull]() 
[@(0.0.255) const]&]
[s2;%% return [*@(128.0.255) true] only if no format properties are 
[*@3 Null] .Only [* CellFormat]s that are [* AllNotNull]() can be used 
to do actual drawings.&]
[s3; &]
[s0;%% ]]