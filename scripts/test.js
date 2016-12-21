const {
  files,
  url,
  curly,
  standard
} = require('./util')

const fileUrl = url + files[0]
const start = Date.now()

standard(fileUrl, function (...args) {
  console.log('done:', Date.now() - start, ...args)
})

curly(fileUrl, function (...args) {
  console.log('done:', Date.now() - start, ...args)
})
