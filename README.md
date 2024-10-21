# ForstFire-Simulation

This tool enables the simulation of the spread of one or more forest fires under various parameters such as

* Tree species (oak, spruce, beech, birch,...) with corresponding:
* * Radius
  * Burning speed
* Wind direction
* Wind speed
* Number of fire sources
* Size of the grid / "forest"

In addition to the highly performant `C++` implementation using several parallelisation calculations, the results can also be visualised directly via `Python`, as in the following examples:

![Example1](./image/README/ex1.mp4 "example1")


```
<video src="./image/README/ex1.mp4" width="320" height="240" controls></video>
```


```html
<video width="320" height="240" controls>
  <source src="./image/README/ex1.mp4" type="video/mp4">
</video>
```


```
![](/image/README/ex1.mp4)
```


## Installation

Install it by building it yourself.

If all dependencies are installed on the system, just run:

```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Run simulation

- Set up the parameters important for the simulation, such as wind direction, in the `model.txt` file
- Starting the simulation with the shell `run.sh` execute the following:

  ```shell
  cd build
  CC=icc CXX=icpc cmake -DCMAKE_BUILD_TYPE=RELEASE ..
  make

  rm fire_*.txt tree_*.txt temp_*.txt stats.txt
  ./fire
  ```

## Generating the visualization

The visualisation is started automatically within `run.sh`.

If this is to be carried out manually, this can be done with `show.sh` or:

```shell
cd py
python3 draw_temperature.py
cd ..

```
