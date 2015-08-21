import pygame

joystick.init()

stick_count = pygame.joystick.get_count()
print "The number of joystick is: ", stick_count

while(1):
	for i in range(stick_count):
		joystick = pygame.joystick.joystick(i)
		joystick.init()
		name = joystick.get_name()
		print "Joystick No.", i, "name is: ", name

		axes = joystick.get_numaxes()
		print "Axes are: ", axes

		for j in range(axes):
			axis = joystick.get_axis(j)
			print "axis values are: %f, %f." %(axis[0], axis[2])



