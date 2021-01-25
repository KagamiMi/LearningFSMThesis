@ECHO OFF
FOR /L %%x IN (1,1,8) DO (
	START /WAIT x64\Release\LearningFSM.exe experiment window_invisible config01.txt fitnessValues.txt
	ECHO %%x
)
PAUSE


