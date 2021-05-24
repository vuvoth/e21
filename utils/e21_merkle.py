from ethsnarks.merkletree import MerkleTree
from ethsnarks.mimc import *
TREE_DEEP = 2

tree = MerkleTree(1<<TREE_DEEP);


print(tree.append(mimc_hash([0], 1)))
print(mimc_hash([0], 1));
print(tree.append(mimc_hash([1], 1)))
print(mimc_hash([1], 1))

print("Tree root", tree.root);
print(tree.proof(0).path);
print(tree.proof(0).address);
print(tree.proof(0).leaf);

tree.update(0, mimc_hash([1], 1));
print(tree.proof(0));

print(tree.proof(1));
print(tree.proof(1).address)
