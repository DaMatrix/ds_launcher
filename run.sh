#!/bin/bash

make -j$( nproc ) && dslink ds_launcher.nds
