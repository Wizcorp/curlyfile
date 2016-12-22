const {
  url,
  files,
  line,
  header,
  bench,
  curly,
  standard
} = util = require('./util')

const func = process.argv[2]
const file = process.argv[3] || files[0]

if (!func || !util[func]) {
  console.log('Usage: node ./scripts/stress [standard|curly] [file]')
  process.exit()
}

if (files.indexOf(file) === -1) {
  console.log('File not found. Available files:', files)
  process.exit()
}

process.on('SIGINT', function () {
  process.stdout.write('\r')
  line()
  process.exit()
})

header()
function run() {
  const fileUrl = url + file
  bench(func, file, fileUrl, util[func], 1000, 20)(run)
}
run()

