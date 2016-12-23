const async = require('async')

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

    async.timesLimit(count, parallel, function (counter, callback) {
      if (counter % 100 === 0) {
        process.stderr.write(`\r>> ${counter} `)
      }

      func(url, callback)
    }, function (error) {
      if (error) {
        return callback(error)
      }

      process.stderr.write(`\r>>          `)
      console.log(`\r| ${file}\t | ${name} \t | ${count} \t | ${parallel} \t | ${Date.now() - start} \t|`)
      callback()
    })
  }
}

/**
 * Serial runner
 */
function serial(funcs) {
  async.series(funcs, line)
}

/**
 * Display
 */
function line(callback) {
  console.log('-----------------------------------------------------------------')
  if (typeof callback == 'function') {
    callback()
  } else if (callback) {
    throw callback // error!
  }
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