topic "";
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:orig`_position: [@(0.0.255) int] 
[* orig`_position]&]
[s3;%% used in column or row size change with click`-and`-drag. When 
mouse left is down, and potentially a row/column size changing 
is initiating, the mouse location is remembered in this variable, 
and will be used in each subsequent MouseMove while left button 
is holding.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:orig`_dimension: [@(0.0.255) int] 
[* orig`_dimension]&]
[s3;%% the [* naturalwidth] of the column or height of the row to be 
resized is remembered when mouse left is down and potentially 
a row/column size changing is initiating. &]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:is`_row: [@(0.0.255) bool] [* is`_row]&]
[s3;%% used in column or row size changing with mouse click`-and`-dragging 
to indicate a row resizing or column resizing is going on or 
about to going on. .&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:orig`_dimensions: Vector<[@(0.0.255) short]> 
[* orig`_dimensions]&]
[s3;%% To remember the orginal dimensions of the columns (rows) following 
the resizing one. This is used only when [* FitColumns]() /[* FitRow]s() 
is true. .&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:scrollx`_changed: [@(0.0.255) bool] 
[* scrollx`_changed] [@(0.0.255) :] [@3 1]&]
[s3;%% cached`_data needs to be updated because of horizontal scrolling 
.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:scrolly`_changed: [@(0.0.255) bool] 
[* scrolly`_changed] [@(0.0.255) :] [@3 1]&]
[s3;%% cached`_data needs to be updated because of vertical scrolling. 
.&]
[s4; &]
[s1; &]
[s2;:lz`:`:GridEz`:`:CachedData`:`:x`_dimension`_recalculate: [@(0.0.255) bool] 
[* x`_dimension`_recalculate] [@(0.0.255) :] [@3 1]&]
[s3;%% width of one or more columns changed as a result rellocation 
in x`-dimension is needed. .&]
[s3;%%  .&]
[s1; ]]