
# Sphinx for Py -- modules mocking

sphinx, when it finds a statement *import something* ... tries to import, and complains if it doesn't exists. This is a problem sometimes.

for avoidig such a situation, there's an extension which *emualtes* a library which doesn't exist.

```{note}
see this page [here](https://stackoverflow.com/questions/15889621/sphinx-how-to-exclude-imports-in-automodule)
```

## sphinx configuration

just add this section:

```py
# -- Options for autodoc -----------------------------------------------------

# mock ros and other stuff
autodoc_mock_imports = ["rospy"]
```
