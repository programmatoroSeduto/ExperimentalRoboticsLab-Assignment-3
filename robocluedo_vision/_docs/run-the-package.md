
# HOW TO run the package

```{attention}
before running the package, make sure to have a robotic environment running. 
```

This package can be launched using the `run.launch` package, providing these ways to launch the project:

## one component

```bash
# run the detection only
roslaunch robocluedo_vision run.launch detection:=true decoding:=false

# run the decode only
roslaunch robocluedo_vision run.launch detection:=false decoding:=false

# run everything
roslaunch robocluedo_vision run.launch detection:=true decoding:=true
# or, simpler,
roslaunch robocluedo_vision run.launch 
```
