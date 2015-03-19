# Amazing Desktops

This is a utility (*cough* toy *cough*) for generating random images
suitable for desktop backgrounds. These images are created by generating
large random mazes and coloring them using Dijkstra's algorithm.

(Get it? A-MAZE-ing? I so kill me.)

Here are a few sample images to get you hungry for more...

![Brush fire](samples/maze-01.png?raw=true "Brush fire")

![Under the sea](samples/maze-02.png?raw=true "Under the sea")

![Thunderclouds](samples/maze-03.png?raw=true "Thunderclouds")

## Installation

The only dependency is libpng, which most systems probably have
preinstalled. If yours complains when you try to build this, then yeah,
you'll want to take care of that.

Assuming your dev environment is set up, the following ought to work:

    $ make

Yup, just that. This will build a `maze` executable in the current
directory.


## Usage

Running without any arguments will generate a 640x480 image, suitable
for previewing. It will use a random color scheme:

    $ ./maze

You can customize the output by passing any of the following arguments:

* `maze s12345678` -- uses the given number to seed the random number
  generator. This lets you (for example) share a result without having
  to send a large image.
* `maze w500` -- sets the width of the image to 500 pixels
* `maze h500` -- sets the height of the image to 500 pixels
* `maze c112233ff` -- adds the given color to the color scheme (the color
  is a 32-byte RGBA value in hexadecimal format (rrggbbaa). You may
  specify up to 100 such entries (though images look best with 5-10).
* `maze p112233ff` -- draws the longest path through the maze in the
  given RGBA color.
* `maze ga` -- use the built-in "autumn" color scheme
* `maze g10` -- generate a random color scheme consisting of 10 colors
* `maze agl` -- generate a maze using the "growing tree" algorithm,
  specifically it's "choose last" variant. Available options are:
  * `agl` --- "choose last" growing tree
  * `agw` --- weighted growing tree
  * `ab` --- binary tree algorithm
  * `aa` --- Aldous-Broder algorithm

For example:

    $ ./maze w1366 h768 g10

The above generates a 1366x768 maze and a color scheme (consisting of
ten colors). The result is then saved to `maze.png`.


## Related (Shameless Plugs)

If you like this, you might also enjoy my book, "Mazes for Programmers":

https://pragprog.com/book/jbmaze/mazes-for-programmers

Information about twelve different maze generation algorithms, lots of
maze-related projects, as well as a demonstration of how to use
Dijkstra's algorithm to achieve the kinds of color effects that
this project uses.


## Authors

Jamis Buck (jamis@jamisbuck.org)


## License

Creative Commons Attribution 4.0 International License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
