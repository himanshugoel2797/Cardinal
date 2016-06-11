#!/bin/bash
astyle -r -n --style=google "*.c" "*.h"
git stage .
git commit -m "Astyle"
git push origin master
