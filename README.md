#raii Example 1

example of raii vs standard vecor with copy contructor in c++ 11

    j@baker:~/src/practice/cpp$ make && ./raii 
    g++ -std=c++11 raii.cc -o raii
    new elf    (400).       addr 0x22b5010
    new elf    (55).        addr 0x22b5050
    new elf    (312).       addr 0x7ffff4f2d3c0
    creating two elves
    new elf    (2).         addr 0x7ffff4f2d430
    copy of    (2)  addr 0x22b5070
    killed elf (2).         addr 0x7ffff4f2d430
    copy of    (312)        addr 0x22b5094
    copy of    (2)  addr 0x22b5090
    killed elf (2).         addr 0x22b5070
    plist[0] 400

    poping two elves.
    killed elf (312).       addr 0x22b5094
    killed elf (400).       addr 0x22b5010
    leaving context.
    killed elf (312).       addr 0x7ffff4f2d3c0
    killed elf (2).         addr 0x22b5090
    killed elf (55).        addr 0x22b5050
