# prinsight : Privacy Preserving Data Insight

- [prinsight : Privacy Preserving Data Insight](#prinsight--privacy-preserving-data-insight)
  - [About prinsight](#about-prinsight)
  - [Dependencies](#dependencies)
  - [Source Layout](#source-layout)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Build & Run Example Application](#build--run-example-application)
  - [Tests](#tests)
  - [Documentation](#documentation)
  - [Roadmap](#roadmap)
  - [Contributing](#contributing)
  - [Security Warnings](#security-warnings)
  - [License](#license)
  - [Acknowledgements](#acknowledgements)

## About prinsight
**prinsight** is a C++ library, aims to provide easy-to-use APIs to build privacy preserving applications utilizing multi-client and decentralized functional encryption techniques. Also all necessary data security batteries are part of SDK. The cryptographic primitives are based on *Decentralized Multi-Client Functional Encryption for Inner Product* paper by [*Chotard et al., 2017*](https://eprint.iacr.org/2017/989.pdf). A big thanks to authors of [CiFEr](https://github.com/fentec-project/CiFEr) library who has provided C-implementation of aforementioned paper. **prinsight SDK** uses *CiFEr* library.

Using **prinsight SDK**, we can build applications where users can share their data in encrypted form along with a shared functional decryption key to a analytics server and server can learn inner-product analysis(such as weighted sum) without accessing data in clear form. The generation of shared functional decryption key can be completely peer-to-peer decentralized way or coordinated by a server.

## Dependencies

 - prinsight SDK depends on: [CiFEr](https://github.com/fentec-project/CiFEr), [libgmp](https://gmplib.org/), [libsodium](https://github.com/jedisct1/libsodium), [amcl](https://github.com/miracl/amcl), [spdlog](https://github.com/gabime/spdlog), [base64](https://github.com/zhicheng/base64), and [nlohmann_json](https://github.com/nlohmann/json).

- Test dependency: [doctest](https://github.com/onqtam/doctest).
- Example applications depends on: [pistache](https://github.com/pistacheio/pistache), [curl](https://github.com/curl/curl), [RESTinCurl](https://github.com/jgaa/RESTinCurl), and [cxxopts](https://github.com/jarro2783/cxxopts).

## Source Layout

- [include](include) - Header files for prinsight SDK   
- [source](source) - prinsight SDK  
- [client](client) - an example REST client app using prinsight SDK 
- [server](server) - an example REST server app using prinsight SDK 

## Getting Started

### Prerequisites

The project is developed on Ubuntu 20.04 machine with usual *build-essential* package, CMake and C/C++11 tool chain.

### Build & Run Example Application
There is sample client and server application where 2 clients send their encrypted data and functional decryption key to server and server learns the sum of their data without accessing their individual clear data.

1. Clone the repo
   ```sh
   git clone https://github.com/dfbydx-com/prinsight.git
   ```
2. In the root directory of local repository, build and install *CiFEr* library and its dependencies first,
      ```sh
      cd build-scripts
      ./build-linux64.sh
      ```
3. In the root directory of local repository, build *data owner client app*,
      ```sh
      cmake -Hclient -Bout/build/client
      cmake --build out/build/client
      ```
4. In the root directory of local repository, build *coordinating server app*,
      ```sh
      cmake -Hserver -Bout/build/server
      cmake --build out/build/server
      ```
5. Open 2 shell prompts and run below commands in each shell to execute 2 client apps,
      ```sh
      ./out/build/client/piclient
      ```
6. Open another shell prompt and run below commands for executing server app,
      ```sh
      ./out/build/server/piserver
      ```
7. Now in another shell prompt, use *curl* to fetch result,
      ```sh
      curl http://localhost:9080/analysis
      ```
    If everything is fine, then above *curl* command returns following string,
    ```
    [["label1",2],["label2",10]]
    ```
## Tests

Build and run the test cases,

```sh
cmake -Htest -Bout/build/test
cmake --build out/build/test
./out/build/test/prinsightTests
```


## Documentation

Work in progress.

## Roadmap

Features
- [ ] TLS support
- [ ] Android build
- [ ] Dart FFI

Code quality and automation
- [ ] Improve logging
- [ ] Unit and integration test cases
- [ ] Code analysis and fuzzing
- [ ] Documentation
- [ ] CI setup


## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Security Warnings

As of now, this project is in very early stage and serves mainly proof-of-concepts, benchmarking and evaluation purpose and not for production use. Also implementation have not been fully-reviewed.

## License

Distributed under the Apache License. See [`LICENSE`](LICENSE) for more information.

## Acknowledgements

We have used this awesome [ModernCPPStarter](https://github.com/TheLartians/ModernCppStarter) template for this project.
