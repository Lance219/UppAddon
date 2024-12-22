topic "";
[ $$0,0#00000000000000000000000000000000:Default]
[H6;0 $$1,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$2,0#37138531426314131252341829483370:codeitem]
[l288;2 $$3,0#27521748481378242620020725143825:desc]
[0 $$4,0#96390100711032703541132217272105:end]
[{_} 
[s1; &]
[s2;:Connection`:`:SendQuery`(const char`*`): [@(0.0.255) bool]_[* SendQuery]([@(0.0.255) c
onst]_[@(0.0.255) char]_`*_[*@3 command])&]
[s3;%%  [%-*@3 command] the SQL command to be asynchronously processed.&]
[s0;l192;b42;a42; [R  -|returns ][*R true][R  if the command was successfully 
dispatched and ][*R false][R  if not (in which case, use ErrorMessage 
to get more information about the failure.]&]
[s0;~~~0~400~799~1199~1598~1998~2398~2797~3197~3596;2 &]
[s0;l192;i96; [R After successfully calling SendQuery, call GetResult 
one or more times to obtain the results. SendQuery cannot be 
called again (on the same connection) until GetResult has returned 
a null pointer, indicating that the command is done.]&]
[s4;%% &]
[s1; &]
[s2;:Connection`:`:GetResult`(`): [_^Result^ Result]_[* GetResult]()&]
[s0;l192;b42;a42; [R Waits for the next result from a prior SendQuery 
call, and returns it. An Empty Result is returned when the command 
is complete and there will be no more results.]&]
[s3;l192; [R3 GetResult must be called repeatedly until it returns 
an Empty Result, indicating that the command is done. (If called 
when no command is active, PQgetResult will just return an Empty 
Result at once.) Each non`-null result from PQgetResult should 
be processed using the same Result accessor functions. Note that 
GetResult will block only if a command is active and the necessary 
response data has not yet been read by ConsumeInput]&]
[s4;l192;3 &]
[s0;l192;b42;a42; [R Using SendQuery and GetResult solves one of Exec`'s 
problems: If a command string contains multiple SQL commands, 
the results of those commands can be obtained individually. (This 
allows a simple form of overlapped processing, by the way: the 
client can be handling the results of one command while the server 
is still working on later queries in the same command string.)]&]
[s0;l192; [R By itself, calling GetResult will still cause the client 
to block until the server completes the next SQL command. This 
can be avoided by proper use of two more functions: ConsumeInput() 
and IsBusy();]&]
[s1; &]
[s2;:Connection`:`:ConsumeInput`(`): [@(0.0.255) bool]_[* ConsumeInput]()&]
[s0;l192;b42;a42; [R If input is available from the server, consume 
it.]&]
[s0;l192;b42;a42; [R ConsumeInput normally returns ][*R true][R  indicating 
`"no error`", but returns ][*R false][R  if there was some kind of 
trouble (in which case ErrorMessage can be consulted). Note that 
the result does not say whether any input data was actually collected. 
After calling ConsumeInput, the application can check IsBusy 
to see if their state has changed.]&]
[s3;l192; [R3 ConsumeInput can be called even if the application is 
not prepared to deal with a result or notification just yet. 
The function will read available data and save it in a buffer, 
thereby causing a select() read`-ready indication to go away. 
The application can thus use ConsumeInput to clear the select() 
condition immediately, and then examine the results at leisure.]&]
[s4;l192; &]
[s0;l192;%% ]]