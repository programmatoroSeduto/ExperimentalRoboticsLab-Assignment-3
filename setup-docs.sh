#! /bin/bash

set -e

echo "====="
echo "===== INSTALLING DOXYGEN"
echo "====="

sudo apt-get install -y doxygen
sudo apt-get install -y doxygen-gui

echo "====="
echo "===== DOXYGEN VERSION"
echo "====="

# doxywizard --help
#v1.8.17
doxygen -v

echo "====="
echo "===== INSTALLING SPHINX"
echo "====="

apt-get install python-setuptools python-dev build-essential  -y
pip3 install sphinx==4.5.0
pip3 install sphinxcontrib-fulltoc
pip3 install rst2pdf
pip3 install sphinxcontrib-needs
pip3 uninstall docutils -y
pip3 install docutils
pip3 install sphinx-needs

echo "====="
echo "===== SPHINX VERSION"
echo "====="

sphinx-quickstart --version

echo "====="
echo "===== INSTALLING SPHINX ADDONS"
echo "====="

pip3 install breathe
pip3 install sphinx-rtd-theme
pip3 install myst-parser

echo "====="
echo "===== INSTALLING JAVA (for PlantUML)"
echo "====="
echo "     see how to install openjdk 13"
echo "https://techoral.com/blog/java/install-openjdk-13-ubuntu.html"

sudo apt-get install -y openjdk-13-jre
# export JAVA_HOME=/usr/lib/jvm/openjdk-13-jdk
# export PATH=$PATH:$JAVA_HOME/bin

echo "====="
echo "===== OPENJDK13 VERSION"
echo "====="

java --version

echo "====="
echo "===== INSTALLING PLANUML"
echo "====="
echo "     see plantUML user guide"
echo "https://chiplicity.readthedocs.io/en/latest/Using_Sphinx/UsingGraphicsAndDiagramsInSphinx.html"
echo "     see graphviz"
echo "https://graphviz.org/download/"

sudo apt-get install -y graphviz
pip3 install sphinxcontrib-plantuml

