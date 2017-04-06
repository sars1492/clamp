# clamp

Command line frontend to mpg123 and ogg123. Play file in background.

## Install

1. Compile: 

	$ gcc -o clamp clamp.c

2. Install:

	$ cp clamp ~/bin/
	$ export PATH=$PATH:~/bin

3. Make directory to store pid files

	$ mkdir /tmp/clamp

## Synopsis

```
usage: clamp [-r] FILE
       clamp [-p] [-s] 

Plays audio FILE (in mp3 or ogg format) in background or pause/unpause,
stop playing actual file when ran without FILE argument.


optional arguments:
  -r                  play file in loop
  -p                  pause/unpause playing file
  -s                  terminate program
```


## Usage examples

1. Play the FILE

        $ clamp FILE

2. Pause acutal playing file

        $ clamp -p

3. Unpause acutal playing file

        $ clamp -p

4. Stop acutal playing file

        $ clamp -s

## License

clamp -- Plays mp3 and ogg audio files.

Copyright (C) 2003  Juraj Szasz (<juraj.szasz3@gmail.com>)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hopet hat it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
