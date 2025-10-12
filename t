#!/bin/bash
cd /home/bane/scripts/programs/banewfn/test

rm wfn/*.chg
rm screen/*.cub
rm command/*.cub
rm */*.out

cd wfn
../../build/banewfn wfn.inp > wfn_test.out

cd ../command
../../build/banewfn command.inp > command_test.out

cd ../screen
../../build/banewfn screen.inp --screen > screen_test.out