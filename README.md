# Linux Downloader
Linux downloader is a command line interface for downloading any Linux distro

### Notes
* If you want to use the server included in the source files, run `python3 server.py {port}` and change the url/port in `src/network.cpp`
* This project is on hold

## Planned features:
* Wrap text when offscreen at bottom (almost working, need to add scrolling too for when files offscreen)
* Huge rewrite for splitting all of website data processing to python
* On the above note, once program is completely finished, implement all python features into the program so no server is needed

## To compile:
```
make
```

## Supported Distros:
- [x] Ubuntu
- [x] Linux Kernel Source (in progress)
- [ ] Arch
- [ ] Gentoo

## Credit:
[httplib.h](https://github.com/yhirose/cpp-httplib)
