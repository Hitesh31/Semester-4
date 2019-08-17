seperate(L,LowSide,HighSide) :-
   divide(L,LeftDiv,RightDiv),              
   element(LeftDiv,LowSide),               
   element(RightDiv,HighSide),                 
   LowSide =:= HighSide.

element([X],X).                  
element(L,T) :-                    
   divide(L,LeftDiv,RightDiv),             
   element(LeftDiv,LowSide),                 
   element(RightDiv,HighSide),                
   operators(LowSide,HighSide,T).            


operators(LowSide,HighSide,LowSide+HighSide).
operators(LowSide,HighSide,LowSide-HighSide).
operators(LowSide,HighSide,LowSide*HighSide).
operators(LowSide,HighSide,LowSide/HighSide) :- HighSide =\= 0.   % dont divide by zero

loop_entry(F,0).
loop_entry(F,N) :-
  N > 0,
  N1 is N-1,
  read(F,L),
  solve(L) ,
  loop_entry(F,N1).
solve(L) :-
  seperate(L,LowSide,HighSide),
  writef('%w=%w\n',[LowSide,HighSide]).
divide(L,L1,L2) :- append(L1,L2,L), L1 = [_|_], L2 = [_|_].

main :- 
   open('input.txt',read,F),
   read(F,N),
   loop_entry(F,N).

