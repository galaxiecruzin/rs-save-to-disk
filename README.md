# rs-save-to-disk

## Setup

1. Clone the librealsense repo, follow the directions to build and install.
2. Clone this repo in the same base dir.
3. from this project directory

```
mkdir build
cd build
cmake ../
make
``` 

## Run the executable with:

```
./rs-save-to-disk
```

## Edit the source file as needed, then to recompile:

```
make clean
make
```

## Feature / Save Multiple
The pseudo code for a first setup is:
Terminal Args:
 Positive or Negative Example? (p/n)
 Press Spacebar to Start
 Press Spacebar to Stop
Some Vars:
Custom Depth Stream Configs
Custom RGB Stream Configs
Saves to:
~/dataset/leave-house/depth/{00001}/001.png
~/dataset/leave-house/color/{00001}/001.png
Things to Note:
-Image processing can be intense. Need to make sure buffers are set higher than normal to cache files.
-30 FPS, so max 150 frames
-Depth cam will likely only run at 10 FPS. May need to run that capture separately? They don’t 100% need to be in sync for the training set creation, though it would be helpful.
Code:
What is the number of the last directory? If nothing, then set directory = 1. If 1, then 2, etc.
On Spacebar, start scripts to camera
Start loop - Max of 5 seconds ->
 If image exists, save image to disk
 If not, copy previously number image in sequence and duplicate it with new name and then save.
 Incremenet counter
After if user hits spacebar at any point, complete a final loop and terminate.

