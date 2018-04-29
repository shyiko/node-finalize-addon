const finalize = require('.')
const assert = require('assert')

assert.throws(() => finalize())
assert.throws(() => finalize({}))
assert.throws(() => finalize({}, () => {}, {}))
assert.throws(() => finalize('', () => {}))
assert.throws(() => finalize(0, () => {}))
assert.throws(() => finalize(true, () => {}))
assert.throws(() => finalize(null, () => {}))
assert.throws(() => finalize(undefined, () => {}))
assert.throws(() => finalize(Symbol(), () => {}))

const handle = setTimeout(() => {
  assert.fail('finalizer not called')
}, 5000)
function finalizer () {
  clearTimeout(handle)
}

function run () {
  const obj = {key: 'value'}
  finalize(obj, finalizer)
}

run()
gc() // --expose-gc
