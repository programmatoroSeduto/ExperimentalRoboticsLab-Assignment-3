
# YAML and Sphinx

```{warning}
unfortunately, not working.
```

```{toctree}
./sphinx-yaml.md
```

see [sphinxcontrib-yaml](https://pypi.org/project/sphinxcontrib-yaml/)

see [the official release on GitHub](https://github.com/Jakski/sphinxcontrib-autoyaml)

## installation

```bash
pip3 install sphinxcontrib-autoyaml
```

## configuring Sphinx

### extensions

```py
extensions = [
	"sphinxcontrib.autoyaml"
]
```

### module

```py
# -- Options for autoyaml ----------------------------------------------------

# Look for YAML files relatively to this directory
autoyaml_root = '.'

# Character(s) which start a documentation comment
autoyaml_doc_delimiter = '##'

# Comment start character(s)
autoyaml_comment = '#'

# Parse comments from nested structures n-levels deep
autoyaml_level = 1
```

## HOW TO use the extension

### reST side

```reST
Some title
==========

Documenting single YAML file.

.. autoyaml:: some_yml_file.yml

```
