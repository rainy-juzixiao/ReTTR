# Changelog

## [1.4.0](https://github.com/rainy-juzixiao/ReTTR/compare/v1.3.1...v1.4.0) (2026-07-20)


### Features

* Added is_bounded_array/is_bounded_array_v. ([9958581](https://github.com/rainy-juzixiao/ReTTR/commit/995858182e296472b35388e99c8bed2c6201030c))
* **any:** add any-wrapping support and bounded array iteration ([48616af](https://github.com/rainy-juzixiao/ReTTR/commit/48616af30a8633b6ae954a360be75dcb5eca23e3))


### Bug Fixes

* Fix some performance issues. ([2d1387a](https://github.com/rainy-juzixiao/ReTTR/commit/2d1387a8c20c514534eee2561cd48a9612988d01))
* **property:** Fix some type issues in property. ([044ed3b](https://github.com/rainy-juzixiao/ReTTR/commit/044ed3bde4e61e39a767196d47577cf552a91785))

## [1.3.1](https://github.com/rainy-juzixiao/ReTTR/compare/v1.3.0...v1.3.1) (2026-07-08)


### Bug Fixes

* correct property inheritance order, readonly setter, and type conversion bugs ([50309fd](https://github.com/rainy-juzixiao/ReTTR/commit/50309fdcf6de0334e90f076f59b3b77bb9c123f0))
* fix main.cpp ([2e3a8ef](https://github.com/rainy-juzixiao/ReTTR/commit/2e3a8ef2ccbe309ddc88d0bce2c07ff072108443))

## [1.3.0](https://github.com/rainy-juzixiao/ReTTR/compare/v1.2.0...v1.3.0) (2026-07-06)


### Features

* **registration:** filter base class accessibility in auto-scan bases ([8301920](https://github.com/rainy-juzixiao/ReTTR/commit/8301920630bca3d6d66c8070a2c2d19d638cafe0))

## [1.2.0](https://github.com/rainy-juzixiao/ReTTR/compare/v1.1.0...v1.2.0) (2026-07-06)


### Features

* **any:** add std::formatter specialization for rettr::any under C++20 ([5e9c4fa](https://github.com/rainy-juzixiao/ReTTR/commit/5e9c4fa2cdcdeb1e3fd6da94b3093fc9125ef828))
* **metadata annotation:** fix factory construct. ([21491fd](https://github.com/rainy-juzixiao/ReTTR/commit/21491fd0ae9f3dae427efbe63d2cf1a2c18f8a14))


### Bug Fixes

* Fix invalid conversion from 'const void*' to 'void*' ([96f1791](https://github.com/rainy-juzixiao/ReTTR/commit/96f1791102b7332722d34c62f4fee240f1d404d5))

## [1.1.0](https://github.com/rainy-juzixiao/ReTTR/compare/v1.0.0...v1.1.0) (2026-07-06)


### Features

* **core:** add object_view-from-any constructor and refine type constraints ([9f4ef4a](https://github.com/rainy-juzixiao/ReTTR/commit/9f4ef4a2252bc84d1af8f5d20dbd10b214f9d7e8))

## 1.0.0 (2026-07-03)


### ⚠ BREAKING CHANGES

* C++26 Static Reflection Auto-Scan Registration & Code Modernization

### Features

* feat:  ([7ae1ae2](https://github.com/rainy-juzixiao/ReTTR/commit/7ae1ae2ce220b280d51ad7164125ddf7588ec14f))
* feat:  ([c814a49](https://github.com/rainy-juzixiao/ReTTR/commit/c814a49494434a288676aa69d02f73e5fc6c8e99))
* feat:  ([db5cb02](https://github.com/rainy-juzixiao/ReTTR/commit/db5cb023a23d435691b43ac9848e975db9ae8bef))
* feat:  ([8c32289](https://github.com/rainy-juzixiao/ReTTR/commit/8c3228909c0a4398cd61ea5dea5e452427e56454))
* feat:  ([573df49](https://github.com/rainy-juzixiao/ReTTR/commit/573df490eb810c16ab2c9fe4f44b719cb02ed1d6))
* feat:  ([37ee4e0](https://github.com/rainy-juzixiao/ReTTR/commit/37ee4e02bf1bedbb9bca972532cf04ce4e703585))
* feat:  ([015276c](https://github.com/rainy-juzixiao/ReTTR/commit/015276c04e5e0996a7833af8427d2a9507b82df5))
* feat:  ([f8a4951](https://github.com/rainy-juzixiao/ReTTR/commit/f8a4951c71da8233da7e5c50f404c0d9896e893f))
* feat:  ([6963842](https://github.com/rainy-juzixiao/ReTTR/commit/69638429252371c468e2e3a2ea8c87c442faffbc))
* feat:  ([0c0fe19](https://github.com/rainy-juzixiao/ReTTR/commit/0c0fe192bce2bce93d62abfa8a340ea8808adafe))
* feat:  ([27adf98](https://github.com/rainy-juzixiao/ReTTR/commit/27adf98d9d5a30bd375ae2fc96fc6b21bcd49468))
* feat:  ([b246840](https://github.com/rainy-juzixiao/ReTTR/commit/b2468406bcc8600325ed42398492f4ce8bb2cad9))
* add --skip-test, --skip-examples, --no-reflection-ts options to install/uninstall scripts ([61e36c1](https://github.com/rainy-juzixiao/ReTTR/commit/61e36c12193d49863e4e8b273269f2287af8a9a8))
* add --skip-test, --skip-examples, --no-reflection-ts options to scripts and docs ([ffe8f61](https://github.com/rainy-juzixiao/ReTTR/commit/ffe8f619c4a5b0d8f9de8a58bd1c35c0cda0a196))
* add metadata auto bind support for property bind. ([d0dfe73](https://github.com/rainy-juzixiao/ReTTR/commit/d0dfe73463f6565c2455f3824cda0874bf2dc81b))
* Add multi-distro Docker support for Ubuntu, Fedora, and Arch Linux ([d388c61](https://github.com/rainy-juzixiao/ReTTR/commit/d388c61fafa029c10fe3301048ec11a73c7e32b2))
* Add standard header items: ([8c0ab9c](https://github.com/rainy-juzixiao/ReTTR/commit/8c0ab9c10da81ecc83d92c6be8d5cb4ea902a226))
* Added any.cppm module unit. ([1f078cf](https://github.com/rainy-juzixiao/ReTTR/commit/1f078cf49ee02bdb57b5fd9f4f4e2c247080e6c3))
* Added c++26 preview header metadata. ([8539796](https://github.com/rainy-juzixiao/ReTTR/commit/853979679473a0e9c8a50dd47b18384d72f65eb4))
* Added fetch_cmake_import_std_module_uid.py ([92078dd](https://github.com/rainy-juzixiao/ReTTR/commit/92078ddf04328ca2a5c7ffc69abfcb454ea97278))
* Added GitHub Actions build for linux platform. ([e40399e](https://github.com/rainy-juzixiao/ReTTR/commit/e40399e80a4b7f82037e61dfe28f95b52ccdbf49))
* Added install and uninstall scripts. ([2c77c2e](https://github.com/rainy-juzixiao/ReTTR/commit/2c77c2eee3267eed7327661dfe70969f2f2b5a81))
* Added install support. ([53fadc1](https://github.com/rainy-juzixiao/ReTTR/commit/53fadc1edb6e6e81d146660d1ad098e94f89c078))
* Added install/uninstall script for Windows PowerShell. ([654e1c9](https://github.com/rainy-juzixiao/ReTTR/commit/654e1c98357f9017fd2e8c08e032c68305fc7803))
* Added metadata scan support for constructor/constructor func. ([feb9285](https://github.com/rainy-juzixiao/ReTTR/commit/feb92859045213aaf12d15bc5a971ebd149a267c))
* Added module build support. ([9e0dd6f](https://github.com/rainy-juzixiao/ReTTR/commit/9e0dd6fe2459d56ec52fede1367e44c6d16b9d86))
* Added parameter names extract support for ctor/ctor_func. ([5d5e1b1](https://github.com/rainy-juzixiao/ReTTR/commit/5d5e1b19361a0bd42958bf679dc826232d5d6171))
* Added parameters names scan as c++26 features. ([5d05f07](https://github.com/rainy-juzixiao/ReTTR/commit/5d05f07546314ad5e5c69f6523ddc40fd4f6239d))
* Added RETTR_BUILD_EXAMPLES and RETTR_BUILD_UNIX_TESTS options. ([f8428ff](https://github.com/rainy-juzixiao/ReTTR/commit/f8428ff9638f88231ecd90dd2dc9422778d7a864))
* Added rettr.cppm ([353bacd](https://github.com/rainy-juzixiao/ReTTR/commit/353bacd813b9e2102e0db0d7c8e97ea5d3a67689))
* Added support for Docker. ([a6beb9a](https://github.com/rainy-juzixiao/ReTTR/commit/a6beb9a3408728032e6a5e72db22233615019b24))
* Added type.cppm and typeinfo.cppm ([b31b2bf](https://github.com/rainy-juzixiao/ReTTR/commit/b31b2bf1661d365f59d4c60ba22a5a7113327344))
* Added unix_unit_test_pipeline.sh ([174e2f1](https://github.com/rainy-juzixiao/ReTTR/commit/174e2f1c4306513bf1a18e42ea460fea762d6197))
* C++26 Static Reflection Auto-Scan Registration & Code Modernization ([b92b234](https://github.com/rainy-juzixiao/ReTTR/commit/b92b234f4aadf77b3108ed2a1794ded1dd39406e))
* improve the error process by using exceptions. ([0f3c55a](https://github.com/rainy-juzixiao/ReTTR/commit/0f3c55a40c306cad102be80c5e42f000b3125da8))
* Improve the metadata string support. ([35df703](https://github.com/rainy-juzixiao/ReTTR/commit/35df703e0e0792f4828e6a2cd4fbba2bd0adcf69))
* Improve the performance for object_view. use stack memory to avoid pointer access. ([e40d654](https://github.com/rainy-juzixiao/ReTTR/commit/e40d654732a0f6448d317c8e8d8c44de4a256602))
* improve the performance for rettr_cast. ([4d40b37](https://github.com/rainy-juzixiao/ReTTR/commit/4d40b37d382165c556932e1e5d74df091e5f6606))
* Improve the scan for property, and added method scan metadata support. ([9a585ac](https://github.com/rainy-juzixiao/ReTTR/commit/9a585ac03ec87a0a257e3c8dde6215d2d77398a6))
* **lunar:** add std::formatter specialization for basic_string_view ([c622af1](https://github.com/rainy-juzixiao/ReTTR/commit/c622af1a33e55eda32a6f1c2ef6d4a3000eb5d2f))
* **lunar:** implement auto-scan registration for member functions ([cb271d9](https://github.com/rainy-juzixiao/ReTTR/commit/cb271d9438d8bbe9003bfa8fc5d6775dd2cfc2bc))
* **lunar:** implement auto-scan registration for member functions ([91b0028](https://github.com/rainy-juzixiao/ReTTR/commit/91b002807edb9939a10581ab85e99563ceee2c9c))
* **lunar:** implement C++26 static reflection auto-scan registration ([7361b9e](https://github.com/rainy-juzixiao/ReTTR/commit/7361b9e514f78091f72fc14546f93f55de1d9061))
* **lunar:** implement make_bases_available for automatic base class registration ([7baf16d](https://github.com/rainy-juzixiao/ReTTR/commit/7baf16d401642471da367eb81e5645b9b3312ec0))
* **modules:** add registration macro header ([e7678ca](https://github.com/rainy-juzixiao/ReTTR/commit/e7678ca98b7ba7d81735b978c6a9bff98836d404))
* **modules:** add rettr.type_list module ([0293656](https://github.com/rainy-juzixiao/ReTTR/commit/0293656241c212fb8c504db89c761fa058b585e5))
* **modules:** complete C++20 module coverage and macro compatibility headers ([b921766](https://github.com/rainy-juzixiao/ReTTR/commit/b921766524fa29dda8fa32ae3a599a81452ee296))


### Bug Fixes

* fix:  ([b963107](https://github.com/rainy-juzixiao/ReTTR/commit/b9631079fc7992280303391cc30f33c7770d0c0c))
* fix:  ([8d46721](https://github.com/rainy-juzixiao/ReTTR/commit/8d46721b2363be582124b89295a6fcc331efe90b))
* any copy problem ([176e2a5](https://github.com/rainy-juzixiao/ReTTR/commit/176e2a5bd7ad48f933c81d2f45d1a11e281e8b68))
* correct copy/move semantics and resource cleanup in invoker_accessor ([8342aa4](https://github.com/rainy-juzixiao/ReTTR/commit/8342aa494dfe9342a5fafe78ee0afebf22f8aa34))
* correct map insert copy-paste bug, match handler return, and add missing tests ([41366d3](https://github.com/rainy-juzixiao/ReTTR/commit/41366d3f92a2349f372be78c391b4f56c2798c00))
* Fix acttion workflow[Prepare Environment] ([e430227](https://github.com/rainy-juzixiao/ReTTR/commit/e430227cd63cdbb4bb312be6baf31838ebfd0429))
* fix basic_object invoke compile error and cleanup enumeration. ensure assignment for enum_entries is constexpr. ([ecd1c6e](https://github.com/rainy-juzixiao/ReTTR/commit/ecd1c6ecd77085afbe20b55a17430a3b87ef792b))
* Fix compile error in cpp17-cpp23 ([1b45f42](https://github.com/rainy-juzixiao/ReTTR/commit/1b45f423fc36d1e66c9500b82c9a5ea9f4a2e121))
* fix compile issues for bind in cpp 26. ([444cec9](https://github.com/rainy-juzixiao/ReTTR/commit/444cec98222973753fbf3cded41378f1eb7e68a5))
* Fix ctor_func get metadata failed. ([acbdca6](https://github.com/rainy-juzixiao/ReTTR/commit/acbdca6ab65ad50fcbd6b7521ef28e7e95fd7c67))
* fix enumeration. ([4c73ed7](https://github.com/rainy-juzixiao/ReTTR/commit/4c73ed72cc9162ca5c566b9e0a2b0dd85f4703e0))
* Fix field pointer bind. ([757b4e4](https://github.com/rainy-juzixiao/ReTTR/commit/757b4e4713795c625ca042a44cb0b1ad4bca1996))
* Fix ignored warning in clang ([e7720f5](https://github.com/rainy-juzixiao/ReTTR/commit/e7720f5c3bc99970d5683c104d024ee4539dc483))
* fix link error in scan data member metadata. ([cd87b08](https://github.com/rainy-juzixiao/ReTTR/commit/cd87b089fdacae634ff4ff3707cd2f6513c48b0f))
* fix object model wrong behavior in copy assignment/copy constructor ([2847dd1](https://github.com/rainy-juzixiao/ReTTR/commit/2847dd18b2c84e5241c0eac1a3382feb3a667d3f))
* fix object_view valid() behavior ([6179f72](https://github.com/rainy-juzixiao/ReTTR/commit/6179f72b7c702ba6f12b21d0718b2b028a496259))
* Fix release-please.yml ([134e8e6](https://github.com/rainy-juzixiao/ReTTR/commit/134e8e6cc703db27b4ad0f2b9f1c145d68f5f588))
* fix some issues. ([8ca59b7](https://github.com/rainy-juzixiao/ReTTR/commit/8ca59b7f7128f522a86a5e77d6bd928f60f4efc8))
* fix test discover when using ctest program. ([7435640](https://github.com/rainy-juzixiao/ReTTR/commit/7435640b125c301d9db6434cf6dbe62675e25fd1))
* Fix tu local warning in some files. ([34d569c](https://github.com/rainy-juzixiao/ReTTR/commit/34d569cc8be9487e010ec2904a896e56f9ec0028))
* fix varadic parameter names scan. ([96b81fd](https://github.com/rainy-juzixiao/ReTTR/commit/96b81fd80279cf14992fac271b44cd41c3d8bff0))
* function assign operator and fix function.hpp behavior ([e7e140c](https://github.com/rainy-juzixiao/ReTTR/commit/e7e140cd97bae15d395b5b946337e6ff0840f52e))
* install/uninstall scripts work from user PWD, uninstall fully cleans lib ([9154c6f](https://github.com/rainy-juzixiao/ReTTR/commit/9154c6f90e5d69770c333cba2fca44f41aa31acb))
* **tests:** guard parameter name tests and fix method filter test under C++26 reflection ([ec7fd0a](https://github.com/rainy-juzixiao/ReTTR/commit/ec7fd0afe19044c4dfb094d041fd0998a99464de))
