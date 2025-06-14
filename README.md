# What's this?

YumLife is a mod for OHOL and AHAP based on [hetuw](https://github.com/hetuw/OneLife).
The goals of this mod are to stay up to date with the latest changes to the vanilla
OHOL and AHAP client, fix bugs, and occasionally add useful features.

# Installing (and updating)

## Windows or Linux

### Steam users

1. Make sure the game is fully updated in Steam.
2. Run the game from Steam once to ensure the Steam login details are properly set up.
3. Download the latest version of the mod from [the Releases page](https://github.com/selb/YumLife/releases). For Windows, this is YumLife_windows.exe.
4. Install the mod into the OHOL/AHAP installation folder (Steam users: right click game > Manage > Browse local files)
5. Run the mod from the OHOL/AHAP installation folder.

### Direct download users

1. Download the latest version of the mod from [the Releases page](https://github.com/selb/YumLife/releases). For Windows, this is YumLife_windows.exe.
2. Install the mod into the OHOL/AHAP installation folder (same folder as the vanilla `OneLife.exe`).
3. Run the mod from the OHOL/AHAP installation folder.

## macOS

### Installing OHOL itself

1. If you bought the game on Steam, use a Windows or Linux version of Steam to run the game once and record your account key from the login screen.
2. Download and extract the Linux version from: `http://onehouronelife.com/ticketServer/server.php?action=show_downloads&ticket_id=YOURKEYHERE`
3. When you run YumLife (see below), it will ask you where you installed OHOL. Choose the directory you extracted to.

### Install/updating YumLife

1. Download YumLife_macos.zip from [the Releases page](https://github.com/selb/YumLife/releases).
2. Extract the YumLife_macos app wherever, preferably to Applications. Overwrite any existing version.

# Usage

Press `H` in-game to see everything the mod can do. A `yumlife.cfg` file is
generated in the OHOL/AHAP install folder and can be tweaked to your liking.

# Troubleshooting

## Make sure the base game is updated

If using Steam, launch Steam and make sure there isn't a pending update on the
base game.

Non-Steam installs will generally update themselves properly, but older installs
may have subtly corrupt data files due to the way YumLife/hetuw updating used to
work. See Reinstall below.

## Clear cache files

Search the OHOL/AHAP installation folder (Steam users: right click game > Manage >
Browse local files) for any `.fcz` files and delete all of them. They will be
regenerated automatically the next time you launch the game.

## Reinstall

Especially with non-Steam installs, there are a variety of issues past and present
that can subtly corrupt data files and leave your game directory in a state that isn't
salvageable. Uninstalling, reinstalling, and then following the
"Installation" section again carefully is the closest thing available to a 100%
certain fix.

The non-Steam version can be re-downloaded from: `http://onehouronelife.com/ticketServer/server.php?action=show_downloads&ticket_id=YOURKEYHERE`

## Still not working?

Open a bug report using the Issues tab above.

# Compiling

Compiling on Linux is recommended for release builds, but a native build on Windows is possible for development use.

## Linux

### Debian/Ubuntu Dependencies

For a Linux build only:

```
sudo apt install g++ make cmake libsdl1.2-dev libglu-dev libgl-dev
```

For a Windows cross build:

```
sudo apt install g++-mingw-w64-i686-win32
```

### Building for Linux

Make and switch to a build directory:

```
mkdir build
cd build
```

Configure and build:

```
cmake .. && make -j8
```

The configuration step may fail due to missing libraries; install these from your distro's package manager and repeat until it succeeds.

### Building for Windows (cross-compiling)

Download and extract [SDL 1.2.15](https://www.libsdl.org/release/SDL-devel-1.2.15-mingw32.tar.gz), placing the `SDL-1.2.15` directory in the root of the repo:

```
curl -O https://www.libsdl.org/release/SDL-devel-1.2.15-mingw32.tar.gz
tar zxvf SDL-devel-1.2.15-mingw32.tar.gz
```

Then build with the included `mingw-cross-toolchain.cmake`, customizing it if necessary if you're on a non-Debian/Ubuntu distro:

```
mkdir crossbuild
cd crossbuild
cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-cross-toolchain.cmake ..
make -j8
```

### Both (for release)

The `build-release.sh` script will perform a fresh build of both Windows and Linux executables in `relbuild/`. If you
symlink your AHAP and/or OHOL directories to `~/ahap` and `~/ohol` respectively, they will be copied there for easy
verification.

## Windows

Download and extract [SDL 1.2.15](https://www.libsdl.org/release/SDL-devel-1.2.15-mingw32.tar.gz), placing the `SDL-1.2.15` directory in the root of the repo.

Install [MSYS2](https://www.msys2.org/) and (optionally) [VS Code](https://code.visualstudio.com/).

In an MSYS2 terminal:

```
pacman -S mingw-w64-i686-{gcc,cmake,make}
```

### VS Code

(If you don't want to use VS Code, jump to the next section.)

Install the CMake plugin and tell it to configure the project, scan for toolkits, then select the `GCC ... i686-w64-mingw32` option.

Pressing F7 or using the "CMake: Build" action will build YumLife_windows.exe in the `build/` directory.

### MSYS2

(If you just want to use VS Code, you can skip this section.)

Launch the "MSYS2 MINGW32" shortcut that MSYS2 installed.

```
$ cd /c/Users/yourname/wherever/you/cloned/this/repo
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . -j
```

YumLife_windows.exe will be in that `build/` directory.

### Caveats

You will need to copy the libwinpthread-1.dll from MSYS (typically at `C:\msys64\mingw32\bin\libwinpthread-1.dll`) to
your OHOL directory to be able to use a .exe built in this way. Because of this additional dependency introduced by
MSYS, distributing this .exe is not recommended.

# Merging upstream changes

First, set up remotes for Jason's OneLife and minorGems repos. This only needs to be
done once. Note that upstream OHOL is two repos, which YumLife condenses into one for
easier forking.

```
$ git remote add OneLife git@github.com:jasonrohrer/OneLife.git
$ git remote add minorGems git@github.com:jasonrohrer/minorGems.git
```

To merge in changes from the OneLife repo, do a `git pull OneLife master` and resolve any
merge conflicts carefully.

Similarly, the minorGems repo can be merged with `git pull minorGems master`. Note that you
will need to move (as in `git mv`) any _new_ files added to that repo into the `minorGems`
directory.

Since YumLife uses CMake instead of Jason's build scripts, manual updates to `CMakeLists.txt`
are needed when upstream source files are added, removed, or moved.