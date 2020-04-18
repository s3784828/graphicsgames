Assignment by Tristan Macaulay s3784828
ISSUES:
The defence system, which to me, looks like a net and thus is a net in the program.
However its collision detection is not properly implemented as I was unable to work why it wasnt working,
you can see my halfway sorta kinda works implementation in getDistanceNet() where my intional and desired implementation has been commented out.

I did implement the ability to shoot multiple projectiles at once.

BIG PROBLEM:
This happens to me with wsl, it could happen for you as well when not using wsl.
At first I would have to re-execute a program for the boat to rotate correctly, now I have to re-execute the program for the projectiles to fire.
For example, I compile the program and then I execute and fire and the program freezes. I close the program in the terminal and then I re-execute and it works.
Sometimes I have to do this once, sometimes even three times. I had this issue with the rotations which I brought up in tutes to Geoff and Jesse. 
But now rotations are no longer an issue, so if freezes try to re-execute. 
After some testing it seems the problem occurs only if you fire from boat1 first or boat2 first, so if you execute the program and only press the fire button for 
boat 1 or boat 2 the program
will freeze (strangely doesnt occur when you fire island cannon), if you press anything else the program will procceed to work as normal. 
I dont know if this is a code issue or a wsl compilation problem.


