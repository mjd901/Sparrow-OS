#!/bin/bash
kill $(ps aux | grep 'bochs' | awk '{print $2}')