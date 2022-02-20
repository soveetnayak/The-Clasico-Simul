# OSN
## Assignment 5
## Question 2

## To compile
```
gcc q2.c -lpthread
```
## To execute
```
./a.out
```

## Structs implemented

* A struct for holding the goal scoring chances.

* A struct for all the groups that has a pointer to the struct array storing the information of all the members in the group.

## Threads

* Because goal scoring opportunities occur sequentially rather than simultaneously, there is a single thread analysing them all. 

* Every spectator has his own thread, which replicates seat assignment, spectator departure, and so forth.

## Goal thread

* Team converted the goal scoring chance to a goal
    * Thread sleeps until the next goal scoring chance.
    * A goal is scored if its probability is greater than a randomly generated float between 0 and 1. On the event of a goal, the thread broadcasts a signal to wake up all the waiting spectator threads to check on their respective conditions.

* Team failed to convert the goal scoring chance to a goal
    * No broadcasting of signal in this case.

## Spectator thread

* Spectator has reached the stadium
    * Because every spectator arrives at the stadium after this time, each thread will sleep for the period of time specified in the input.

* Spectator has got a seat in a zone
    * The program first determines whether a seat is available for the spectator based on his type. If no seats are currently available in valid zones, the conditional variable for cond_spectate causes it to wait until a Spectator exits the stadium before checking for seat availability again. 
    * If there is still no seat, it will wait until someone exits the stadium. However, because it is conditioned to check only after someone exits the stadium, busy waiting does not occur.

* Spectator couldn’t get a seat
    * After the above conditional variable gets the signal, everytime a while condition checks if the patience time of the spectator has been crossed or not. 
    * If the patience time has exceeded, then seat is not allocated.

* Spectator leaving for dinner
    * Conditional timed wait is used to simulate each spectator's exit. To avoid busy waiting, a while loop is factored inside of which this conditional timed wait is implemented. 
    * As a result, every spectator with a seat enters a loop. If the amount of goals scored by the other team exceeds the observer's enraged_value, the spectator departs the loop. The conditional wait is signalled and the event(Spectator leaving due to poor defensive performance) is checked every time a goal is scored by either team. Because this is a conditional timed wait, if the current time surpasses the spectator's spectating time (same for all spectators), the wait condition also exits, publishing the relevant event.

* Spectator is leaving due to the bad performance of his team
    * It is covered in the above point.