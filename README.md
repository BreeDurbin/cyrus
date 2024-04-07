![Debian Build]([https://github.com/github/docs/actions/workflows/main.yml/badge.svg](https://github.com/BreeDurbin/cyrus/blob/main/.github/workflows/cmake-single-platform.yml))

```
 ██████╗██╗   ██╗██████╗ ██╗   ██╗███████╗
██╔════╝╚██╗ ██╔╝██╔══██╗██║   ██║██╔════╝
██║      ╚████╔╝ ██████╔╝██║   ██║███████╗
██║       ╚██╔╝  ██╔══██╗██║   ██║╚════██║
╚██████╗   ██║   ██║  ██║╚██████╔╝███████║
 ╚═════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚══════╝
```

A combat and world tracker for ttrpgs.

Track initiative easy, create encounters before you session so you can jsut add them to the combat and have all your monsters roll initiative automatically. Initiative sorts itself. 

Dependencies:
```
make
cmake 3.28.3
qt6
boost libraries 1.83.0
```

To build:
```
# make build directory if doesn't already exist
mkdir build
cd build
# generate cmake files
cmake ..
# compile
make
```


## Contributors

If you want to contribute to cyrus, please read our [contribution guidelines](CONTRIBUTING.md).

Thanks goes to these wonderful people:

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tbody>
    <tr>
      <td align="center" valign="top" width="14.28%"><a href="https://breedurbin.carrd.co"><img src="https://avatars.githubusercontent.com/u/88691414?v=4?s=100" width="100px;" alt="Bree Lynne Durbin"/><br /><sub><b>Bree Lynne Durbin</b></sub></a><br /><a href="#code-BreeDurbin" title="Code">💻</a></td>
    </tr>
  </tbody>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

