CW2- Text Adventure Game:
For compilation:

You can either use makefile, and run make in command prompt.
Or

Simply run, > g++ TextBasedAdventure.cpp -o TextBasedAdventure
After compilation either way, execute program by:
./TextBasedAdventure map1.json

Note: you can replace map1.json with any map file in json format like map2.json, map3.json provided in assignment which I also included in zip file.


For four map functions these are some possible commands for win and loss:

Note: These are some sample commands to execute the game. Many other patterns of commands can be used in this game.


map1.json
Win:
take gun, go east, kill zombie
Lose:
go east, kill zombie (player dies as he has no gun)
take gun, go east, go east (player dies as he leaves room without killing zombie with more aggressiveness)

map2.json
Win:
take gun, go east, take bullet, go north, take silver bullet, kill zombie, go north, kill werewolf
Lose:
1.	take gun, go east, go north, take silver bullet, kill zombie 
(you missed to take normal bullet to kill zombie from room 2, so you lose)
2.	take gun, go east, take bullet, go north, take silver bullet, go north
(you tried to leave room without killing enemy so you lose)

map3.json
Win:
go up, go right, take blue gem, go left, take gun, go back,go back, go left, take red gem, kill big guy,go back, go front, take green gem
Objective: To collect all objects needed
Lose:
go up, go left, go back (leave room without killing enemy)
go up, go left, kill big guy (You have no required objects to kill the enemy)

map4.json
Win:
go up, go left, take gun, go back, go right, kill big guy, go left
Objective : To reach a certain room, here its room 4)

Lose:
go up, go right, go left (Lost because you tried to leave room without killing enemy)
go up, go right, kill big guy (you don’t have necessary object to kill enemy, so you lost)



