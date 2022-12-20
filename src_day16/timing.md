TIME12: 197 millis
Max-Value afterwards: 1707
     game_step_calls: 86460
TIME12: 16 millis
Max-Value afterwards: 1707
     game_step_calls: 29149

100 - 130

GCC 11.3.0, performance gov, static scheduler, procbind spread

omp_set_max_active_levels(1); omp_set_dynamic(1);
- Game_step_calls after 10 seconds:  11677733
- Game_step_calls after 20 seconds:  23176615
- Game_step_calls after 40 seconds:  42952381
- Game_step_calls after 60 seconds:  61482758
- Game_step_calls after 100 seconds: 98462961

omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16310999
- Game_step_calls after 20 seconds:  32466389
- Game_step_calls after 40 seconds:  64940234
- Game_step_calls after 60 seconds:  96828892
- Game_step_calls after 100 seconds: 160048251
- Game_step_calls after 120 seconds: 191420598

omp_set_max_active_levels(2); omp_set_dynamic(1);
- Game_step_calls after 10 seconds:  16884151
- Game_step_calls after 20 seconds:  34417439
- Game_step_calls after 40 seconds:  68865992
- Game_step_calls after 60 seconds:  103163494
- Game_step_calls after 100 seconds: 171124182
- Game_step_calls after 120 seconds: 205360878
-> sometimes quite a bit slower, prob due to CPU freq
     - e.g. 8060871 after 60

omp_set_max_active_levels(2); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  14217209
- Game_step_calls after 20 seconds:  28974512
- Game_step_calls after 40 seconds:  59817119
- Game_step_calls after 60 seconds:  91823325
- Game_step_calls after 100 seconds: 155297450
- Game_step_calls after 120 seconds: 187764723
-> more reliably fast than set_dynamic(1)

omp_set_max_active_levels(3); omp_set_dynamic(1);
- Game_step_calls after 10 seconds:  8420015
- Game_step_calls after 20 seconds:  17332797
- Game_step_calls after 40 seconds:  33203971
- Game_step_calls after 60 seconds:  -
- Game_step_calls after 100 seconds: -
- Game_step_calls after 120 seconds: -

omp_set_max_active_levels(3); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  7608393
- Game_step_calls after 20 seconds:  16118582
- Game_step_calls after 40 seconds:  31831524
- Game_step_calls after 60 seconds:  51967370
- Game_step_calls after 100 seconds: -
- Game_step_calls after 120 seconds: -

omp_set_max_active_levels(0); omp_set_dynamic(1);
- Game_step_calls after 10 seconds: 1336950
- Game_step_calls after 20 seconds: 2725165
- Game_step_calls after 40 seconds: 5456696

omp_set_max_active_levels(0); omp_set_dynamic(0);
- Game_step_calls after 10 seconds: 1342234
- Game_step_calls after 20 seconds: 2723626
- Game_step_calls after 40 seconds: 5440885

-> Fastest: omp_set_max_active_levels(1); omp_set_dynamic(0);


omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16310999
- Game_step_calls after 20 seconds:  32466389
- Game_step_calls after 40 seconds:  64940234
- Game_step_calls after 60 seconds:  96828892
- Game_step_calls after 100 seconds: 160048251
- Game_step_calls after 120 seconds: 191420598

Test with CLANG 14
omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  15499874
- Game_step_calls after 20 seconds:  30806811
- Game_step_calls after 40 seconds:  61326299
- Game_step_calls after 60 seconds:  91094917
- Game_step_calls after 100 seconds: 150441654
- Game_step_calls after 120 seconds: 179556134

Test with GCC 12.1.0
omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16311317
- Game_step_calls after 20 seconds:  32421038
- Game_step_calls after 40 seconds:  64400838
- Game_step_calls after 60 seconds:  95932574
- Game_step_calls after 100 seconds: 158223970
- Game_step_calls after 120 seconds: 188956092

omp_set_max_active_levels(2); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  13898042
- Game_step_calls after 20 seconds:  28671363

omp_set_max_active_levels(2); omp_set_dynamic(1);
- Game_step_calls after 10 seconds:  1326676
- Game_step_calls after 20 seconds:  2683623
- Game_step_calls after 40 seconds:  5423708

Test back with GCC 11.3.0
omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16302805
- Game_step_calls after 20 seconds:  32318485
- Game_step_calls after 40 seconds:  63895908
- Game_step_calls after 60 seconds:  95452437
- Game_step_calls after 100 seconds: 157964229
- Game_step_calls after 120 seconds: 188902808

Test with GCC 12.1.0 again
omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16213513
- Game_step_calls after 20 seconds:  32198032
- Game_step_calls after 40 seconds:  64161698
- Game_step_calls after 60 seconds:  95661695
- Game_step_calls after 100 seconds: 157867987
- Game_step_calls after 120 seconds: 188547293


========================================================0
template:
- Game_step_calls after 10 seconds:  
- Game_step_calls after 20 seconds:  
- Game_step_calls after 40 seconds:  
- Game_step_calls after 60 seconds:  
- Game_step_calls after 100 seconds: 
- Game_step_calls after 120 seconds: 

continue with 12.1.0
- added: set(CMAKE_CXX_COMPILER_TARGET x86_64-linux-gnu)

omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16271026
- Game_step_calls after 20 seconds:  32111421
- Game_step_calls after 40 seconds:  64325269
- Game_step_calls after 60 seconds:  95844176
- Game_step_calls after 100 seconds: 158218806
- Game_step_calls after 120 seconds: 189038553
-> within measurement error range

with set march native:
omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16171473
- Game_step_calls after 20 seconds:  32131930
- Game_step_calls after 40 seconds:  64191352
- Game_step_calls after 60 seconds:  95788120
- Game_step_calls after 100 seconds: 158328824
- Game_step_calls after 120 seconds: 189145409
-> within measurement error range

With some code opts:
omp_set_max_active_levels(1); omp_set_dynamic(0);
- Game_step_calls after 10 seconds:  16481668
- Game_step_calls after 20 seconds:  32742751
- Game_step_calls after 40 seconds:  64964356
- Game_step_calls after 60 seconds:  96919894
- Game_step_calls after 100 seconds: 160216367
- Game_step_calls after 120 seconds: 191292670

More code opts:
- Game_step_calls after 40 seconds:  65755620
- Game_step_calls after 60 seconds:  97736678
- Game_step_calls after 100 seconds: 160719626
- Game_step_calls after 120 seconds: 191585504

More:
- Game_step_calls after 10 seconds:  16423149
- Game_step_calls after 20 seconds:  32724023
- Game_step_calls after 40 seconds:  65307056
- Game_step_calls after 60 seconds:  -
- Game_step_calls after 100 seconds: -
- Game_step_calls after 120 seconds: 
More:
- Game_step_calls after 10 seconds:  16045475
- Game_step_calls after 20 seconds:  32224135
- Game_step_calls after 40 seconds:  64036506
- Game_step_calls after 60 seconds:  95633199
- Game_step_calls after 100 seconds: 157758058

More (combine_player_search_info):
- Game_step_calls after 10 seconds:  16331742
- Game_step_calls after 20 seconds:  32421823
- Game_step_calls after 40 seconds:  64471511
- Game_step_calls after 60 seconds:  96135068
- Game_step_calls after 100 seconds: 159526098

More (combine_player_search_info again):
- Game_step_calls after 10 seconds:  16636185
- Game_step_calls after 20 seconds:  32899304
- Game_step_calls after 40 seconds:  65716910
- Game_step_calls after 60 seconds:  98059278
- Game_step_calls after 100 seconds: 161299751

More - replaced attempted count with atomic bool
- Game_step_calls after 10 seconds:  16550047
- Game_step_calls after 20 seconds:  33007827
- Game_step_calls after 40 seconds:  65887133
- Game_step_calls after 60 seconds:  98115201
- Game_step_calls after 100 seconds: 161720531

proc-bind close for both-player-choose loop
- Game_step_calls after 10 seconds:  16409871
- Game_step_calls after 20 seconds:  32650938
- Game_step_calls after 40 seconds:  64991350
- Game_step_calls after 60 seconds:  96937386
- Game_step_calls after 100 seconds: 160244648
- Game_step_calls after 200 seconds: 302149860
-> revert to spread

schedule dynamic for both-player-choose loop
- Game_step_calls after 10 seconds:  17483914
- Game_step_calls after 20 seconds:  34984220
- Game_step_calls after 40 seconds:  70079955
- Game_step_calls after 60 seconds:  105495011
- Game_step_calls after 100 seconds: 175664047
-> kept

schedule dynamic for all omp loops
- Game_step_calls after 10 seconds:  17565048
- Game_step_calls after 20 seconds:  35071150
- Game_step_calls after 40 seconds:  70415499
- Game_step_calls after 60 seconds:  105886025
- Game_step_calls after 100 seconds: 176763037
-> kept

proc-bind close for all omp loops
- Game_step_calls after 10 seconds:  17749333
- Game_step_calls after 20 seconds:  35340702
- Game_step_calls after 40 seconds:  70970035
- Game_step_calls after 60 seconds:  106627997
- Game_step_calls after 100 seconds: 177750349
-> kept

optis in combine_player_search_info
- Game_step_calls after 10 seconds:  17678156
- Game_step_calls after 20 seconds:  35189779
- Game_step_calls after 40 seconds:  70508739
- Game_step_calls after 60 seconds:  105964948
- Game_step_calls after 100 seconds: -
-> prob down to measurement, kept.

more optis in combine_player_search_info
- Game_step_calls after 10 seconds:  17600535
- Game_step_calls after 20 seconds:  35094606
- Game_step_calls after 40 seconds:  70440370
- Game_step_calls after 60 seconds:  105677697
- Game_step_calls after 100 seconds: -
-> kept.

optis in game_step_v1
- Game_step_calls after 10 seconds:  19894592
- Game_step_calls after 20 seconds:  39619324
- Game_step_calls after 40 seconds:  79526169
- Game_step_calls after 60 seconds:  119464558
- Game_step_calls after 100 seconds: 198987578
- Game_step_calls after 120 seconds: 238392715
-> kept.

Control:
- 10:  19832208
- 20:  39750942
- 40:  79274545
- 60:  118843598
- 100: 196640045

build_search_info opti + move variant
- Game_step_calls after 10 seconds:  32313095
- Game_step_calls after 20 seconds:  65046654
- Game_step_calls after 40 seconds:  131208422
- Game_step_calls after 60 seconds:  197088686
- Game_step_calls after 100 seconds: 327868160
- Game_step_calls after 120 seconds: 391991293
- Game_step_calls after 200 seconds: 652549253
- Game_step_calls after 240 seconds: not needed
-> dramatic improvement

Game_step_calls after 233 seconds: 705478123
TIME2: 232533563 mus
TIME2: 232533 millis

TIME12: 233808 millis
Max-Value afterwards: 2548
     game_step_calls: 705478123
 
!!!!




Other stuff:
- Game_step_calls after 10 seconds:  34930958
- Game_step_calls after 20 seconds:  70433061
- Game_step_calls after 40 seconds:  141634656
- Game_step_calls after 60 seconds:  212404887
- Game_step_calls after 100 seconds: 353404095
- Game_step_calls after 120 seconds: 423871792

Optis:
- Game_step_calls after 10 seconds:  37098248
- Game_step_calls after 20 seconds:  73937827
- Game_step_calls after 40 seconds:  147485399
- Game_step_calls after 60 seconds:  220919628
- Game_step_calls after 100 seconds: 368490597
- Game_step_calls after 120 seconds: 442165027

TIME12: 201874 millis
Max-Value afterwards: 2548
     game_step_calls: 705478123

lucky run:
TIME12: 198527 millis
Max-Value afterwards: 2548
     game_step_calls: 705478123

sum up game as soon as both players idle:
TIME12: 183613 millis
Max-Value afterwards: 2548
     game_step_calls: 645168935
-> missed it in one method (ah, but here this never actually applies -> probably same result?) ->
TIME12: 185669 millis
Max-Value afterwards: 2548
     game_step_calls: 645168935
-> yes. Use this to advantage:...

- include sorting and pruning
TIME12: 182596 millis
Max-Value afterwards: 2548
     game_step_calls: 645168935

... don't consider valves with 0 flow for next steps. + prune aggressively 
TIME12: 12450 millis
Max-Value afterwards: 2548
     game_step_calls: 35959425

expand dropping valves with 0 flow:
TIME12: 6800 millis
Max-Value afterwards: 2548
     game_step_calls: 18135613

Safer pruning
TIME12: 11315 millis
Max-Value afterwards: 2548
     game_step_calls: 33599887

Optimize pairs
TIME12: 5253 millis
Max-Value afterwards: 2548
     game_step_calls: 11893876

Safer Pruning
TIME12: 9932 millis
Max-Value afterwards: 2548
     game_step_calls: 28496627