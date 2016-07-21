#!/bin/bash
astyle -r -n --exclude=root --style=google "*.c" "*.h"
git stage .
git commit -m "Astyle"
git push origin master
