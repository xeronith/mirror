topic "CoWork";
[2 $$0,0#00000000000000000000000000000000:Default]
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,0#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[{_} 
[ {{10000@(113.42.0) [s0;%% [*@7;4 CoWork]]}}&]
[s3; &]
[s1;:CoWork`:`:class: [@(0.0.255)3 class][3 _][*3 CoWork]&]
[s9;%% This class is indented as general parallelization tool. Whenever 
jobs (e.g. loop iterations) are independent (they do not share 
any data between iterations), CoWork can be used to relatively 
easily spawn loop iterations over threads and thus over CPU cores. 
Note that previous statement does [* not] preclude CoWork iterations 
to share data at all `- sharing data using Mutex or similar serialization 
mechanisms still works. CoWork works with fixed`-size thread 
pool, which is created during initialization phase (which first 
CoWork constructor is called). No more thread are created or 
destroyed during normal work. Nesting of CoWork instances is 
also possible. Thread pool is normally terminated when the main 
thread finishes.&]
[s9;%% No synchronization is required to access CoWork instances 
from various threads (CoWork is internally synchronized).&]
[s9;%% [*/ Implementation notes: ]Current implementation has single 
global FIFO stack for 2048 scheduled jobs. When there is no slot 
available when scheduling the job, it is performed immediately 
by Do. Finish method has to wait until all jobs scheduled by 
CoWork.instance are finished, while waiting it attempts to perform 
scheduled jobs from the same instance. That way work always progresses 
even if there is shortage of worker threads.&]
[s0;%% &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s3; &]
[s5;:Upp`:`:CoWork`:`:TrySchedule`(Upp`:`:Function`<void`(`)`>`&`&`): [@(0.0.255) stati
c bool]_[* TrySchedule]([_^Upp`:`:Function^ Function]<[@(0.0.255) void]_()>`&`&_[*@3 fn])
&]
[s5;:Upp`:`:CoWork`:`:TrySchedule`(const Upp`:`:Function`<void`(`)`>`&`): [@(0.0.255) s
tatic] [@(0.0.255) bool]_[* TrySchedule]([@(0.0.255) const]_[_^Upp`:`:Function^ Function]<
[@(0.0.255) void]_()>`&_[*@3 fn])&]
[s2;%% This is a low`-level function that attempts to schedule [%-*@3 fn] 
to be executed by worker thread. Returns true if [%-*@3 fn] was 
scheduled, false if not (in case there is no slot left in scheduling 
stacks). Not that this function only schedules the function, 
the exact time of execution is unknown.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:CoWork`:`:Schedule`(Upp`:`:Function`<void`(`)`>`&`&`): [@(0.0.255) static 
void]_[* Schedule]([_^Upp`:`:Function^ Function]<[@(0.0.255) void]_()>`&`&_[*@3 fn])&]
[s5;:Upp`:`:CoWork`:`:Schedule`(const Upp`:`:Function`<void`(`)`>`&`): [@(0.0.255) stat
ic] [@(0.0.255) void]_[* Schedule]([@(0.0.255) const]_[_^Upp`:`:Function^ Function]<[@(0.0.255) v
oid]_()>`&_[*@3 fn])&]
[s2;%% Similar to TrySchedule, but always schedules [%-*@3 fn] `- even 
if it has to wait for slot to become available.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:CoWork`:`:Do`(Upp`:`:Function`<void`(`)`>`&`&`): [@(0.0.255) void]_[* Do]([_^Upp`:`:Function^ F
unction]<[@(0.0.255) void]_()>`&`&_[*@3 fn])&]
[s5;:Upp`:`:CoWork`:`:Do`(const Upp`:`:Function`<void`(`)`>`&`): [@(0.0.255) void]_[* Do](
[@(0.0.255) const]_[_^Upp`:`:Function^ Function]<[@(0.0.255) void]_()>`&_[*@3 fn])&]
[s5;:Upp`:`:CoWork`:`:operator`&`(const Upp`:`:Function`<void`(`)`>`&`): [_^Upp`:`:CoWork^ C
oWork][@(0.0.255) `&]_[* operator`&]([@(0.0.255) const]_[_^Upp`:`:Function^ Function]<[@(0.0.255) v
oid]_()>`&_[*@3 fn])&]
[s5;:Upp`:`:CoWork`:`:operator`&`(Upp`:`:Function`<void`(`)`>`&`&`): [_^Upp`:`:CoWork^ C
oWork][@(0.0.255) `&]_[* operator`&]([_^Upp`:`:Function^ Function]<[@(0.0.255) void]_()>`&
`&_[*@3 fn])&]
[s2;%% Schedules [%-*@3 fn] to be executed. All changes to data done 
before Do are visible in the scheduled code. The order of execution 
or whether the code is execute in another or calling thread is 
not specified. In certain situations (no scheduling slot available), 
Do can perform scheduled job immediately in calling thread.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:CoWork`:`:FinLock`(`): [@(0.0.255) static] [@(0.0.255) void]_[* FinLock]()&]
[s2;%% This functions is to be called in scheduled routine. Its purpose 
is to serialize access to shared data at the end of the routine. 
The rationale is that CoWork has to lock some mutex anyway after 
scheduled code finishes, so FinLock can lock this mutex a bit 
earlier, joining two mutex locks into single one. Of course, 
as with all locks, execution of locked code should be short as 
not to cause congestion of CoWork scheduling.&]
[s3;%% &]
[s4; &]
[s5;:CoWork`:`:Finish`(`): [@(0.0.255) void]_[* Finish]()&]
[s2;%% Waits until all jobs scheduled using Do (or operator`&) are 
finished. All changes to data performed by scheduled threads 
are visible after Finish. While waiting, Finish can perform scheduled 
jobs.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:CoWork`:`:IsFinished`(`): [@(0.0.255) bool]_[* IsFinished]()&]
[s2;%% Checkes whether all jobs scheduled using Do (or operator`&) 
are finished. All changes to data performed by scheduled threads 
are visible after IsFinished returns true (so this is basically 
non`-blocking variant of Finish).&]
[s3;%% &]
[s4; &]
[s5;:CoWork`:`:`~CoWork`(`): [@(0.0.255) `~][* CoWork]()&]
[s2;%% Calls Finish().&]
[s3; &]
[s4; &]
[s5;:Upp`:`:CoWork`:`:IsWorker`(`): [@(0.0.255) static] [@(0.0.255) bool]_[* IsWorker]()&]
[s2;%% Returns true if current thread is CoWork worker thread.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:CoWork`:`:SetPoolSize`(int`): [@(0.0.255) static void]_[* SetPoolSize]([@(0.0.255) i
nt]_[*@3 n])&]
[s2;%% Adjusts the thread pool size (default pool size is CPU`_Cores() 
`+ 2).&]
[s0; ]]