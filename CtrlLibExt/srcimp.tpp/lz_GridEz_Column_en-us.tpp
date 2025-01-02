topic "";
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[ $$0,0#00000000000000000000000000000000:Default]
[{_}%EN-US 
[s1;%- &]
[s2;:lz`:`:GridEz`:`:Column`:`:canresize:%- [@(0.0.255) bool] [* canresize] 
[@(0.0.255) :] [@3 1]&]
[s3; indicates if the column allows resizing through UI. Note this 
flag works together with GridEz`'s [* colcanresize] to control 
is a column are actually allowed to be resized. .&]
[s4;%- &]
[s0; [* On user changing a column`'s width with mouse click`-dragging 
in ][*/ FitColumns][*  mode.]&]
[s0;* &]
[s0; [2 A column ][*2 c][2  has a ][C@3;2 naturalwidth][2  member variable 
which is its desired width should there be no scaling. And it 
also has a ][C@3;2 scaleweight][2  member variable which determines 
how responsive it`'s to size change. Suppose a ][*2 GridEz g][2  
has a total client area width of ][@(128.0.255)2 grid`_cx][2 ,][@8;2  
][2 while the total of the ][C@3;2 naturalwidth][2 s of all its non`-hidden 
columns is ][@(128.0.255)2 total`_width][2 , and the total of the 
][C@3;2 scaleweight][2 s of all its non`-hidden columns is ][@(128.0.255)2 total`_weigh
t][2 , then a non`-hidden column with ][C@3;2 naturalwidth][2  ][@(128.0.255)2 cx][2  
and ][C@3;2 scaleweight][2  ][@(128.0.255)2 w][2  has actual width, as 
display on the screen, of ]&]
[s0;= [2  ][@(0.128.128)2 cx `+ w/total`_weight`*(grid`_cx`-total`_width)]&]
[s0;2 &]
[s0; [2 The increase in a column`'s width is gained by proportionally 
reducing width of each column following it, and vice versa. All 
][C@3;2 scaleweight][2 s are unaffected by the action.]]]