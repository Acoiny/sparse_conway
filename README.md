# Conways game of life
This implementation uses a sparse set to store the cells,
allowing for theoretically infinite size.

# Installation
To build this program SFML2 is needed.
Find Installation Instruction [here](https://www.sfml-dev.org/download/sfml/2.6.2/).

# Usage
```bash
usage: ./debug_sparse_conway <options>

options:
           -w <width>       sets the width of the window
           -h <height>      sets the height of the window
           -c               display controls

```

# Controls

```bash
Controls:
    Keyboard:
           wasd         move the camera around
           space        hold to run simulation as fast as possible
           c            press to advance by one simulation step

    Mouse:
           Left         hold to use current drawing tool
           Right        cycle through tools
           Wheel        zoom in/out
```
