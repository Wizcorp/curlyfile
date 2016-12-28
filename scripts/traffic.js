const {
  url,
  files,
  line,
  header,
  bench,
  curly,
  standard
} = util = require('./util')

const funcName = process.argv[2]
const func = util[funcName]

let fileToDownload
let fileUrl

if (!func) {
  return console.error('Usage: node scripts/traffic [curly|standard]')
}

function run(error, ts) {
  if (error) {
    return console.log('Error:', error)
  }

  const now = Date.now()
  if (fileToDownload && ts) {
    console.log(`>> ${fileToDownload}\t:\t${(now - ts)}`)
  }

  ts = now
  fileToDownload = files[Math.floor((Math.random() * process.hrtime()[1]) % files.length)]
  fileUrl = url + fileToDownload

  func(fileUrl, function (error) {
    run(error, ts)
  })
}

for (let count = 0 ; count < 10; count += 1) {
  run()
}