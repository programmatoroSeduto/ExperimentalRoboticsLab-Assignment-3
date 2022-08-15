
# How to setup a Doxygen/Sphinx workspace

## 1 -- setup Sphinx

1. `sphinx-quickstart` without separating the documentation and the code

2. configuration file (see the specific section about it)

3. ...

## 2 -- setup Doxygen

1. create a configuration file inside the root folder of the project using `doxywizard`

2. output as XML file in the folder `_build/xml` inside the root

3. (optional) output as HTML file in the folder `_build/html` inside the root

4. *... other config options?*

# Sphinx -- config file configuration

Here's the procedure I followed to configure this workspace. 

First lines, section `path setup`, copy and paste this:

```py
# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.

import os
import sys
import subprocess

sys.path.insert(0, os.path.abspath('.'))
```

section `general configuration`: copy and paste this. Here are the most significant plugins:

- [`myst_parser`](https://myst-parser.readthedocs.io/en/latest/index.html) : it enables Sphinx to parse the `.md` files. [Here](https://myst-parser.readthedocs.io/en/latest/configuration.html) you can fnd many other configuration settings for myst parser.
- `sphinx.ext.autodoc` : a very useful tool for generating automatically the documentation from source code
- `sphinx.ext.napoleon` : this project uses Google Docstrings for the code documentation, when feasible. This is useful especially for the Python code
- [`breathe`](https://breathe.readthedocs.io/en/latest/quickstart.html) : Sphinx works well for Python code, but it not so good for C++ code. This plugin takes the XML generated from Doxygen, then translates it into a format usable in Sphinx. 

```py
# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
# set completo
extensions = [
	'sphinx.ext.autodoc',
	'sphinx.ext.napoleon',
	'sphinx.ext.autosummary',
	'sphinx.ext.doctest',
	'sphinx.ext.viewcode',
	'sphinx.ext.githubpages',
	'sphinx.ext.intersphinx',
	'sphinx.ext.todo',
	'sphinx.ext.coverage',
	'sphinx.ext.mathjax',
	'sphinx.ext.ifconfig',
	'sphinx.ext.inheritance_diagram',
	'sphinx.ext.duration',
	'myst_parser',
	'breathe'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
```

Section `Options for HTML output`:

```python
# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx-rtd-theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# name of the main document (it will be the homepage of the docs)
master_doc = 'index'

# how to interpret the files
source_suffix = {
    '.rst': 'restructuredtext',
    '.txt': 'restructuredtext',
    '.md': 'markdown',
}
```

Add a new section `Syntax Hilighting`:

```python
# -- Syntax Hilighting -------------------------------------------------------

# allow Pygments to guess the language
highlight_language = 'guess'
```

Add a new section for the todo list plugin:

```python
# -- Options for todo extension ----------------------------------------------

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = True
```

**Breathe Options**, add it to the config file (remember to replace **???** with the name of the project you gave in Doxygen)

```py
# -- Options for breathe -----------------------------------------------------

# generate the Doxygen XML documentation
subprocess.call( 'doxygen Doxyfile', shell=True )

# path of the doxygen generated HTML
breathe_projects = {
  "???": "_build/xml/"
}

# title of the Doxygen project
breathe_default_project = "robocluedo"
breathe_default_members = ('members', 'undoc-members')
```

**PlantUML** : first modify the extensions, ...

```py
extensions = [
	# ...
	'sphinx.ext.graphviz', 
	'sphinxcontrib.plantuml'
]
```

... then add the module for PlantUML.

```py
# -- plantUML extension ------------------------------------------------------

plantuml = 'java -Djava.awt.headless=true -jar ./_ext/plantuml.jar'

```
