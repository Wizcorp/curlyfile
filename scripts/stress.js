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

if (files.indexOf(file) === -1 && file !== 'random') {
  console.log('File not found. Available files: random,', files)
  process.exit()
}

process.on('SIGINT', function () {
  process.stdout.write('\r')
  line()
  process.exit()
})

header()
function run(error) {
  if (error) {
    return console.log(error)
  }

  let fileToDownload = file

  if (file === 'random') {
    fileToDownload = files[Math.floor(Math.random() * files.length)]
  }

  const fileUrl = url + fileToDownload
  bench(func, fileToDownload, fileUrl, util[func], 100, 20)(run)
}
run()

