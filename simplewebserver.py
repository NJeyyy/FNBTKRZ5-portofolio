import bottle

Theserver = bottle.Bottle()
class EnableCors(object):
  name = 'enable_cors'
  api = 2
  def apply(self, fn): # it previously have context as param, dunno for what tho it's not even being used
    def _enable_cors(*args, **kwargs):
      # set CORS headers
      bottle.response.headers['Access-Control-Allow-Origin'] = '*'
      bottle.response.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, OPTIONS, DELETE'
      bottle.response.headers['Access-Control-Allow-Headers'] = 'Origin, Accept, Content-Type, X-Requested-With, X-CSRF-Token'
      bottle.response.headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0"
      bottle.response.headers["Expires"] = "0"
      if bottle.request.method != 'OPTIONS':
        # actual request
        return fn(*args, **kwargs)
    return _enable_cors

@Theserver.route('/<filepath:path>') #allow static file access
def serve_static(filepath):
    resp = bottle.static_file(filepath, root='./')
    resp.set_header("Cache-Control", "public, max-age=60") # revalidate cache every 60 seconds
    return resp
@Theserver.route("/") # load main html
def loadp():
  return bottle.template("index.html") # pick the main .html file

@Theserver.route("/custom-path-or-route")
def custom():
  return "Custom message it will send or show on the page" # return some message

# I am not sure how to make the GET only route, but i do wanna know how.

@Theserver.post('some_method', method=['POST'], name='some_method')
def view():
    data = bottle.request.json #this is because it gonna get json data, but i do wanna know what if it had other type of data
    return {"status": "SUCCESS", "response":data}
    

Theserver.install(EnableCors())
Theserver.run(host="localhost", port=8080)