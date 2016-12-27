![Curlyfile](./logo.png)

Direct file download in Node.js, using libcurl.

The goal of this module is to avoid sending data in JavaScript-land;
while piping works perfectly fine, it also increases CPU usage for
no good reason when all we want is to download a file to disk.

So this module simply pipes libcurl to a file, all in C/C++.

Requirements
------------

Currently tested on Windows 10 and macOS.

  - CMake 3.5+

Installation
------------

```bash
npm install --save Curlyfile
```

Usage
-----

```javascript
const Curlyfile = require('curlyfile').Curlyfile
const curly = new Curlyfile()
curly.download('http://some.file.com/hello', '/tmp/test-curly', function (error) {
  console.log('Done!', error ? error : null)
})
```

Development
-----------

```bash
git clone ... 
cd curlyfile
git submodule update --init
# Or any cmake.js command - npm run -- cmake -h for more details
npm run cmake build 
```

Todo
----

  - [] Windows benchmark not very good
  - [] Proper tests
  - [] Cancel API
  - [] Progress API (# of bytes downloaded so far)