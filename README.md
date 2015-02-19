# Introduction

Most virtual software synthesizers use the *knob* GUI elemnt, which is imported from physical synthesizers. Musicians like knobs. On a physical synthesizer you can manipulate the knob with your hand, and have it change a parameter for the sound. In a virtual softsynth, the knob can be manipulated with the mouse. 

Knobs present a technical challenge for the synth GUI programmer. In most cases it looks *terrible* if you just put up an image of your knob, and rotate it when the user moves the mouse. Rotating the mouse does not take the lighting conditions into effect, and it's.. ugly.

There are multiple ways to fix this, but most ofen GUI programmers "cheat", and use a set of prerendered images of the knob turned in different positions. For instance, 128 images of the knob in different positions is usually enough discrete steps that you'll not notice that it's not a continous rotation.

It would be tedious to draw all 128 images by hand, so people turn to tools to help. Knobster is basically such a tool.

# Related work

Other tools exist to solve this problem.

* Knobman: http://www.g200kg.com/en/software/knobman.html
  Knobman is a GUI based tool. Because it has a GUI, there are limitations to its flexibility.

# Knobster
Knobster uses GLSL *fragment shaders* to generate images on the GPU. This might sound like a weird way to do it, but it has some advantages. 

1. A large community of people who are really good at generating graphics with fragment shaders exist.
2. It's very flexible.
3. You can change the generation code on the fly, without compiling or restarting the program.

# Learning how to write shader programs for Knobster

Here is a relevant [blog post](http://revivalizer.dk/blog/2015/02/19/generating-gui-graphics-with-math/).

# Using Knobster

To use Knobster you need to do two things:

1. Create or edit a generator file (a GLSL fragment shader)
2. Call knobster with the right command-line arguments

## The generator

The generator is a fragment shader file that contains two functions:

```
vec4 generate(vec2 pos, float v)
```

This function takes two arguments: the point we're rendering (*pos*) and the position of the the knob (*v*).

* `pos` is a `vec2` containing the *normalized* rendering point, that is, both x and y coordinates have been scaled to fit into the range 0.0 to 1.0.
* `v` is the normalized knob position. If you specify that you want to generate a strip with 128 knob images, for image 0 `v` will be 0.0, and for image 127, `v` will be 1.0.

The return color must be in linear space, that is, must not be gamma corrected.

```
vec4 gamma(vec4 col)
```

This function must return a gamma corrected version of `col`. If you do not wish to do gamma correction, you can just return `col` unmodified from this function.

The reason we have a separate function for gamma correction, is that the oversampling *must* be done in linear space for best results. And if we returned a gamma corrected color from `generate`, that wouldn't be possible.

## Command-line arguments

Knobster has two modes, *display* and *generate*:
* In *display* mode, Knobster opens a window in which the knob is drawn. The window is updated when the generator file is changed.
* * In *generate* mode, Knobster opens a window and draws the downsampled knobs as fast as possible, copies them to a png file, and exits.

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
Knobster uses premake to generate Visual Studio solution and project files. You can read more about premake here: http://industriousone.com/premake-quick-start

1. Clone this repository
2. Clone the '64klibs' repository into the same parent folder, so that the two repositories are side by side
3. Make sure that you have *premake5.exe* in your *$PATH*, or put it directly into the root of the knobster folder, along the *premake5.lua* file. You can find premake5.exe in *64klibs/premake.*
4.  Open a command window
5.  `cd` into the knobster folder
6.  run `premake5.exe <toolset>` where *toolset* is the toolset you want to generate build files for, e.g. *vs2012*. You can see options on the premake quick start page, linked above.
7.  Open up the solution file now generated in *build/knobster.sln*
8.  Proceed as normal
