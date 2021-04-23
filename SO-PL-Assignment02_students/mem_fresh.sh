#!/bin/bash

echo $(grep MemFree /proc/meminfo) : $(date | cut -c5-24)
