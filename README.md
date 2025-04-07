# Callgrind-Viewer

# Callgrind File format
version: 1

creator: my_profiler

pid: 12345

cmd: ./myprogram

events: 
Execution Time, 
Call Count, 
Memory Usage

fl=(myprogram)

fn=main

10 1 1000

fl=(myprogram)

fn=helper

10 2 10
fl=(myprogram)

fn=main
15 1 30

fl=(myprogram)

fn=another_helper

20 1 40


### With this format, the applyfilter() can parse any .callgrind or callgrind.out format files.
