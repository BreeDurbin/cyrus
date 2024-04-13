![Debian Build Status](https://img.shields.io/github/actions/workflow/status/BreeDurbin/cyrus/cmake-single-platform.yml?style=flat-square&logo=ubuntu&label=Ubuntu%20Build&color=#E95420)
![GPL License](https://img.shields.io/github/license/BreeDurbin/cyrus?style=flat-square&logo=gnu&label=License)




![alt text](./docs/image/CYRUS_BANNER.png)


A combat and world tracker for ttrpgs.

Track initiative easy, create encounters before you session so you can just add them to the combat and have all your monsters roll initiative automatically. Add new monsters/players to the combat witha  single button. Handle delayed actions in combat by dragging and dropping. Initiative sorts itself. 

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

