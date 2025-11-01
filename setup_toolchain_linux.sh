#!/usr/bin/env sh
node Tools/arm_toolchain_setup/index.cjs

if [ ! $CI ] ; then
  read -p "Press enter to continue"
fi