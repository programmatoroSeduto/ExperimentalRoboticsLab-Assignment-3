#! /bin/bash

# make html
./reload_docs.sh

git add *
# git commit -m "script commit"
# git commit -m "documentation (finally, ready for the final setup)"
git commit -m "deployment (working on setup)"
git push repo main
