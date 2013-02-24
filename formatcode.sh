#!/bin/bash
astyle --indent=tab=4 --brackets=attach --pad-oper --pad-header --align-pointer=name --unpad-paren --indent-preprocessor --convert-tabs $@
