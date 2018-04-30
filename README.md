# node-finalize-addon [![Build Status](https://travis-ci.org/shyiko/node-finalize-addon.svg?branch=master)](https://travis-ci.org/shyiko/node-finalize-addon) [![npm](https://img.shields.io/npm/v/finalize-addon.svg)](https://www.npmjs.com/package/finalize-addon)

Object [finalization](https://en.wikipedia.org/wiki/Finalizer) based on [N-API](https://nodejs.org/api/n-api.html) for Node.js v8.6.0+.  
In a nutshell, it allows you to register a callback which will be executed when/if 
observable object becomes eligible for garbage collection. 

## Installation

```sh
npm install finalize-addon
```

## Usage

```javascript
const finalize = require('finalize-addon')
const finalizeCallback = () => {
  // warn if fd/sockets are left open, etc
}

function main () {
  const obj = ...
  finalize(obj, finalizeCallback)
}

main()
// obj is now eligible for garbage collection 
// (un-comment line below to force gc to run (requires `node --expose-gc`))
// gc() 
```

> `require`ing N-API addon in Node.js < v10.0.0 results in `Warning: N-API is an experimental feature and could change at any time.`. 
Use `NODE_NO_WARNINGS=1 node ...` or `node --no-warnings ...` to suppress.

## License

All code, unless specified otherwise, is licensed under the [MIT](LICENSE) license.  
Copyright (c) 2018 Stanley Shyiko.
