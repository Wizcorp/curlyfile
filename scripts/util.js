const url = 'http://localhost:8080/'

const files = [
  '4k',
  '32k',
  '128k',
  '1m',
  '5m',
  '10m'
]

/**
 * CURLYFILE
 */
const {
  Curlyfile
} = require('../')
const curlyfile = new Curlyfile()
function curly(file, callback) {
  curlyfile.download(file, '/tmp/test-curly', callback)
}

/**
 * Standard
 */
const http = require('http')
const dns = require('dns')
const dnscache = require('dnscache')({
  enable: true,
  ttl: 300,
  cachesize: 1000
})
const fs = require('fs')
function standard(file, callback) {
  http.get(file, function (res) {
    const dest = fs.WriteStream('/tmp/test-standard')
    res.pipe(dest)
    res.on('error', callback)
    res.on('end', callback)
  })
}

/**
 * Benchmark generator
 */
function bench(name, file, url, func, count, parallel = 1) {
  return function(callback) {
    const start = Date.now()
    let countdown = count

    function run() {
      func(url, function (error) {
	      if (error) {
          console.log('Error:', error)
          return
        }

        if (countdown % 50 === 0) {
          process.stderr.write(`\r>> ${countdown} `)
        }

        countdown -= 1

        if (countdown === 0) {
          process.stderr.write(`\r       `)
          console.log(`\r| ${file}\t | ${name} \t | ${count} \t | ${parallel} \t | ${Date.now() - start} \t|`)
          callback()
        } else if (countdown >= parallel) {
          run()
        }
      })
    }

    for (let d = 0; d < parallel; d += 1) {
      run()
    }
  }
}

/**
 * Serial runner
 */
function serial(funcs) {
  function run() {
    if (funcs.length === 0) {
      return
    }

    const func = funcs.shift()
    setTimeout(function () {
      func(run)
    }, 500)
  }

  run()
}

/**
 * Display
 */
function line(callback) {
  console.log('-----------------------------------------------------------------')
  callback && callback()
}

function header() {
  line()
  console.log('| File\t | Function\t | Count\t | \\\\\t | Time\t\t|')
  line()
}

module.exports = {
  url,
  files,
  line,
  header,
  serial,
  bench,
  curly,
  standard
}