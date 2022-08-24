
# ReStructured Directives -- practical documentation

:link: more about the reST syntax [here](https://docs.typo3.org/m/typo3/docs-how-to-document/main/en-us/WritingReST/Index.html)

:link: [a quick reference](https://docs.typo3.org/m/typo3/docs-how-to-document/main/en-us/WritingReST/CheatSheet.html) about reST

:link: a excellent reference about reST and sphinx [here](https://chiplicity.readthedocs.io/en/latest/Using_Sphinx/index.html)

## Include a file into another file

:warning: NOt working for .md files! :warning:

```rst
.. include:: <yourfile.format>
```

## MarkDown Vs. reStructured

sintassi reST:

```
.. directivename:: arguments
   :key1: val1
   :key2: val2

   This is
   directive content
```

in MarkDown diventa

```
\```{directivename} arguments
---
key1: val1
key2: val2
---
This is
directive content
\```
```

## TocTree directive

:point_up: a reST file can contain more than one `.. toctree::` directive. 

general syntax:

```rst
.. toctree::
	...
	:option: value
	...
	
	...
	file.name
	...
```

file name syntax:

- `path/of/the/file.tp` prints the title of the page and the link
- `this page <path/of/the/file.tp>` print *this page* with the link to the page

### ToC tree in markdown

```
\```{toctree} 
---
maxdepth: 1
caption: this is a toc
glob:
hidden:
---

... link ...
link name </link/path>

\```
```

## Index Template

:point_up: reSt is truly horrible. Use the reST only for indexing the documentation. And nothing else. 

```rst
.. _your-tag:

page title
==========

.. toctree::
	:maxdepth: 1
	:caption: ToC title
	:titlesonly:
	:glob:
	:hidden:
	
	...
	link text <./path/link.format>
	...

```

the page can be referred using both `:ref:<your-tag>` and `:any:<your-tag>`. **WITH BACKTICKS!**

## Add some code

### from Python

This directive inserts the entire documentation related to the py file you're referring to. Just one instructor for generating the documentation. 

:point_up: in Sphinx, you say where to generate the documentation, pointing the page with this directive. In Doxygen instead, the file system is fixed. 

```rst
.. automodule:: your_py_file
    :members:
    :noindex:
```

### from Doxygen/Breathe

same observations as before for what concerns the cpp files. 

```rst
.. doxygenfile:: your-cpp-file.cpp
    :project: your-project-name
```

## Images

```
.. image:: someimage.png
   :class: with-border with-shadow
   :alt: Textual alternative to the image
```

## tabs

:link: see [tabs in reST](https://docs.typo3.org/m/typo3/docs-how-to-document/main/en-us/WritingReST/CheatSheet.html#tabs)

```rst
.. tabs::

   .. group-tab:: bash

      .. code-block:: bash

         touch example-project-directory/public/FIRST_INSTALL

   .. group-tab:: powershell

      .. code-block:: powershell

         echo $null >> public/FIRST_INSTALL
```

## UML diagrams

[here](https://chiplicity.readthedocs.io/en/latest/Using_Sphinx/UsingGraphicsAndDiagramsInSphinx.html)

### Graphs with graphiz

```py
extensions = ['sphinx.ext.graphviz']
```

see the example on the Internet.

### Graphs with plantUML

see in particular [plantUML on gitHub](https://github.com/sphinx-contrib/plantuml) 

install the extension:

```bash
pip install sphinxcontrib-plantuml
```

extension:

```py
extensions = ['sphinxcontrib.plantuml']
```

### plantUML and markdown

```
\```{uml} 
@startuml

@endtuml
\```
```

## Links

```rst
`anchor text <URL>`__
```
