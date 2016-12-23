const os = require('os')
const cluster = require('cluster')
const path = require('path')
const http = require('http')
const paperboy = require('paperboy')

const PORT = 8080
const MAX_WORKERS = os.cpus().length - 1

if (cluster.isMaster) {
  for (let i = 0; i < MAX_WORKERS; i += 1) {
    cluster.fork()
  }
  console.log(`[${process.pid}] Started ${MAX_WORKERS} workers, listening on port ${PORT}`)
  return
}

http.createServer(function(req, res) {
  paperboy.deliver(path.dirname(__filename), req, res)
}).listen(PORT, function () {
  console.log(`[${process.pid}] worker running`)
})
