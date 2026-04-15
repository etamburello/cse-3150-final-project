# cse-3150-final-project-documentation(section 5)
- organized the project into separate classes such as AS, Graph, Policy, BGP, and ROV to keep things modular and easier to manage(Graph does most of the heavy lifting)
- used unorderedmap for storing AS nodes and routing tables to keep lookups fast, and pointers (sharedptr and uniqueptr) to avoid unnecessary copying and manage memory safely
- each AS has a Policy pointer (p), which lets allows for switch between normal BGP behavior and ROV behavior using polymorphism. by default, each AS uses BGP, but can be replaced with ROV(see setROV() function) without changing the rest of the code
- BGP class manages routing logic and uses a receive queue to store incoming announcements and a local RIB (localrib from the instructions) to store the best route per prefix
- for propagating, the implementation of the three-phase propagation model (up, across, down) allows for the prevention of multi-hop propagation. the up and down phases to use the rank structure to ensure correct order, while the across phase sends to all peers first and then processes everything
- ROV class extends BGP and overrides the receive() function to immediately drop invalid announcements (rovinvalid == true)
-
