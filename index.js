const weak = require('./build/Release/weak')
const weakMap = new WeakMap()

module.exports = function (obj, finalizer) {
  if (arguments.length !== 2) {
    throw new Error(`Expected 2 arguments, instead got ${arguments.length}`)
  }
  if (Object(obj) !== obj) {
    throw new Error('arguments[0] cannot be primitive (string/number/boolean/null/undefined/symbol)')
  }
  if (typeof finalizer !== 'function') {
    throw new Error('arguments[1] must be a function')
  }
  weakMap.set(obj, weak(finalizer))
}
