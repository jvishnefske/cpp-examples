body_template = """
<!html>
<html>
<head>
<title>%(title)s</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<!-- bootstrap css from cdn -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">
<!-- no bootstrap js -->
<!-- use boostrap to create a header, menu, and new user form -->
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>
<!-- bootstrap theme -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous">
<!-- custom css -->
<link rel="stylesheet" href="%(css)s">
<!-- custom javascript -->
<script src="%(js)s"></script>
</head>
<body>
<div class="container">
<div class="row">
<div class="col-md-12">
<h1>%(title)s</h1>
<p>%(description)s</p>
<p>%(footer)s</p>
</div>
</div>
</div>
</body>
</html>

description="""

# the footer
footer = """
<p>
<a href="http://www.cdc.gov/">CDC</a>
<a href="http://www.hhs.gov/">HHS</a>
<a href="http://www.hrsa.gov/">HRSA</a>
<a href="http://www.hud.gov/">HUD</a>
<a href="http://www.justice.gov/">DOJ</a>
<a href="http://www.care.gov/">CMS</a>
<a href="http://www.usaid.gov/">USAID</a>
<a href="http://www.acf.hhs.gov/">ACF</a>
<a href="http://www.nimh.nih.gov/">NIH</a>
<a href="http://www.samhsa.gov/">SAMHSA</a>
<a href="http://www.cde.state.co.us/">CDE</a>
</p>
"""

# the menu
menu = """
<nav class="navbar navbar-default">
  <div class="container-fluid">
    <!-- Brand and toggle get grouped for better mobile display -->
    <div class="navbar-header">
      <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1" aria-expanded="false">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
      <a class="navbar-brand" href="#">%(title)s</a>
    </div>

    <!-- Collect the nav links, forms, and other content for toggling -->
    <div class="collapse navbar-colapse" id="bs-example-navbar-collapse-1">
      <ul class="nav navbar-nav">
        <li class="active"><a href="#">Link <span class="sr-only">(current)</span></a></li>
        <li><a href="#">Link</a></li>
        <li class="dropdown">
          <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">Dropdown <span class="caret"></span></a>
          <ul class="dropdown-menu">
            <li><a href="#">Action</a></li>
            <li><a href="#">Another action</a></li>
            <li><a href="#">Something else here</a></li>
            <li role="separator" class="divider"></li>
            <li><a href="#">Separated link</a></li>
            <li role="separator" class="divider"></li>
d            <li><a href="#">One more separated link</a></li>
          </ul>
        </li>
      </ul>
      <form class="navbar-form navbar-left">
        <div class="form-group">          <input type="text" class="form-control" placeholder="Search">
        </div>
        <button type="submit" class="btn btn-default">Submit</button>
      </form>
      <ul class="nav navbar-nav navbar-right">
        <li><a href="#">Link</a></li>
        <li class="dropdown">
          <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">Dropdown <span class="caret"></span></a>
          <ul class="dropdown-menu">
            <li><a href="#">Action</a></li>
            <li><a href="#">Another action</a></li>
            <li><a href="#">Something else here</a></li>
            <li role="separator" class="divider"></li>
            <li><a href="#">Separated link</a></li>
          </ul>
        </li>
      </ul>
    </div><!-- /.navbar-collapse -->
  </div><!-- /.container-fluid -->
</nav>
"""

# the header
header = """
<div class="jumbotron">
  <div class="container">
    <h1>%(title)s</h1>
    <p>%(description)s</p>
  </div>
</div>
"""

# the footer
footer = """
<footer class="footer">
  <div class="container">
    <p class="text-muted">%(footer)s</p>
  </div>
</footer>
"""

# the body
body = """
<div class="container">
  <div class="row">
    <div class="col-md-12">
      <h2>%(title)s</h2>
      <p>%(description)s</p>
    </div>
  </div>
</div>
"""

# the form
form = """
<div class="container">
  <div class="row">
    <div class="col-md-12">
      <form class="form-horizontal" role="form">
        <div class="form-group">
          <label for="inputEmail1" class="col-lg-2 control-label">Email</label>
          <div class="col-lg-10">
            <input type="email" class="form-control" id="inputEmail1" placeholder="Email">
          </div>
        </div>
        <div class="form-group">
          <label for="inputPassword1" class="col-lg-2 control-label">Password</label>
          <div class="col-lg-10">
            <input type="password" class="form-control" id="inputPassword1" placeholder="Password">
          </div>
        </div>
        <div class="form-group">
          <div class="col-lg-offset-2 col-lg-10">
            <div class="checkbox">
              <label>
                <input type="checkbox"> Remember me
              </label>
            </div>
          </div>
        </div>
        <div class="form-group">
          <div class="col-lg-offset-2 col-lg-10">
            <button type="submit" class="btn btn-default">Sign in</button>
          </div>
        </div>
      </form>
    </div>
  </div>
</div>
"""

# the table
table = """
<div class="container">
  <div class="row">
    <div class="col-md-12">
      <table class="table">
        <thead>
          <tr>
            <th>#</th>
            <th>First Name</th>
            <th>Last Name</th>
            <th>Username</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>1</td>
            <td>Mark</td>
            <td>Otto</td>
            <td>@mdo</td>
          </tr>
          <tr>
            <td>2</td>
            <td>Jacob</td>
            <td>Thornton</td>
            <td>@fat</td>
          </tr>
          <tr>
            <td>3</td>
            <td>Larry</td>
            <td>the Bird</td>
            <td>@twitter</td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</div>
"""
import flask
from flask import Flask, render_template
from flask_wtf.csrf import CSRFProtect


app = Flask(__name__)
csrf = CSRFProtect(app)


@app.route("/")
def index():
    "render and return body_template"
    return render_template(
        "index.jinja",
        title="Home",
        description="This is the home page",
        footer=footer,
        body=body,
        menu=menu,
    )


@app.route("/about")
def about():
    "render and return body_template"
    return render_template(
        "index.jinja",
        title="About",
        description="This is the about page",
        footer=footer,
        body=body,
        menu=menu,
    )


@app.route("/contact")
def contact():
    "render and return body_template"
    return render_template(
        "body_template",
        title="Contact",
        description="This is the contact page",
        footer=footer,
        body=body,
        menu=menu,
    )


@app.route("/newuser")
def newuser():
    "render and return form"
    return render_template(
        "form_template",
        title="New User",
        description="This is the new user page",
        footer=footer,
        form=form,
    )


if __name__ == "__main__":
    app.run(debug=True)
