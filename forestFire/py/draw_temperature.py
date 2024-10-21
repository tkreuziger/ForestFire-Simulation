
import matplotlib as mpl
from matplotlib import pyplot as plt
from matplotlib import colors
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.ticker import MultipleLocator
import matplotlib.animation as anim
import numpy as np
import os
import warnings


warnings.filterwarnings("ignore", category=mpl.cbook.mplDeprecation)


fig = plt.figure(1)
temp_files = [f for f in os.listdir('../build') if f[0:4] == 'temp']
tree_files = [f for f in os.listdir('../build') if f[0:4] == 'tree']
fire_files = [f for f in os.listdir('../build') if f[0:4] == 'fire' and f[-4:] == ".txt"]

stats = None
with open('../build/stats.txt', 'r') as f:
	stats = f.read().split('\n')

num_start_fires = 0
for i in range(len(stats)):
	if stats[i].startswith('Start fire'):
		num_start_fires += 1
	else:
		break

temp_img = None
tree_img = None
fire_img = None

temp_cm = mpl.colors.LinearSegmentedColormap.from_list('temp_colormap', ['blue', 'green', 'yellow', 'orange', 'red'], 1000)

with open('../treeInfos.txt', 'r') as f:
    content = f.readlines()

content = [x.strip() for x in content] 

color_list = []
tree_bounds = []
tree_labels = []
for line in content:
	color_list.append(line.split(' ')[4])
	tree_bounds.append(float(line.split(' ')[0]))
	tree_bounds.append(int(tree_bounds[len(tree_bounds) - 1]) + 0.5)
	tree_labels.append(line.split(' ')[1])

tree_cm = colors.ListedColormap(color_list)
tree_norm = colors.BoundaryNorm(tree_bounds, tree_cm.N)

fire_cm = colors.ListedColormap(['white', 'red', 'black'])
fire_bounds=[0, 0.5, 1, 1.5, 2]
fire_norm = colors.BoundaryNorm(fire_bounds, fire_cm.N)

frame = 1
frames = len(fire_files)
temp_sp = None

temp_tree_sp = None


for i in range(len(temp_files)):
	temp = np.genfromtxt('../build/temp_' + str(i) + ".txt", delimiter=' ')
	tree = np.genfromtxt('../build/tree_' + str(i) + ".txt", delimiter=' ')
	fire = np.genfromtxt('../build/fire_' + str(i) + ".txt", delimiter=' ')

	for x in range(fire.shape[0]):
		for y in range(fire.shape[1]):
			if fire[x, y] > 0:
				fire[x, y] = 1
			elif tree[x, y] == 126:
				fire[x, y] = 2

	
	if temp_img is None:
		temp_sp = plt.subplot(131)
		temp_img = plt.imshow(temp, interpolation='nearest', cmap=temp_cm, origin='lower')
		temp_sp.set_title('Temperature\nAvg. Temp.: {:0.2f}°\nMin. Temp.: {:0.2f}°\nMax. Temp.: {:0.2f}°'.format(temp.mean(), temp.min(), temp.max()))
		temp_img.set_clim([0, 600])	#temp.max()

		divider3 = make_axes_locatable(temp_sp)
		cax3 = divider3.append_axes("right", size="5%", pad=0.05)
		cbar3 = plt.colorbar(temp_img, cmap=temp_cm, cax=cax3, format="%.2f")
	else:
		temp_img.set_data(temp)
		temp_sp.set_title('Temperature\nAvg. Temp.: {:0.2f}°\nMin. Temp.: {:0.2f}°\nMax. Temp.: {:0.2f}°'.format(temp.mean(), temp.min(), temp.max()))
		
	#plt.text(-100, 100, "hello world")

	if tree_img is None:
		temp_tree_sp = plt.subplot(132)
		tree_img = plt.imshow(tree, interpolation='nearest', cmap=tree_cm, origin='lower', norm=tree_norm)
		temp_tree_sp.set_title('Trees')

		divider3 = make_axes_locatable(temp_tree_sp)
		cax3 = divider3.append_axes("right", size="5%", pad=0.05)
		cbar3 = plt.colorbar(tree_img, cmap=tree_cm, cax=cax3, format="%.2f")
		cbar3.ax.set_yticklabels(tree_labels)
	else:
		tree_img.set_data(tree)

	if fire_img is None:
		sp = plt.subplot(133)
		fire_img = plt.imshow(fire, interpolation='nearest', cmap=fire_cm, origin='lower', norm=fire_norm)
		sp.set_title('Fire\n{}'.format('\n'.join(stats[0:num_start_fires])))
	else:
		fire_img.set_data(fire)

	plt.pause(0.15)

	plt.suptitle('Frame #' + str(i + 1) + " / " + str(frames) + '\n' + stats[i + num_start_fires])
	frame += 1

	plt.draw()

plt.show()
