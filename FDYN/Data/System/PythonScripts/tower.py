#can be executed from FlowerQT python console using 'execfile("./PythonScripts/tower.py")'
#define scaling
scaling = 0.1
#define dimenson
x_d = 15*scaling
y_d = 3*scaling
z_d = 1*scaling


def kapla_tower(floors=1):
	count = 1
	for floor in range(floors):
		y = 1.625 + floor * 4.0
		if floor % 2 == 0:
			for x_val in range(1,6):
				for y_val in range(1,6):
					obj = Scene.createObject("SPH rigid cuboid", "Kapla_" + str(count))
					obj.setScale(vec3f(x_d, y_d, z_d))
					x = -3 + x_val * 2.0
					z = -3 + y_val * 2.0
					obj.setPosition(vec3f(x, y, z))
					obj.setRigidType(0)
					count += 1

			for x_val in range(1,4):
				for y_val in range(1,6):
					obj = Scene.createObject("SPH rigid cuboid", "Kapla_" + str(count))
					obj.setScale(vec3f(3, 1, 1))
					x = -3 + x_val * 3.0
					z = -3 + y_val * 2.0
					obj.setPosition(vec3f(x, y + 2, z))
					obj.setRigidType(0)
					count += 1
		else:
			for x_val in range(1,6):
				for y_val in range(1,6):
					obj = Scene.createObject("SPH rigid cuboid", "Kapla_" + str(count))
					obj.setScale(vec3f(1, 3, 1))
					x = -3 + x_val * 2.0
					z = -3 + y_val * 2.0
					obj.setPosition(vec3f(x, y, z))
					obj.setRigidType(0)
					count += 1

			for x_val in range(1,6):
				for y_val in range(1,4):
					obj = Scene.createObject("SPH rigid cuboid", "Kapla_" + str(count))
					obj.setScale(vec3f(1, 1, 3))
					x = -3 + x_val * 2.0
					z = -3 + y_val * 3.0
					obj.setPosition(vec3f(x, y + 2, z))
					obj.setRigidType(0)
					count += 1






