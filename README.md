# Introduction

Most virtual software synthesizers use the *knob* GUI concept, which is imported from physical synthesizers. On a physical synthesizer you can turn a knob with your hands, and have it change a parameter for the sound. In a virtual softsynth, the knob can be manipulated with the mouse. Musicians like knobs.

There is a technical challenge for the synth GUI programmer here. In most cases it looks terrible if you just represent the knob with an image that rotates, because that doesn't take light shining on the knob into account. You can fix this by doing a realtime 3D rendering of the knob, but that's a little overkill.

Most GUI programmers use a set of prerendered images of the knob turned in different positions. For instance, a strip of 128 such images is usually enough discrete steps that you'll not notice that it's not a continous rotation.

It would be tedious to draw all 128 images by hand, so people turn to tools to help. knobster is basically such a tool.

# Related work

Other tools exist to solve this problem.

* Knobman: http://www.g200kg.com/en/software/knobman.html
  Knobman is a GUI based tool. Because it has a GUI, there are limitations to its flexibility.

# knobster
knobster uses *fragment shaders* to generate images on the GPU. This might sound like a weird way to do it, but it has some advantages. 

1. A large community of people who are really good at generating graphics with fragment shaders exist.
2. It's very flexible.
3. You can change the generation code on the fly, without compiling or restarting the program.

# Using knobster

To use knobster you need to do two things:

1. Create or edit a generator file (a fragment shader in disguise)
2. Call knobster with the right command-line arguments

## The generator

The generator is a fragment shader file that contains two functions:

```
vec4 generate(vec2 pos, float v)
```

This function takes two arguments, the point we're rendering (*pos*) and the position of the the knob (*v*).

* `pos` is a `vec2` containing the *normalized* rendering point, that is, from 0-1
* `v` is the normalized knob position. If you specify that you want to generate a strip with 128 knob images, for image 0 `v` will be 0.0, and for image 127, `v` will be 1.0.

It is important that the color returned from `generate` is in linear space, that is, not gamma-corrected.

```
vec4 gamma(vec4 col)
```

This function must return a gamma corrected version of `col`. If you do not wish to do gamma correction, you can just return col directly in this function.

The reason we have a separate function for `gamma` correction, is that the oversampling *must* be done in linear space. And if we returned a gamma corrected color from `generate`, that wouldn't be possible.

## Command-line arguments

knobster has two modes, *display* and *generate*. In *display* mode, knobster opens a window in which the knob is drawn. The window is updated when the generator file is changed. In *generate* mode, knobster opens a window and draws the downsampled knobs as fast as possible, copies them to a png file, and exits.

### Mode selection

* `-d`, `--display`  
  select display mode
* `-g`, `--generate`  
  select generate mode

### Input file

* `-k <path>`, `--knobfile <path>`  
  path to generator file

### Image size
* `-w <arg>`, `--width <arg>`  
  image width in pixels
* `-h <arg>`, `--height <arg>`  
  image height in pixels
* `-o <arg>`, `--oversampling <arg>`
  oversampling factor

### Output options (only neccessary for generate mode)
* `-n <arg>`, `--num <arg>`
  number of knob images in strip
* `-f <arg>`, `--file <arg>`
  path to ouput PNG file

# Build from source
knobster uses premake to generate Visual Studio solution and project files. You can read more about premake here: http://industriousone.com/premake-quick-start

1. Clone this repository
2. Clone the '64klibs' repository into the same parent folder, so that the two repositories are side by side
3. Make sure that you have *premake5.exe* in your *$PATH*, or put it directly into the root of the knobster folder, along the *premake5.lua* file. You can find premake5.exe in *64klibs/premake.*
4.  Open a command window
5.  `cd` into the knobster folder
6.  run `premake5.exe <toolset>` where *toolset* is the toolset you want to generate build files for, e.g. *vs2012*. You can see options on the premake quick start page, linked above.
7.  Open up the solution file now generated in *build/knobster.sln*
8.  Proceed as normal
