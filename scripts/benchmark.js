const {
  files,
  url,
  line,
  header,
  serial,
  bench,
  curly,
  standard
} = require('./util')

const benchmarks = []
const count = 1000
const parallel = 10
const parallellest = 50

files.forEach(function (file) {
  const fileUrl = url + file
  benchmarks.push(bench(`Standard`, file, fileUrl, standard, count))
  benchmarks.push(bench(`Curly`, file, fileUrl, curly, count))
  benchmarks.push(line)

  benchmarks.push(bench(`Standard`, file, fileUrl, standard, count, parallel))
  benchmarks.push(bench(`Curly`, file, fileUrl, curly, count, parallel))
  benchmarks.push(line)

  benchmarks.push(bench(`Standard `, file, fileUrl, standard, count, parallellest))
  benchmarks.push(bench(`Curly`, file, fileUrl, curly, count, parallellest))
  benchmarks.push(line)
})

header()
serial(benchmarks)
