# network-data-distributor
Manages the allocation and retrieval of meta-data or keys in a distributed storage system.

The storage system switches between states of update one and query one, depending on the type of traffic being exhibited. When the majority of users are querying for keys and other metadata, keys and metadata are stored at every node and updates go to each node, while queries can go to any single node. When the majority of users are updating keys and metadata, keys and metadata are only guaranteed to be at a single node and queries go to all nodes.

This project primarily serves as a way for me to learn and practice socket programming. 
