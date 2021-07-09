# TODO:

- Chapter 14 challenge 1 ideas
    - Store an array of where lines start (make these starts a struct)
        - Store offset of first instruction in the start and the line number
    - Use a searching algorithm to find line
    - Figure out how to change writeChunk() to accommodate this
- Look into ways to optimize run(), instead of using a switch:
    - "direct threaded code," "jump table," "computed goto" (Chapter 15)
- Swap to a register-based VM