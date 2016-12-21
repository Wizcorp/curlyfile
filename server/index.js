const path = require('path')
const http = require('http')
const paperboy = require('paperboy')

http.createServer(function(req, res) {
  paperboy.deliver(path.dirname(__filename), req, res)
}).listen(8080)